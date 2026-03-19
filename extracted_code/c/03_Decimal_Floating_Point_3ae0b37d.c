/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 628
 * Language: c
 * Block ID: 3ae0b37d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 decimal32_operations.c -o decimal32_operations
 *
 * _Decimal32 基本运算
 */

#include <stdio.h>

/* 打印 _Decimal32 数组 */
void print_prices(const char* title, _Decimal32* prices, int count) {
    printf("%s:\n", title);
    for (int i = 0; i < count; i++) {
        printf("  [%d]: $%.2f\n", i, (double)prices[i]);
    }
    printf("\n");
}

int main(void) {
    printf("=== _Decimal32 运算示例 ===\n\n");

    /* 基本算术运算 */
    printf("1. 基本算术运算:\n");
    _Decimal32 a = 123.456DF;
    _Decimal32 b = 78.9DF;

    _Decimal32 sum = a + b;
    _Decimal32 diff = a - b;
    _Decimal32 prod = a * b;
    _Decimal32 quot = a / b;

    printf("  a = %.3f\n", (double)a);
    printf("  b = %.3f\n", (double)b);
    printf("  a + b = %.3f\n", (double)sum);
    printf("  a - b = %.3f\n", (double)diff);
    printf("  a * b = %.3f\n", (double)prod);
    printf("  a / b = %.6f\n\n", (double)quot);

    /* 价格计算示例 */
    printf("2. 零售价格计算:\n");
    _Decimal32 base_price = 29.99DF;
    _Decimal32 discount_rate = 0.15DF;  /* 15% 折扣 */
    _Decimal32 tax_rate = 0.08DF;       /* 8% 税费 */

    _Decimal32 discount_amount = base_price * discount_rate;
    _Decimal32 discounted_price = base_price - discount_amount;
    _Decimal32 tax_amount = discounted_price * tax_rate;
    _Decimal32 final_price = discounted_price + tax_amount;

    printf("  原价: $%.2f\n", (double)base_price);
    printf("  折扣 (15%%): -$%.2f\n", (double)discount_amount);
    printf("  折后价: $%.2f\n", (double)discounted_price);
    printf("  税费 (8%%): +$%.2f\n", (double)tax_amount);
    printf("  最终价格: $%.2f\n\n", (double)final_price);

    /* 批量处理 */
    printf("3. 批量价格处理:\n");
    _Decimal32 prices[] = {9.99DF, 19.99DF, 29.99DF, 49.99DF, 99.99DF};
    int n = sizeof(prices) / sizeof(prices[0]);

    _Decimal32 subtotal = 0.0DF;
    for (int i = 0; i < n; i++) {
        subtotal += prices[i];
    }

    print_prices("商品价格", prices, n);
    printf("  小计: $%.2f\n", (double)subtotal);

    _Decimal32 avg_price = subtotal / n;
    printf("  平均价格: $%.2f\n\n", (double)avg_price);

    /* 精度限制演示 */
    printf("4. 精度限制 (7位有效数字):\n");
    _Decimal32 precise = 1234567.0DF;  /* 正好7位 */
    _Decimal32 overflow = 12345678.0DF; /* 8位 - 可能丢失精度 */

    printf("  7位数字 1234567: %.1f (精确)\n", (double)precise);
    printf("  8位数字 12345678: %.1f (注意精度)\n\n", (double)overflow);

    /* 货币转换 */
    printf("5. 货币转换:\n");
    _Decimal32 usd_amount = 100.00DF;
    _Decimal32 exchange_rate = 7.2456DF; /* USD to CNY */

    _Decimal32 cny_amount = usd_amount * exchange_rate;
    printf("  USD %.2f = CNY %.4f\n", (double)usd_amount, (double)cny_amount);

    return 0;
}
