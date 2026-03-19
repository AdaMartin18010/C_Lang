/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 1131
 * Language: c
 * Block ID: 9a89f98a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file formatting_example.c
 * @brief C代码格式规范示例
 *
 * 格式规范：
 * - 缩进: 4空格（不使用Tab）
 * - 行宽: 最大100字符
 * - 括号: K&R风格（函数大括号换行，控制语句不换行）
 * - 空格: 操作符两侧、逗号后、分号后（for循环）
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 函数定义 ========== */

/**
 * @brief 计算两个整数的和
 *
 * 函数大括号在新行（K&R风格）
 */
int calculate_sum(int a, int b)
{
    return a + b;
}

/**
 * @brief 处理数组数据
 */
void process_array(const int *data, size_t count)
{
    /* 控制结构大括号在同一行 */
    if (data == NULL || count == 0) {
        fprintf(stderr, "Invalid input\n");
        return;
    }

    /* for循环格式 */
    for (size_t i = 0; i < count; i++) {
        printf("%d\n", data[i]);
    }

    /* while循环格式 */
    size_t remaining = count;
    while (remaining > 0) {
        remaining--;
    }

    /* do-while循环格式 */
    int retries = 3;
    do {
        retries--;
    } while (retries > 0);
}

/**
 * @brief 复杂条件格式
 */
int complex_condition_example(int a, int b, int c, int d)
{
    /* 简单条件：单行 */
    if (a > 0) {
        return 1;
    }

    /* 复杂条件：多行，操作符在前 */
    if (a > 0 && b > 0 && c > 0 && d > 0 &&
        (a + b) < 100 &&
        (c * d) > 50) {
        return 2;
    }

    /* switch语句格式 */
    switch (a) {
        case 0:
            printf("Zero\n");
            break;
        case 1:
        case 2:
            printf("One or Two\n");
            break;
        default:
            printf("Other: %d\n", a);
            break;
    }

    /* 长参数列表：每行一个参数 */
    int result = some_function_with_many_params(
        param_one,
        param_two,
        param_three,
        param_four
    );
    (void)result;

    return 0;
}

/* ========== 垂直间距 ========== */

/* 相关语句分组 */
typedef struct {
    int x;
    int y;
} point_t;

/* 空行分隔不同逻辑组 */
typedef struct {
    point_t top_left;
    point_t bottom_right;
} rectangle_t;

/* 函数之间用两个空行分隔 */


void function_one(void)
{
    /* 局部变量声明在一起 */
    int a = 1;
    int b = 2;
    int c = 3;

    /* 空行分隔不同逻辑步骤 */
    int sum = a + b + c;

    /* 另一逻辑步骤 */
    double average = sum / 3.0;
    (void)average;
}


void function_two(void)
{
    /* ... */
}


/* ========== 水平对齐 ========== */

/* 相关声明可以对齐（可选） */
typedef enum {
    ERROR_NONE          = 0,
    ERROR_INVALID_PARAM = -1,
    ERROR_OUT_OF_MEMORY = -2,
    ERROR_IO_FAILED     = -3,
    ERROR_TIMEOUT       = -4
} error_code_t;

/* 结构体字段对齐（可选） */
typedef struct {
    char     name[32];
    int      id;
    double   score;
    uint32_t flags;
} student_t;

/* ========== 行长控制 ========== */

void long_line_example(void)
{
    /* ❌ 错误：行过长 */
    /* int result = very_long_function_name(first_very_long_parameter, second_very_long_parameter, third_parameter); */

    /* ✅ 正确：合理断行 */
    int result = very_long_function_name(
        first_very_long_parameter,
        second_very_long_parameter,
        third_parameter
    );

    /* 字符串常量断行 */
    const char *message =
        "This is a very long message that needs to be split "
        "across multiple lines for better readability.";
    (void)message;
    (void)result;
}

/* 占位函数，仅用于编译 */
static int very_long_function_name(int a, int b, int c) { (void)a; (void)b; (void)c; return 0; }
static int param_one, param_two, param_three, param_four;
static int some_function_with_many_params(int a, int b, int c, int d) { (void)a;(void)b;(void)c;(void)d; return 0; }

int main(void)
{
    return 0;
}
