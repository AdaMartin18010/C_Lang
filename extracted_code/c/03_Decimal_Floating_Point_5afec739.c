/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1361
 * Language: c
 * Block ID: 5afec739
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal_operations.c -o decimal_operations
 *
 * 十进制浮点数基本运算
 */

#include <stdio.h>

/* 算术运算演示 */
void demonstrate_arithmetic(void) {
    printf("=== 算术运算 ===\n\n");

    _Decimal64 a = 100.00DD;
    _Decimal64 b = 30.00DD;

    /* 加法 */
    _Decimal64 sum = a + b;
    printf("加法: %.2f + %.2f = %.2f\n", (double)a, (double)b, (double)sum);

    /* 减法 */
    _Decimal64 diff = a - b;
    printf("减法: %.2f - %.2f = %.2f\n", (double)a, (double)b, (double)diff);

    /* 乘法 */
    _Decimal64 prod = a * b;
    printf("乘法: %.2f * %.2f = %.2f\n", (double)a, (double)b, (double)prod);

    /* 除法 */
    _Decimal64 quot = a / b;
    printf("除法: %.2f / %.2f = %.4f\n", (double)a, (double)b, (double)quot);

    /* 取余 - 十进制浮点不支持 % 运算符 */
    /* 使用 fmod 或手动计算 */
    _Decimal64 mod_result = a - (int)(a / b) * b;
    printf("取余: fmod(%.2f, %.2f) ≈ %.2f\n\n", (double)a, (double)b, (double)mod_result);
}

/* 比较运算演示 */
void demonstrate_comparison(void) {
    printf("=== 比较运算 ===\n\n");

    _Decimal64 x = 1.0DD;
    _Decimal64 y = 1.00DD;  /* 与 x 相等 */
    _Decimal64 z = 1.01DD;

    printf("x = 1.0DD, y = 1.00DD, z = 1.01DD\n\n");

    /* 相等比较 */
    printf("x == y: %s (注意: 1.0 和 1.00 相等)\n", x == y ? "true" : "false");
    printf("x == z: %s\n", x == z ? "true" : "false");

    /* 不等比较 */
    printf("x != z: %s\n", x != z ? "true" : "false");

    /* 大小比较 */
    printf("x < z: %s\n", x < z ? "true" : "false");
    printf("z > x: %s\n", z > x ? "true" : "false");
    printf("x <= y: %s\n", x <= y ? "true" : "false");
    printf("z >= y: %s\n\n", z >= y ? "true" : "false");

    /* NaN 比较 */
    printf("NaN 比较:\n");
    _Decimal64 nan_val = 0.0DD / 0.0DD;  /* 产生 NaN */
    printf("NaN == NaN: %s (NaN 不等于任何值，包括自身)\n",
           nan_val == nan_val ? "true" : "false");
}

/* 一元运算 */
void demonstrate_unary(void) {
    printf("=== 一元运算 ===\n\n");

    _Decimal64 val = 42.5DD;

    /* 正负号 */
    _Decimal64 neg = -val;
    _Decimal64 pos = +val;

    printf("原值: %.2f\n", (double)val);
    printf("负号: %.2f\n", (double)neg);
    printf("正号: %.2f\n\n", (double)pos);

    /* 自增自减 */
    _Decimal64 inc = val;
    _Decimal64 dec = val;

    inc++;  /* 后缀递增 */
    dec--;  /* 后缀递减 */

    printf("后缀 ++: %.2f (原值加1)\n", (double)inc);
    printf("后缀 --: %.2f (原值减1)\n\n", (double)dec);
}

/* 复合赋值运算 */
void demonstrate_compound(void) {
    printf("=== 复合赋值运算 ===\n\n");

    _Decimal64 total = 1000.00DD;

    printf("初始总额: %.2f\n", (double)total);

    total += 250.50DD;  /* 加 */
    printf("+= 250.50: %.2f\n", (double)total);

    total -= 100.00DD;  /* 减 */
    printf("-= 100.00: %.2f\n", (double)total);

    total *= 1.08DD;    /* 乘 (加 8% 税) */
    printf("*= 1.08:   %.2f\n", (double)total);

    total /= 2.0DD;     /* 除 */
    printf("/= 2.0:    %.2f\n", (double)total);
}

int main(void) {
    printf("=== 十进制浮点数基本运算 ===\n\n");

    demonstrate_arithmetic();
    demonstrate_comparison();
    demonstrate_unary();
    demonstrate_compound();

    return 0;
}
