/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 343
 * Language: c
 * Block ID: 64e4de22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 显式标记未使用参数 */
void callback(void *user_data, int event)
{
    (void)user_data;  /* 明确不使用 */
    
    process_event(event);
}

/* ✅ 合规 - 使用属性标记 */
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

void handler(int required, int UNUSED optional)
{
    /* 只使用required */
    process(required);
}

/* ✅ 合规 - 接口兼容 */
void interface_impl(int a, int b, int c)
{
    /* 此实现不需要b和c */
    (void)b;
    (void)c;
    
    use(a);
}
