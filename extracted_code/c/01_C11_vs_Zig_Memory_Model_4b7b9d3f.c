/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 345
 * Language: c
 * Block ID: 4b7b9d3f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Sequential Consistency 示例
#include <stdatomic.h>

_Atomic int x = ATOMIC_VAR_INIT(0);
_Atomic int y = ATOMIC_VAR_INIT(0);

void thread1(void) {
    // SeqCst: 所有线程以相同顺序看到此操作
    atomic_store_explicit(&x, 1, memory_order_seq_cst);
    int r1 = atomic_load_explicit(&y, memory_order_seq_cst);
    // r1 == 0 表示 Thread2 的 x=1 还没看到
}

void thread2(void) {
    atomic_store_explicit(&y, 1, memory_order_seq_cst);
    int r2 = atomic_load_explicit(&x, memory_order_seq_cst);
    // r2 == 0 表示 Thread1 的 y=1 还没看到
}

// 不可能同时 r1 == 0 && r2 == 0（使用 SeqCst）
// 但可能使用 Relaxed!
