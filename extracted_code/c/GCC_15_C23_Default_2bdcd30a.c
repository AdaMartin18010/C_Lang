/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 989
 * Language: c
 * Block ID: 2bdcd30a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 为什么使用 sinpi 而不是 sin(M_PI * x)? */

#include <math.h>
#include <stdio.h>

void compare_precision(void) {
    /* 问题: M_PI 是近似值 */
    double x = 1.0;

    /* 传统方法: 使用近似的 π */
    double result1 = sin(M_PI * x);
    printf("sin(M_PI * 1.0) = %.17g\n", result1);
    /* 输出可能不是精确的 0，因为 M_PI 是近似值 */

    /* C23 方法: sinpi 直接计算 sin(πx) */
    double result2 = sinpi(x);
    printf("sinpi(1.0) = %.17g\n", result2);
    /* 输出应该是精确的 0 */

    /* 另一个例子: x = 0.5 */
    double result3 = sin(M_PI * 0.5);
    double result4 = sinpi(0.5);

    printf("sin(M_PI * 0.5) = %.17g\n", result3);
    printf("sinpi(0.5) = %.17g\n", result4);
}

/* 应用场景: 信号处理 */
void signal_processing_example(void) {
    const int N = 1024;
    const double f = 0.1;  /* 频率 */

    for (int n = 0; n < N; n++) {
        /* 生成正弦波: sin(2πfn) */
        /* 传统: sin(2 * M_PI * f * n) */
        /* C23: sinpi(2 * f * n) - 更精确 */
        double sample = sinpi(2 * f * n);
        (void)sample;  /* 使用 sample */
    }
}

int main(void) {
    compare_precision();
    signal_processing_example();
    return 0;
}
