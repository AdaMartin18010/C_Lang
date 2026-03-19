/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 2713
 * Language: c
 * Block ID: 035829c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * money_example.c - 货币库使用示例
 *
 * 编译: gcc -std=c23 moneylib.c money_example.c -o money_example
 */

#include "moneylib.h"
#include <stdio.h>

int main(void) {
    printf("=== 货币计算库使用示例 ===\n\n");

    /* 初始化 */
    money_init();

    /* 1. 创建货币对象 */
    printf("1. 创建货币对象:\n");
    Money* price = money_create(CURRENCY_USD, 199.99DD);
    Money* tax = money_create(CURRENCY_USD, 0.0875DD);

    char buffer[64];
    money_format(price, buffer, sizeof(buffer));
    printf("   商品价格: %s\n", buffer);

    /* 2. 计算税费 */
    printf("\n2. 计算税费:\n");
    MoneyError err;
    Money* tax_amount = money_percentage(price, 0.0875DD, &err);

    if (err == MONEY_OK) {
        money_format(tax_amount, buffer, sizeof(buffer));
        printf("   税费 (8.75%%): %s\n", buffer);
    } else {
        money_print_error(err);
    }

    /* 3. 计算总价 */
    printf("\n3. 计算总价:\n");
    Money* total = money_add(price, tax_amount, &err);

    if (err == MONEY_OK) {
        money_format(total, buffer, sizeof(buffer));
        printf("   含税总价: %s\n", buffer);
    }

    /* 4. 批量订单 */
    printf("\n4. 批量订单处理:\n");
    Money* items[] = {
        money_create(CURRENCY_USD, 49.99DD),
        money_create(CURRENCY_USD, 99.99DD),
        money_create(CURRENCY_USD, 29.99DD),
        money_create(CURRENCY_USD, 15.00DD)
    };

    int num_items = sizeof(items) / sizeof(items[0]);
    money_t subtotal = money_sum((const Money**)items, num_items, &err);

    printf("   订单包含 %d 项:\n", num_items);
    for (int i = 0; i < num_items; i++) {
        money_format(items[i], buffer, sizeof(buffer));
        printf("     项目 %d: %s\n", i + 1, buffer);
    }
    printf("   小计: $%.2f\n", (double)subtotal);

    money_t avg = money_average((const Money**)items, num_items, &err);
    printf("   平均价格: $%.2f\n", (double)avg);

    money_t min = money_min((const Money**)items, num_items);
    money_t max = money_max((const Money**)items, num_items);
    printf("   最低价格: $%.2f\n", (double)min);
    printf("   最高价格: $%.2f\n", (double)max);

    /* 5. 汇率转换 */
    printf("\n5. 汇率转换:\n");
    Money* usd_amount = money_create(CURRENCY_USD, 1000.00DD);
    Money* cny_amount = money_convert(usd_amount, CURRENCY_CNY, 7.2456DD, &err);

    if (err == MONEY_OK) {
        money_format(usd_amount, buffer, sizeof(buffer));
        printf("   原金额: %s\n", buffer);
        money_format(cny_amount, buffer, sizeof(buffer));
        printf("   转换后: %s (汇率 7.2456)\n", buffer);
    }

    /* 6. 复利计算 */
    printf("\n6. 复利计算:\n");
    money_t principal = 10000.00DD;
    rate_t annual_rate = 0.05DD;
    int years = 10;

    money_t final = money_calculate_compound_interest(principal, annual_rate, 12, years);

    printf("   本金: $%.2f\n", (double)principal);
    printf("   年利率: %.2f%%\n", (double)(annual_rate * 100.0DD));
    printf("   投资年限: %d 年 (月复利)\n", years);
    printf("   最终金额: $%.2f\n", (double)final);
    printf("   利息收入: $%.2f\n", (double)(final - principal));

    /* 清理 */
    money_free(price);
    money_free(tax);
    money_free(tax_amount);
    money_free(total);
    for (int i = 0; i < num_items; i++) {
        money_free(items[i]);
    }
    money_free(usd_amount);
    money_free(cny_amount);

    money_cleanup();

    printf("\n=== 示例完成 ===\n");

    return 0;
}
