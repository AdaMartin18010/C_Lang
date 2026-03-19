/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 824
 * Language: c
 * Block ID: 67216d22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 日志系统性能监控
 */

typedef struct {
    _Atomic uint64_t messages_logged;
    _Atomic uint64_t bytes_written;
    _Atomic uint64_t dropped_messages;
    _Atomic uint64_t write_latency_us;
    _Atomic uint64_t buffer_full_events;

    // 每秒统计
    uint64_t last_second_messages;
    uint64_t last_check_time;
} LogMetrics;

// 记录指标
void record_log_metrics(LogMetrics *metrics, size_t bytes,
                         uint64_t latency_us)
{
    atomic_fetch_add(&metrics->messages_logged, 1);
    atomic_fetch_add(&metrics->bytes_written, bytes);
    atomic_fetch_add(&metrics->write_latency_us, latency_us);
}

// 获取每秒日志速率
uint64_t get_logs_per_second(LogMetrics *metrics)
{
    uint64_t now = get_timestamp_us();
    uint64_t current = atomic_load(&metrics->messages_logged);

    uint64_t elapsed = now - metrics->last_check_time;
    uint64_t diff = current - metrics->last_second_messages;

    if (elapsed > 0) {
        return (diff * 1000000) / elapsed;
    }
    return 0;
}
