/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 2132
 * Language: c
 * Block ID: 24de02a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 快速参考
#include <stdatomic.h>

// 声明
_Atomic int a = ATOMIC_VAR_INIT(0);

// 基本操作
int v = atomic_load(&a);
atomic_store(&a, 42);
int old = atomic_exchange(&a, 100);
int prev = atomic_fetch_add(&a, 1);

// 显式内存序
atomic_store_explicit(&a, 1, memory_order_release);
int x = atomic_load_explicit(&a, memory_order_acquire);

// CAS
int expected = 0;
bool ok = atomic_compare_exchange_strong(&a, &expected, 1);

// 栅栏
atomic_thread_fence(memory_order_seq_cst);
