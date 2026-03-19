/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 387
 * Language: c
 * Block ID: 5f2e7674
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 使用复合字面量 */
void process_point(struct Point p);
process_point((struct Point){.x = 1, .y = 2});  /* 复合字面量 */

/* ❌ 违反 - 匿名数组 */
int *get_data(void) {
    return (int[]){1, 2, 3};  /* 生命周期问题！ */
}
