/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 175
 * Language: c
 * Block ID: 0b9419ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// SDS (Simple Dynamic String) - Redis 的字符串实现
struct __attribute__ ((__packed__)) sdshdr64 {
    uint64_t len;        // 已使用长度
    uint64_t alloc;      // 分配的总长度
    unsigned char flags; // 标志位
    char buf[];          // 柔性数组存储实际数据
};

// 优势分析：
// 1. O(1) 时间获取长度
// 2. 预分配减少内存重分配
// 3. 二进制安全
// 4. 兼容 C 字符串函数
