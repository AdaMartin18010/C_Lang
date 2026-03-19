/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 68
 * Language: c
 * Block ID: 1d5cec63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 辐射效应注入测试框架
#include <setjmp.h>

static jmp_buf radiation_recovery;
static volatile int fault_injected = 0;

// 模拟单粒子翻转
void inject_seu(void *target, size_t size) {
    if (size > 0) {
        size_t bit_pos = rand() % (size * 8);
        size_t byte_idx = bit_pos / 8;
        int bit_idx = bit_pos % 8;

        ((uint8_t*)target)[byte_idx] ^= (1 << bit_idx);
        fault_injected = 1;
    }
}

// 辐射容错测试宏
#define RADIATION_TEST_BEGIN() \
    if (setjmp(radiation_recovery) != 0) { \
        printf("Recovered from radiation fault\n"); \
    }

#define RADIATION_TEST_END()

// 双模块冗余比较
int tmr_vote(int a, int b, int c) {
    if (a == b) return a;
    if (a == c) return a;
    if (b == c) return b;
    // 三者都不一致，触发错误处理
    return -1;
}

// 内存扫描检测 SEU
void memory_scrubber(uint32_t *memory, size_t size,
                     uint32_t *golden_copy) {
    for (size_t i = 0; i < size; i++) {
        if (memory[i] != golden_copy[i]) {
            // 检测到差异，可能是 SEU
            printf("SEU detected at address %p, correcting...\n",
                   &memory[i]);
            memory[i] = golden_copy[i];
            log_seu_event(i);
        }
    }
}
