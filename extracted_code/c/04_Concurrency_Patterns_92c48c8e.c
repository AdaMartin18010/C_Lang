/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 837
 * Language: c
 * Block ID: 92c48c8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

缓存行 (64字节):
┌────────────────────────────────────────────────────────────────┐
│ 变量A (线程1修改) │ 变量B (线程2修改) │ ...                    │
└────────────────────────────────────────────────────────────────┘
        │                    │
        ▼                    ▼
    CPU 0缓存             CPU 1缓存
    (修改A使B失效)        (修改B使A失效)

结果: 频繁的缓存同步，性能下降

解决方案: 填充到缓存行边界
struct PaddedCounter {
    atomic_int count;
    char padding[64 - sizeof(atomic_int)];  // 填充
};
