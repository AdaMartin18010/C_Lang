/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_Radiation_Hardening.md
 * Line: 231
 * Language: c
 * Block ID: 86173f76
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 控制流签名 - 检测非法跳转
#define SIGNATURE_SEED 0xA5A5A5A5

// 函数签名宏
#define FUNC_ENTRY(sig) \
    volatile uint32_t __cf_sig = (sig) ^ __builtin_return_address(0); \
    if (__cf_sig != EXPECTED_SIG_##sig) goto control_flow_error

#define FUNC_EXIT(sig) \
    if (__cf_sig != ((sig) ^ __builtin_return_address(0))) goto control_flow_error

// 更可靠：内联签名检查
static inline void check_signature(uint32_t expected, uint32_t actual) {
    if (expected != actual) {
        // 触发故障安全
        fault_safe_state();
    }
}

// 看门狗和心跳
typedef struct {
    volatile uint32_t last_beat;
    uint32_t timeout;
    uint32_t missed_beats;
} Watchdog;

void watchdog_init(Watchdog *wd, uint32_t timeout_ms) {
    wd->timeout = timeout_ms;
    wd->last_beat = get_system_tick();
    wd->missed_beats = 0;
}

void watchdog_feed(Watchdog *wd) {
    wd->last_beat = get_system_tick();
    wd->missed_beats = 0;
}

void watchdog_check(Watchdog *wd) {
    uint32_t now = get_system_tick();
    uint32_t elapsed = now - wd->last_beat;

    if (elapsed > wd->timeout) {
        wd->missed_beats++;

        if (wd->missed_beats >= 3) {
            // 系统复位
            system_reset();
        }
    }
}

// 任务心跳宏
#define TASK_HEARTBEAT(wd) do { \
    static uint32_t __beat_count = 0; \
    if (++__beat_count >= 1000) { \
        watchdog_feed(wd); \
        __beat_count = 0; \
    } \
} while(0)
