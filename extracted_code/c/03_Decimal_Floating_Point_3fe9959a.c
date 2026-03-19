/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 57
 * Language: c
 * Block ID: 3fe9959a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 binary_precision_problem.c -o binary_precision_problem
 *
 * 演示二进制浮点数的精度问题
 */

#include <stdio.h>
#include <stdint.h>

int main(void) {
    /*
     * 问题 1: 0.1 无法精确表示
     * 在二进制浮点中，0.1 = 0.0001100110011... (无限循环)
     */
    double price = 0.1;
    double total = 0.0;

    /* 累加 0.1 十次 */
    for (int i = 0; i < 10; i++) {
        total += price;
    }

    printf("=== 二进制浮点数精度问题演示 ===\n\n");

    printf("期望结果: 0.1 * 10 = 1.0\n");
    printf("实际结果: 0.1 * 10 = %.17f\n\n", total);

    /* 比较结果 */
    if (total == 1.0) {
        printf("比较: total == 1.0 ? 是\n");
    } else {
        printf("比较: total == 1.0 ? 否 (精度丢失!)\n");
        printf("误差: %.17f\n\n", total - 1.0);
    }

    /*
     * 问题 2: 财务计算中的舍入误差累积
     */
    double amount1 = 0.05;  /* 5分钱 */
    double amount2 = 0.06;  /* 6分钱 */
    double sum = amount1 + amount2;

    printf("财务计算示例:\n");
    printf("0.05 + 0.06 = %.17f\n", sum);
    printf("期望结果: 0.11\n");
    printf("实际结果: %.17f\n\n", sum);

    /*
     * 问题 3: 除法运算中的精度问题
     */
    double a = 10.0;
    double b = 3.0;
    double result = (a / b) * 3;

    printf("除法运算:\n");
    printf("(10.0 / 3.0) * 3 = %.17f\n", result);
    printf("期望结果: 10.0\n");
    printf("实际结果: %.17f\n\n", result);

    /*
     * 问题 4: 显示二进制表示
     */
    union {
        double d;
        uint64_t u;
    } converter;

    converter.d = 0.1;
    printf("0.1 的 IEEE 754 二进制表示:\n");
    printf("十六进制: 0x%016llX\n", (unsigned long long)converter.u);
    printf("这不是精确的 0.1，而是最接近的近似值\n");

    return 0;
}
