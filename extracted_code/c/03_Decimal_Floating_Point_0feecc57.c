/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 725
 * Language: c
 * Block ID: 0feecc57
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal64_details.c -o decimal64_details
 *
 * _Decimal64 详细说明 - 最实用的十进制浮点类型
 */

#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== _Decimal64 详解 ===\n\n");

    /* 基本信息 */
    printf("基本信息:\n");
    printf("  存储大小: %zu 字节 (%zu 位)\n", sizeof(_Decimal64), sizeof(_Decimal64) * 8);
    printf("  C23 关键字: _Decimal64\n");
    printf("  后缀: DD (Decimal Double)\n\n");

    /* 精度 */
    printf("精度规范:\n");
    printf("  有效数字: 16位十进制数字\n");
    printf("  相对精度: 约 1 × 10^-16\n");
    printf("  足够精度: 存储到 99,999,999,999,999.99 (百万亿级别)\n\n");

    /* 指数范围 */
    printf("指数范围:\n");
    printf("  最小指数: -383\n");
    printf("  最大指数: +384\n");
    printf("  偏置值 (bias): 398\n\n");

    /* 数值范围 */
    printf("数值范围:\n");
    printf("  最小正规数: 1 × 10^-383\n");
    printf("  最大正规数: 9.999999999999999 × 10^384\n");
    printf("  最小次正规数: 1 × 10^-398\n\n");

    /* 财务应用评估 */
    printf("财务应用评估:\n");
    printf("  ✓ 适合: 几乎所有财务计算\n");
    printf("  ✓ 适合: 企业级财务报表\n");
    printf("  ✓ 适合: 银行利息计算\n");
    printf("  ✓ 适合: 税务计算\n");
    printf("  推荐: 作为默认十进制浮点类型\n\n");

    /* 与 double 比较 */
    printf("与 double 比较:\n");
    printf("  存储大小: 相同 (8字节)\n");
    printf("  精度: 16位十进制 vs 约15-17位十进制等效\n");
    printf("  关键优势: 精确表示十进制分数\n");
    printf("  关键劣势: 运算速度较慢\n\n");

    /* 编码细节 */
    printf("编码细节:\n");
    printf("  位布局 (64位 BID 编码):\n");
    printf("    63: 符号位\n");
    printf("    62-51: 组合字段 (12位)\n");
    printf("    50-0: 有效数字系数 (51位)\n");
    printf("  最大系数: 2^53 - 1 = 9,007,199,254,740,991\n");
    printf("  可精确表示16位十进制整数\n");

    return 0;
}
