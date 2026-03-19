/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 538
 * Language: c
 * Block ID: bad83445
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 所有参数都使用 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* ✅ 合规 - 使用所有参数 */
#define CHECK(x, y) ((x) > (y))

/* ✅ 合规 - 使用可变参数 */
#define LOG(level, msg, ...) \
    printf("[%d] " msg "\n", level, ##__VA_ARGS__)
