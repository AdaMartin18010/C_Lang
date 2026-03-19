/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 233
 * Language: c
 * Block ID: a870bc83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整原子操作API
#include <stdatomic.h>

// 存储操作
void atomic_store(volatile A *obj, C desired);
void atomic_store_explicit(volatile A *obj, C desired, memory_order order);

// 加载操作
C atomic_load(volatile A *obj);
C atomic_load_explicit(volatile A *obj, memory_order order);

// 交换操作
C atomic_exchange(volatile A *obj, C desired);
C atomic_exchange_explicit(volatile A *obj, C desired, memory_order order);

// 比较并交换 (CAS)
bool atomic_compare_exchange_strong(volatile A *obj, C *expected, C desired);
bool atomic_compare_exchange_weak(volatile A *obj, C *expected, C desired);
bool atomic_compare_exchange_strong_explicit(
    volatile A *obj, C *expected, C desired,
    memory_order success, memory_order failure);

// 算术操作
C atomic_fetch_add(volatile A *obj, M operand);
C atomic_fetch_sub(volatile A *obj, M operand);
C atomic_fetch_or(volatile A *obj, M operand);
C atomic_fetch_xor(volatile A *obj, M operand);
C atomic_fetch_and(volatile A *obj, M operand);
