/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 294
 * Language: c
 * Block ID: 5231f2ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 显式转换，附带检查 */
if (large > SHRT_MAX || large < SHRT_MIN) {
    /* 处理溢出 */
} else {
    short small = (short)large;  /* 安全的显式转换 */
}

/* ✅ 合规 - 显式转换 */
unsigned int unsigned_val = (unsigned int)signed_val;  /* 明确意图 */

/* ✅ 合规 - 四舍五入 */
int i = (int)round(f);  /* 明确舍入方式 */

/* ✅ 合规 - 使用匹配的类型 */
int64_t result = calc();  /* 类型匹配 */
uint16_t safe_value = (uint16_t)(value & 0xFFFF);  /* 显式掩码 */
