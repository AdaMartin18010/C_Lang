/*
 * Auto-generated from: 07_Modern_Toolchain\03_CI_CD_DevOps\README.md
 * Line: 658
 * Language: c
 * Block ID: 2f4c7f15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// test_math.c
#include "unity.h"
#include "math_utils.h"

void setUp(void) {
    // 每个测试前执行
}

void tearDown(void) {
    // 每个测试后执行
}

// 测试用例
void test_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
    TEST_ASSERT_EQUAL_INT(100, add(50, 50));
}

void test_add_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-5, add(-2, -3));
    TEST_ASSERT_EQUAL_INT(0, add(-5, 5));
}

void test_add_overflow(void) {
    int a = INT_MAX;
    int b = 1;
    // 检测溢出
    int result;
    int status = add_safe(a, b, &result);
    TEST_ASSERT_EQUAL_INT(ERROR_OVERFLOW, status);
}

void test_divide_by_zero(void) {
    // 测试除零保护
    TEST_ASSERT_EQUAL_INT(0, divide(10, 0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_negative_numbers);
    RUN_TEST(test_add_overflow);
    RUN_TEST(test_divide_by_zero);
    return UNITY_END();
}
