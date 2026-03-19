/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\02_Preprocessor.md
 * Line: 97
 * Language: c
 * Block ID: e38aff64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 调试日志宏
#ifdef DEBUG
    #define LOG(fmt, ...) fprintf(stderr, "[%s:%d] " fmt "\n", \
                                  __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) ((void)0)
#endif

// ##__VA_ARGS__ 处理空参数情况
#define ERROR(msg, ...) printf("Error: " msg "\n", ##__VA_ARGS__)

// 使用
LOG("Value: %d, Name: %s", 42, "test");
ERROR("File not found");  // 无额外参数也OK

// 计数宏（用于自动参数计数）
define COUNT_ARGS(...) _COUNT_ARGS(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
define _COUNT_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N

// 自动选择重载（简化版）
define OVERLOAD(func, ...) CONCAT(func, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)
define CONCAT(a, b) a ## b
