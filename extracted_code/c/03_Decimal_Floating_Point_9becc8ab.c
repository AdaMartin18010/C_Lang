/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 1633
 * Language: c
 * Block ID: 9becc8ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 financial_best_practices.c -o financial_best_practices
 *
 * 财务计算最佳实践
 */

#include <stdio.h>
#include <stdbool.h>

/* ==================== 类型定义 ==================== */

/* 货币类型 - 使用 _Decimal64 作为默认 */
typedef _Decimal64 Money;

/* 利率类型 */
typedef _Decimal64 Rate;

/* 数量类型 */
typedef _Decimal64 Quantity;

/* ==================== 常量定义 ==================== */

/* 货币后缀 */
#define MONEY_SUFFIX DD
#define RATE_SUFFIX DD

/* 常用常量 */
#define ZERO_MONEY 0.00DD
#define ONE_HUNDRED_PERCENT 1.00DD

/* ==================== 舍入函数 ==================== */

/* 舍入到指定小数位 */
Money round_to_cents(Money amount) {
    /* 使用银行家舍入 (四舍六入五成双) */
    Money multiplier = 100.0DD;
    Money scaled = amount * multiplier;

    /* 手动实现银行家舍入 */
    Money int_part = (Money)(long long)scaled;
    Money frac_part = scaled - int_part;

    if (frac_part > 0.5DD) {
        int_part += 1.0DD;
    } else if (frac_part == 0.5DD) {
        /* 五成双：整数部分为偶数则舍去，为奇数则进位 */
        long long int_val = (long long)int_part;
        if (int_val % 2 != 0) {
            int_part += 1.0DD;
        }
    } else if (frac_part < -0.5DD) {
        int_part -= 1.0DD;
    } else if (frac_part == -0.5DD) {
        long long int_val = (long long)int_part;
        if (int_val % 2 != 0) {
            int_part -= 1.0DD;
        }
    }

    return int_part / multiplier;
}

/* 向上舍入 (财务常用) */
Money round_up_to_cents(Money amount) {
    Money multiplier = 100.0DD;
    Money scaled = amount * multiplier;
    Money int_part = (Money)(long long)scaled;

    if (scaled > int_part) {
        int_part += 1.0DD;
    } else if (scaled < int_part) {
        /* 负数处理 */
        /* 对于负数，向上舍入意味着向零靠近 */
    }

    return int_part / multiplier;
}

/* 向下舍入 */
Money round_down_to_cents(Money amount) {
    Money multiplier = 100.0DD;
    Money scaled = amount * multiplier;
    Money int_part = (Money)(long long)scaled;

    return int_part / multiplier;
}

/* ==================== 货币运算函数 ==================== */

/* 安全的加法 */
Money money_add(Money a, Money b) {
    return a + b;
}

/* 安全的减法 */
Money money_subtract(Money a, Money b) {
    return a - b;
}

/* 计算折扣 */
Money calculate_discount(Money original_price, Rate discount_rate) {
    return round_to_cents(original_price * discount_rate);
}

/* 计算税费 */
Money calculate_tax(Money amount, Rate tax_rate) {
    return round_to_cents(amount * tax_rate);
}

/* 计算最终价格 */
Money calculate_final_price(Money original_price, Rate discount_rate, Rate tax_rate) {
    Money discounted_price = original_price - calculate_discount(original_price, discount_rate);
    Money tax = calculate_tax(discounted_price, tax_rate);
    return discounted_price + tax;
}

/* 计算百分比 */
Rate calculate_percentage(Money part, Money total) {
    if (total == ZERO_MONEY) {
        return ZERO_MONEY;
    }
    return part / total;
}

/* ==================== 验证函数 ==================== */

/* 检查金额是否为非负 */
bool is_non_negative(Money amount) {
    return amount >= ZERO_MONEY;
}

