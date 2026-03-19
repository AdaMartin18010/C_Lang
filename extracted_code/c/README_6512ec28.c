/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\README.md
 * Line: 138
 * Language: c
 * Block ID: 6512ec28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// test/support/test_config.h
#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include "unity.h"

// 内存泄漏检测
#define TEST_MALLOC_TRACE 1

// 覆盖率插桩
#define COVERAGE_ENABLED 1

// 测试超时
#define TEST_TIMEOUT_MS 1000

// 断言宏增强
#define TEST_ASSERT_IN_RANGE(actual, lower, upper) \
    TEST_ASSERT((actual) >= (lower) && (actual) <= (upper))

#define TEST_ASSERT_BITS_SET(value, bits) \
    TEST_ASSERT_BITS((bits), (bits), (value))

#endif
