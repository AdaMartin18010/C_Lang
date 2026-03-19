/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 756
 * Language: c
 * Block ID: 3b55a6a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 同步规则演示
#include <stdatomic.h>
#include <stdbool.h>

_Atomic int flag = ATOMIC_VAR_INIT(0);
int data1 = 0, data2 = 0;

// 规则 1: Release-Acquire 配对
void thread1_rule1(void) {
    data1 = 1;
    data2 = 2;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

void thread2_rule1(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1);
    // 保证看到 data1=1, data2=2
    assert(data1 == 1);
    assert(data2 == 2);
}

// 规则 2: 传递性
_Atomic int flag2 = ATOMIC_VAR_INIT(0);

void thread1_rule2(void) {
    data1 = 1;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

void thread2_rule2(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1);
    data2 = 2;
    atomic_store_explicit(&flag2, 1, memory_order_release);
}

void thread3_rule2(void) {
    while (atomic_load_explicit(&flag2, memory_order_acquire) != 1);
    // 通过传递性，保证看到 data1=1, data2=2
    assert(data1 == 1);
    assert(data2 == 2);
}

// 规则 3: SeqCst 全序
_Atomic int x = ATOMIC_VAR_INIT(0);
_Atomic int y = ATOMIC_VAR_INIT(0);

void thread1_rule3(void) {
    atomic_store_explicit(&x, 1, memory_order_seq_cst);
    int r1 = atomic_load_explicit(&y, memory_order_seq_cst);
}

void thread2_rule3(void) {
    atomic_store_explicit(&y, 1, memory_order_seq_cst);
    int r2 = atomic_load_explicit(&x, memory_order_seq_cst);
}
// 不可能 r1=0 && r2=0（使用 SeqCst）
