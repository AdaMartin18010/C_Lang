/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 582
 * Language: c
 * Block ID: 02bc1993
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 向后goto（ spaghetti code ）*/
start:
    if (condition) {
        goto start;  /* 向后跳转！ */
    }

/* ❌ 违反 - 向后continue */
for (int i = 0; i < 10; i++) {
back:
    if (retry) {
        i--;  /* 模拟向后跳转 */
        goto back;
    }
}

/* ❌ 违反 - setjmp/longjmp */
#include <setjmp.h>
jmp_buf env;

void func(void)
{
    if (setjmp(env) == 0) {
        /* 首次进入 */
    } else {
        /* longjmp返回 */
    }
}
