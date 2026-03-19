/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1282
 * Language: c
 * Block ID: 1a93c274
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 literal_traps.c -o literal_traps
 *
 * 字面量使用中的陷阱
 */

#include <stdio.h>

int main(void) {
    printf("=== 字面量使用陷阱 ===\n\n");

    /* 陷阱 1: 忘记后缀导致精度丢失 */
    printf("陷阱 1: 忘记后缀\n");

    _Decimal64 wrong = 0.1;    /* 警告: 0.1 是 double，不是 _Decimal64 */
    _Decimal64 correct = 0.1DD; /* 正确: 明确指定十进制浮点 */

    printf("  wrong = 0.1 (double 转换): %.17f\n", (double)wrong);
    printf("  correct = 0.1DD: %.17f\n", (double)correct);
    printf("  注意: 某些编译器可能警告隐式转换\n\n");

    /* 陷阱 2: 指数过大或过小 */
    printf("陷阱 2: 指数范围检查\n");

    /* 这些在编译时会检查 */
    _Decimal32 d32_max = 9.999999e96DF;   /* 接近最大指数 */
    _Decimal32 d32_min = 1e-95DF;          /* 接近最小正规数 */

    printf("  Decimal32 最大正规数附近: %.6e\n", (double)d32_max);
    printf("  Decimal32 最小正规数: %.6e\n\n", (double)d32_min);

    /* 陷阱 3: 精度截断 */
    printf("陷阱 3: 精度截断\n");

    _Decimal32 d32_precise = 1.23456789DF;  /* 8位数字，但只有7位精度 */
    _Decimal64 d64_precise = 1.234567890123456789DD; /* 18位数字，但只有16位精度 */

    printf("  1.23456789DF (8位) 存储为: %.7f\n", (double)d32_precise);
    printf("  1.234567890123456789DD (18位) 存储为: %.16f\n\n", (double)d64_precise);

    /* 陷阱 4: 混合运算 */
    printf("陷阱 4: 混合类型运算\n");

    _Decimal32 d32_val = 1.5DF;
    _Decimal64 d64_val = 2.5DD;

    /* 混合运算会转换为更宽的类型 */
    _Decimal64 result = d32_val + d64_val;  /* d32_val 提升为 _Decimal64 */

    printf("  1.5DF + 2.5DD = %.1f\n", (double)result);
    printf("  注意: 窄类型会自动提升为宽类型\n\n");

    /* 最佳实践 */
    printf("=== 最佳实践 ===\n\n");

    printf("1. 始终使用后缀指定字面量类型:\n");
    printf("   _Decimal64 price = 99.99DD;  // 正确\n");
    printf("   _Decimal64 wrong = 99.99;     // 可能有精度问题\n\n");

    printf("2. 使用宏定义常量:\n");
    printf("   #define TAX_RATE 0.08DD\n");
    printf("   #define PI 3.14159265358979323846DL\n\n");

    printf("3. 注意运算结果的类型:\n");
    printf("   _Decimal32 a = 1.0DF;\n");
    printf("   _Decimal64 b = 2.0DD;\n");
    printf("   auto c = a + b;  // c 是 _Decimal64\n");

    return 0;
}
