/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 558
 * Language: c
 * Block ID: ace400ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal32_details.c -o decimal32_details
 *
 * _Decimal32 详细说明
 */

#include <stdio.h>
#include <float.h>
#include <stdint.h>

int main(void) {
    printf("=== _Decimal32 详解 ===\n\n");

    /* 基本信息 */
    printf("基本信息:\n");
    printf("  存储大小: %zu 字节 (%zu 位)\n", sizeof(_Decimal32), sizeof(_Decimal32) * 8);
    printf("  C23 关键字: _Decimal32\n");
    printf("  后缀: DF (Decimal Float)\n\n");

    /* 精度 */
    printf("精度规范:\n");
    printf("  有效数字: 7位十进制数字\n");
    printf("  相对精度: 约 1 × 10^-7\n");
    printf("  足够精度: 存储到 9999999 (千万级别)\n\n");

    /* 指数范围 */
    printf("指数范围:\n");
    printf("  最小指数: -95 (emax = 96, emin = 95)\n");
    printf("  最大指数: +96\n");
    printf("  偏置值 (bias): 101\n\n");

    /* 数值范围 */
    printf("数值范围:\n");
    printf("  最小正规数: 1 × 10^-95\n");
    printf("  最大正规数: 9.999999 × 10^96\n");
    printf("  最小次正规数: 1 × 10^-101\n\n");

    /* 财务应用评估 */
    printf("财务应用评估:\n");
    printf("  ✓ 适合: 小额交易 (< 1000万)\n");
    printf("  ✗ 不适合: 大型财务报表\n");
    printf("  ✗ 不适合: 需要高精度计算\n");
    printf("  建议: 仅用于存储, 计算使用 _Decimal64\n\n");

    /* 实际例子 */
    printf("使用示例:\n");
    _Decimal32 price = 99.99DF;
    _Decimal32 quantity = 1000.0DF;
    _Decimal32 total = price * quantity;

    printf("  单价: %.2f (显示为 double)\n", (double)price);
    printf("  数量: %.0f\n", (double)quantity);
    printf("  总价: %.2f\n\n", (double)total);

    /* 编码示例 */
    printf("编码细节:\n");
    printf("  位布局 (32位):\n");
    printf("    31: 符号位\n");
    printf("    30-24: 组合字段 (7位)\n");
    printf("    23-20: 系数位 0-3 (4位)\n");
    printf("    19-10: 系数位 4-9 (10位 DPD)\n");
    printf("    9-0: 系数位 10-19 (10位 DPD)\n");

    return 0;
}
