/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 171
 * Language: c
 * Block ID: cb357412
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Relaxed 内存序示例
#include <stdatomic.h>
#include <stdint.h>

_Atomic uint64_t counter = ATOMIC_VAR_INIT(0);

void increment_relaxed(void) {
    // Relaxed 递增 - 无同步保证
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
}

uint64_t get_relaxed(void) {
    // Relaxed 读取
    return atomic_load_explicit(&counter, memory_order_relaxed);
}
