/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 935
 * Language: c
 * Block ID: d60cf89c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 RCU 简化版
typedef struct {
    _Atomic(void*) data;
    _Atomic int readers;
} RCU;

void* rcu_read_lock(RCU* rcu) {
    atomic_fetch_add_explicit(&rcu->readers, 1, memory_order_acquire);
    return atomic_load_explicit(&rcu->data, memory_order_consume);
}

void rcu_read_unlock(RCU* rcu) {
    atomic_fetch_sub_explicit(&rcu->readers, 1, memory_order_release);
}

void rcu_synchronize(RCU* rcu) {
    while (atomic_load_explicit(&rcu->readers, memory_order_acquire) > 0);
}
