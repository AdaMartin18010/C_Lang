/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 518
 * Language: c
 * Block ID: 2793a93d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：认为volatile就是原子操作
volatile int counter = 0;

void increment_wrong(void) {
    counter++;  // 非原子！可能有数据竞争
}

// ✅ 正确：使用原子操作
atomic_int counter = 0;

void increment_correct(void) {
    atomic_fetch_add(&counter, 1);  // 真正的原子操作
}
