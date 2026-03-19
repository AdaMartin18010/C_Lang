/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 288
 * Language: c
 * Block ID: 053651ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 局部变量隐藏全局变量 */
int count = 0;  /* 全局计数器 */

void increment(void)
{
    int count = 0;  /* 隐藏全局count！ */
    count++;  /* 局部变量自增 */
}

/* ❌ 违反 - 参数隐藏全局变量 */
int timeout = 30;  /* 全局默认超时 */

void connect(int timeout)  /* 参数隐藏全局变量 */
{
    /* 内部timeout是参数，不是全局变量 */
}

/* ❌ 违反 - 块作用域隐藏 */
void func(void)
{
    int x = 10;
    {
        int x = 20;  /* 隐藏外层x */
        printf("%d\n", x);  /* 打印20 */
    }
    printf("%d\n", x);  /* 打印10 - 容易混淆 */
}
