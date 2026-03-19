/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 2211
 * Language: c
 * Block ID: 4c284022
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * moneylib.c - 货币计算库完整实现
 *
 * 编译: gcc -std=c23 -c moneylib.c -o moneylib.o
 */

#include "moneylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ==================== 内部辅助函数 ==================== */

/* 复制货币代码 */
static void copy_currency_code(char* dest, const char* src) {
    if (src == NULL || strlen(src) != 3) {
        strcpy(dest, "XXX");  /* 未知货币 */
    } else {
        strncpy(dest, src, 3);
        dest[3] = '\0';
    }
}

/* 检查货币代码是否匹配 */
static bool currencies_match(const char* a, const char* b) {
    return strcmp(a, b) == 0;
}

/* 执行舍入 */
static money_t do_round(money_t value, int decimal_places, RoundingMode mode) {
    money_t multiplier = MONEY_ONE;
    for (int i = 0; i < decimal_places; i++) {
        multiplier *= (money_t)10.0DD;
    }

    money_t scaled = value * multiplier;
    money_t int_part = (money_t)(long long)scaled;
    money_t frac_part = scaled - int_part;
    money_t abs_frac = frac_part < MONEY_ZERO ? -frac_part : frac_part;

    switch (mode) {
        case ROUND_HALF_UP:
            if (abs_frac >= (money_t)0.5DD) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            }
            break;

        case ROUND_HALF_DOWN:
            if (abs_frac > (money_t)0.5DD) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            }
            break;

        case ROUND_HALF_EVEN:
            if (abs_frac > (money_t)0.5DD) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            } else if (abs_frac == (money_t)0.5DD) {
                long long int_val = (long long)int_part;
                if (int_val % 2 != 0) {
                    int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
                }
            }
            break;

        case ROUND_UP:
            if (abs_frac > MONEY_ZERO) {
                int_part += (frac_part >= MONEY_ZERO) ? MONEY_ONE : -MONEY_ONE;
            }
            break;

        case ROUND_DOWN:
            /* 截断，什么都不做 */
            break;

        case ROUND_CEILING:
            if (frac_part > MONEY_ZERO) {
                int_part += MONEY_ONE;
            }
            break;

        case ROUND_FLOOR:
            if (frac_part < MONEY_ZERO) {
                int_part -= MONEY_ONE;
            }
            break;
    }

    return int_part / multiplier;
}

/* ==================== 初始化 ==================== */

MoneyError money_init(void) {
    /* 可以在这里设置全局配置 */
    return MONEY_OK;
}

void money_cleanup(void) {
    /* 清理资源 */
}

/* ==================== 创建和销毁 ==================== */

Money* money_create(const char* currency_code, money_t amount) {
    if (!money_is_valid_amount(amount)) {
        return NULL;
    }

    Money* m = (Money*)malloc(sizeof(Money));
    if (m == NULL) {
        return NULL;
    }

    m->amount = amount;
    copy_currency_code(m->currency_code, currency_code);
    m->rounding = ROUND_HALF_EVEN;

    return m;
}

Money* money_create_zero(const char* currency_code) {
    return money_create(currency_code, MONEY_ZERO);
}

void money_free(Money* m) {
    free(m);
}

/* ==================== 基本操作 ==================== */

MoneyError money_set_amount(Money* m, money_t amount) {
    if (m == NULL) {
        return MONEY_ERR_NULL_PTR;
    }
    if (!money_is_valid_amount(amount)) {
        return MONEY_ERR_NEGATIVE_AMOUNT;
    }

    m->amount = amount;
    return MONEY_OK;
}

money_t money_get_amount(const Money* m) {
    return (m == NULL) ? MONEY_ZERO : m->amount;
}

const char* money_get_currency(const Money* m) {
    return (m == NULL) ? "XXX" : m->currency_code;
}

MoneyError money_set_rounding(Money* m, RoundingMode mode) {
    if (m == NULL) {
        return MONEY_ERR_NULL_PTR;
    }
    m->rounding = mode;
    return MONEY_OK;
}

/* ==================== 算术运算 ==================== */

