/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 360
 * Language: c
 * Block ID: 5c836964
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "unity.h"
#include "module_under_test.h"

void setUp(void) {
    // 每个测试前执行
}

void tearDown(void) {
    // 每个测试后执行
}

void test_add_should_return_sum(void) {
    TEST_ASSERT_EQUAL(5, add(2, 3));
    TEST_ASSERT_EQUAL(0, add(-1, 1));
}

void test_divide_should_handle_division_by_zero(void) {
    error_t err = divide(10, 0, NULL);
    TEST_ASSERT_EQUAL(ERR_DIVISION_BY_ZERO, err);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_should_return_sum);
    RUN_TEST(test_divide_should_handle_division_by_zero);
    return UNITY_END();
}
