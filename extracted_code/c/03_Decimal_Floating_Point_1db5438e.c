/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 147
 * Language: c
 * Block ID: 1db5438e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal_types_demo.c -o decimal_types_demo
 * 注意: 需要 GCC 12+ 或 Clang 15+ 支持 _Decimal 类型
 */

#include <stdio.h>

/* C23 标准引入了三种十进制浮点类型 */

int main(void) {
    printf("=== C23 十进制浮点类型 ===\n\n");

    /* _Decimal32: 7位有效数字, 精度约 1E-7 */
    _Decimal32 d32 = 0.1DF;
    printf("_Decimal32  大小: %zu 字节\n", sizeof(_Decimal32));

    /* _Decimal64: 16位有效数字, 精度约 1E-16 */
    _Decimal64 d64 = 0.1DD;
    printf("_Decimal64  大小: %zu 字节\n", sizeof(_Decimal64));

    /* _Decimal128: 34位有效数字, 精度约 1E-34 */
    _Decimal128 d128 = 0.1DL;
    printf("_Decimal128 大小: %zu 字节\n", sizeof(_Decimal128));

    printf("\n=== 精度比较 ===\n");
    printf("所有类型都能精确表示 0.1!\n");

    return 0;
}
