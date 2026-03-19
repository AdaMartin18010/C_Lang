/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 252
 * Language: c
 * Block ID: 62db40c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 [C23相关] - C23溢出检查 */
#if __STDC_VERSION__ >= 202311L
    #include <stdckdint.h>
#endif

int add_safe(int a, int b, int *result) {
#if __STDC_VERSION__ >= 202311L
    return ckd_add(result, a, b);  /* C23溢出检查 */
#else
    /* C11/C17手动检查 */
    if (b > 0 && a > INT_MAX - b) return 0;  /* 溢出 */
    if (b < 0 && a < INT_MIN - b) return 0;  /* 下溢 */
    *result = a + b;
    return 1;
#endif
}

/* ✅ 合规 [C:2023延续] - 空指针检查 */
void process_safe(int *p) {
    if (p != NULL) {
        *p = 42;
    }
}
