/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 524
 * Language: c
 * Block ID: b82c4ab1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.6 - 宏参数未使用 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))  /* 如果只用a */

/* 错误的使用 */
#define CHECK(x, y) ((x) > 0)  /* y未使用 */

/* ❌ 违反 - 多参数宏中的未使用 */
#define LOG(level, msg, ...) \
    printf("[%d] %s\n", level, msg)  /* __VA_ARGS__未使用 */
