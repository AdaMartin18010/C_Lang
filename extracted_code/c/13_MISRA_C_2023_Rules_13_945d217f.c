/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 309
 * Language: c
 * Block ID: 945d217f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 类型安全的可变参数 */

/* 1. 使用宏包装（编译时检查）*/
#define LOG_INFO(...) log_message(LOG_LEVEL_INFO, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __VA_ARGS__)

/* 2. 明确的格式字符串 */
void log_message(int level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    /* 格式字符串必须是常量 */
    vfprintf(log_file, fmt, args);

    va_end(args);
}

/* 3. 替代方案：使用结构体 */
typedef struct {
    const char *key;
    union {
        int i;
        double d;
        const char *s;
        bool b;
    } value;
    enum { TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_BOOL } type;
} LogParam;

void log_structured(const char *message,
                    size_t param_count,
                    const LogParam *params);

/* 使用 */
LogParam params[] = {
    {"user_id", {.i = 123}, TYPE_INT},
    {"action", {.s = "login"}, TYPE_STRING}
};
log_structured("User action", 2, params);
