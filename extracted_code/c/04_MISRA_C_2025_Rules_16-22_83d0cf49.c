/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 408
 * Language: c
 * Block ID: 83d0cf49
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 硬编码密钥 */
const char* api_key = "sk-1234567890abcdef";  /* 违反规则 */

/* 符合规范 - 安全存储 */
/* 从安全存储（密钥库、环境变量）加载 */
char api_key[256];
if (load_from_keyring("api_key", api_key, sizeof(api_key)) != 0) {
    /* 错误处理 */
}
