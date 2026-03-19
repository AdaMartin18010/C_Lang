/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 943
 * Language: c
 * Block ID: 3c36acb4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 新数学函数示例 */
#include <math.h>
#include <stdio.h>

int main(void) {
    /* acospi: 以半圈为单位的反余弦 */
    double result1 = acospi(0.5);  /* = 1/3 (60度/180度) */
    printf("acospi(0.5) = %f (1/3)\n", result1);

    /* asinpi: 以半圈为单位的反正弦 */
    double result2 = asinpi(0.5);  /* = 1/6 (30度/180度) */
    printf("asinpi(0.5) = %f (1/6)\n", result2);

    /* atanpi: 以半圈为单位的反正切 */
    double result3 = atanpi(1.0);  /* = 1/4 (45度/180度) */
    printf("atanpi(1.0) = %f (1/4)\n", result3);

    /* atan2pi: 象限正确的 atanpi */
    double result4 = atan2pi(1.0, -1.0);  /* = 3/4 (135度/180度) */
    printf("atan2pi(1.0, -1.0) = %f (3/4)\n", result4);

    /* cospi: cos(πx)，更精确的整数倍计算 */
    double result5 = cospi(1.0);  /* cos(π) = -1 */
    printf("cospi(1.0) = %f\n", result5);

    double result6 = cospi(0.5);  /* cos(π/2) = 0 */
    printf("cospi(0.5) = %f\n", result6);

    /* sinpi: sin(πx) */
    double result7 = sinpi(0.5);  /* sin(π/2) = 1 */
    printf("sinpi(0.5) = %f\n", result7);

    double result8 = sinpi(1.0);  /* sin(π) = 0 */
    printf("sinpi(1.0) = %f\n", result8);

    /* tanpi: tan(πx) */
    double result9 = tanpi(0.25);  /* tan(π/4) = 1 */
    printf("tanpi(0.25) = %f\n", result9);

    return 0;
}
