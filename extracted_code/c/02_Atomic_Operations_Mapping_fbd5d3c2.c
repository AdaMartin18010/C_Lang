/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 248
 * Language: c
 * Block ID: fbd5d3c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 原子加载
_Atomic uint64_t value = ATOMIC_VAR_INIT(0);

uint64_t load_examples(void) {
    // 默认 SeqCst 加载
    uint64_t v1 = atomic_load(&value);

    // 显式内存序
    uint64_t v2 = atomic_load_explicit(&value, memory_order_relaxed);
    uint64_t v3 = atomic_load_explicit(&value, memory_order_acquire);

    return v1 + v2 + v3;
}
