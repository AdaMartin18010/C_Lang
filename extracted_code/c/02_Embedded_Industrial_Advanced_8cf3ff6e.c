/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1787
 * Language: c
 * Block ID: 8cf3ff6e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 结构化日志系统
// 支持本地存储和远程上传
// ============================================================

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

typedef struct {
    uint32_t timestamp;
    LogLevel level;
    const char *module;
    const char *message;
    uint32_t error_code;
} LogEntry;

// 环形日志缓冲区
#define LOG_BUFFER_SIZE 100
static LogEntry log_buffer[LOG_BUFFER_SIZE];
static volatile uint32_t log_write_idx = 0;

void system_log(LogLevel level, const char *module,
                const char *fmt, ...) {
    LogEntry *entry = &log_buffer[log_write_idx % LOG_BUFFER_SIZE];

    entry->timestamp = get_system_tick();
    entry->level = level;
    entry->module = module;

    va_list args;
    va_start(args, fmt);
    vsnprintf(entry->message, MAX_LOG_MSG_LEN, fmt, args);
    va_end(args);

    log_write_idx++;

    // 高级别错误立即上传
    if (level >= LOG_LEVEL_ERROR) {
        trigger_log_upload();
    }
}

// 崩溃信息保存
void save_crash_info(uint32_t *stack_trace, int depth) {
    CrashInfo info;
    info.magic = CRASH_MAGIC;
    info.timestamp = get_rtc_time();
    info.task_name = xTaskGetCurrentTaskName();

    for (int i = 0; i < depth && i < MAX_STACK_DEPTH; i++) {
        info.stack_trace[i] = stack_trace[i];
    }

    // 保存到RTC内存或Flash
    rtc_memory_write(RTC_CRASH_INFO_ADDR, &info, sizeof(info));
}

// 死机原因分析
void analyze_crash(void) {
    CrashInfo info;
    if (rtc_memory_read(RTC_CRASH_INFO_ADDR, &info, sizeof(info)) == SUCCESS) {
        if (info.magic == CRASH_MAGIC) {
            ESP_LOGE(TAG, "Previous crash at %s", info.timestamp);
            ESP_LOGE(TAG, "Task: %s", info.task_name);

            for (int i = 0; i < MAX_STACK_DEPTH; i++) {
                ESP_LOGE(TAG, "  [%d] 0x%08X", i, info.stack_trace[i]);
            }
        }
    }
}
