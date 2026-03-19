/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 138
 * Language: c
 * Block ID: f948c055
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 外层作用域 */
int count = 0;  /* 全局变量 */

void func(void)
{
    int count = 1;  /* ❌ 隐藏了全局count */
    /* 这里的count是局部变量，全局count被隐藏 */
}
