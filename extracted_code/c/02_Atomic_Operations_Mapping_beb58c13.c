/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 458
 * Language: c
 * Block ID: beb58c13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 Acquire-Release
int data = 0;
_Atomic bool flag = ATOMIC_VAR_INIT(false);

void producer(void) {
    data = 42;
    atomic_store_explicit(&flag, true, memory_order_release);
}

void consumer(void) {
    while (!atomic_load_explicit(&flag, memory_order_acquire));
    int value = data;  // 保证看到 42
}
