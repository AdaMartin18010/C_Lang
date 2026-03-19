/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 424
 * Language: c
 * Block ID: 8fcd7426
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 可预测的随机数 */
int token = rand();  /* 不安全 */

/* 符合规范 - 加密安全 */
#include <openssl/rand.h>
unsigned char token[32];
if (RAND_bytes(token, sizeof(token)) != 1) {
    /* 错误处理 */
}
