/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 576
 * Language: c
 * Block ID: 321c5753
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用常量格式 */
void log_user_message(const char *user_msg)
{
    printf("%s", user_msg);  /* 安全的字符串输出 */
}

/* ✅ 合规 - 使用fprintf的安全版本 */
fprintf(file, "%s", user_msg);

/* ✅ 合规 - 预定义格式 */
#define FMT_INT "%d\n"
#define FMT_STR "%s\n"
#define FMT_HEX "0x%x\n"

printf(FMT_INT, value);
printf(FMT_STR, str);