Money* money_add(const Money* a, const Money* b, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (a == NULL || b == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (!currencies_match(a->currency_code, b->currency_code)) {
        if (err != NULL) *err = MONEY_ERR_CURRENCY_MISMATCH;
        return NULL;
    }

    money_t result = a->amount + b->amount;
    return money_create(a->currency_code, result);
}

Money* money_subtract(const Money* a, const Money* b, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (a == NULL || b == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (!currencies_match(a->currency_code, b->currency_code)) {
        if (err != NULL) *err = MONEY_ERR_CURRENCY_MISMATCH;
        return NULL;
    }

    money_t result = a->amount - b->amount;
    return money_create(a->currency_code, result);
}

Money* money_multiply(const Money* m, quantity_t multiplier, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    money_t result = m->amount * multiplier;
    result = do_round(result, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(m->currency_code, result);
}

Money* money_divide(const Money* m, quantity_t divisor, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (divisor == (quantity_t)0.0DD) {
        if (err != NULL) *err = MONEY_ERR_DIVISION_BY_ZERO;
        return NULL;
    }

    money_t result = m->amount / divisor;
    result = do_round(result, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(m->currency_code, result);
}

Money* money_percentage(const Money* m, rate_t percent, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    money_t result = m->amount * percent;
    result = do_round(result, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(m->currency_code, result);
}

/* ==================== 舍入 ==================== */

money_t money_round(money_t amount, int decimal_places, RoundingMode mode) {
    return do_round(amount, decimal_places, mode);
}

money_t money_round_to_cents(money_t amount) {
    return do_round(amount, 2, ROUND_HALF_EVEN);
}

/* ==================== 比较 ==================== */

int money_compare(const Money* a, const Money* b) {
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }

    if (!currencies_match(a->currency_code, b->currency_code)) {
        /* 不同货币，按货币代码比较 */
        return strcmp(a->currency_code, b->currency_code);
    }

    if (a->amount < b->amount) return -1;
    if (a->amount > b->amount) return 1;
    return 0;
}

bool money_equals(const Money* a, const Money* b) {
    return money_compare(a, b) == 0;
}

bool money_less_than(const Money* a, const Money* b) {
    return money_compare(a, b) < 0;
}

bool money_greater_than(const Money* a, const Money* b) {
    return money_compare(a, b) > 0;
}

bool money_is_zero(const Money* m) {
    return (m == NULL) ? true : (m->amount == MONEY_ZERO);
}

bool money_is_positive(const Money* m) {
    return (m == NULL) ? false : (m->amount > MONEY_ZERO);
}

bool money_is_negative(const Money* m) {
    return (m == NULL) ? false : (m->amount < MONEY_ZERO);
}

/* ==================== 汇率转换 ==================== */

Money* money_convert(const Money* m, const char* target_currency,
                     rate_t exchange_rate, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (m == NULL || target_currency == NULL) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return NULL;
    }

    if (exchange_rate <= (rate_t)0.0DD) {
        if (err != NULL) *err = MONEY_ERR_INVALID_RATE;
        return NULL;
    }

    money_t converted = m->amount * exchange_rate;
    converted = do_round(converted, DEFAULT_DECIMAL_PLACES, m->rounding);

    return money_create(target_currency, converted);
}

/* ==================== 财务计算 ==================== */

money_t money_calculate_discount(money_t original, rate_t discount_rate) {
    money_t discount = original * discount_rate;
    return do_round(discount, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

money_t money_calculate_tax(money_t amount, rate_t tax_rate) {
    money_t tax = amount * tax_rate;
    return do_round(tax, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

money_t money_calculate_interest(money_t principal, rate_t rate, int periods) {
    money_t interest = principal * rate * (money_t)periods;
    return do_round(interest, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

money_t money_calculate_compound_interest(money_t principal, rate_t rate,
                                           int times_per_year, int years) {
    quantity_t n = (quantity_t)times_per_year;
    quantity_t base = (quantity_t)1.0DD + rate / n;

    int exponent = times_per_year * years;
    quantity_t factor = (quantity_t)1.0DD;

    for (int i = 0; i < exponent; i++) {
        factor *= base;
    }

    money_t result = principal * (money_t)factor;
    return do_round(result, DEFAULT_DECIMAL_PLACES, ROUND_HALF_UP);
}

/* ==================== 批量处理 ==================== */

money_t money_sum(const Money** monies, int count, MoneyError* err) {
    if (err != NULL) *err = MONEY_OK;

    if (monies == NULL && count > 0) {
        if (err != NULL) *err = MONEY_ERR_NULL_PTR;
        return MONEY_ZERO;
    }

    if (count == 0) {
        return MONEY_ZERO;
    }

    const char* currency = monies[0]->currency_code;
    money_t total = MONEY_ZERO;

    for (int i = 0; i < count; i++) {
        if (monies[i] == NULL) {
            if (err != NULL) *err = MONEY_ERR_NULL_PTR;
            return MONEY_ZERO;
        }
        if (!currencies_match(monies[i]->currency_code, currency)) {
            if (err != NULL) *err = MONEY_ERR_CURRENCY_MISMATCH;
            return MONEY_ZERO;
        }
        total += monies[i]->amount;
    }

    return total;
}

money_t money_average(const Money** monies, int count, MoneyError* err) {
    money_t sum = money_sum(monies, count, err);
    if (err != NULL && *err != MONEY_OK) {
        return MONEY_ZERO;
    }

    if (count == 0) {
        if (err != NULL) *err = MONEY_ERR_DIVISION_BY_ZERO;
        return MONEY_ZERO;
    }

    return sum / (money_t)count;
}

money_t money_min(const Money** monies, int count) {
    if (monies == NULL || count == 0) {
        return MONEY_ZERO;
    }

    money_t min = monies[0]->amount;
    for (int i = 1; i < count; i++) {
        if (monies[i] != NULL && monies[i]->amount < min) {
            min = monies[i]->amount;
        }
    }

    return min;
}

money_t money_max(const Money** monies, int count) {
    if (monies == NULL || count == 0) {
        return MONEY_ZERO;
    }

    money_t max = monies[0]->amount;
    for (int i = 1; i < count; i++) {
        if (monies[i] != NULL && monies[i]->amount > max) {
            max = monies[i]->amount;
        }
    }

    return max;
}

/* ==================== 格式化 ==================== */

int money_format(const Money* m, char* buffer, size_t buffer_size) {
    if (m == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }

    return snprintf(buffer, buffer_size, "%s %.2f",
                    m->currency_code, (double)m->amount);
}

int money_format_with_symbol(const Money* m, char* buffer, size_t buffer_size) {
    if (m == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }

    const char* symbol = "$";  /* 简化处理，实际应根据货币代码选择符号 */
    if (strcmp(m->currency_code, CURRENCY_EUR) == 0) symbol = "€";
    else if (strcmp(m->currency_code, CURRENCY_GBP) == 0) symbol = "£";
    else if (strcmp(m->currency_code, CURRENCY_JPY) == 0) symbol = "¥";
    else if (strcmp(m->currency_code, CURRENCY_CNY) == 0) symbol = "¥";

    return snprintf(buffer, buffer_size, "%s%.2f %s",
                    symbol, (double)m->amount, m->currency_code);
}

/* ==================== 验证 ==================== */

bool money_is_valid_amount(money_t amount) {
    /* 检查是否为有限数 */
    /* 注意: C23 十进制浮点应始终有限，除非显式创建无穷或 NaN */

    /* 检查是否在合理范围内 */
    money_t max = (money_t)999999999999.99DD;  /* 万亿级别 */
    money_t min = (money_t)-999999999999.99DD;

    return (amount >= min && amount <= max);
}

bool money_is_valid_currency_code(const char* code) {
    if (code == NULL || strlen(code) != 3) {
        return false;
    }

    /* 检查是否都是大写字母 */
    for (int i = 0; i < 3; i++) {
        if (code[i] < 'A' || code[i] > 'Z') {
            return false;
        }
    }

    return true;
}

/* ==================== 错误处理 ==================== */

const char* money_error_string(MoneyError err) {
    switch (err) {
        case MONEY_OK: return "成功";
        case MONEY_ERR_NULL_PTR: return "空指针错误";
        case MONEY_ERR_NEGATIVE_AMOUNT: return "金额不能为负";
        case MONEY_ERR_OVERFLOW: return "数值溢出";
        case MONEY_ERR_INVALID_RATE: return "无效的汇率/费率";
        case MONEY_ERR_DIVISION_BY_ZERO: return "除零错误";
        case MONEY_ERR_PRECISION_LOSS: return "精度丢失警告";
        case MONEY_ERR_CURRENCY_MISMATCH: return "货币类型不匹配";
        default: return "未知错误";
    }
}

void money_print_error(MoneyError err) {
    fprintf(stderr, "货币计算错误: %s\n", money_error_string(err));
}
