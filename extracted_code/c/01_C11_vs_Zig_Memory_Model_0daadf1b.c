/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 929
 * Language: c
 * Block ID: 0daadf1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 数据竞争示例
#include <threads.h>
#include <stdio.h>

int shared = 0;  // 非原子变量

int thread_func(void* arg) {
    // 与 main 线程的读取构成数据竞争
    shared = 42;  // 写操作
    return 0;
}

int main(void) {
    thrd_t t;
    thrd_create(&t, thread_func, NULL);

    // 与 thread_func 的写入构成数据竞争
    int value = shared;  // 读操作 - 未定义行为！

    thrd_join(t, NULL);
    printf("value: %d\n", value);  // 可能输出任意值，或崩溃
    return 0;
}

// 正确版本 - 使用原子变量
#include <stdatomic.h>

_Atomic int shared_atomic = ATOMIC_VAR_INIT(0);

int thread_func_safe(void* arg) {
    atomic_store_explicit(&shared_atomic, 42, memory_order_release);
    return 0;
}

int main_safe(void) {
    thrd_t t;
    thrd_create(&t, thread_func_safe, NULL);

    // 使用 acquire 读取，与 release 写入配对
    int value = atomic_load_explicit(&shared_atomic, memory_order_acquire);

    thrd_join(t, NULL);
    printf("value: %d\n", value);  // 保证输出 42
    return 0;
}
