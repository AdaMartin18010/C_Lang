/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\02_Atomic_Operations.md
 * Line: 355
 * Language: c
 * Block ID: f0b3d859
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 显式内存屏障 (底层)
#include <stdatomic.h>

void memory_fences() {
    // 全屏障
    atomic_thread_fence(memory_order_seq_cst);

    // 获取屏障
    atomic_thread_fence(memory_order_acquire);

    // 释放屏障
    atomic_thread_fence(memory_order_release);

    // 信号量屏障 (进程间)
    atomic_signal_fence(memory_order_seq_cst);
}

// 编译器屏障 (GCC/Clang)
#define compiler_barrier() __asm__ __volatile__("" ::: "memory")

void compiler_fence_example() {
    int x = 1;
    compiler_barrier();  // 阻止编译器重排
    int y = x;
}
