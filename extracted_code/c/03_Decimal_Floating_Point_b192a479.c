/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1042
 * Language: c
 * Block ID: b192a479
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal128_precision.c -o decimal128_precision
 *
 * _Decimal128 超高精度计算示例
 */

#include <stdio.h>

/* 高精度 Pi 计算 (使用 Machin-like 公式) */
_Decimal128 compute_pi_decimal128(void) {
    /*
     * Machin 公式: π/4 = 4×arctan(1/5) - arctan(1/239)
     * 使用泰勒级数: arctan(x) = x - x^3/3 + x^5/5 - x^7/7 + ...
     */

    _Decimal128 pi = 0.0DL;
    _Decimal128 one = 1.0DL;
    _Decimal128 four = 4.0DL;
    _Decimal128 five = 5.0DL;
    _Decimal128 two39 = 239.0DL;

    /* 计算 4×arctan(1/5) */
    _Decimal128 arctan_1_5 = 0.0DL;
    _Decimal128 x = one / five;
    _Decimal128 x2 = x * x;
    _Decimal128 term = x;
    _Decimal128 n = one;

    for (int i = 0; i < 50; i++) {
        arctan_1_5 += term / n;
        term = -term * x2;
        n += 2.0DL;
    }

    /* 计算 arctan(1/239) */
    _Decimal128 arctan_1_239 = 0.0DL;
    x = one / two39;
    x2 = x * x;
    term = x;
    n = one;

    for (int i = 0; i < 20; i++) {
        arctan_1_239 += term / n;
        term = -term * x2;
        n += 2.0DL;
    }

    pi = four * (four * arctan_1_5 - arctan_1_239);
    return pi;
}

/* 高精度金融模型计算 */
_Decimal128 high_precision_npv(_Decimal128* cashflows, int n, _Decimal128 rate) {
    /*
     * 净现值 (NPV) 计算
     * NPV = Σ(CF_t / (1+r)^t) for t = 0 to n
     */

    _Decimal128 npv = 0.0DL;
    _Decimal128 discount_factor = 1.0DL;  /* (1+r)^0 = 1 */
    _Decimal128 one_plus_r = 1.0DL + rate;

    for (int t = 0; t < n; t++) {
        npv += cashflows[t] / discount_factor;
        discount_factor *= one_plus_r;
    }

    return npv;
}

int main(void) {
    printf("=== _Decimal128 超高精度计算 ===\n\n");

    /* 1. 高精度 Pi 计算 */
    printf("1. 高精度 Pi 计算:\n");
    _Decimal128 pi = compute_pi_decimal128();

    /* 格式化输出 Pi 的前34位 */
    printf("  Pi (前34位): ");
    _Decimal128 pi_temp = pi;
    _Decimal128 ten = 10.0DL;

    /* 整数部分 */
    printf("3.");
    pi_temp = pi_temp - 3.0DL;

    /* 小数部分 */
    for (int i = 0; i < 34; i++) {
        pi_temp = pi_temp * ten;
        int digit = (int)pi_temp;
        printf("%d", digit);
        pi_temp = pi_temp - (_Decimal128)digit;
    }
    printf("\n  参考值:      3.14159265358979323846264338327950288...\n\n");

    /* 2. 超高精度货币计算 */
    printf("2. 超高精度货币计算:\n");
    _Decimal128 national_debt = 31415926535897932384626433832.7950288DL;
    _Decimal128 population = 331000000.0DL;  /* 3.31亿 */

    _Decimal128 debt_per_person = national_debt / population;

    printf("  国家债务: %.4f\n", (double)national_debt);
    printf("  人口: %.0f\n", (double)population);
    printf("  人均债务: %.4f\n\n", (double)debt_per_person);

    /* 3. 高精度 NPV 计算 */
    printf("3. 高精度净现值 (NPV) 计算:\n");
    _Decimal128 cashflows[] = {
        -1000000.0DL,   /* 初始投资 */
        200000.0DL,     /* 第1年 */
        250000.0DL,     /* 第2年 */
        300000.0DL,     /* 第3年 */
        350000.0DL,     /* 第4年 */
        400000.0DL,     /* 第5年 */
        450000.0DL,     /* 第6年 */
        500000.0DL      /* 第7年 */
    };
    int num_years = sizeof(cashflows) / sizeof(cashflows[0]);
    _Decimal128 discount_rate = 0.08DL;  /* 8% 折现率 */

    _Decimal128 npv_result = high_precision_npv(cashflows, num_years, discount_rate);

    printf("  项目现金流 (10年):\n");
    for (int i = 0; i < num_years; i++) {
        printf("    第%d年: $%.2f\n", i, (double)cashflows[i]);
    }
    printf("  折现率: %.2f%%\n", (double)(discount_rate * 100.0DL));
    printf("  NPV: $%.4f\n\n", (double)npv_result);

    if (npv_result > 0.0DL) {
        printf("  结论: NPV > 0, 项目值得投资\n");
    } else {
        printf("  结论: NPV < 0, 项目不值得投资\n");
    }

    return 0;
}
