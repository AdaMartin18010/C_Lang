/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 279
 * Language: c
 * Block ID: 5aa13894
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11内存模型
#include <stdatomic.h>

atomic_int flag = ATOMIC_VAR_INIT(0);
int data = 0;

// 线程A
void producer() {
    data = 42;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

// 线程B
void consumer() {
    while (atomic_load_explicit(&flag, memory_order_acquire) == 0);
    // 保证看到 data = 42
    assert(data == 42);
}
