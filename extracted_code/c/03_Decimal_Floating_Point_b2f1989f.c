/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 433
 * Language: c
 * Block ID: b2f1989f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 encoding_format.c -o encoding_format
 *
 * 十进制浮点数编码格式详解
 */

#include <stdio.h>
#include <stdint.h>

/*
 * Decimal64 BID 编码布局 (Intel/AMD 使用)
 *
 * 位布局 (64位):
 *  63: 符号位 (S)
 *  62-51: 组合字段 (G)
 *  50-0: 有效数字 (T) - 使用二进制整数编码
 */

void print_decimal64_layout(void) {
    printf("=== Decimal64 BID 编码布局 ===\n\n");

    printf("位分布 (从高位到低位):\n");
    printf("  63    : 符号位 (S) - 0为正, 1为负\n");
    printf("  62-51  : 组合字段 (G) - 包含指数和高位数字\n");
    printf("  50-0   : 有效数字 (T) - 二进制整数编码的尾数\n\n");

    printf("组合字段编码:\n");
    printf("  G4G3G2 是 111 时: 指数为 G0G1T 的形式 (特殊值)\n");
    printf("  其他情况: 指数直接编码\n\n");

    printf("特殊值编码:\n");
    printf("  +Infinity: 0x7800000000000000\n");
    printf("  -Infinity: 0xF800000000000000\n");
    printf("  NaN      : G4G3 = 11, G2 = 1\n");
    printf("  sNaN     : G4G3 = 11, G2 = 0\n");
}

int main(void) {
    print_decimal64_layout();

    printf("\n=== 编码示例 ===\n\n");

    /* 数值 123.456 的编码概念 */
    printf("数值: 123.456\n");
    printf("科学计数法: 1.23456 × 10^2\n");
    printf("或: 123456 × 10^-3\n\n");

    printf("在 BID 编码中:\n");
    printf("  符号位: 0 (正数)\n");
    printf("  指数: -3 (编码后)\n");
    printf("  系数: 123456 (二进制存储)\n\n");

    /* DPD 编码概念 */
    printf("=== DPD 编码 (IBM 使用) ===\n\n");
    printf("DPD 每 10位 编码 3个十进制数字 (0-999)\n");
    printf("编码表:\n");
    printf("  a,b,c,d,e,f,g,h,i 是 3个BCD数字的位\n");
    printf("  p,q,r 是编码位\n\n");

    return 0;
}
