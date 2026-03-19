/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 232
 * Language: c
 * Block ID: c04c262d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 [C:2023延续] - 有符号整数溢出（UB） */
int add(int a, int b) {
    return a + b;  /* 溢出 = 灾难 */
}

/* ❌ 违反 [C:2023延续] - 未检查空指针 */
void process(int *p) {
    *p = 42;  /* p可能为NULL */
}

/* ❌ 违反 [C23相关] - constexpr修改 [待确认] */
#if __STDC_VERSION__ >= 202311L
    constexpr int arr[] = {1, 2, 3};
    /* arr[0] = 5; */  /* 修改constexpr数据是UB */
#endif
