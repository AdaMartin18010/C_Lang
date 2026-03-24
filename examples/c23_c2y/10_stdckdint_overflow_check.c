/*
 * C23 <stdckdint.h> 溢出检查算术示例
 * 编译: gcc -std=c23 -o 10_stdckdint_overflow_check 10_stdckdint_overflow_check.c
 *       clang -std=c23 -o 10_stdckdint_overflow_check 10_stdckdint_overflow_check.c
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdckdint.h>  // C23新增头文件

int main(void) {
    printf("C23 stdckdint.h Overflow Checking Demo\n\n");
    
    // 基本的溢出检查加法
    int a = INT_MAX - 10;
    int b = 20;
    int sum;
    
    bool overflow = ckd_add(&sum, a, b);
    printf("ckd_add(%d, %d):\n", a, b);
    printf("  Result: %d\n", sum);
    printf("  Overflow: %s\n", overflow ? "YES" : "NO");
    
    // 无溢出的情况
    a = 100;
    b = 200;
    overflow = ckd_add(&sum, a, b);
    printf("\nckd_add(%d, %d):\n", a, b);
    printf("  Result: %d\n", sum);
    printf("  Overflow: %s\n", overflow ? "YES" : "NO");
    
    // 减法溢出检查
    int c = INT_MIN;
    int d = 1;
    int diff;
    
    overflow = ckd_sub(&diff, c, d);
    printf("\nckd_sub(INT_MIN, 1):\n");
    printf("  Result: %d\n", diff);
    printf("  Overflow: %s\n", overflow ? "YES" : "NO");
    
    // 乘法溢出检查
    int e = 100000;
    int f = 100000;
    int prod;
    
    overflow = ckd_mul(&prod, e, f);
    printf("\nckd_mul(100000, 100000):\n");
    printf("  Result: %d\n", prod);
    printf("  Overflow: %s\n", overflow ? "YES" : "NO");
    
    // 不同类型
    printf("\n--- Different Types ---\n");
    
    unsigned int ua = UINT_MAX;
    unsigned int ub = 1;
    unsigned int usum;
    
    overflow = ckd_add(&usum, ua, ub);
    printf("\nUnsigned ckd_add(UINT_MAX, 1):\n");
    printf("  Result: %u\n", usum);
    printf("  Overflow (wrap): %s\n", overflow ? "YES" : "NO");
    
    // 64位整数
    int64_t big_a = INT64_MAX - 100;
    int64_t big_b = 200;
    int64_t big_sum;
    
    overflow = ckd_add(&big_sum, big_a, big_b);
    printf("\n64-bit ckd_add(INT64_MAX - 100, 200):\n");
    printf("  Result: %lld\n", (long long)big_sum);
    printf("  Overflow: %s\n", overflow ? "YES" : "NO");
    
    // 实际应用场景：安全数组索引计算
    printf("\n--- Practical Example: Safe Array Access ---\n");
    
    size_t row = 1000;
    size_t col = 2000;
    size_t cols_per_row = 3000;
    size_t index;
    
    // 计算二维数组索引：row * cols_per_row + col
    if (ckd_mul(&index, row, cols_per_row) ||
        ckd_add(&index, index, col)) {
        printf("Array index calculation overflow!\n");
    } else {
        printf("Array index: %zu (safe)\n", index);
    }
    
    // 溢出情况
    row = SIZE_MAX;
    col = 1;
    if (ckd_add(&index, row, col)) {
        printf("Detected overflow in index calculation!\n");
    }
    
    // 安全计算宏示例
    #define safe_add(result, a, b) \
        do { if (ckd_add(&(result), (a), (b))) { \
            fprintf(stderr, "Overflow in addition\n"); \
            result = 0; \
        }} while(0)
    
    int safe_result;
    safe_add(safe_result, INT_MAX, 1);
    printf("\nSafe add result after overflow: %d\n", safe_result);
    
    printf("\nstdckdint demo complete!\n");
    
    return 0;
}
