/*
 * C2y typeof(return) 示例
 * 编译: gcc -std=c2y -o 23_typeof_return 23_typeof_return.c
 *       clang -std=c2y -o 23_typeof_return 23_typeof_return.c
 * 注意: typeof(return) 是 C2y 提案特性 (N3454)，允许在函数体内推导返回类型
 */

#include <stdio.h>
#include <stdint.h>

/* 使用 typeof(return) 声明与返回类型匹配的变量 */
int64_t compute_factorial(int n) {
    typeof(return) result = 1;  /* typeof(return) == int64_t */

    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

double normalize_value(double x, double min, double max) {
    typeof(return) range = max - min;  /* typeof(return) == double */

    if (range == 0.0) {
        return 0.0;
    }
    return (x - min) / range;
}

/* 宏中使用 typeof(return) 创建泛型辅助 */
#define DECLARE_RETURN_VAR(name, init) \
    typeof(return) name = (init)

unsigned int fibonacci(unsigned int n) {
    DECLARE_RETURN_VAR(a, 0);  /* unsigned int a = 0 */
    DECLARE_RETURN_VAR(b, 1);  /* unsigned int b = 1 */

    for (unsigned int i = 0; i < n; i++) {
        unsigned int temp = a + b;
        a = b;
        b = temp;
    }
    return a;
}

int main(void) {
    printf("typeof(return) in C2y (N3454)\n");
    printf("=============================\n");

    printf("factorial(10) = %lld\n", compute_factorial(10));
    printf("normalize(75, 0, 100) = %f\n", normalize_value(75.0, 0.0, 100.0));
    printf("fibonacci(10) = %u\n", fibonacci(10));

    return 0;
}
