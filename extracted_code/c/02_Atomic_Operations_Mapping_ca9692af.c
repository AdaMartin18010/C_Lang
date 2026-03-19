/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 657
 * Language: c
 * Block ID: ca9692af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 线程栅栏
#include <stdatomic.h>

void fence_examples(void) {
    atomic_thread_fence(memory_order_acquire);
    atomic_thread_fence(memory_order_release);
    atomic_thread_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_seq_cst);
}

// Release-Acquire 栅栏配对
int data = 0;
_Atomic int flag = ATOMIC_VAR_INIT(0);

void producer_with_fence(void) {
    data = 42;
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&flag, 1, memory_order_relaxed);
}

void consumer_with_fence(void) {
    while (atomic_load_explicit(&flag, memory_order_relaxed) != 1);
    atomic_thread_fence(memory_order_acquire);
    int value = data;  // 保证看到 42
}
