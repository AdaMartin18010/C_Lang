/*
 * Auto-generated from: 00_VERSION_TRACKING\C23_to_C2y_Roadmap.md
 * Line: 395
 * Language: c
 * Block ID: 03becb96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// multi_version.h - 多版本C标准兼容头
#ifndef MULTI_VERSION_H
#define MULTI_VERSION_H

// 检测C标准版本
#if __STDC_VERSION__ >= 202311L
    #define C23 1
#elif __STDC_VERSION__ >= 201112L
    #define C11 1
#elif __STDC_VERSION__ >= 199901L
    #define C99 1
#else
    #define C89 1
#endif

// nullptr兼容性
#ifndef C23
    #if defined(__cplusplus)
        // C++已有nullptr
    #elif defined(__GNUC__) || defined(__clang__)
        #define nullptr ((void*)0)
    #else
        #define nullptr NULL
    #endif
#endif

// constexpr兼容性
#ifndef C23
    #define constexpr const
#endif

// typeof兼容性
#ifndef C23
    #if defined(__GNUC__) || defined(__clang__)
        #define typeof __typeof__
        #define typeof_unqual(T) __typeof__(T)
    #else
        #error "typeof not supported on this compiler"
    #endif
#endif

// 属性兼容性
#ifndef C23
    #if defined(__GNUC__) || defined(__clang__)
        #define [[nodiscard]] __attribute__((warn_unused_result))
        #define [[maybe_unused]] __attribute__((unused))
        #define [[deprecated]] __attribute__((deprecated))
        #define [[noreturn]] __attribute__((noreturn))
    #elif defined(_MSC_VER)
        #define [[nodiscard]] _Check_return_
        #define [[maybe_unused]]
        #define [[deprecated]] __declspec(deprecated)
        #define [[noreturn]] __declspec(noreturn)
    #else
        #define [[nodiscard]]
        #define [[maybe_unused]]
        #define [[deprecated]]
        #define [[noreturn]]
    #endif
#endif

// stdbit.h兼容性
#ifndef C23
    #include <stdint.h>

    #if defined(__GNUC__) || defined(__clang__)
        #define stdc_count_ones(x) __builtin_popcount(x)
        #define stdc_count_zeros(x) __builtin_clz(x)
        #define stdc_bit_width(x) (32 - __builtin_clz(x))
    #else
        // 通用实现
        static inline int stdc_count_ones(unsigned x) {
            int count = 0;
            while (x) { count++; x &= x - 1; }
            return count;
        }
        // ... 其他函数
    #endif
#endif

// stdckdint.h兼容性
#ifndef C23
    #include <stdbool.h>
    #include <limits.h>

    #define ckd_add(overflow, a, b) \
        ({ typeof(a) _a = (a), _b = (b); \
           *(overflow) = (_b > 0 && _a > INT_MAX - _b) || \
                        (_b < 0 && _a < INT_MIN - _b); \
           _a + _b; })
    // ... 其他函数
#endif

#endif // MULTI_VERSION_H
