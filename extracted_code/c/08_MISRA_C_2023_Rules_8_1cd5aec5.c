/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 561
 * Language: c
 * Block ID: 1cd5aec5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 用户控制的格式字符串 */
void log_user_message(const char *user_msg)
{
    printf(user_msg);  /* 格式字符串攻击！ */
}

/* ❌ 违反 - 拼接格式字符串 */
char fmt[100];
sprintf(fmt, "Value: %%%ds", width);  /* 动态格式 */
printf(fmt, value);
