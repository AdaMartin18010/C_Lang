/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 425
 * Language: c
 * Block ID: dc1069d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线程不安全的日志系统
static char log_buffer[4096];
static FILE* log_file;

void log_message(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // 危险：所有线程共享 log_buffer！
    vsnprintf(log_buffer, sizeof(log_buffer), format, args);

    fprintf(log_file, "%s\n", log_buffer);
    fflush(log_file);

    va_end(args);
}
