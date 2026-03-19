/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 295
 * Language: c
 * Block ID: bc31da25
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用函数代替宏 */
void log_message(const char *msg)
{
    printf("%s\n", msg);
}
log_message("Hello");

/* ✅ 合规 - 明确命名 */
int var_count = 0;  /* 不使用宏生成 */

/* ✅ 合规 - 使用类型安全的方式 */
typedef struct {
    int count;
} counter_t;
counter_t var_count;
