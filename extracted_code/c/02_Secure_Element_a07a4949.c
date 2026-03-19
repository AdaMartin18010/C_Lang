/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Secure_Element.md
 * Line: 470
 * Language: c
 * Block ID: a07a4949
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* se_operations.c - 安全元件操作示例 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "se_pcsc.h"

/* GP标准指令 */
#define GP_CLA          0x00
#define GP_INS_SELECT   0xA4
#define GP_INS_GET_DATA 0xCA
#define GP_INS_GET_RESPONSE 0xC0

/* 选择AID */
static SEError se_select_aid(SEContext *ctx, const uint8_t *aid, uint8_t aidLen)
{
    APDUCommand cmd = {
        .cla = GP_CLA,
        .ins = GP_INS_SELECT,
        .p1 = 0x04,  /* 通过名称选择 */
        .p2 = 0x00,
        .lc = aidLen
    };
    memcpy(cmd.data, aid, aidLen);

    APDUResponse resp;
    SEError err = se_transmit(ctx, &cmd, &resp);
    if (err != SE_SUCCESS) return err;

    if (resp.sw1 != 0x90 || resp.sw2 != 0x00) {
        fprintf(stderr, "选择失败: SW=%02X%02X\n", resp.sw1, resp.sw2);
        return SE_ERROR_RESPONSE;
    }

    printf("成功选择应用\n");
    return SE_SUCCESS;
}

/* 验证PIN */
SEError se_verify_pin(SEContext *ctx, const uint8_t *pin, uint8_t pinLen)
{
    APDUCommand cmd = {
        .cla = 0x00,
        .ins = 0x20,  /* VERIFY */
        .p1 = 0x00,
        .p2 = 0x00,   /* PIN引用 */
        .lc = pinLen
    };
    memcpy(cmd.data, pin, pinLen);

    APDUResponse resp;
    SEError err = se_transmit(ctx, &cmd, &resp);
    if (err != SE_SUCCESS) return err;

    if (resp.sw1 == 0x90 && resp.sw2 == 0x00) {
        printf("PIN验证成功\n");
        return SE_SUCCESS;
    } else if (resp.sw1 == 0x63) {
        int retries = resp.sw2 & 0x0F;
        fprintf(stderr, "PIN错误，剩余尝试次数: %d\n", retries);
        return SE_ERROR_RESPONSE;
    } else {
        fprintf(stderr, "PIN验证失败: SW=%02X%02X\n", resp.sw1, resp.sw2);
        return SE_ERROR_RESPONSE;
    }
}

/* 读取二进制数据 */
SEError se_read_binary(SEContext *ctx, uint16_t offset,
                        uint8_t *data, uint8_t length)
{
    APDUCommand cmd = {
        .cla = 0x00,
        .ins = 0xB0,  /* READ BINARY */
        .p1 = (offset >> 8) & 0x7F,
        .p2 = offset & 0xFF,
        .le = length
    };

    APDUResponse resp;
    SEError err = se_transmit(ctx, &cmd, &resp);
    if (err != SE_SUCCESS) return err;

    if (resp.sw1 == 0x90 && resp.sw2 == 0x00) {
        memcpy(data, resp.data, resp.dataLen);
        printf("成功读取 %d 字节\n", resp.dataLen);
        return SE_SUCCESS;
    }

    return SE_ERROR_RESPONSE;
}

/* 主函数示例 */
int main(int argc, char *argv[])
{
    SEContext ctx;
    SEError err;

    printf("=== 安全元件通信示例 ===\n\n");

    /* 初始化 */
    err = se_init(&ctx);
    if (err != SE_SUCCESS) {
        fprintf(stderr, "初始化失败: %s\n", se_error_string(err));
        return 1;
    }

    /* 列出读卡器 */
    char readers[10][256];
    int readerCount = 10;
    err = se_list_readers(&ctx, readers, &readerCount);
    if (err != SE_SUCCESS) {
        fprintf(stderr, "无法列出读卡器: %s\n", se_error_string(err));
        se_release(&ctx);
        return 1;
    }

    printf("发现 %d 个读卡器:\n", readerCount);
    for (int i = 0; i < readerCount; i++) {
        printf("  [%d] %s\n", i + 1, readers[i]);
    }

    if (readerCount == 0) {
        printf("未找到读卡器\n");
        se_release(&ctx);
        return 1;
    }

    /* 连接第一个读卡器 */
    err = se_connect(&ctx, readers[0]);
    if (err != SE_SUCCESS) {
        fprintf(stderr, "连接失败: %s\n", se_error_string(err));
        se_release(&ctx);
        return 1;
    }

    /* 示例：选择应用 (AID: A0000000030000) */
    uint8_t aid[] = {0xA0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00};
    se_select_aid(&ctx, aid, sizeof(aid));

    /* 清理 */
    se_release(&ctx);
    printf("\n通信结束\n");

    return 0;
}
