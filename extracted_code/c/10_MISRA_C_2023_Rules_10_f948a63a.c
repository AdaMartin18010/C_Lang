/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 213
 * Language: c
 * Block ID: f948a63a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 不必要的大作用域 */
int global_temp;  /* 只在函数中使用 */

void func(void)
{
    int i;  /* 只在循环中使用 */
    /* ... 很多代码 ... */
    for (i = 0; i < 10; i++) { }
}

/* ❌ 违反 - 文件作用域过大 */
static FILE *fp;  /* 只在单个函数中使用 */

void process_file(void)
{
    fp = fopen("file", "r");
    /* 使用fp */
    fclose(fp);
}
