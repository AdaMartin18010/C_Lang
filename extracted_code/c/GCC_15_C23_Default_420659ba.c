/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 623
 * Language: c
 * Block ID: 420659ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* glibc 2.42 数学优化示例 */
#include <math.h>
#include <stdio.h>

/* 新增的高精度数学函数 */
int main(void) {
    double x = 0.5;

    /* 原有的函数 (仍然可用) */
    double s = sin(x);
    double c = cos(x);

    /* glibc 2.42 优化的内部实现 */
    /* - 使用 FMA (乘加融合) 指令 */
    /* - 改进的精度 */
    /* - 更快的收敛 */

    printf("sin(%f) = %.17g\n", x, s);
    printf("cos(%f) = %.17g\n", x, c);

    return 0;
}
