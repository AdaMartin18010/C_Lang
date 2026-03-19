/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 1070
 * Language: c
 * Block ID: dce2a1a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 基本原子操作完整示例
#include <stdatomic.h>
#include <stdio.h>
#include <stdint.h>

// 声明原子变量
_Atomic int32_t counter = ATOMIC_VAR_INIT(0);
_Atomic uint64_t big_counter = ATOMIC_VAR_INIT(0);
_Atomic bool flag = ATOMIC_VAR_INIT(false);
_Atomic(void*) ptr = ATOMIC_VAR_INIT(NULL);

void basic_operations(void) {
    // 加载和存储
    int32_t val = atomic_load(&counter);
    atomic_store(&counter, 100);

    // 显式内存序
    atomic_store_explicit(&counter, 200, memory_order_release);
    val = atomic_load_explicit(&counter, memory_order_acquire);

    // 获取并修改
    int32_t old = atomic_fetch_add(&counter, 1);  // ++counter
    old = atomic_fetch_sub(&counter, 1);           // --counter
    old = atomic_fetch_and(&counter, 0xFF);        // counter &= 0xFF
    old = atomic_fetch_or(&counter, 0x10);         // counter |= 0x10
    old = atomic_fetch_xor(&counter, 0x01);        // counter ^= 0x01

    // 交换
    int32_t prev = atomic_exchange(&counter, 999);

    // 比较交换 (CAS)
    int32_t expected = 999;
    bool success = atomic_compare_exchange_strong(
        &counter, &expected, 1000
    );
    // 成功: counter = 1000, success = true
    // 失败: expected = 当前值, success = false

    // 弱 CAS（允许伪失败，但更快）
    expected = 1000;
    success = atomic_compare_exchange_weak(
        &counter, &expected, 2000
    );
}

// 不同内存序的 CAS
void cas_with_order(void) {
    int32_t exp = 0;
    atomic_compare_exchange_strong_explicit(
        &counter,
        &exp,
        1,
        memory_order_acq_rel,   // 成功时内存序
        memory_order_relaxed    // 失败时内存序
    );
}
