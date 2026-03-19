/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 245
 * Language: c
 * Block ID: 3ff37e04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Acquire-Release 语义示例
#include <stdatomic.h>
#include <stdbool.h>

_Atomic bool flag = ATOMIC_VAR_INIT(false);
int data = 0;

// 生产者线程
void producer(void) {
    data = 42;
    // Release: 确保此操作之前的写入对其他线程可见
    atomic_store_explicit(&flag, true, memory_order_release);
}

// 消费者线程
void consumer(void) {
    // Acquire: 确保看到 flag=true 后，能看到 producer 的写入
    bool expected = true;
    while (!atomic_compare_exchange_weak_explicit(
        &flag, &expected, false,
        memory_order_acquire, memory_order_relaxed));

    // 现在可以安全读取 data
    int result = data;  // 保证看到 42
}
