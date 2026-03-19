/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 3388
 * Language: c
 * Block ID: e32651b2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 轻量级日志系统 - 环形缓冲区实现
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* 日志级别 */
typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
} log_level_t;

/* 日志配置 */
#define LOG_BUFFER_SIZE     256
#define LOG_MESSAGE_MAX     32
#define CURRENT_LOG_LEVEL   LOG_LEVEL_DEBUG

/* 日志条目 */
typedef struct {
    uint32_t timestamp;
    uint8_t level;
    char message[LOG_MESSAGE_MAX];
} log_entry_t;

/* 环形日志缓冲区 */
static struct {
    log_entry_t entries[LOG_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    uint8_t count;
    log_level_t level_filter;
} log_ring;

/* 初始化 */
void log_init(void)
{
    log_ring.head = 0;
    log_ring.tail = 0;
    log_ring.count = 0;
    log_ring.level_filter = CURRENT_LOG_LEVEL;
}

/* 写入日志 */
void log_write(log_level_t level, const char *msg)
{
    if (level > log_ring.level_filter) return;

    uint16_t idx = log_ring.head;

    log_ring.entries[idx].timestamp = get_tick_ms();
    log_ring.entries[idx].level = level;
    strncpy(log_ring.entries[idx].message, msg, LOG_MESSAGE_MAX - 1);
    log_ring.entries[idx].message[LOG_MESSAGE_MAX - 1] = '\0';

    log_ring.head = (idx + 1) % LOG_BUFFER_SIZE;

    if (log_ring.count < LOG_BUFFER_SIZE) {
        log_ring.count++;
    } else {
        /* 覆盖旧数据 */
        log_ring.tail = log_ring.head;
    }
}

/* 日志宏 */
#define LOG_E(msg)  log_write(LOG_LEVEL_ERROR, msg)
#define LOG_W(msg)  log_write(LOG_LEVEL_WARN, msg)
#define LOG_I(msg)  log_write(LOG_LEVEL_INFO, msg)
#define LOG_D(msg)  log_write(LOG_LEVEL_DEBUG, msg)

/* 导出日志 */
void log_dump(void (*output_func)(const char *))
{
    char buf[64];
    const char *level_str[] = {"", "E", "W", "I", "D", "V"};

    while (log_ring.tail != log_ring.head) {
        log_entry_t *entry = &log_ring.entries[log_ring.tail];

        mini_snprintf(buf, sizeof(buf), "[%lu]%s:%s\r\n",
                      entry->timestamp,
                      level_str[entry->level],
                      entry->message);

        output_func(buf);

        log_ring.tail = (log_ring.tail + 1) % LOG_BUFFER_SIZE;
    }
}

/* ============================================================================
 * 性能计数器
 * ============================================================================ */

/* 简单性能计数器 */
typedef struct {
    const char *name;
    uint32_t count;
    uint32_t total_time_us;
    uint32_t max_time_us;
} perf_counter_t;

#define MAX_PERF_COUNTERS   8

static perf_counter_t perf_counters[MAX_PERF_COUNTERS];
static uint8_t perf_counter_count = 0;

perf_counter_t* perf_register(const char *name)
{
    if (perf_counter_count >= MAX_PERF_COUNTERS) return NULL;

    perf_counter_t *counter = &perf_counters[perf_counter_count++];
    counter->name = name;
    counter->count = 0;
    counter->total_time_us = 0;
    counter->max_time_us = 0;

    return counter;
}

void perf_begin(perf_counter_t *counter)
{
    counter->start_time = get_micros();
}

void perf_end(perf_counter_t *counter)
{
    uint32_t elapsed = get_micros() - counter->start_time;

    counter->count++;
    counter->total_time_us += elapsed;
    if (elapsed > counter->max_time_us) {
        counter->max_time_us = elapsed;
    }
}

void perf_report(void)
{
    char buf[64];

    for (int i = 0; i < perf_counter_count; i++) {
        perf_counter_t *c = &perf_counters[i];
        uint32_t avg = c->count > 0 ? c->total_time_us / c->count : 0;

        mini_snprintf(buf, sizeof(buf), "%s: cnt=%lu avg=%lu max=%lu\r\n",
                      c->name, c->count, avg, c->max_time_us);
        uart_send_string(buf);
    }
}
