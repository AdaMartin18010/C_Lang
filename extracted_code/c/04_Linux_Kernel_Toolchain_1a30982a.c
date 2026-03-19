/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 863
 * Language: c
 * Block ID: 1a30982a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// KUnit - 内核单元测试
#include <kunit/test.h>

static void my_test_case(struct kunit *test)
{
    int result = my_function(2, 3);
    KUNIT_EXPECT_EQ(test, result, 5);
}

static struct kunit_case my_test_cases[] = {
    KUNIT_CASE(my_test_case),
    {}
};

static struct kunit_suite my_suite = {
    .name = "my-module",
    .test_cases = my_test_cases,
};
kunit_test_suite(my_suite);
