/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Secure_Element.md
 * Line: 234
 * Language: c
 * Block ID: 2604ab72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* se_pcsc.c - PC/SC接口实现 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "se_pcsc.h"

/* 错误代码转字符串 */
const char* se_error_string(SEError err)
{
    switch (err) {
        case SE_SUCCESS: return "成功";
        case SE_ERROR_NO_READERS: return "未找到读卡器";
        case SE_ERROR_NO_CARD: return "未检测到卡片";
        case SE_ERROR_COMMUNICATION: return "通信错误";
        case SE_ERROR_PROTOCOL: return "协议错误";
        case SE_ERROR_RESPONSE: return "响应错误";
        case SE_ERROR_MEMORY: return "内存错误";
        default: return "未知错误";
    }
}

/* 初始化PC/SC上下文 */
SEError se_init(SEContext *ctx)
{
    if (!ctx) return SE_ERROR_MEMORY;

    memset(ctx, 0, sizeof(SEContext));

    LONG rv = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &ctx->hContext);
    if (rv != SCARD_S_SUCCESS) {
        fprintf(stderr, "SCardEstablishContext失败: 0x%08X\n", (unsigned int)rv);
        return SE_ERROR_COMMUNICATION;
    }

    return SE_SUCCESS;
}

/* 释放PC/SC上下文 */
SEError se_release(SEContext *ctx)
{
    if (!ctx) return SE_ERROR_MEMORY;

    if (ctx->hCard) {
        SCardDisconnect(ctx->hCard, SCARD_LEAVE_CARD);
        ctx->hCard = 0;
    }

    if (ctx->hContext) {
        SCardReleaseContext(ctx->hContext);
        ctx->hContext = 0;
    }

    return SE_SUCCESS;
}

/* 列出可用读卡器 */
SEError se_list_readers(SEContext *ctx, char readers[][256], int *count)
{
    if (!ctx || !readers || !count) return SE_ERROR_MEMORY;

    DWORD readersLen = SCARD_AUTOALLOCATE;
    LPSTR readerList = NULL;

    LONG rv = SCardListReaders(ctx->hContext, NULL, (LPSTR)&readerList, &readersLen);
    if (rv != SCARD_S_SUCCESS) {
        if (rv == SCARD_E_NO_READERS_AVAILABLE) {
            return SE_ERROR_NO_READERS;
        }
        fprintf(stderr, "SCardListReaders失败: 0x%08X\n", (unsigned int)rv);
        return SE_ERROR_COMMUNICATION;
    }

    /* 解析读卡器列表(以\0分隔的双\0结束字符串) */
    int idx = 0;
    char *ptr = readerList;
    while (*ptr != '\0' && idx < *count) {
        strncpy(readers[idx], ptr, 255);
        readers[idx][255] = '\0';
        ptr += strlen(ptr) + 1;
        idx++;
    }
    *count = idx;

    SCardFreeMemory(ctx->hContext, readerList);
    return SE_SUCCESS;
}

/* 连接智能卡 */
SEError se_connect(SEContext *ctx, const char *reader)
{
    if (!ctx || !reader) return SE_ERROR_MEMORY;

    strncpy(ctx->readerName, reader, 255);
    ctx->readerName[255] = '\0';

    DWORD activeProtocol;
    LONG rv = SCardConnect(ctx->hContext, reader,
                           SCARD_SHARE_SHARED,
                           SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                           &ctx->hCard, &activeProtocol);

    if (rv != SCARD_S_SUCCESS) {
        if (rv == SCARD_E_NO_SMARTCARD) {
            return SE_ERROR_NO_CARD;
        }
        fprintf(stderr, "SCardConnect失败: 0x%08X\n", (unsigned int)rv);
        return SE_ERROR_COMMUNICATION;
    }

    ctx->protocol = activeProtocol;

    /* 获取ATR */
    DWORD atrLen = sizeof(ctx->atr);
    rv = SCardStatus(ctx->hCard, NULL, NULL, NULL, NULL, ctx->atr, &atrLen);
    if (rv == SCARD_S_SUCCESS) {
        ctx->atrLen = atrLen;
    }

    printf("已连接到: %s\n", reader);
    printf("协议: %s\n", (activeProtocol == SCARD_PROTOCOL_T0) ? "T=0" : "T=1");
    printf("ATR: ");
    for (DWORD i = 0; i < ctx->atrLen; i++) {
        printf("%02X ", ctx->atr[i]);
    }
    printf("\n");

    return SE_SUCCESS;
}

