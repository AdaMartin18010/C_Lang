/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 1257
 * Language: c
 * Block ID: c9b32760
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// debug.c - 软件调试工具

#include "hal.h"

// 断言宏
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            debug_printf("ASSERT failed: %s:%d: %s\n", \
                        __FILE__, __LINE__, #cond); \
            debug_backtrace(); \
            system_control(SYS_RESET); \
        } \
    } while(0)

// 日志级别
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
} log_level_t;

static log_level_t current_log_level = LOG_DEBUG;

// 格式化输出
void debug_printf(const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    uart_puts(buf);
}

// 十六进制转储
void hexdump(const void *data, size_t len) {
    const uint8_t *p = data;

    for (size_t i = 0; i < len; i += 16) {
        debug_printf("%08lx: ", (uintptr_t)p + i);

        // 十六进制
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            debug_printf("%02x ", p[i + j]);
        }

        // 填充
        for (size_t j = len - i; j < 16; j++) {
            debug_printf("   ");
        }

        // ASCII
        debug_printf(" |");
        for (size_t j = 0; j < 16 && i + j < len; j++) {
            char c = p[i + j];
            debug_printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        debug_printf("|\n");
    }
}

// 栈回溯
void debug_backtrace(void) {
    debug_printf("\n=== Stack Backtrace ===\n");

    uintptr_t *fp = __builtin_frame_address(0);
    int depth = 0;

    while (fp && depth < 20) {
        uintptr_t ra = fp[-1];  // 返回地址
        debug_printf("#%d: 0x%016lx\n", depth, ra);

        fp = (uintptr_t *)fp[-2];  // 上一个帧指针
        depth++;
    }
}

// 性能计时器
static uint64_t perf_start_cycle;

void perf_start(void) {
    perf_start_cycle = read_csr(cycle);
}

void perf_end(const char *label) {
    uint64_t end_cycle = read_csr(cycle);
    uint64_t cycles = end_cycle - perf_start_cycle;
    double us = (double)cycles / (CPU_FREQ / 1000000.0);

    debug_printf("%s: %lu cycles (%.2f us)\n", label, cycles, us);
}
