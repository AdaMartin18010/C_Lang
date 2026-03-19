/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 695
 * Language: c
 * Block ID: 6c14a989
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 1: if 声明的基本用法
 * 演示变量声明、条件判断和作用域规则
 */

#include <stdio.h>
#include <stdbool.h>

int compute_value(int input) {
    return input * 2 + 1;
}

void basic_usage_demo(void) {
    // 基本用法
    if (int x = compute_value(5); x > 10) {
        printf("x = %d is greater than 10\n", x);
    } else {
        printf("x = %d is not greater than 10\n", x);
    }
    // x 在这里不可见

    // 多变量声明
    if (int a = 10, b = 20; a + b > 25) {
        printf("Sum %d is greater than 25\n", a + b);
    }

    // 使用布尔类型
    if (bool flag = compute_value(3) > 5; flag) {
        printf("Flag is true\n");
    }

    // 复杂条件
    if (int val = compute_value(7); val > 10 && val < 20) {
        printf("Value %d is in range (10, 20)\n", val);
    }
}

int main(void) {
    printf("=== Basic Usage Demo ===\n");
    basic_usage_demo();
    return 0;
}
