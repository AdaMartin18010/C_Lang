/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1863
 * Language: c
 * Block ID: a9712cb2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 financial_validation.c -o financial_validation
 *
 * 财务数据验证和错误处理
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* 错误代码 */
typedef enum {
    ERR_NONE = 0,
    ERR_NEGATIVE_AMOUNT,
    ERR_OVERFLOW,
    ERR_INVALID_RATE,
    ERR_DIVISION_BY_ZERO,
    ERR_PRECISION_LOSS,
    ERR_INVALID_INPUT
} FinancialError;

/* 结果结构体 */
typedef struct {
    _Decimal64 value;
    FinancialError error;
    const char* error_message;
} FinancialResult;

/* 错误消息 */
const char* get_error_message(FinancialError err) {
    switch (err) {
        case ERR_NONE: return "成功";
        case ERR_NEGATIVE_AMOUNT: return "金额不能为负";
        case ERR_OVERFLOW: return "数值溢出";
        case ERR_INVALID_RATE: return "无效的费率";
        case ERR_DIVISION_BY_ZERO: return "除零错误";
        case ERR_PRECISION_LOSS: return "精度丢失警告";
        case ERR_INVALID_INPUT: return "无效输入";
        default: return "未知错误";
    }
}

/* 安全的除法 */
FinancialResult safe_divide(_Decimal64 numerator, _Decimal64 denominator) {
    FinancialResult result = {0};

    if (denominator == 0.0DD) {
        result.error = ERR_DIVISION_BY_ZERO;
        result.error_message = get_error_message(ERR_DIVISION_BY_ZERO);
        return result;
    }

    result.value = numerator / denominator;
    result.error = ERR_NONE;
    result.error_message = get_error_message(ERR_NONE);
    return result;
}

/* 安全的费率计算 */
FinancialResult safe_rate_calculation(_Decimal64 amount, _Decimal64 rate) {
    FinancialResult result = {0};

    /* 验证费率范围 */
    if (rate < 0.0DD || rate > 100.0DD) {
        result.error = ERR_INVALID_RATE;
        result.error_message = get_error_message(ERR_INVALID_RATE);
        return result;
    }

    /* 验证金额 */
    if (amount < 0.0DD) {
        result.error = ERR_NEGATIVE_AMOUNT;
        result.error_message = get_error_message(ERR_NEGATIVE_AMOUNT);
        return result;
    }

    /* 计算并检查溢出 */
    _Decimal64 temp = amount * rate;
    if (temp > 999999999999.99DD) {
        result.error = ERR_OVERFLOW;
        result.error_message = get_error_message(ERR_OVERFLOW);
        return result;
    }

    result.value = temp;
    result.error = ERR_NONE;
    result.error_message = get_error_message(ERR_NONE);
    return result;
}

/* 批量计算验证 */
bool validate_transaction_batch(_Decimal64* amounts, int count,
                                 _Decimal64* expected_total) {
    _Decimal64 calculated_total = 0.0DD;

    for (int i = 0; i < count; i++) {
        /* 检查每个金额 */
        if (amounts[i] < 0.0DD) {
            printf("错误: 第 %d 项金额为负: %.2f\n", i, (double)amounts[i]);
            return false;
        }

        /* 检查累加溢出 */
        _Decimal64 new_total = calculated_total + amounts[i];
        if (new_total < calculated_total) {
            printf("错误: 累加溢出\n");
            return false;
        }

        calculated_total = new_total;
    }

    /* 验证总和 */
    if (expected_total != NULL) {
        if (calculated_total != *expected_total) {
            printf("警告: 计算总和 (%.2f) 与预期 (%.2f) 不符\n",
                   (double)calculated_total, (double)*expected_total);
            return false;
        }
    }

    printf("验证通过: 总和 = %.2f\n", (double)calculated_total);
    return true;
}

/* 精度检查 */
bool check_precision(_Decimal64 value, int expected_decimal_places) {
    _Decimal64 multiplier = 1.0DD;
    for (int i = 0; i < expected_decimal_places; i++) {
        multiplier *= 10.0DD;
    }

    _Decimal64 scaled = value * multiplier;
    _Decimal64 rounded = (_Decimal64)(long long)scaled;

    return (scaled == rounded);
}

int main(void) {
    printf("=== 财务数据验证和错误处理 ===\n\n");

    /* 1. 除法错误处理 */
    printf("1. 安全除法:\n");
    FinancialResult res1 = safe_divide(100.0DD, 0.0DD);
    printf("   100.0 / 0.0: %s\n", res1.error == ERR_NONE ? "成功" : res1.error_message);

    FinancialResult res2 = safe_divide(100.0DD, 4.0DD);
    printf("   100.0 / 4.0: %s, 结果 = %.2f\n\n",
           res2.error_message, (double)res2.value);

    /* 2. 费率计算验证 */
    printf("2. 费率计算验证:\n");
    FinancialResult res3 = safe_rate_calculation(1000.0DD, 0.08DD);
    printf("   计算 8%% 税费: %s, 结果 = %.2f\n",
           res3.error_message, (double)res3.value);

    FinancialResult res4 = safe_rate_calculation(-100.0DD, 0.08DD);
    printf("   负数金额: %s\n", res4.error_message);

    FinancialResult res5 = safe_rate_calculation(100.0DD, 150.0DD);
    printf("   无效费率: %s\n\n", res5.error_message);

    /* 3. 批量交易验证 */
    printf("3. 批量交易验证:\n");
    _Decimal64 transactions[] = {100.00DD, 200.50DD, 50.25DD, 300.75DD};
    int num_trans = sizeof(transactions) / sizeof(transactions[0]);
    _Decimal64 expected = 651.50DD;

    printf("   交易列表:\n");
    for (int i = 0; i < num_trans; i++) {
        printf("     交易 %d: $%.2f\n", i + 1, (double)transactions[i]);
    }
    printf("   预期总和: $%.2f\n", (double)expected);

    bool valid = validate_transaction_batch(transactions, num_trans, &expected);
    printf("   验证结果: %s\n\n", valid ? "通过" : "失败");

    /* 4. 精度检查 */
    printf("4. 精度检查:\n");
    _Decimal64 price1 = 99.99DD;
    _Decimal64 price2 = 99.999DD;

    printf("   %.2f 有2位小数精度? %s\n",
           (double)price1, check_precision(price1, 2) ? "是" : "否");
    printf("   %.3f 有2位小数精度? %s\n",
           (double)price2, check_precision(price2, 2) ? "是" : "否");

    return 0;
}
