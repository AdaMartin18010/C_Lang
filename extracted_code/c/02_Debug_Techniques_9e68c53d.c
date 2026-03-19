/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\02_Debug_Techniques.md
 * Line: 183
 * Language: c
 * Block ID: 9e68c53d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分级日志系统

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NONE
} LogLevel;

static LogLevel g_log_level = LOG_LEVEL_INFO;
static FILE *g_log_file = NULL;

void log_init(const char *filename, LogLevel level) {
    g_log_level = level;
    if (filename) {
        g_log_file = fopen(filename, "a");
    }
    if (!g_log_file) {
        g_log_file = stderr;
    }
}

void log_message(LogLevel level, const char *file, int line,
                 const char *func, const char *fmt, ...) {
    if (level < g_log_level) return;

    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    // 时间戳
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[26];
    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    // 输出到文件
    fprintf(g_log_file, "[%s] [%s] %s:%d (%s) ",
            time_str, level_str[level], file, line, func);

    va_list args;
    va_start(args, fmt);
    vfprintf(g_log_file, fmt, args);
    va_end(args);

    fprintf(g_log_file, "\n");
    fflush(g_log_file);

    // 致命错误终止程序
    if (level == LOG_LEVEL_FATAL) {
        abort();
    }
}

// 宏包装
#define LOG_DEBUG(...) log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)  log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...)  log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_FATAL(...) log_message(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

// 条件日志（避免高开销计算）
#define LOG_DEBUG_IF(cond, ...) do { if (cond) LOG_DEBUG(__VA_ARGS__); } while(0)
