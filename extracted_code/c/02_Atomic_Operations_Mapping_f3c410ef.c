/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 437
 * Language: c
 * Block ID: f3c410ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Relaxed 内存序
_Atomic uint64_t counter = ATOMIC_VAR_INIT(0);

void relaxed_example(void) {
    // 纯计数器，不需要同步
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
}
