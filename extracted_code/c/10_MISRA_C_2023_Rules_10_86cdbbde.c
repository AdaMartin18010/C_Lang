/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 320
 * Language: c
 * Block ID: 86cdbbde
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用不同名称 */
int g_count = 0;  /* g_前缀表示全局 */

void increment(void)
{
    int local_count = 0;
    local_count++;
    g_count++;  /* 明确使用全局变量 */
}

/* ✅ 合规 - 参数使用描述性名称 */
int g_timeout_ms = 30;

void connect(int timeout_value)
{
    /* 明确意图 */
    int effective_timeout = timeout_value ? timeout_value : g_timeout_ms;
}

/* ✅ 合规 - 避免重复命名 */
void func(void)
{
    int outer_val = 10;
    {
        int inner_val = 20;  /* 不同名称 */
        printf("inner: %d\n", inner_val);
    }
    printf("outer: %d\n", outer_val);
}
