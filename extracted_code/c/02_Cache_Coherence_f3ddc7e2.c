/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 793
 * Language: c
 * Block ID: f3ddc7e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 多个变量可能在同一缓存行
struct shared_data {
    atomic_t counter1;  // CPU0频繁更新
    atomic_t counter2;  // CPU1频繁更新
    atomic_t counter3;  // CPU2频繁更新
};

// ✅ 正确: 确保每个变量独占缓存行
struct aligned_data {
    atomic_t counter1;
    char pad1[60];  // 填充到64字节

    atomic_t counter2;
    char pad2[60];

    atomic_t counter3;
    char pad3[60];
} __attribute__((aligned(64)));
