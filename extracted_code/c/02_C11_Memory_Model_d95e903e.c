/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 540
 * Language: c
 * Block ID: d95e903e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：使用relaxed进行同步
_Atomic int data = 0;
_Atomic int flag = 0;

void producer_bad(void) {
    data = 42;
    atomic_store_explicit(&flag, 1, memory_order_relaxed);  // 错误！
}

void consumer_bad(void) {
    while (atomic_load_explicit(&flag, memory_order_relaxed) == 0) {}
    int x = data;  // 可能看不到42！
}

// ✅ 正确：使用release/acquire
void producer_good(void) {
    data = 42;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

void consumer_good(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) == 0) {}
    int x = data;  // 保证看到42
}
