/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 495
 * Language: c
 * Block ID: eaf434f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Sequential Consistency
_Atomic int x = ATOMIC_VAR_INIT(0);
_Atomic int y = ATOMIC_VAR_INIT(0);

void thread1(void) {
    atomic_store_explicit(&x, 1, memory_order_seq_cst);
    int r1 = atomic_load_explicit(&y, memory_order_seq_cst);
    (void)r1;
}

void thread2(void) {
    atomic_store_explicit(&y, 1, memory_order_seq_cst);
    int r2 = atomic_load_explicit(&x, memory_order_seq_cst);
    (void)r2;
}
// 不可能 r1 == 0 && r2 == 0
