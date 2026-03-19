/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
 * Line: 61
 * Language: c
 * Block ID: e08de35b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* c23_features.c - C23新特性演示 */
#if __STDC_VERSION__ < 202311L
    #error "This code requires C23 standard"
#endif

#include <stdio.h>
#include <stdbit.h>
#include <stdckdint.h>

/* C23: constexpr - 编译期常量 */
constexpr int MAX_BUFFER_SIZE = 4096;

/* C23: typeof - 类型推导 */
typedef typeof(int) my_int_t;

/* C23: auto 关键字 */
void demo_auto(void) {
    auto x = 42;
    auto y = 3.14;
    printf("auto types: x=%d, y=%f\n", x, y);
}

/* C23: 属性标准化 */
[[nodiscard]] int important_function(void) {
    return 42;
}

[[noreturn]] void fatal_error(const char *msg) {
    fprintf(stderr, "FATAL: %s\n", msg);
    __builtin_trap();
}

/* C23: 枚举基础类型 */
enum Color : uint8_t {
    COLOR_RED, COLOR_GREEN, COLOR_BLUE
};

/* C23: stdbit.h 位操作 */
void demo_stdbit(void) {
    unsigned int x = 0b10110011;
    printf("popcount(%u) = %d\n", x, stdc_count_ones_uint(x));
    printf("bit_width(%u) = %d\n", x, stdc_bit_width_uint(x));
}

/* C23: stdckdint.h 溢出检查 */
void demo_overflow_check(void) {
    int a = INT_MAX;
    int b = 1;
    int result;
    bool overflow = ckd_add(&result, a, b);
    printf("overflow = %d\n", overflow);
}

/* C23: 空指针nullptr */
void demo_nullptr(void) {
    int *p = nullptr;
    if (p == nullptr) {
        printf("Pointer is null\n");
    }
}

int main(void) {
    demo_auto();
    demo_stdbit();
    demo_overflow_check();
    demo_nullptr();
    return 0;
}
