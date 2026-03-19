/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 128
 * Language: c
 * Block ID: 23ec888f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 有符号整数溢出（UB） */
int add(int a, int b) {
    return a + b;  /* 溢出 = 灾难 */
}

/* ❌ 违反 - 未检查空指针 */
void process(int *p) {
    *p = 42;  /* p可能为NULL */
}
