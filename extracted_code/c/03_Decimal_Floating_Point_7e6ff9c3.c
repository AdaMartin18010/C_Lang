/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 499
 * Language: c
 * Block ID: 7e6ff9c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 special_values.c -o special_values
 *
 * 十进制浮点特殊值处理
 */

#include <stdio.h>
#include <math.h>
#include <string.h>

/* 检查十进制浮点数的宏 (概念性) */

int main(void) {
    printf("=== 十进制浮点特殊值 ===\n\n");

    /* 无穷大 */
    printf("1. 无穷大 (Infinity)\n");
    printf("   正无穷: 1.0DD / 0.0DD\n");
    printf("   负无穷: -1.0DD / 0.0DD\n");
    printf("   比较: Infinity > 任何有限数\n\n");

    /* NaN */
    printf("2. NaN (Not a Number)\n");
    printf("   静默 NaN (qNaN): 0.0DD / 0.0DD 的结果\n");
    printf("   信号 NaN (sNaN): 未初始化的十进制变量\n");
    printf("   比较: NaN != NaN (总是成立)\n\n");

    /* 零的符号 */
    printf("3. 有符号零\n");
    printf("   +0.0DD: 正零\n");
    printf("   -0.0DD: 负零\n");
    printf("   比较: +0.0DD == -0.0DD (相等)\n");
    printf("   1.0DD / +0.0DD = +Infinity\n");
    printf("   1.0DD / -0.0DD = -Infinity\n\n");

    /* 次正规数 */
    printf("4. 次正规数 (Subnormal/_denormal)\n");
    printf("   比最小正规数还小的数\n");
    printf("   精度逐渐降低\n");
    printf("   用于平滑下溢\n\n");

    /* 未初始化值 */
    printf("5. 陷阱表示\n");
    printf("   某些位模式被保留为陷阱\n");
    printf("   访问时可能产生异常\n");

    return 0;
}
