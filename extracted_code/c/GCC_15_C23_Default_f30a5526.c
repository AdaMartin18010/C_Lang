/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1050
 * Language: c
 * Block ID: f30a5526
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 新增对数和指数函数 */
#include <math.h>
#include <stdio.h>

int main(void) {
    /* exp10: 10的幂 */
    printf("exp10(3.0) = %f (1000)\n", exp10(3.0));

    /* exp10m1: 10^x - 1，小值更精确 */
    double small = 1e-10;
    printf("exp10m1(%g) = %.17g\n", small, exp10m1(small));
    printf("exp10(%g)-1 = %.17g\n", small, exp10(small) - 1);
    /* exp10m1 对小值更精确 */

    /* logp1: ln(1+x)，接近 0 时更精确 */
    printf("logp1(0.001) = %.17g\n", logp1(0.001));
    printf("log(1.001) = %.17g\n", log(1.001));

    /* rsqrt: 快速倒数平方根 */
    printf("rsqrt(4.0) = %f (0.5)\n", rsqrt(4.0));
    printf("1/sqrt(4.0) = %f\n", 1/sqrt(4.0));

    return 0;
}
