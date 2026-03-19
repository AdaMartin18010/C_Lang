/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\05_Concurrency_Knowledge_Graph.md
 * Line: 319
 * Language: c
 * Block ID: c6bececb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Acquire-Release 配对（生产者-消费者）
_Atomic(int) flag = 0;
int data = 0;

// 生产者线程
void producer() {
    data = 42;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

// 消费者线程
void consumer() {
    while (atomic_load_explicit(&flag, memory_order_acquire) == 0);
    // 保证看到 data = 42
    assert(data == 42);
}
