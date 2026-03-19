/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 116
 * Language: c
 * Block ID: 468b8a8a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 [C:2023延续] - 标准C */
void foo(void);

static void bar(void) {  /* 独立的静态函数 */
    printf("standard C\n");
}

void foo(void)
{
    bar();
}

/* ✅ 合规 [C23相关] - 检查C23支持 */
#if __STDC_VERSION__ >= 202311L
    /* C23代码 */
    constexpr int MAX_SIZE = 100;
#else
    /* C11/C17兼容代码 */
    #define MAX_SIZE 100
#endif
