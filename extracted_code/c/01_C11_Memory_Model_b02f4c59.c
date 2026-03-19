/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 270
 * Language: c
 * Block ID: b02f4c59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 初始化 */
_Atomic(int) counter = ATOMIC_VAR_INIT(0);
atomic_int counter2 = ATOMIC_VAR_INIT(0);

/* C23中更简单 */
_Atomic int counter3 = 0;  /* 直接初始化 */

/* 加载和存储 */
int value = atomic_load(&counter);
atomic_store(&counter, 42);

/* 显式内存序 */
int val = atomic_load_explicit(&counter, memory_order_acquire);
atomic_store_explicit(&counter, 42, memory_order_release);

/* 交换 */
int old = atomic_exchange(&counter, 100);

/* 比较交换 (CAS) - 无锁编程核心 */
int expected = 0;
bool success = atomic_compare_exchange_strong(
    &counter,           /* 原子变量 */
    &expected,          /* 预期值 (失败时更新为当前值) */
    1                   /* 新值 */
);
/* 返回true表示交换成功，false表示失败 */

/* 弱CAS (可能伪失败，但更高效) */
while (!atomic_compare_exchange_weak(&counter, &expected, 1)) {
    expected = 0;  /* 重新设置预期值 */
}
