/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 309
 * Language: c
 * Block ID: 636b601b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 rounding_modes.c -o rounding_modes
 *
 * 演示不同的舍入模式
 */

#include <stdio.h>
#include <fenv.h>

int main(void) {
    printf("=== IEEE 754 舍入模式 ===\n\n");

    /* 获取当前舍入模式 */
    int current_mode = fegetround();

    printf("当前舍入模式: ");
    switch (current_mode) {
        case FE_TONEAREST:  printf("向最近舍入(默认)\n"); break;
        case FE_UPWARD:     printf("向正无穷舍入\n"); break;
        case FE_DOWNWARD:   printf("向负无穷舍入\n"); break;
        case FE_TOWARDZERO: printf("向零舍入\n"); break;
        default:            printf("未知\n");
    }

    printf("\n十进制浮点数支持以下舍入模式:\n");
    printf("  1. roundTiesToEven    - 银行家舍入(默认)\n");
    printf("  2. roundTiesToAway    - 远离零舍入\n");
    printf("  3. roundTowardPositive - 向上舍入\n");
    printf("  4. roundTowardNegative - 向下舍入\n");
    printf("  5. roundTowardZero    - 向零截断\n\n");

    /* 银行家舍入示例 */
    printf("银行家舍入(roundTiesToEven)示例:\n");
    printf("  2.5 -> 2 (2是偶数)\n");
    printf("  3.5 -> 4 (4是偶数)\n");
    printf("  这种舍入方式减少了累积误差\n\n");

    /* 财务舍入示例 */
    printf("财务舍入(roundTiesToAway)示例:\n");
    printf("  2.5 -> 3 (远离零)\n");
    printf("  3.5 -> 4 (远离零)\n");
    printf("  这种舍入方式符合直觉，常用于财务\n");

    return 0;
}
