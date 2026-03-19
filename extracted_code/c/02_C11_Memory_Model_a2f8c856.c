/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 150
 * Language: c
 * Block ID: a2f8c856
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 经典的生产者-消费者模式
#include <stdatomic.h>
#include <stdbool.h>

_Atomic int data = 0;
_Atomic bool ready = false;

// 生产者线程
void producer(void) {
    data = 42;  // 普通写
    // release: 保证之前的所有写操作对acquire可见
    atomic_store_explicit(&ready, true, memory_order_release);
}

// 消费者线程
void consumer(void) {
    // acquire: 保证看到release之前的所有写
    while (!atomic_load_explicit(&ready, memory_order_acquire)) {
        // 等待
    }
    // 此时一定能看到 data == 42
    int value = data;
    (void)value;
}
