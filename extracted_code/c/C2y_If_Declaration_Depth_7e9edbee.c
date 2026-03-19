/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 1290
 * Language: c
 * Block ID: 7e9edbee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 5: if 声明与 for 声明对比
 * 演示两种声明控制结构的区别和适用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// C99 for 声明回顾
void for_declaration_demo(void) {
    printf("=== C99 for declaration ===\n");

    // for 声明：变量只在循环体内可见
    for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
    }
    // i 在这里不可见

    // 嵌套 for 声明
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("(%d, %d) ", i, j);
        }
        printf("\n");
    }
}

// C2y if 声明
void if_declaration_demo(void) {
    printf("\n=== C2y if declaration ===\n");

    // if 声明：变量在 if 和 else 分支都可见
    if (int x = 10; x > 5) {
        printf("if branch: x = %d\n", x);
    } else {
        printf("else branch: x = %d\n", x);  // x 在 else 分支也可见
    }
    // x 在这里不可见

    // 链式 if 声明
    if (int a = 1; a == 0) {
        printf("Case 0\n");
    } else if (int b = 2; b == 1) {
        printf("Case 1\n");
    } else if (int c = 3; c == 3) {
        printf("Case 3: a=%d, b=%d, c=%d\n", a, b, c);
    }
}

// 对比：资源获取
void resource_comparison(void) {
    printf("\n=== Resource acquisition comparison ===\n");

    // 传统方式（for 声明风格不适用）
    printf("Traditional way:\n");
    FILE* fp1 = fopen("test1.txt", "r");
    if (fp1 == NULL) {
        perror("fopen");
    } else {
        // 使用 fp1
        fclose(fp1);
    }
    // fp1 仍然可见

    // C2y if 声明方式
    printf("\nC2y if declaration way:\n");
    if (FILE* fp2 = fopen("test2.txt", "r"); fp2 == NULL) {
        perror("fopen");
    } else {
        // 使用 fp2
        fclose(fp2);
    }
    // fp2 不可见

    // 模拟 for 声明风格（不自然）
    printf("\nFor-style (awkward):\n");
    FILE* fp3;
    for (fp3 = fopen("test3.txt", "r"); fp3 != NULL; fclose(fp3), fp3 = NULL) {
        // 使用 fp3 - 但只执行一次
        break;
    }
    if (fp3 == NULL) {
        perror("fopen");
    }
}

// 对比：查找操作
void lookup_comparison(void) {
    printf("\n=== Lookup comparison ===\n");

    int key = 42;

    // 传统方式
    printf("Traditional way:\n");
    int* result1 = lookup_table1(key);
    if (result1 != NULL) {
        printf("Found in table1: %d\n", *result1);
    } else {
        int* result2 = lookup_table2(key);
        if (result2 != NULL) {
            printf("Found in table2: %d\n", *result2);
        } else {
            printf("Not found\n");
        }
    }
    // result1, result2 仍然可见

    // C2y if 声明方式
    printf("\nC2y if declaration way:\n");
    if (int* r1 = lookup_table1(key); r1 != NULL) {
        printf("Found in table1: %d\n", *r1);
    } else if (int* r2 = lookup_table2(key); r2 != NULL) {
        printf("Found in table2: %d\n", *r2);
    } else {
        printf("Not found\n");
    }
    // r1, r2 都不可见

    // for 声明方式（不适用）
    printf("\nFor-style (not applicable for lookups)\n");
}

// 对比：迭代 vs 条件
void iteration_vs_condition(void) {
    printf("\n=== Iteration vs Condition ===\n");

    // for 声明用于迭代
    printf("For declaration for iteration:\n");
    int data[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        printf("data[%d] = %d\n", i, data[i]);
    }

    // if 声明用于条件分支
    printf("\nIf declaration for conditions:\n");
    if (int sum = data[0] + data[1] + data[2]; sum > 50) {
        printf("Sum %d is greater than 50\n", sum);
    } else {
        printf("Sum %d is not greater than 50\n", sum);
    }

    // 组合使用
    printf("\nCombined usage:\n");
    for (int i = 0; i < 5; i++) {
        if (int doubled = data[i] * 2; doubled > 50) {
            printf("data[%d] = %d, doubled = %d (> 50)\n", i, data[i], doubled);
        }
    }
}

// 模拟查找函数
int table1_data = 100;
int table2_data = 200;

int* lookup_table1(int key) {
    (void)key;
    return NULL;  // 模拟未找到
}

int* lookup_table2(int key) {
    (void)key;
    return &table2_data;  // 模拟找到
}

int main(void) {
    for_declaration_demo();
    if_declaration_demo();
    resource_comparison();
    lookup_comparison();
    iteration_vs_condition();
    return 0;
}
