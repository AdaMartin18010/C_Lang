/*
 * C23/C2y 兼容性宏示例
 * 编译: gcc -std=c23 -o 20_compatibility_macros 20_compatibility_macros.c
 *       clang -std=c23 -o 20_compatibility_macros 20_compatibility_macros.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * C23/C2y 兼容性头文件 (c23_compat.h)
 * 在实际项目中，这部分应该放在单独的头文件中
 * ============================================================ */

#ifndef C23_COMPAT_H
#define C23_COMPAT_H

/* 检测C标准版本 */
#ifndef __STDC_VERSION__
    #define __STDC_VERSION__ 199901L
#endif

#define C23_STDC_VERSION 202311L
#define C2Y_STDC_VERSION 202400L

#define IS_C23 (__STDC_VERSION__ >= C23_STDC_VERSION)
#define IS_C2Y (__STDC_VERSION__ >= C2Y_STDC_VERSION)

/* nullptr 兼容性 */
#if IS_C23
    #define C23_NULLPTR nullptr
#else
    #define C23_NULLPTR NULL
#endif

/* typeof 兼容性 */
#if defined(__GNUC__) || defined(__clang__)
    #define C23_TYPEOF(expr) __typeof__(expr)
#elif IS_C23
    #define C23_TYPEOF(expr) typeof(expr)
#else
    #error "typeof not supported"
#endif

/* auto 兼容性 */
#if IS_C23
    #define C23_AUTO auto
#else
    #define C23_AUTO /* not supported */
#endif

/* static_assert 兼容性 */
#if IS_C23
    #define C23_STATIC_ASSERT(cond, ...) static_assert(cond, ##__VA_ARGS__)
#else
    #define C23_STATIC_ASSERT(cond, ...) _Static_assert(cond, ##__VA_ARGS__)
#endif

/* 属性兼容性 */
#if IS_C23
    #define C23_DEPRECATED [[deprecated]]
    #define C23_NODISCARD [[nodiscard]]
    #define C23_MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__)
    #define C23_DEPRECATED __attribute__((deprecated))
    #define C23_NODISCARD __attribute__((warn_unused_result))
    #define C23_MAYBE_UNUSED __attribute__((unused))
#else
    #define C23_DEPRECATED
    #define C23_NODISCARD
    #define C23_MAYBE_UNUSED
#endif

/* _Countof 兼容性 */
#if IS_C2Y
    #define C23_COUNTOF(arr) _Countof(arr)
#else
    #define C23_COUNTOF(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* defer 兼容性 (简化版) */
#if IS_C2Y
    #define C23_DEFER(stmt) defer stmt
#else
    /* 不支持 defer 时提供警告 */
    #define C23_DEFER(stmt) \
        _Pragma("message \"defer not supported, manual cleanup required\"")
#endif

#endif /* C23_COMPAT_H */

/* ============================================================
 * 示例代码
 * ============================================================ */

// 使用兼容性宏
C23_DEPRECATED
void old_function(void) {
    printf("This is an old function\n");
}

C23_NODISCARD
int* allocate_int(void) {
    return malloc(sizeof(int));
}

// 泛型宏（使用兼容性 typeof）
#define c23_max(a, b) ({              \
    C23_TYPEOF(a) _a = (a);           \
    C23_TYPEOF(b) _b = (b);           \
    _a > _b ? _a : _b;                \
})

// 类型推导示例（如果支持）
void type_inference_demo(void) {
#if IS_C23
    C23_AUTO x = 42;           // int
    C23_AUTO y = 3.14;         // double
    C23_AUTO s = "hello";      // const char*
    
    printf("auto x = %d (int)\n", x);
    printf("auto y = %f (double)\n", y);
    printf("auto s = %s (const char*)\n", s);
#else
    printf("auto type inference not available\n");
#endif
}

// 静态断言示例
void static_assert_demo(void) {
    C23_STATIC_ASSERT(sizeof(int) >= 4);
    C23_STATIC_ASSERT(CHAR_BIT == 8, "8-bit bytes required");
    
    printf("Static assertions passed\n");
}

// 数组计数示例
void countof_demo(void) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    printf("Array has %zu elements\n", (size_t)C23_COUNTOF(arr));
    
    // 使用计数遍历
    printf("Elements: ");
    for (size_t i = 0; i < C23_COUNTOF(arr); i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// 泛型交换
#define c23_swap(a, b) do {           \
    C23_TYPEOF(a) _tmp = (a);         \
    (a) = (b);                        \
    (b) = _tmp;                       \
} while(0)

void swap_demo(void) {
    int a = 10, b = 20;
    printf("Before swap: a=%d, b=%d\n", a, b);
    c23_swap(a, b);
    printf("After swap: a=%d, b=%d\n", a, b);
    
    double x = 1.5, y = 2.5;
    printf("Before swap: x=%f, y=%f\n", x, y);
    c23_swap(x, y);
    printf("After swap: x=%f, y=%f\n", x, y);
}

// 平台检测
void platform_info(void) {
    printf("\n=== Platform Information ===\n");
    printf("__STDC_VERSION__: %ld\n", (long)__STDC_VERSION__);
    printf("C23 support: %s\n", IS_C23 ? "YES" : "NO");
    printf("C2y support: %s\n", IS_C2Y ? "YES" : "NO");
    
    #ifdef __GNUC__
    printf("Compiler: GCC %d.%d\n", __GNUC__, __GNUC_MINOR__);
    #endif
    
    #ifdef __clang__
    printf("Compiler: Clang %d.%d\n", __clang_major__, __clang_minor__);
    #endif
    
    #ifdef _MSC_VER
    printf("Compiler: MSVC %d\n", _MSC_VER);
    #endif
}

int main(void) {
    printf("C23/C2y Compatibility Macros Demo\n");
    printf("==================================\n\n");
    
    platform_info();
    
    printf("\n--- Type Inference ---\n");
    type_inference_demo();
    
    printf("\n--- Static Assertions ---\n");
    static_assert_demo();
    
    printf("\n--- Generic max ---\n");
    printf("max(10, 20) = %d\n", c23_max(10, 20));
    printf("max(3.14, 2.71) = %f\n", c23_max(3.14, 2.71));
    
    printf("\n--- Array Count ---\n");
    countof_demo();
    
    printf("\n--- Generic Swap ---\n");
    swap_demo();
    
    printf("\nCompatibility macros demo complete!\n");
    
    return 0;
}
