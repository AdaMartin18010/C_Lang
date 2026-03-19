/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\03_Lockless_Ring_Buffer_SPSC_MPMC.md
 * Line: 355
 * Language: c
 * Block ID: 158c4a72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ head和tail在同一缓存行，导致缓存行弹跳
struct {
    _Atomic uint64_t head;
    _Atomic uint64_t tail;  // 与head同缓存行！
};

// ✅ 使用填充分离
struct {
    _Atomic uint64_t head;
    char _pad[CACHE_LINE_SIZE - sizeof(uint64_t)];
    _Atomic uint64_t tail;
    char _pad2[CACHE_LINE_SIZE - sizeof(uint64_t)];
};
