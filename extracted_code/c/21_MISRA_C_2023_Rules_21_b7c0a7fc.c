/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\21_MISRA_C_2023_Rules_21.md
 * Line: 190
 * Language: c
 * Block ID: b7c0a7fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 移位宽度 >= 类型宽度 */
uint32_t x = 1 << 32;  /* 未定义行为 */

/* ❌ 违反 - 负移位 */
int y = 1 << -1;  /* 未定义行为 */

/* ❌ 违反 - 右移负数 */
int z = -1 >> 1;  /* 实现定义的 */

/* ❌ 违反 - 左移有符号负数 */
int w = -1 << 1;  /* 未定义行为 */

/* ❌ 违反 - 移位溢出 */
int overflow = 0x40000000 << 1;  /* 溢出 */
