/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\03_Testing_Frameworks.md
 * Line: 25
 * Language: c
 * Block ID: ff3c451d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "unity.h"
#include "math_lib.h"

void setUp(void) {}
void tearDown(void) {}

void test_add(void) {
    TEST_ASSERT_EQUAL(5, add(2, 3));
    TEST_ASSERT_EQUAL(-1, add(2, -3));
    TEST_ASSERT_EQUAL(0, add(0, 0));
}

void test_divide(void) {
    TEST_ASSERT_EQUAL(2, divide(6, 3));
    TEST_ASSERT_EQUAL(-2, divide(6, -3));
}

void test_divide_by_zero(void) {
    TEST_ASSERT_EQUAL(0, divide(6, 0));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_divide);
    RUN_TEST(test_divide_by_zero);
    return UNITY_END();
}
