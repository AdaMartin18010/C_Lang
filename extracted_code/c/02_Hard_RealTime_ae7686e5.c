/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 571
 * Language: c
 * Block ID: ae7686e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 高精度时间戳 - 用于事件排序和测量
// ============================================================================

// 64位时间戳 (防止32位溢出)
typedef union {
    uint64_t full;
    struct {
        uint32_t low;
        uint32_t high;
    } part;
} Timestamp;

// 全局时钟
typedef struct {
    Timestamp base_time;        // 基准时间
    uint32_t tick_rate_hz;      // 时钟频率
    uint32_t last_tick;         // 上次tick
    uint32_t overflow_count;    // 溢出计数
} GlobalClock;

// 获取当前时间戳 (线程安全)
Timestamp get_timestamp(GlobalClock *clk) {
    Timestamp ts;
    uint32_t tick = read_hardware_timer();

    disable_interrupts();

    ts.part.high = clk->overflow_count;
    ts.part.low = tick;

    // 检查是否刚溢出
    if (tick < clk->last_tick) {
        ts.part.high++;
    }

    enable_interrupts();

    return ts;
}

// 时间戳比较
bool timestamp_before(Timestamp a, Timestamp b) {
    return a.full < b.full;
}

uint64_t timestamp_diff_us(Timestamp a, Timestamp b, uint32_t freq_hz) {
    uint64_t diff_ticks = (a.full > b.full) ? (a.full - b.full) : (b.full - a.full);
    return (diff_ticks * 1000000ULL) / freq_hz;
}

// ============================================================================
// 分布式时钟同步 (用于多ECU系统)
// ============================================================================

#define SYNC_INTERVAL_MS    100
#define MAX_CLOCK_DRIFT_PPM 100     // 最大允许漂移 100ppm

typedef struct {
    uint32_t local_time;
    uint32_t master_time;
    int32_t offset;
    int32_t drift_ppm;
    bool synchronized;
} ClockSync;

void clock_sync_update(ClockSync *sync, uint32_t received_master_time) {
    uint32_t local = get_milliseconds();

    // 计算偏移
    sync->offset = (int32_t)(received_master_time - local);

    // 计算漂移
    if (sync->local_time != 0) {
        uint32_t elapsed_local = local - sync->local_time;
        uint32_t elapsed_master = received_master_time - sync->master_time;

        if (elapsed_local > 0) {
            sync->drift_ppm = (int32_t)((elapsed_master - elapsed_local) * 1000000LL / elapsed_local);
        }
    }

    sync->local_time = local;
    sync->master_time = received_master_time;

    // 检查漂移是否在允许范围
    if (abs(sync->drift_ppm) > MAX_CLOCK_DRIFT_PPM) {
        log_warning(WARN_CLOCK_DRIFT_EXCEEDED, sync->drift_ppm);
    }

    sync->synchronized = true;
}

// 获取同步后的时间
uint32_t get_synchronized_time(const ClockSync *sync) {
    return get_milliseconds() + sync->offset;
}
