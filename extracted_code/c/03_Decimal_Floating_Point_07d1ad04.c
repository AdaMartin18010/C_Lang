/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 974
 * Language: c
 * Block ID: 07d1ad04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal128_details.c -o decimal128_details
 *
 * _Decimal128 详细说明 - 超高精度十进制浮点
 */

#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== _Decimal128 详解 ===\n\n");

    /* 基本信息 */
    printf("基本信息:\n");
    printf("  存储大小: %zu 字节 (%zu 位)\n", sizeof(_Decimal128), sizeof(_Decimal128) * 8);
    printf("  C23 关键字: _Decimal128\n");
    printf("  后缀: DL (Decimal Long)\n\n");

    /* 精度 */
    printf("精度规范:\n");
    printf("  有效数字: 34位十进制数字\n");
    printf("  相对精度: 约 1 × 10^-34\n");
    printf("  足够精度: 存储天文数字级别的精确值\n\n");

    /* 指数范围 */
    printf("指数范围:\n");
    printf("  最小指数: -6143\n");
    printf("  最大指数: +6144\n");
    printf("  偏置值 (bias): 6176\n\n");

    /* 数值范围 */
    printf("数值范围:\n");
    printf("  最小正规数: 1 × 10^-6143\n");
    printf("  最大正规数: 约 9.99 × 10^6144\n");
    printf("  最小次正规数: 1 × 10^-6176\n");
    printf("  注意: 最大数值远超宇宙原子数量 (~10^80)\n\n");

    /* 应用场景 */
    printf("应用场景:\n");
    printf("  ✓ 超高精度科学计算\n");
    printf("  ✓ 国家级别财务核算\n");
    printf("  ✓ 高精度物理模拟\n");
    printf("  ✓ 中间计算结果保持精度\n");
    printf("  ✗ 一般商业应用 (性能开销大)\n\n");

    /* 与 _Decimal64 比较 */
    printf("与 _Decimal64 比较:\n");
    printf("  存储: 2倍 (16 vs 8 字节)\n");
    printf("  精度: 2倍以上 (34 vs 16 位)\n");
    printf("  范围: 指数范围大16倍\n");
    printf("  速度: 显著较慢\n\n");

    /* 编码细节 */
    printf("编码细节:\n");
    printf("  位布局 (128位 BID 编码):\n");
    printf("    127: 符号位\n");
    printf("    126-113: 组合字段 (14位)\n");
    printf("    112-0: 有效数字系数 (113位)\n");
    printf("  最大系数: 2^113 - 1\n");
    printf("  可精确表示34位十进制整数\n");

    return 0;
}
