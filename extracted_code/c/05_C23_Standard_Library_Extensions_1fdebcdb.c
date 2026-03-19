/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\05_C23_Standard_Library_Extensions.md
 * Line: 2166
 * Language: c
 * Block ID: 1fdebcdb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 特性检测宏
#if __STDC_VERSION__ >= 202311L
    #include <stdbit.h>
    #include <stdckdint.h>
#else
    // 回退实现

    // stdbit.h 回退
    #ifdef __GNUC__
        #define stdc_popcount_ui32(x) __builtin_popcount(x)
        #define stdc_leading_zeros_ui32(x) __builtin_clz(x)
        #define stdc_trailing_zeros_ui32(x) __builtin_ctz(x)
    #elif defined(_MSC_VER)
        #include <intrin.h>
        #define stdc_popcount_ui32(x) __popcnt(x)
        #define stdc_leading_zeros_ui32(x) __lzcnt(x)
        #define stdc_trailing_zeros_ui32(x) _BitScanForward(NULL, x)
    #else
        // 软件实现
        static inline unsigned int popcount_soft(uint32_t x) {
            x = x - ((x >> 1) & 0x55555555);
            x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
            x = (x + (x >> 4)) & 0x0F0F0F0F;
            x = x + (x >> 8);
            x = x + (x >> 16);
            return x & 0x3F;
        }
        #define stdc_popcount_ui32(x) popcount_soft(x)
    #endif

    // stdckdint.h 回退
    static inline bool ckd_add_int(int *result, int a, int b) {
        if (b > 0 && a > INT_MAX - b) return true;
        if (b < 0 && a < INT_MIN - b) return true;
        *result = a + b;
        return false;
    }

    // memset_explicit 回退
    static inline void *memset_explicit(void *s, int c, size_t n) {
        volatile unsigned char *p = s;
        while (n--) *p++ = c;
        return s;
    }

    // strdup 回退
    static inline char *strdup(const char *s) {
        size_t len = strlen(s) + 1;
        char *copy = malloc(len);
        if (copy) memcpy(copy, s, len);
        return copy;
    }
#endif
