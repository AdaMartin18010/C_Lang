/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1489
 * Language: c
 * Block ID: 4182429e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal_conversions.c -o decimal_conversions
 *
 * 十进制浮点数类型转换
 */

#include <stdio.h>
#include <stdint.h>

/* 不同类型间的转换 */
void demonstrate_type_conversions(void) {
    printf("=== 十进制浮点类型间转换 ===\n\n");

    /* 窄类型到宽类型 (安全) */
    _Decimal32 d32 = 123.456DF;
    _Decimal64 d64_from32 = d32;  /* 自动提升 */
    _Decimal128 d128_from32 = d32;

    printf("窄到宽转换 (安全):\n");
    printf("  _Decimal32  %.3f\n", (double)d32);
    printf("  -> _Decimal64  %.6f\n", (double)d64_from32);
    printf("  -> _Decimal128 (保持精度)\n\n");

    /* 宽类型到窄类型 (可能丢失精度) */
    _Decimal64 d64_precise = 123456789.123456789DD;
    _Decimal32 d32_from64 = (_Decimal32)d64_precise;  /* 显式转换 */

    printf("宽到窄转换 (可能丢失精度):\n");
    printf("  _Decimal64  %.9f\n", (double)d64_precise);
    printf("  -> _Decimal32  %.6f (精度降低)\n\n", (double)d32_from64);

    /* 混合运算自动提升 */
    _Decimal32 a = 1.5DF;
    _Decimal64 b = 2.5DD;
    auto result = a + b;  /* 结果为 _Decimal64 */

    printf("混合运算自动提升:\n");
    printf("  _Decimal32 + _Decimal64 = _Decimal64\n");
    printf("  结果: %.2f\n\n", (double)result);
}

/* 与整数类型的转换 */
void demonstrate_integer_conversions(void) {
    printf("=== 与整数类型转换 ===\n\n");

    /* 整数到十进制浮点 */
    int int_val = 12345;
    long long_val = 999999999999LL;

    _Decimal64 d_from_int = (_Decimal64)int_val;
    _Decimal64 d_from_long = (_Decimal64)long_val;

    printf("整数到十进制浮点:\n");
    printf("  int %d -> %.0f\n", int_val, (double)d_from_int);
    printf("  long long %lld -> %.0f\n\n", long_val, (double)d_from_long);

    /* 十进制浮点到整数 (截断) */
    _Decimal64 d_val = 12345.6789DD;

    int i_from_d = (int)d_val;           /* 截断小数部分 */
    long long ll_from_d = (long long)d_val;

    printf("十进制浮点到整数 (截断):\n");
    printf("  _Decimal64 %.4f -> int %d\n", (double)d_val, i_from_d);
    printf("  _Decimal64 %.4f -> long long %lld\n\n", (double)d_val, ll_from_d);

    /* 溢出检查 */
    _Decimal64 large_d = 1e20DD;
    int overflow = (int)large_d;

    printf("溢出处理:\n");
    printf("  %.0e -> int %d (溢出，结果未定义或截断)\n\n", (double)large_d, overflow);
}

/* 与二进制浮点转换 */
void demonstrate_float_conversions(void) {
    printf("=== 与二进制浮点转换 ===\n\n");

    printf("警告: 二进制浮点到十进制浮点转换可能引入误差!\n\n");

    /* float/double 到 _Decimal64 */
    double binary_val = 0.1;  /* 不精确的二进制表示 */
    _Decimal64 decimal_from_binary = (_Decimal64)binary_val;

    printf("double 到 _Decimal64:\n");
    printf("  double 0.1 (实际值): %.17f\n", binary_val);
    printf("  -> _Decimal64: %.17f\n", (double)decimal_from_binary);
    printf("  注意: 不精确的二进制值被保留!\n\n");

    /* 正确做法: 使用十进制字面量 */
    _Decimal64 correct_decimal = 0.1DD;
    printf("正确的十进制字面量:\n");
    printf("  _Decimal64 0.1DD: %.17f (精确)\n\n", (double)correct_decimal);

    /* _Decimal64 到 double */
    _Decimal64 precise_decimal = 0.1DD;
    double binary_from_decimal = (double)precise_decimal;

    printf("_Decimal64 到 double:\n");
    printf("  _Decimal64 0.1DD: 精确\n");
    printf("  -> double: %.17f\n", binary_from_decimal);
    printf("  注意: 转换为二进制后可能不精确\n");
}

/* 字符串转换 */
void demonstrate_string_conversions(void) {
    printf("\n=== 字符串转换 ===\n\n");

    printf("字符串到十进制浮点 (使用 sscanf):\n");
    printf("  代码: _Decimal64 val; sscanf(\"123.456\", \"%%f\", &val);\n");
    printf("  注意: 需要转换说明符支持\n\n");

    printf("十进制浮点到字符串 (使用 snprintf):\n");
    printf("  代码: snprintf(buf, sizeof(buf), \"%%.2f\", (double)val);\n");
    printf("  注意: 通常先转换为 double 再格式化\n\n");

    /* 实际示例 */
    _Decimal64 price = 1234.56DD;
    char buffer[64];

    /* 转换为 double 后格式化 */
    snprintf(buffer, sizeof(buffer), "%.2f", (double)price);
    printf("示例: _Decimal64 1234.56DD -> 字符串 \"%s\"\n", buffer);
}

int main(void) {
    printf("=== 十进制浮点数类型转换 ===\n\n");

    demonstrate_type_conversions();
    demonstrate_integer_conversions();
    demonstrate_float_conversions();
    demonstrate_string_conversions();

    return 0;
}
