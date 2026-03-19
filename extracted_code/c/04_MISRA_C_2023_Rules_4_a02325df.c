/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\04_MISRA_C_2023_Rules_4.md
 * Line: 180
 * Language: c
 * Block ID: a02325df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 明确区分 */
int g_debug_level = 0;  /* g_ 前缀表示全局 */

void set_debug_level(void)
{
    int local_debug = 1;  /* 明确不同的名称 */
    g_debug_level = local_debug;  /* 明确赋值给全局变量 */
}

/* ✅ 合规 - 参数使用不同名称 */
int timeout_ms = 1000;

void init(int timeout_value)  /* 不同名称 */
{
    timeout_ms = timeout_value;  /* 明确赋值 */
}

/* ✅ 合规 - 使用指针明确修改全局变量 */
int counter = 0;

void increment(void)
{
    extern int counter;  /* 正确：与全局声明一致 */
    counter++;
}
