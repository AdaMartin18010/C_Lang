/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 142
 * Language: c
 * Block ID: 9b8a72ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - C23溢出检查 */
#include <stdckdint.h>
int add_safe(int a, int b, int *result) {
    return ckd_add(result, a, b);  /* 返回true表示溢出 */
}

/* ✅ 合规 - 空指针检查 */
void process_safe(int *p) {
    if (p != NULL) {
        *p = 42;
    }
}