/* 检查金额是否在合理范围内 */
bool is_reasonable_amount(Money amount) {
    Money max_reasonable = 999999999999.99DD;  /* 万亿级别 */
    Money min_reasonable = -999999999999.99DD;
    return amount >= min_reasonable && amount <= max_reasonable;
}

/* ==================== 演示 ==================== */

int main(void) {
    printf("=== 财务计算最佳实践 ===\n\n");

    /* 1. 舍入演示 */
    printf("1. 舍入函数演示:\n");
    Money amounts[] = {10.005DD, 10.015DD, 10.025DD, 10.035DD, -10.005DD};
    int n = sizeof(amounts) / sizeof(amounts[0]);

    printf("   银行家舍入 (四舍六入五成双):\n");
    for (int i = 0; i < n; i++) {
        printf("   %.3f -> %.2f\n", (double)amounts[i],
               (double)round_to_cents(amounts[i]));
    }
    printf("\n");

    /* 2. 完整交易计算 */
    printf("2. 完整交易计算:\n");
    Money price = 199.99DD;
    Rate discount = 0.15DD;  /* 15% 折扣 */
    Rate tax = 0.0875DD;     /* 8.75% 税 */

    Money final = calculate_final_price(price, discount, tax);
    Money discount_amount = calculate_discount(price, discount);
    Money tax_amount = calculate_tax(price - discount_amount, tax);

    printf("   原价: $%.2f\n", (double)price);
    printf("   折扣 (15%%): -$%.2f\n", (double)discount_amount);
    printf("   折后价: $%.2f\n", (double)(price - discount_amount));
    printf("   税费 (8.75%%): +$%.2f\n", (double)tax_amount);
    printf("   最终价格: $%.2f\n\n", (double)final);

    /* 3. 批量处理 */
    printf("3. 批量订单处理:\n");
    struct {
        char* name;
        Money unit_price;
        Quantity quantity;
        Rate item_discount;
    } items[] = {
        {"Widget A", 49.99DD, 10.0DD, 0.10DD},
        {"Widget B", 99.99DD, 5.0DD, 0.15DD},
        {"Widget C", 29.99DD, 20.0DD, 0.05DD}
    };

    int num_items = sizeof(items) / sizeof(items[0]);
    Money subtotal = ZERO_MONEY;

    for (int i = 0; i < num_items; i++) {
        Money line_total = items[i].unit_price * items[i].quantity;
        Money line_discount = calculate_discount(line_total, items[i].item_discount);
        Money line_net = line_total - line_discount;

        printf("   %s:\n", items[i].name);
        printf("     单价: $%.2f x %.0f\n",
               (double)items[i].unit_price, (double)items[i].quantity);
        printf("     小计: $%.2f\n", (double)line_total);
        printf("     折扣: -$%.2f\n", (double)line_discount);
        printf("     净额: $%.2f\n", (double)line_net);

        subtotal += line_net;
    }

    Money order_tax = calculate_tax(subtotal, 0.0875DD);
    Money order_total = subtotal + order_tax;

    printf("   -------------------------\n");
    printf("   订单小计: $%.2f\n", (double)subtotal);
    printf("   税费: $%.2f\n", (double)order_tax);
    printf("   订单总计: $%.2f\n\n", (double)order_total);

    /* 4. 验证函数 */
    printf("4. 金额验证:\n");
    Money valid_amount = 1000.00DD;
    Money invalid_amount = -500.00DD;
    Money huge_amount = 9999999999999.99DD;

    printf("   $%.2f 是非负? %s\n", (double)valid_amount,
           is_non_negative(valid_amount) ? "是" : "否");
    printf("   $%.2f 是非负? %s\n", (double)invalid_amount,
           is_non_negative(invalid_amount) ? "是" : "否");
    printf("   $%.2f 是合理金额? %s\n", (double)huge_amount,
           is_reasonable_amount(huge_amount) ? "是" : "否");

    return 0;
}
