/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 446
 * Language: c
 * Block ID: a396d1c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线程安全的日志系统
#include <pthread.h>

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
static FILE* log_file;

// 使用线程局部存储
_Thread_local char thread_buffer[4096];

void log_message_safe(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // 方案 1：使用线程局部缓冲区
    vsnprintf(thread_buffer, sizeof(thread_buffer), format, args);

    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "[%lu] %s\n", pthread_self(), thread_buffer);
    fflush(log_file);
    pthread_mutex_unlock(&log_mutex);

    va_end(args);
}

// 更高效的方案：批量写入
#define LOG_BUFFER_SIZE 65536

typedef struct {
    char buffer[LOG_BUFFER_SIZE];
    size_t used;
    pthread_mutex_t mutex;
} log_buffer_t;

void log_message_buffered(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // 格式化到线程局部缓冲区
    char temp[4096];
    int len = vsnprintf(temp, sizeof(temp), format, args);
    va_end(args);

    pthread_mutex_lock(&global_log_buffer.mutex);

    // 检查是否需要刷新
    if (global_log_buffer.used + len + 2 > LOG_BUFFER_SIZE) {
        fwrite(global_log_buffer.buffer, 1, global_log_buffer.used, log_file);
        global_log_buffer.used = 0;
    }

    // 追加到缓冲区
    memcpy(global_log_buffer.buffer + global_log_buffer.used, temp, len);
    global_log_buffer.used += len;
    global_log_buffer.buffer[global_log_buffer.used++] = '\n';

    pthread_mutex_unlock(&global_log_buffer.mutex);
}
