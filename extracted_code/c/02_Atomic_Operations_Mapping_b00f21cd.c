/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 278
 * Language: c
 * Block ID: b00f21cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 原子存储
_Atomic uint64_t value = ATOMIC_VAR_INIT(0);

void store_examples(void) {
    atomic_store(&value, 42);
    atomic_store_explicit(&value, 42, memory_order_relaxed);
    atomic_store_explicit(&value, 42, memory_order_release);
}
