/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\01_MISRA_C_2023_Rules_1-5.md
 * Line: 46
 * Language: c
 * Block ID: 7145685e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 标准C */
void foo(void);

static void bar(void) {  /* 独立的静态函数 */
    printf("standard C\n");
}

void foo(void)
{
    bar();
}
