/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 562
 * Language: c
 * Block ID: f4ebb7ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 Rule 2.7 */
void callback(int event, void *data)
{
    /* 只使用event，未使用data */
    process_event(event);
}

/* ❌ 违反 - 接口要求但必须实现的空函数 */
void unused_handler(int arg1, int arg2)
{
    /* 空实现，参数未使用 */
}
