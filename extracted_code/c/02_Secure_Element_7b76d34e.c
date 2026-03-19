/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Secure_Element.md
 * Line: 170
 * Language: c
 * Block ID: 7b76d34e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* se_pcsc.h - PC/SC安全元件通信头文件 */
#ifndef SE_PCSC_H
#define SE_PCSC_H

#include <windows.h>
#include <winscard.h>
#include <stdint.h>
#include <stdbool.h>

/* 错误代码 */
typedef enum {
    SE_SUCCESS = 0,
    SE_ERROR_NO_READERS = -1,
    SE_ERROR_NO_CARD = -2,
    SE_ERROR_COMMUNICATION = -3,
    SE_ERROR_PROTOCOL = -4,
    SE_ERROR_RESPONSE = -5,
    SE_ERROR_MEMORY = -6
} SEError;

/* 安全元件上下文 */
typedef struct {
    SCARDCONTEXT hContext;      /* PC/SC上下文 */
    SCARDHANDLE hCard;          /* 卡片句柄 */
    char readerName[256];       /* 读卡器名称 */
    DWORD protocol;             /* 当前协议 */
    uint8_t atr[32];            /* 复位应答 */
    DWORD atrLen;               /* ATR长度 */
} SEContext;

/* APDU命令结构 */
typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t lc;
    uint8_t data[256];
    uint8_t le;
} APDUCommand;

/* APDU响应结构 */
typedef struct {
    uint8_t data[256];
    uint8_t dataLen;
    uint8_t sw1;
    uint8_t sw2;
} APDUResponse;

/* 函数声明 */
SEError se_init(SEContext *ctx);
SEError se_release(SEContext *ctx);
SEError se_list_readers(SEContext *ctx, char readers[][256], int *count);
SEError se_connect(SEContext *ctx, const char *reader);
SEError se_disconnect(SEContext *ctx);
SEError se_transmit(SEContext *ctx, const APDUCommand *cmd, APDUResponse *resp);
const char* se_error_string(SEError err);

#endif /* SE_PCSC_H */
