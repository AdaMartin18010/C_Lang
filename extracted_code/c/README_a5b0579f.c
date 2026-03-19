/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 665
 * Language: c
 * Block ID: a5b0579f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 断言与调试宏
#include <assert.h>

#ifdef NDEBUG
    #define DEBUG_PRINT(fmt, ...) ((void)0)
#else
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG %s:%d] " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// 编译时断言
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)

// 运行时检查宏
#define RUNTIME_ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "Assertion failed: %s at %s:%d\n", \
                #cond, __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

// 可达性标记 (C23)
// [[unreachable]] 标记不可达代码路径
