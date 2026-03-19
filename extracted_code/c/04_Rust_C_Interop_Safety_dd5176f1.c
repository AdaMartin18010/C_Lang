/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 1016
 * Language: c
 * Block ID: dd5176f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C程序：调用Rust数学库 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rust_math_lib.h"

/* 测试基本数学函数 */
void test_basic_math(void) {
    printf("\n=== 基本数学函数测试 ===\n");

    /* 加法 */
    int sum = rust_add(10, 20);
    printf("rust_add(10, 20) = %d\n", sum);

    /* 乘法 */
    double product = rust_multiply(3.14, 2.0);
    printf("rust_multiply(3.14, 2.0) = %.4f\n", product);

    /* 阶乘 */
    int fact = rust_factorial(5);
    printf("rust_factorial(5) = %d\n", fact);

    /* 错误情况 */
    int bad_fact = rust_factorial(-1);
    printf("rust_factorial(-1) = %d (错误)\n", bad_fact);
}

/* 测试字符串处理 */
void test_string_processing(void) {
    printf("\n=== 字符串处理测试 ===\n");

    const char* input = "Hello, FFI World!";
    char* result = rust_process_string(input);

    if (result != NULL) {
        printf("输入: %s\n", input);
        printf("输出: %s\n", result);

        /* 重要：释放Rust分配的内存 */
        rust_free_string(result);
    } else {
        printf("字符串处理失败\n");
    }

    /* 测试空指针 */
    char* null_result = rust_process_string(NULL);
    if (null_result == NULL) {
        printf("正确处理空指针输入\n");
    }
}

/* 测试数据结构 */
void test_data_structures(void) {
    printf("\n=== 数据结构测试 ===\n");

    /* 测试点距离 */
    Point2D p1 = {0.0, 0.0};
    Point2D p2 = {3.0, 4.0};
    double dist = rust_point_distance(p1, p2);
    printf("点(%.1f, %.1f)到点(%.1f, %.1f)的距离 = %.4f\n",
           p1.x, p1.y, p2.x, p2.y, dist);

    /* 测试向量长度 */
    Point2D p = {3.0, 4.0};
    double mag = rust_vector_magnitude(&p);
    printf("向量(%.1f, %.1f)的长度 = %.4f\n", p.x, p.y, mag);

    /* 测试空指针处理 */
    double null_mag = rust_vector_magnitude(NULL);
    printf("空指针向量的长度 = %.1f (错误指示)\n", null_mag);
}

/* 测试批量处理 */
void test_batch_processing(void) {
    printf("\n=== 批量处理测试 ===\n");

    /* 创建点数组 */
    Point2D points[] = {
        {3.0, 4.0},
        {5.0, 12.0},
        {8.0, 15.0},
        {7.0, 24.0}
    };
    size_t count = sizeof(points) / sizeof(points[0]);

    /* 分配结果数组 */
    double* results = malloc(count * sizeof(double));
    if (results == NULL) {
        printf("内存分配失败\n");
        return;
    }

    /* 调用Rust函数批量处理 */
    size_t processed = rust_process_points(points, count, results);
    printf("处理了 %zu/%zu 个点\n", processed, count);

    /* 输出结果 */
    for (size_t i = 0; i < processed; i++) {
        printf("点[%zu] (%5.1f, %5.1f) 的模 = %6.4f\n",
               i, points[i].x, points[i].y, results[i]);
    }

    free(results);
}

/* 测试错误处理 */
void test_error_handling(void) {
    printf("\n=== 错误处理测试 ===\n");

    double result;
    ErrorCode code;

    /* 正常除法 */
    code = rust_safe_divide(10.0, 2.0, &result);
    printf("10.0 / 2.0 = %.2f (代码: %d, %s)\n",
           result, code, rust_error_message(code));

    /* 除零错误 */
    code = rust_safe_divide(10.0, 0.0, &result);
    printf("10.0 / 0.0 错误 (代码: %d, %s)\n",
           code, rust_error_message(code));

    /* 空指针错误 */
    code = rust_safe_divide(10.0, 2.0, NULL);
    printf("空指针结果 (代码: %d, %s)\n",
           code, rust_error_message(code));

    /* 所有错误消息 */
    printf("\n所有错误消息:\n");
    for (int i = 0; i <= 4; i++) {
        printf("  %d: %s\n", i, rust_error_message((ErrorCode)i));
    }
}

/* 主函数 */
int main(void) {
    printf("Rust数学库C调用示例\n");
    printf("====================\n");

    test_basic_math();
    test_string_processing();
    test_data_structures();
    test_batch_processing();
    test_error_handling();

    printf("\n所有测试完成!\n");
    return 0;
}
