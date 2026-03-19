/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 579
 * Language: c
 * Block ID: c8403ba2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 显式忽略未使用的参数 */
void callback(int event, void *data)
{
    (void)data;  /* 显式表示故意忽略 */
    process_event(event);
}

/* ✅ 合规 - 使用GCC属性（如果允许扩展） */
#ifdef __GNUC__
    #define UNUSED __attribute__((unused))
#else
    #define UNUSED
#endif

void callback(int event, void *data UNUSED)
{
    process_event(event);
}

/* ✅ 合规 - 删除不需要的参数 */
void simplified_callback(int event)
{
    process_event(event);
}
