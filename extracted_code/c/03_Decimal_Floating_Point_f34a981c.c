/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 792
 * Language: c
 * Block ID: f34a981c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal64_advanced.c -o decimal64_advanced
 *
 * _Decimal64 高级运算示例
 */

#include <stdio.h>
#include <math.h>

/* 复利的精确计算 */
_Decimal64 compound_interest(_Decimal64 principal, _Decimal64 rate,
                              int times_per_year, int years) {
    /*
     * 复利公式: A = P × (1 + r/n)^(n×t)
     * P = 本金
     * r = 年利率
     * n = 每年复利次数
     * t = 年数
     */

    _Decimal64 n = (_Decimal64)times_per_year;
    _Decimal64 t = (_Decimal64)years;

    /* 计算 (1 + r/n) */
    _Decimal64 base = 1.0DD + (rate / n);

    /* 计算指数 n×t */
    int exponent = times_per_year * years;

    /* 手动计算幂 (因为 pow 可能不直接支持 _Decimal64) */
    _Decimal64 result = 1.0DD;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }

    return principal * result;
}

/* 贷款月供计算 */
_Decimal64 loan_payment(_Decimal64 principal, _Decimal64 monthly_rate, int months) {
    /*
     * 月供公式: M = P × [r(1+r)^n] / [(1+r)^n - 1]
     * P = 贷款本金
     * r = 月利率
     * n = 还款月数
     */

    if (monthly_rate == 0.0DD) {
        return principal / (_Decimal64)months;
    }

    /* 计算 (1+r)^n */
    _Decimal64 one_plus_r = 1.0DD + monthly_rate;
    _Decimal64 pow_n = 1.0DD;
    for (int i = 0; i < months; i++) {
        pow_n *= one_plus_r;
    }

    _Decimal64 numerator = monthly_rate * pow_n;
    _Decimal64 denominator = pow_n - 1.0DD;

    return principal * (numerator / denominator);
}

int main(void) {
    printf("=== _Decimal64 高级财务计算 ===\n\n");

    /* 1. 复利计算 */
    printf("1. 复利计算:\n");
    _Decimal64 principal = 10000.00DD;  /* 本金 10,000 */
    _Decimal64 annual_rate = 0.05DD;     /* 年利率 5% */
    int years = 10;

    printf("  本金: $%.2f\n", (double)principal);
    printf("  年利率: %.2f%%\n", (double)(annual_rate * 100.0DD));
    printf("  投资年限: %d 年\n", years);

    _Decimal64 different_frequencies[5];
    const char* freq_names[] = {"年复利", "半年复利", "季复利", "月复利", "日复利"};
    int frequencies[] = {1, 2, 4, 12, 365};

    for (int i = 0; i < 5; i++) {
        different_frequencies[i] = compound_interest(principal, annual_rate,
                                                      frequencies[i], years);
        printf("  %s: $%.2f\n", freq_names[i], (double)different_frequencies[i]);
    }
    printf("\n");

    /* 2. 贷款计算 */
    printf("2. 房屋贷款计算:\n");
    _Decimal64 loan_amount = 500000.00DD;  /* 贷款 50万 */
    _Decimal64 annual_loan_rate = 0.045DD;  /* 年利率 4.5% */
    int loan_years = 30;
    int total_months = loan_years * 12;
    _Decimal64 monthly_rate = annual_loan_rate / 12.0DD;

    _Decimal64 monthly_payment = loan_payment(loan_amount, monthly_rate, total_months);
    _Decimal64 total_payment = monthly_payment * (_Decimal64)total_months;
    _Decimal64 total_interest = total_payment - loan_amount;

    printf("  贷款金额: $%.2f\n", (double)loan_amount);
    printf("  年利率: %.2f%%\n", (double)(annual_loan_rate * 100.0DD));
    printf("  贷款期限: %d 年\n", loan_years);
    printf("  月供: $%.2f\n", (double)monthly_payment);
    printf("  总还款: $%.2f\n", (double)total_payment);
    printf("  总利息: $%.2f\n\n", (double)total_interest);

    /* 3. 税务计算 */
    printf("3. 阶梯税率计算:\n");
    _Decimal64 income = 150000.00DD;  /* 年收入 15万 */

    /* 简化阶梯税率 */
    _Decimal64 tax = 0.0DD;
    _Decimal64 remaining = income;

    /* 0-50,000: 10% */
    _Decimal64 bracket1 = 50000.00DD;
    _Decimal64 rate1 = 0.10DD;
    _Decimal64 taxable1 = remaining < bracket1 ? remaining : bracket1;
    tax += taxable1 * rate1;
    remaining -= taxable1;

    /* 50,001-100,000: 20% */
    if (remaining > 0.0DD) {
        _Decimal64 bracket2 = 50000.00DD;
        _Decimal64 rate2 = 0.20DD;
        _Decimal64 taxable2 = remaining < bracket2 ? remaining : bracket2;
        tax += taxable2 * rate2;
        remaining -= taxable2;
    }

    /* 100,001+: 30% */
    if (remaining > 0.0DD) {
        _Decimal64 rate3 = 0.30DD;
        tax += remaining * rate3;
    }

    _Decimal64 effective_rate = tax / income;

    printf("  年收入: $%.2f\n", (double)income);
    printf("  应缴税款: $%.2f\n", (double)tax);
    printf("  税后收入: $%.2f\n", (double)(income - tax));
    printf("  实际税率: %.2f%%\n\n", (double)(effective_rate * 100.0DD));

    /* 4. 批量交易处理 */
    printf("4. 批量交易处理:\n");
    _Decimal64 transactions[] = {
        1234.56DD, 789.12DD, 3456.78DD, 901.23DD,
        5678.90DD, 234.56DD, 8901.23DD, 456.78DD,
        6789.01DD, 123.45DD
    };
    int num_transactions = sizeof(transactions) / sizeof(transactions[0]);

    _Decimal64 total_amount = 0.0DD;
    _Decimal64 max_transaction = transactions[0];
    _Decimal64 min_transaction = transactions[0];

    for (int i = 0; i < num_transactions; i++) {
        total_amount += transactions[i];
        if (transactions[i] > max_transaction) max_transaction = transactions[i];
        if (transactions[i] < min_transaction) min_transaction = transactions[i];
    }

    _Decimal64 avg_transaction = total_amount / (_Decimal64)num_transactions;

    printf("  交易数量: %d\n", num_transactions);
    printf("  总金额: $%.2f\n", (double)total_amount);
    printf("  平均金额: $%.2f\n", (double)avg_transaction);
    printf("  最大金额: $%.2f\n", (double)max_transaction);
    printf("  最小金额: $%.2f\n", (double)min_transaction);

    return 0;
}
