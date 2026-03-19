/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\04_High_Performance_Computing.md
 * Line: 326
 * Language: c
 * Block ID: f6129064
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 多线程修改同一缓存行的不同变量
typedef struct {
    int counter;
} Counter;

Counter counters[8];  // 可能位于同一缓存行

// ✅ 填充到缓存行大小
typedef struct {
    int counter;
    char pad[60];  // 64 - sizeof(int)
} PaddedCounter;

PaddedCounter safe_counters[8];
