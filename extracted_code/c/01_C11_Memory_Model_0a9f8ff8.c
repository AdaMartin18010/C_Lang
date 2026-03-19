/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 127
 * Language: c
 * Block ID: 0a9f8ff8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 最常用的内存序组合，用于线程间传递数据 */

_Atomic(int*) shared_ptr = NULL;
int data = 0;  /* 非原子数据 */

/* Thread 1: 生产者 */
void produce(void) {
    data = 42;  /* 准备数据 */

    /* Release: 保证之前的写入对获取方可见 */
    atomic_store_explicit(&shared_ptr, &data, memory_order_release);
}

/* Thread 2: 消费者 */
void consume(void) {
    int* ptr;

    /* Acquire: 保证看到发布方的所有写入 */
    while ((ptr = atomic_load_explicit(&shared_ptr,
                                        memory_order_acquire)) == NULL) {
        /* 自旋等待 */
    }

    /* 这里可以安全访问*ptr，保证看到data=42 */
    printf("Got: %d\n", *ptr);
}

/* Acquire-Release 语义:
 * Release: 之前的所有读写操作不会重排到后面
 * Acquire: 之后的所有读写操作不会重排到前面
 * 形成同步点
 */