/* 断开智能卡连接 */
SEError se_disconnect(SEContext *ctx)
{
    if (!ctx || !ctx->hCard) return SE_ERROR_MEMORY;

    SCardDisconnect(ctx->hCard, SCARD_LEAVE_CARD);
    ctx->hCard = 0;

    return SE_SUCCESS;
}

/* 发送APDU命令 */
SEError se_transmit(SEContext *ctx, const APDUCommand *cmd, APDUResponse *resp)
{
    if (!ctx || !cmd || !resp) return SE_ERROR_MEMORY;

    /* 构造APDU */
    uint8_t apdu[261];
    int apduLen = 0;

    apdu[apduLen++] = cmd->cla;
    apdu[apduLen++] = cmd->ins;
    apdu[apduLen++] = cmd->p1;
    apdu[apduLen++] = cmd->p2;

    /* Lc字段 */
    if (cmd->lc > 0) {
        if (cmd->lc <= 255) {
            apdu[apduLen++] = cmd->lc;
            memcpy(apdu + apduLen, cmd->data, cmd->lc);
            apduLen += cmd->lc;
        } else {
            /* 扩展Lc */
            apdu[apduLen++] = 0x00;
            apdu[apduLen++] = (cmd->lc >> 8) & 0xFF;
            apdu[apduLen++] = cmd->lc & 0xFF;
            memcpy(apdu + apduLen, cmd->data, cmd->lc);
            apduLen += cmd->lc;
        }
    }

    /* Le字段 */
    if (cmd->le > 0) {
        apdu[apduLen++] = cmd->le;
    } else if (cmd->le == 0 && cmd->lc == 0) {
        apdu[apduLen++] = 0x00;  /* 期望最大响应 */
    }

    /* 发送APDU */
    uint8_t response[258];
    DWORD respLen = sizeof(response);
    DWORD proto = ctx->protocol;

    LONG rv = SCardTransmit(ctx->hCard,
                           (proto == SCARD_PROTOCOL_T0) ? SCARD_PCI_T0 : SCARD_PCI_T1,
                           apdu, apduLen, NULL, response, &respLen);

    if (rv != SCARD_S_SUCCESS) {
        fprintf(stderr, "SCardTransmit失败: 0x%08X\n", (unsigned int)rv);
        return SE_ERROR_COMMUNICATION;
    }

    /* 解析响应 */
    if (respLen >= 2) {
        resp->sw1 = response[respLen - 2];
        resp->sw2 = response[respLen - 1];
        resp->dataLen = (uint8_t)(respLen - 2);
        if (resp->dataLen > 0) {
            memcpy(resp->data, response, resp->dataLen);
        }

        /* 获取更多数据 (SW1=0x61) */
        while (resp->sw1 == 0x61) {
            uint8_t get_response[] = { 0x00, 0xC0, 0x00, 0x00, resp->sw2 };
            respLen = sizeof(response);

            rv = SCardTransmit(ctx->hCard,
                              (proto == SCARD_PROTOCOL_T0) ? SCARD_PCI_T0 : SCARD_PCI_T1,
                              get_response, sizeof(get_response), NULL, response, &respLen);

            if (rv != SCARD_S_SUCCESS) break;

            if (respLen >= 2) {
                uint8_t moreLen = (uint8_t)(respLen - 2);
                if (resp->dataLen + moreLen <= sizeof(resp->data)) {
                    memcpy(resp->data + resp->dataLen, response, moreLen);
                    resp->dataLen += moreLen;
                }
                resp->sw1 = response[respLen - 2];
                resp->sw2 = response[respLen - 1];
            }
        }

        return SE_SUCCESS;
    }

    return SE_ERROR_RESPONSE;
}
