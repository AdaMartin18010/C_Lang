/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 515
 * Language: c
 * Block ID: 1cbf4ffd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 修改循环计数器 */
for (int i = 0; i < 10; i++) {
    if (condition) {
        i++;  /* 修改循环计数器！ */
    }
}

/* ❌ 违反 - 复杂的循环控制 */
int i = 0;
while (i < 10) {
    i += 2;  /* 修改控制变量 */
    if (error) {
        i = 0;  /* 重置计数器 */
    }
}
