/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 363
 * Language: c
 * Block ID: 89f2981f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 ieee754_decimal_standard.c -o ieee754_decimal_standard
 *
 * IEEE 754-2008 十进制浮点标准介绍
 */

#include <stdio.h>
#include <stdint.h>

/* IEEE 754-2008 定义的三种十进制格式 */

typedef struct {
    uint32_t fields;
    /* 实际存储使用 DPD (Densely Packed Decimal) 编码 */
} IEEE754_Decimal32;

typedef struct {
    uint64_t fields[2];
} IEEE754_Decimal64;

typedef struct {
    uint64_t fields[4];
} IEEE754_Decimal128;

int main(void) {
    printf("=== IEEE 754-2008 十进制浮点标准 ===\n\n");

    printf("标准发布: 2008年8月\n");
    printf("主要更新: 增加了十进制浮点算术\n\n");

    printf("=== 三种十进制格式 ===\n\n");

    printf("1. decimal32 (_Decimal32 in C)\n");
    printf("   存储大小: 32位 (4字节)\n");
    printf("   精度: 7位十进制数字\n");
    printf("   指数范围: -95 到 +96\n");
    printf("   数值范围: ~1.0E-95 到 ~9.99E+96\n\n");

    printf("2. decimal64 (_Decimal64 in C)\n");
    printf("   存储大小: 64位 (8字节)\n");
    printf("   精度: 16位十进制数字\n");
    printf("   指数范围: -383 到 +384\n");
    printf("   数值范围: ~1.0E-383 到 ~9.99E+384\n\n");

    printf("3. decimal128 (_Decimal128 in C)\n");
    printf("   存储大小: 128位 (16字节)\n");
    printf("   精度: 34位十进制数字\n");
    printf("   指数范围: -6143 到 +6144\n");
    printf("   数值范围: ~1.0E-6143 到 ~9.99E+6144\n\n");

    printf("=== 编码方式 ===\n\n");

    printf("IEEE 754-2008 定义了两种编码方式:\n");
    printf("  1. DPD (Densely Packed Decimal) - 密集打包十进制\n");
    printf("     - 使用 10位编码 3个十进制数字\n");
    printf("     - 编码效率: 1000/1024 ≈ 97.7%\n\n");

    printf("  2. BID (Binary Integer Decimal) - 二进制整数十进制\n");
    printf("     - 使用二进制整数存储尾数\n");
    printf("     - 更适合硬件实现\n\n");

    printf("C语言实现通常使用 BID 编码 (Intel/AMD 处理器)\n");

    return 0;
}
