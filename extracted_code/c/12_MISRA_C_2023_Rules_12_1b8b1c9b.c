/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 194
 * Language: c
 * Block ID: 1b8b1c9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 修改循环变量 */
for (int i = 0; i < 10; i++) {
    if (skip_condition) {
        i++;  /* 修改循环计数器！ */
    }
}

/* ❌ 违反 - 条件中修改 */
for (int i = 0; i < n; i++) {
    if (remove_item(i)) {
        n--;  /* 修改边界 */
        i--;  /* 回退计数器 */
    }
}

/* ❌ 违反 - 复杂控制 */
for (int i = 0; i < 100; i += step) {
    if (condition) {
        step = 2;  /* 修改步长 */
    }
}
