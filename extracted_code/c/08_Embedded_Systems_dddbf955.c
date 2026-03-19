/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 1129
 * Language: c
 * Block ID: dddbf955
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// itm_log.c

#include "core_cm4.h"

void ITM_Enable(void) {
    // 使能ITM
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    ITM->LAR = 0xC5ACCE55;  // 解锁
    ITM->TER[0] = 1;        // 使能端口0
    ITM->TCR = ITM_TCR_ITMENA_Msk;
}

void ITM_SendChar(char c) {
    if ((ITM->TCR & ITM_TCR_ITMENA_Msk) &&
        (ITM->TER[0] & 1)) {
        while (ITM->PORT[0].u32 == 0);  // 等待空闲
        ITM->PORT[0].u8 = c;
    }
}

void ITM_Print(const char *str) {
    while (*str) {
        ITM_SendChar(*str++);
    }
}

// 重定向printf
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
    return len;
}
