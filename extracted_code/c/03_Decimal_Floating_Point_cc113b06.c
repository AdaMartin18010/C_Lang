/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 234
 * Language: c
 * Block ID: cc113b06
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 precision_analysis.c -o precision_analysis -lm
 *
 * 深入分析精度问题
 */

#include <stdio.h>
#include <math.h>

/* 自定义函数：打印二进制浮点误差 */
void analyze_binary_error(double val, const char* description) {
    double rounded = round(val * 100.0) / 100.0;
    double error = fabs(val - rounded);

    printf("  %s:\n", description);
    printf("    存储值: %.17f\n", val);
    printf("    期望的两位小数: %.2f\n", rounded);
    printf("    绝对误差: %.2e\n", error);
}

int main(void) {
    printf("=== 精度问题详细分析 ===\n\n");

    /* 常见的财务数值 */
    printf("1. 常见财务数值的二进制表示误差:\n");
    analyze_binary_error(0.01, "1分钱 (0.01)");
    analyze_binary_error(0.05, "5分钱 (0.05)");
    analyze_binary_error(0.10, "1角钱 (0.10)");
    analyze_binary_error(0.25, "2角5分 (0.25) - 可以精确表示!");
    printf("\n");

    /* 为什么 0.25 可以精确表示 */
    printf("2. 为什么 0.25 可以精确表示?\n");
    printf("   0.25 = 1/4 = 1/2^2\n");
    printf("   二进制表示: 0.01 (精确的 2位)\n\n");

    /* 累积误差演示 */
    printf("3. 累积误差问题:\n");
    double binary_sum = 0.0;
    _Decimal64 decimal_sum = 0.0DD;

    /* 累加 1000 次 0.001 */
    for (int i = 0; i < 1000; i++) {
        binary_sum += 0.001;
        decimal_sum += 0.001DD;
    }

    printf("   累加 1000 次 0.001:\n");
    printf("   期望结果: 1.0\n");
    printf("   二进制结果: %.15f\n", binary_sum);
    printf("   十进制结果: %.15f (转换为double显示)\n\n", (double)decimal_sum);

    /* 比较误差 */
    printf("4. 误差比较:\n");
    double binary_error = fabs(binary_sum - 1.0);
    _Decimal64 decimal_error = decimal_sum - 1.0DD;

    printf("   二进制误差: %.2e\n", binary_error);
    printf("   十进制误差: 0 (精确计算)\n\n");

    /* 实际影响 */
    printf("5. 实际财务影响:\n");
    printf("   假设每天处理 100 万笔交易\n");
    printf("   每笔交易误差 ~1e-16\n");
    printf("   日累积误差: ~1e-10 (看似很小)\n");
    printf("   但法律要求: 必须精确到分!\n");
    printf("   因此二进制浮点不适合财务计算\n");

    return 0;
}
