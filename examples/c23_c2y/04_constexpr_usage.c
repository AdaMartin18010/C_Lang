/*
 * C23 constexpr 关键字示例
 * 编译: gcc -std=c23 -o 04_constexpr_usage 04_constexpr_usage.c
 *       clang -std=c23 -o 04_constexpr_usage 04_constexpr_usage.c
 */

#include <stdio.h>
#include <math.h>

// constexpr 变量 - 编译期常量
constexpr int max_buffer_size = 1024;
constexpr double pi = 3.14159265359;
constexpr char greeting[] = "Hello, C23!";

// constexpr 与枚举对比
enum { OLD_STYLE_MAX = 100 };
constexpr int new_style_max = 100;  // 类型安全，有作用域

// constexpr 数组
constexpr int fibonacci[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

// constexpr 结构体
struct Point {
    int x;
    int y;
};

constexpr struct Point origin = {0, 0};
constexpr struct Point unit_x = {1, 0};

// 在编译期计算
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int square(int x) {
    return x * x;
}

int main(void) {
    // 使用 constexpr 变量
    printf("constexpr variables:\n");
    printf("max_buffer_size = %d\n", max_buffer_size);
    printf("pi = %f\n", pi);
    printf("greeting = %s\n", greeting);
    
    // 编译期计算的数组大小
    int buffer[max_buffer_size];  // 使用 constexpr 作为数组大小
    printf("\nArray size: %zu\n", sizeof(buffer) / sizeof(buffer[0]));
    
    // 使用 constexpr 数组
    printf("\nFibonacci sequence:\n");
    for (size_t i = 0; i < sizeof(fibonacci) / sizeof(fibonacci[0]); i++) {
        printf("%d ", fibonacci[i]);
    }
    printf("\n");
    
    // 使用 constexpr 结构体
    printf("\nconstexpr struct:\n");
    printf("origin: (%d, %d)\n", origin.x, origin.y);
    printf("unit_x: (%d, %d)\n", unit_x.x, unit_x.y);
    
    // 编译期计算
    printf("\nCompile-time calculations:\n");
    printf("factorial(5) = %d\n", factorial(5));
    printf("square(10) = %d\n", square(10));
    
    // 用于静态断言
    static_assert(max_buffer_size > 0);
    static_assert(sizeof(fibonacci) > 0);
    
    // constexpr 与宏的对比
    printf("\nconstexpr vs macro:\n");
    printf("OLD_STYLE_MAX = %d (no type)\n", OLD_STYLE_MAX);
    printf("new_style_max = %d (int type)\n", new_style_max);
    printf("sizeof(OLD_STYLE_MAX) = %zu\n", sizeof(OLD_STYLE_MAX));  // 通常是 sizeof(int)
    printf("sizeof(new_style_max) = %zu\n", sizeof(new_style_max));  // sizeof(int)
    
    return 0;
}
