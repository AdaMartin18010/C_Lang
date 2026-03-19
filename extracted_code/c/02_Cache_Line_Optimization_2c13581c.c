/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\02_Cache_Line_Optimization.md
 * Line: 420
 * Language: c
 * Block ID: 2c13581c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误: 可能跨越缓存行边界
struct __attribute__((packed)) BadLayout {
    char a;
    int b;  // 可能跨越两个缓存行！
    char c;
};

// 正确: 自然对齐
struct GoodLayout {
    int b;   // 4字节
    char a;  // 1字节
    char c;  // 1字节
    // 2字节填充
};
