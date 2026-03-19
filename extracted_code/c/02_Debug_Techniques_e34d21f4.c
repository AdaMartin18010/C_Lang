/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\02_Debug_Techniques.md
 * Line: 251
 * Language: c
 * Block ID: e34d21f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 增强断言

#ifdef NDEBUG
    #define ASSERT(cond) ((void)0)
#else
    #define ASSERT(cond) do { \
        if (!(cond)) { \
            fprintf(stderr, "Assertion failed: %s\n", #cond); \
            fprintf(stderr, "  at %s:%d (%s)\n", __FILE__, __LINE__, __func__); \
            __builtin_trap(); \
        } \
    } while(0)
#endif

// 静态断言
#define STATIC_ASSERT(cond) _Static_assert(cond, #cond)

// 编译期断言数组大小
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define ASSERT_ARRAY_SIZE(arr, expected) \
    STATIC_ASSERT(ARRAY_SIZE(arr) == expected)

// 调试打印
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] %s:%d " fmt "\n", \
                                          __FILE__, __LINE__, ##__VA_ARGS__)
    #define DEBUG_DUMP(ptr, size) dump_hex(ptr, size)
#else
    #define DEBUG_PRINT(...) ((void)0)
    #define DEBUG_DUMP(...) ((void)0)
#endif
