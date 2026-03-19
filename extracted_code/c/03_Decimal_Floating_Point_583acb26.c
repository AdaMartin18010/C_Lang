/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 2062
 * Language: c
 * Block ID: 583acb26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * moneylib.h - 货币计算库头文件
 *
 * 编译: gcc -std=c23 moneylib.c example.c -o money_example
 */

#ifndef MONEYLIB_H
#define MONEYLIB_H

#include <stdbool.h>
#include <stdint.h>

/* ==================== 类型定义 ==================== */

/* 货币类型 - 使用 _Decimal64 确保精确计算 */
typedef _Decimal64 money_t;

/* 利率类型 */
typedef _Decimal64 rate_t;

/* 数量类型 */
typedef _Decimal64 quantity_t;

/* 错误代码 */
typedef enum {
    MONEY_OK = 0,
    MONEY_ERR_NULL_PTR,
    MONEY_ERR_NEGATIVE_AMOUNT,
    MONEY_ERR_OVERFLOW,
    MONEY_ERR_INVALID_RATE,
    MONEY_ERR_DIVISION_BY_ZERO,
    MONEY_ERR_PRECISION_LOSS,
    MONEY_ERR_CURRENCY_MISMATCH
} MoneyError;

/* 舍入模式 */
typedef enum {
    ROUND_HALF_UP,      /* 四舍五入 */
    ROUND_HALF_DOWN,    /* 五舍六入 */
    ROUND_HALF_EVEN,    /* 银行家舍入 (默认) */
    ROUND_UP,           /* 向上舍入 */
    ROUND_DOWN,         /* 向下舍入 (截断) */
    ROUND_CEILING,      /* 向正无穷舍入 */
    ROUND_FLOOR         /* 向负无穷舍入 */
} RoundingMode;

/* 货币结构体 */
typedef struct {
    money_t amount;          /* 金额 */
    char currency_code[4];   /* ISO 4217 货币代码 (如 "USD", "CNY") */
    RoundingMode rounding;   /* 舍入模式 */
} Money;

/* 货币汇率 */
typedef struct {
    char from_currency[4];
    char to_currency[4];
    rate_t rate;
} ExchangeRate;

/* ==================== 常量 ==================== */

/* 货币代码 */
#define CURRENCY_USD "USD"
#define CURRENCY_CNY "CNY"
#define CURRENCY_EUR "EUR"
#define CURRENCY_JPY "JPY"
#define CURRENCY_GBP "GBP"

/* 常用常量 */
#define MONEY_ZERO ((money_t)0.00DD)
#define MONEY_ONE ((money_t)1.00DD)
#define MONEY_HUNDRED ((money_t)100.00DD)

/* 默认小数位 */
#define DEFAULT_DECIMAL_PLACES 2

/* ==================== 函数声明 ==================== */

/* 初始化 */
MoneyError money_init(void);
void money_cleanup(void);

/* 创建和销毁 */
Money* money_create(const char* currency_code, money_t amount);
Money* money_create_zero(const char* currency_code);
void money_free(Money* m);

/* 基本操作 */
MoneyError money_set_amount(Money* m, money_t amount);
money_t money_get_amount(const Money* m);
const char* money_get_currency(const Money* m);
MoneyError money_set_rounding(Money* m, RoundingMode mode);

/* 算术运算 */
Money* money_add(const Money* a, const Money* b, MoneyError* err);
Money* money_subtract(const Money* a, const Money* b, MoneyError* err);
Money* money_multiply(const Money* m, quantity_t multiplier, MoneyError* err);
Money* money_divide(const Money* m, quantity_t divisor, MoneyError* err);
Money* money_percentage(const Money* m, rate_t percent, MoneyError* err);

/* 舍入 */
money_t money_round(money_t amount, int decimal_places, RoundingMode mode);
money_t money_round_to_cents(money_t amount);

/* 比较 */
int money_compare(const Money* a, const Money* b);
bool money_equals(const Money* a, const Money* b);
bool money_less_than(const Money* a, const Money* b);
bool money_greater_than(const Money* a, const Money* b);
bool money_is_zero(const Money* m);
bool money_is_positive(const Money* m);
bool money_is_negative(const Money* m);

/* 汇率转换 */
Money* money_convert(const Money* m, const char* target_currency,
                     rate_t exchange_rate, MoneyError* err);

/* 财务计算 */
money_t money_calculate_discount(money_t original, rate_t discount_rate);
money_t money_calculate_tax(money_t amount, rate_t tax_rate);
money_t money_calculate_interest(money_t principal, rate_t rate, int periods);
money_t money_calculate_compound_interest(money_t principal, rate_t rate,
                                           int times_per_year, int years);

/* 批量处理 */
money_t money_sum(const Money** monies, int count, MoneyError* err);
money_t money_average(const Money** monies, int count, MoneyError* err);
money_t money_min(const Money** monies, int count);
money_t money_max(const Money** monies, int count);

/* 格式化 */
int money_format(const Money* m, char* buffer, size_t buffer_size);
int money_format_with_symbol(const Money* m, char* buffer, size_t buffer_size);

/* 验证 */
bool money_is_valid_amount(money_t amount);
bool money_is_valid_currency_code(const char* code);

/* 错误处理 */
const char* money_error_string(MoneyError err);
void money_print_error(MoneyError err);

#endif /* MONEYLIB_H */
