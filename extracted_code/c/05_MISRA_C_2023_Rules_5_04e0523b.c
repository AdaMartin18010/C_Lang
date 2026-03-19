/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\05_MISRA_C_2023_Rules_5.md
 * Line: 400
 * Language: c
 * Block ID: 04e0523b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用具名变量 */
struct Point p = {.x = 1, .y = 2};
process_point(p);

/* ✅ 合规 - 明确的数组 */
static const int data[] = {1, 2, 3};
int *get_data(void) {
    return (int *)data;
}
