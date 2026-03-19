/*
 * Auto-generated from: 12_Practice_Exercises\06_Performance_Optimization.md
 * Line: 383
 * Language: c
 * Block ID: 00626d12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 热函数内联
__attribute__((always_inline)) static inline int hot_function(int x);

// 冷函数（错误处理）
__attribute__((cold)) void error_handler(const char *msg);

// 纯函数（无副作用）
__attribute__((pure)) int square(int x);

// 常量函数
__attribute__((const)) int max(int a, int b);

// 热/冷标签
__attribute__((hot)) void frequently_called(void);
__attribute__((cold)) void rarely_called(void);

// 对齐
__attribute__((aligned(64))) float cache_line_aligned[16];

// 打包结构体
__attribute__((packed)) struct Compact {
    char a;
    int b;
    char c;
};
