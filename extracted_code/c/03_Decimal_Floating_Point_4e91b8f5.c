/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 185
 * Language: c
 * Block ID: 4e91b8f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 radix_comparison.c -o radix_comparison
 *
 * 比较二进制浮点和十进制浮点的基数差异
 */

#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== 浮点数基数(Radix)比较 ===\n\n");

    /* 二进制浮点数使用基数 2 */
    printf("二进制浮点数 (float/double):\n");
    printf("  基数 (FLT_RADIX): %d\n", FLT_RADIX);
    printf("  表示方式: 数字 × 2^指数\n");
    printf("  可精确表示的分数: 1/2, 1/4, 1/8, 3/4, ...\n");
    printf("  不能精确表示的分数: 1/10, 1/5, 1/100, ...\n\n");

    /* 十进制浮点数使用基数 10 */
    printf("十进制浮点数 (_Decimal32/64/128):\n");
    printf("  基数: 10\n");
    printf("  表示方式: 数字 × 10^指数\n");
    printf("  可精确表示的分数: 1/10, 1/100, 1/1000, ...\n");
    printf("  这正是财务计算需要的!\n\n");

    /* 具体例子 */
    printf("=== 具体例子 ===\n");

    double binary_val = 0.1;
    _Decimal64 decimal_val = 0.1DD;

    printf("数值: 0.1\n");
    printf("  二进制 double:  不精确 (近似值)\n");
    printf("  十进制 _Decimal64: 精确表示\n\n");

    /* 数学原理 */
    printf("=== 数学原理 ===\n");
    printf("1/10 = 1/(2×5) = 0.1 (十进制)\n");
    printf("1/10 在二进制中 = 0.00011001100110011... (无限循环)\n");
    printf("因此二进制浮点无法精确存储 0.1\n");

    return 0;
}
