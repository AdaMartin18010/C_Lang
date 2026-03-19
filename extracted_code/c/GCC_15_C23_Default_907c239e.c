/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 300
 * Language: c
 * Block ID: 907c239e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 在代码中检测编译器和标准版本 */
#include <stdio.h>

/* 检测 GCC 版本 */
#ifdef __GNUC__
    #define GCC_VERSION (__GNUC__ * 10000 + \
                         __GNUC_MINOR__ * 100 + \
                         __GNUC_PATCHLEVEL__)
    #define GCC_VERSION_SINCE(major, minor, patch) \
        (GCC_VERSION >= ((major) * 10000 + (minor) * 100 + (patch)))
#else
    #define GCC_VERSION 0
    #define GCC_VERSION_SINCE(major, minor, patch) 0
#endif

/* 检测 C 标准版本 */
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 202311L
        #define C_STANDARD 23
        #define C23_SUPPORTED 1
        #define C17_SUPPORTED 1
        #define C11_SUPPORTED 1
        #define C99_SUPPORTED 1
    #elif __STDC_VERSION__ >= 201710L
        #define C_STANDARD 17
        #define C23_SUPPORTED 0
        #define C17_SUPPORTED 1
        #define C11_SUPPORTED 1
        #define C99_SUPPORTED 1
    #elif __STDC_VERSION__ >= 201112L
        #define C_STANDARD 11
        #define C23_SUPPORTED 0
        #define C17_SUPPORTED 0
        #define C11_SUPPORTED 1
        #define C99_SUPPORTED 1
    #elif __STDC_VERSION__ >= 199901L
        #define C_STANDARD 99
        #define C23_SUPPORTED 0
        #define C17_SUPPORTED 0
        #define C11_SUPPORTED 0
        #define C99_SUPPORTED 1
    #else
        #define C_STANDARD 90
    #endif
#else
    #define C_STANDARD 89
#endif

/* 功能适配宏 */
#if C23_SUPPORTED
    /* C23: 原生支持 */
    #include <stdbool.h>
    #define NODISCARD [[nodiscard]]
    #define MAYBE_UNUSED [[maybe_unused]]
    #define FALLTHROUGH [[fallthrough]]
#elif defined(__has_c_attribute)
    /* 可能有属性支持 */
    #if __has_c_attribute(nodiscard)
        #define NODISCARD [[nodiscard]]
    #else
        #define NODISCARD
    #endif
#else
    /* 旧版本: 使用 GNU 扩展或空定义 */
    #define NODISCARD __attribute__((warn_unused_result))
    #define MAYBE_UNUSED __attribute__((unused))
    #define FALLTHROUGH __attribute__((fallthrough))
#endif

/* nullptr 适配 */
#if C23_SUPPORTED
    /* C23: 原生 nullptr */
#elif defined(__cplusplus)
    /* C++: 使用其 nullptr */
#else
    /* C99/C11: 模拟 nullptr */
    #define nullptr ((void*)0)
#endif

/* static_assert 适配 */
#if C23_SUPPORTED || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
    /* C11+ 或 C23: 直接使用 */
#elif defined(__GNUC__)
    /* GNU C: 使用 _Static_assert */
    #define static_assert _Static_assert
#else
    /* 无静态断言支持 */
    #define static_assert(cond, msg) ((void)0)
#endif

NODISCARD int important_function(void) {
    return 42;
}

int main(void) {
    printf("C Standard: C%d\n", C_STANDARD);
    printf("GCC Version: %d\n", GCC_VERSION);
    printf("C23 Support: %s\n", C23_SUPPORTED ? "Yes" : "No");

    /* 使用适配后的 static_assert */
    static_assert(sizeof(int) >= 4, "int must be at least 32 bits");

    /* 注意: 忽略返回值会触发警告 */
    important_function();  /* 警告: 返回值被忽略 */

    return 0;
}
