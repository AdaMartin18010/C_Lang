/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\03_Performance_Optimization.md
 * Line: 224
 * Language: c
 * Block ID: ff134e55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 多线程修改同一缓存行
typedef struct {
    int counter;
} Counter;

Counter counters[8];  // 每个64字节？不一定！

// ✅ 填充到缓存行大小
typedef struct {
    int counter;
    char pad[60];  // 64 - sizeof(int)
} PaddedCounter;
