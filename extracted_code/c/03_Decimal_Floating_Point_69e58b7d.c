/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1191
 * Language: c
 * Block ID: 69e58b7d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 literal_suffixes.c -o literal_suffixes
 *
 * 十进制浮点数字面量后缀详解
 */

#include <stdio.h>

int main(void) {
    printf("=== 十进制浮点数字面量后缀 ===\n\n");

    /* 后缀说明 */
    printf("C23 标准定义的后缀:\n\n");

    printf("1. DF 后缀 - Decimal Float (_Decimal32)\n");
    printf("   示例: 3.14159DF, 0.1DF, 1e-7DF\n");
    printf("   大小: 4字节\n");
    printf("   精度: 7位有效数字\n\n");

    printf("2. DD 后缀 - Decimal Double (_Decimal64)\n");
    printf("   示例: 3.141592653589793DD, 0.1DD, 1e-16DD\n");
    printf("   大小: 8字节\n");
    printf("   精度: 16位有效数字\n\n");

    printf("3. DL 后缀 - Decimal Long (_Decimal128)\n");
    printf("   示例: 3.14159265358979323846264338327950288DL\n");
    printf("   大小: 16字节\n");
    printf("   精度: 34位有效数字\n\n");

    /* 字面量示例 */
    printf("=== 字面量表示示例 ===\n\n");

    /* 标准十进制表示 */
    printf("标准十进制表示:\n");
    _Decimal32 d32_1 = 123.456DF;
    _Decimal64 d64_1 = 123.4567890123456DD;
    _Decimal128 d128_1 = 123.4567890123456789012345678901234DL;

    printf("  123.456DF = %.6f (显示为double)\n", (double)d32_1);
    printf("  123.4567890123456DD = %.13f\n", (double)d64_1);
    printf("  123.4567890123456789012345678901234DL (精度超过double显示范围)\n\n");

    /* 科学计数法 */
    printf("科学计数法:\n");
    _Decimal32 d32_sci = 1.5e6DF;       /* 1.5 × 10^6 = 1500000 */
    _Decimal64 d64_sci = 1.5e-6DD;      /* 1.5 × 10^-6 = 0.0000015 */
    _Decimal128 d128_sci = 1.5e30DL;    /* 1.5 × 10^30 */

    printf("  1.5e6DF = %.0f\n", (double)d32_sci);
    printf("  1.5e-6DD = %.9f\n", (double)d64_sci);
    printf("  1.5e30DL = %.6e (科学计数法显示)\n\n", (double)d128_sci);

    /* 整数表示 */
    printf("整数表示 (自动转换为十进制浮点):\n");
    _Decimal32 d32_int = 42DF;
    _Decimal64 d64_int = 1000000DD;

    printf("  42DF = %.0f\n", (double)d32_int);
    printf("  1000000DD = %.0f\n\n", (double)d64_int);

    /* 负数 */
    printf("负数:\n");
    _Decimal32 d32_neg = -123.45DF;
    _Decimal64 d64_neg = -9.99e10DD;

    printf("  -123.45DF = %.2f\n", (double)d32_neg);
    printf("  -9.99e10DD = %.2f\n\n", (double)d64_neg);

    /* 前导零和小数点 */
    printf("前导零和小数点:\n");
    _Decimal32 d32_lead = 0.001DF;
    _Decimal64 d64_lead = .5DD;         /* 等价于 0.5 */
    _Decimal128 d128_lead = 5.DL;       /* 等价于 5.0 */

    printf("  0.001DF = %.3f\n", (double)d32_lead);
    printf("  .5DD = %.1f\n", (double)d64_lead);
    printf("  5.DL = %.1f\n\n", (double)d128_lead);

    /* 十六进制表示 (注意: 十进制浮点不支持十六进制!) */
    printf("重要说明:\n");
    printf("  十进制浮点字面量不支持十六进制表示!\n");
    printf("  0x1.fffffep+2 是二进制浮点字面量\n");
    printf("  十进制浮点必须使用十进制表示法\n");

    return 0;
}
