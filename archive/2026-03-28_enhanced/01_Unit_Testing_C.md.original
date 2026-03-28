# C 单元测试

> **层级**: L3 (核心层)
> **目标**: 掌握 C 单元测试方法、框架选择与工程实践
> **MISRA C**: 兼容

---

## 目录

- [C 单元测试](#c-单元测试)
  - [目录](#目录)
  - [1. 测试框架](#1-测试框架)
    - [1.1 Unity 框架](#11-unity-框架)
    - [1.2 CMocka 框架](#12-cmocka-框架)
    - [1.3 Check 框架](#13-check-框架)
    - [1.4 框架选择标准](#14-框架选择标准)
  - [2. 测试设计](#2-测试设计)
    - [2.1 AAA 模式](#21-aaa-模式)
    - [2.2 参数化测试](#22-参数化测试)
    - [2.3 边界值分析](#23-边界值分析)
    - [2.4 等价类划分](#24-等价类划分)
  - [3. Mock 技术](#3-mock-技术)
    - [3.1 函数打桩](#31-函数打桩)
    - [3.2 弱符号替换](#32-弱符号替换)
    - [3.3 链接期替换](#33-链接期替换)
    - [3.4 行为验证](#34-行为验证)
  - [4. 覆盖率](#4-覆盖率)
    - [4.1 语句覆盖](#41-语句覆盖)
    - [4.2 分支覆盖](#42-分支覆盖)
    - [4.3 MC/DC 覆盖](#43-mcdc-覆盖)
    - [4.4 覆盖率工具使用](#44-覆盖率工具使用)
  - [5. 测试驱动开发](#5-测试驱动开发)
    - [5.1 TDD 流程](#51-tdd-流程)
    - [5.2 红绿重构循环](#52-红绿重构循环)
  - [6. 深入理解](#6-深入理解)
    - [6.1 形式化定义](#61-形式化定义)
    - [6.2 测试充分性](#62-测试充分性)
    - [6.3 层次映射](#63-层次映射)
  - [7. 思维表征](#7-思维表征)
    - [7.1 测试策略决策树](#71-测试策略决策树)
    - [7.2 测试框架对比矩阵](#72-测试框架对比矩阵)
    - [7.3 覆盖率类型层次图](#73-覆盖率类型层次图)
  - [8. 总结](#8-总结)

---

## 1. 测试框架

### 1.1 Unity 框架

Unity 是专为嵌入式系统设计的轻量级 C 单元测试框架，特点：

- **零依赖**: 单头文件 + 单源文件即可运行
- **资源占用低**: 适合资源受限的嵌入式环境
- **断言丰富**: 支持多种数据类型的断言

**核心结构**:

```c
/* test_example.c - Unity 完整测试示例 */
#include "unity.h"
#include "calculator.h"  /* 被测模块 */

/* setUp 在每个测试用例前执行 */
void setUp(void)
{
    /* 初始化被测模块 */
    calculator_init();
}

/* tearDown 在每个测试用例后执行 */
void tearDown(void)
{
    /* 清理资源 */
    calculator_cleanup();
}

/* 测试用例：整数加法 */
void test_add_positive_numbers(void)
{
    /* Arrange */
    int32_t a = 10;
    int32_t b = 20;
    int32_t expected = 30;

    /* Act */
    int32_t result = calculator_add(a, b);

    /* Assert */
    TEST_ASSERT_EQUAL_INT32(expected, result);
}

/* 测试用例：加法溢出检测 */
void test_add_overflow_detection(void)
{
    /* Arrange */
    int32_t a = INT32_MAX;
    int32_t b = 1;

    /* Act */
    calc_status_t status = calculator_add_safe(a, b, NULL);

    /* Assert */
    TEST_ASSERT_EQUAL(CALC_OVERFLOW, status);
}

/* 测试用例：浮点数比较（考虑精度） */
void test_divide_floating_point(void)
{
    /* Arrange */
    double a = 10.0;
    double b = 3.0;
    double expected = 3.333333;

    /* Act */
    double result = calculator_divide(a, b);

    /* Assert - 使用 Delta 断言处理浮点误差 */
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, expected, result);
}

/* 测试用例：字符串断言 */
void test_error_message(void)
{
    /* Arrange */
    calculator_set_error(CALC_ERROR_DIV_BY_ZERO);

    /* Act */
    const char* msg = calculator_get_error_msg();

    /* Assert */
    TEST_ASSERT_EQUAL_STRING("Division by zero", msg);
}

/* 主函数 - 运行所有测试 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_overflow_detection);
    RUN_TEST(test_divide_floating_point);
    RUN_TEST(test_error_message);

    return UNITY_END();
}
```

**常用断言宏**:

| 断言宏 | 用途 |
|--------|------|
| `TEST_ASSERT_TRUE(condition)` | 条件为真 |
| `TEST_ASSERT_FALSE(condition)` | 条件为假 |
| `TEST_ASSERT_EQUAL(expected, actual)` | 整型相等 |
| `TEST_ASSERT_EQUAL_INT32(exp, act)` | 32位整型相等 |
| `TEST_ASSERT_EQUAL_PTR(exp, act)` | 指针相等 |
| `TEST_ASSERT_NULL(ptr)` | 指针为NULL |
| `TEST_ASSERT_EQUAL_STRING(exp, act)` | 字符串相等 |
| `TEST_ASSERT_DOUBLE_WITHIN(delta, exp, act)` | 浮点数在范围内 |
| `TEST_FAIL_MESSAGE(msg)` | 强制失败 |

### 1.2 CMocka 框架

CMocka 是支持 Mock 的 C 测试框架，特点：

- **内置 Mock 支持**: 无需额外工具
- **异常处理**: 支持测试崩溃恢复
- **丰富的输出格式**: 支持 TAP、XML 等

```c
/* test_cmocka_example.c */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* 被测函数依赖的函数声明 */
extern int32_t sensor_read_temperature(void);

/* 被测函数 */
int32_t temperature_monitor_check(void)
{
    int32_t temp = sensor_read_temperature();
    if (temp > 80) {
        return -1;  /* 过热 */
    }
    return 0;  /* 正常 */
}

/* Mock 实现 */
int32_t sensor_read_temperature(void)
{
    /* 从测试配置返回值 */
    return mock_type(int32_t);
}

/* 测试用例：正常温度 */
static void test_normal_temperature(void **state)
{
    (void)state;

    /* 配置 Mock 返回值 */
    will_return(sensor_read_temperature, 50);

    /* 执行测试 */
    int32_t result = temperature_monitor_check();

    /* 验证 */
    assert_int_equal(0, result);
}

/* 测试用例：过热状态 */
static void test_overheat_condition(void **state)
{
    (void)state;

    /* 配置 Mock 返回值 */
    will_return(sensor_read_temperature, 85);

    /* 执行测试 */
    int32_t result = temperature_monitor_check();

    /* 验证 */
    assert_int_equal(-1, result);
}

/* 参数化测试 */
static void test_temperature_thresholds(void **state)
{
    int32_t temp = *(int32_t *)*state;

    will_return(sensor_read_temperature, temp);

    int32_t result = temperature_monitor_check();

    if (temp > 80) {
        assert_int_equal(-1, result);
    } else {
        assert_int_equal(0, result);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_normal_temperature),
        cmocka_unit_test(test_overheat_condition),
    };

    /* 带状态的参数化测试 */
    int32_t thresholds[] = {-10, 0, 50, 80, 81, 100};
    const struct CMUnitTest param_tests[] = {
        cmocka_unit_test_prestate(test_temperature_thresholds, &thresholds[0]),
        cmocka_unit_test_prestate(test_temperature_thresholds, &thresholds[1]),
        cmocka_unit_test_prestate(test_temperature_thresholds, &thresholds[2]),
        cmocka_unit_test_prestate(test_temperature_thresholds, &thresholds[3]),
        cmocka_unit_test_prestate(test_temperature_thresholds, &thresholds[4]),
        cmocka_unit_test_prestate(test_temperature_thresholds, &thresholds[5]),
    };

    return cmocka_run_group_tests(tests, NULL, NULL) |
           cmocka_run_group_tests(param_tests, NULL, NULL);
}
```

### 1.3 Check 框架

Check 是功能丰富的 C 单元测试框架，特点：

- **多进程测试**: 每个测试在独立进程运行
- **超时控制**: 支持测试用例超时设置
- **灵活配置**: 支持多种测试运行模式

```c
/* test_check_example.c */
#include <check.h>
#include "stack.h"

/* 测试套件前置 */
void stack_setup(void)
{
    stack_init();
}

/* 测试套件后置 */
void stack_teardown(void)
{
    stack_cleanup();
}

/* 测试用例：空栈检查 */
START_TEST(test_stack_empty_after_init)
{
    ck_assert_int_eq(stack_is_empty(), 1);
    ck_assert_int_eq(stack_size(), 0);
}
END_TEST

/* 测试用例：压栈和弹栈 */
START_TEST(test_push_and_pop)
{
    int32_t value = 42;

    ck_assert_int_eq(stack_push(value), 0);
    ck_assert_int_eq(stack_size(), 1);
    ck_assert_int_eq(stack_is_empty(), 0);

    int32_t popped;
    ck_assert_int_eq(stack_pop(&popped), 0);
    ck_assert_int_eq(popped, value);
    ck_assert_int_eq(stack_size(), 0);
}
END_TEST

/* 测试用例：栈溢出检测 */
START_TEST(test_stack_overflow)
{
    /* 填充栈到容量上限 */
    for (int32_t i = 0; i < STACK_MAX_SIZE; i++) {
        ck_assert_int_eq(stack_push(i), 0);
    }

    /* 尝试超容 */
    ck_assert_int_eq(stack_push(999), -1);  /* 应返回错误 */
}
END_TEST

/* 测试用例：栈下溢检测 */
START_TEST(test_stack_underflow)
{
    int32_t value;
    ck_assert_int_eq(stack_pop(&value), -1);  /* 空栈弹出应失败 */
}
END_TEST

/* 创建测试套件 */
Suite* stack_suite(void)
{
    Suite *s = suite_create("Stack");
    TCase *tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, stack_setup, stack_teardown);

    tcase_add_test(tc_core, test_stack_empty_after_init);
    tcase_add_test(tc_core, test_push_and_pop);
    tcase_add_test(tc_core, test_stack_overflow);
    tcase_add_test(tc_core, test_stack_underflow);

    /* 设置超时（秒） */
    tcase_set_timeout(tc_core, 5);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    int32_t number_failed;
    Suite *s = stack_suite();
    SRunner *sr = srunner_create(s);

    /* 设置输出格式 */
    srunner_set_xml(sr, "test_results.xml");
    srunner_set_fork_status(sr, CK_FORK);  /* 每个测试独立进程 */

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}
```

### 1.4 框架选择标准

| 维度 | Unity | CMocka | Check |
|------|-------|--------|-------|
| **资源占用** | 极低 | 低 | 中等 |
| **Mock 支持** | 需配合 CMock | 原生支持 | 需手动实现 |
| **依赖数量** | 0 | minimal | minimal |
| **进程隔离** | 无 | 无 | 有 |
| **超时控制** | 无 | 无 | 有 |
| **输出格式** | 文本 | 多格式 | 多格式 |
| **适用场景** | 嵌入式/裸机 | 通用 C 项目 | 复杂测试需求 |

**选择建议**:

- **嵌入式/资源受限**: 选择 Unity
- **需要内置 Mock**: 选择 CMocka
- **需要进程隔离/超时**: 选择 Check
- **快速启动**: 选择 Unity（单文件部署）

---

## 2. 测试设计

### 2.1 AAA 模式

AAA (Arrange-Act-Assert) 是测试用例设计的基本模式：

```c
/* AAA 模式示例 */
void test_user_authentication_valid_credentials(void)
{
    /* === Arrange (准备) === */
    user_t user;
    auth_result_t result;
    const char* valid_username = "admin";
    const char* valid_password = "secure_pass";

    /* 初始化被测对象 */
    memset(&user, 0, sizeof(user));
    auth_system_init();

    /* 准备测试数据 */
    user_create(&user, valid_username, valid_password);
    user_activate(&user);

    /* === Act (执行) === */
    result = auth_system_login(valid_username, valid_password);

    /* === Assert (断言) === */
    TEST_ASSERT_EQUAL(AUTH_SUCCESS, result);
    TEST_ASSERT_TRUE(user.is_authenticated);
}
```

**AAA 模式优势**:

- 结构清晰，易于理解
- 便于发现测试缺失（缺少 Arrange/Act/Assert）
- 支持测试代码重构

### 2.2 参数化测试

参数化测试通过数据驱动减少重复代码：

```c
/* Unity + 参数化测试实现 */
#include "unity.h"

/* 测试数据结构 */
typedef struct {
    const char* name;
    int32_t dividend;
    int32_t divisor;
    int32_t expected_quotient;
    int32_t expected_remainder;
    int32_t expected_status;
} division_test_case_t;

/* 测试用例表 */
static const division_test_case_t division_tests[] = {
    /* name              dividend  divisor  quotient  remainder  status */
    {"positive_even",      10,       2,       5,        0,       0},
    {"positive_odd",       10,       3,       3,        1,       0},
    {"negative_dividend", -10,       3,      -3,       -1,       0},
    {"negative_divisor",   10,      -3,      -3,        1,       0},
    {"both_negative",     -10,      -3,       3,       -1,       0},
    {"divide_by_one",      10,       1,      10,        0,       0},
    {"zero_dividend",       0,       5,       0,        0,       0},
    /* 边界值 */
    {"max_int",      INT32_MAX,      2,       0,        0,       0},
};

/* 参数化测试执行函数 */
void test_division_parametrized(void)
{
    const size_t num_tests = sizeof(division_tests) / sizeof(division_tests[0]);

    for (size_t i = 0; i < num_tests; i++) {
        const division_test_case_t* tc = &division_tests[i];

        /* Arrange */
        int32_t quotient;
        int32_t remainder;

        /* Act */
        int32_t status = safe_divide(tc->dividend, tc->divisor,
                                      &quotient, &remainder);

        /* Assert */
        char msg[128];
        snprintf(msg, sizeof(msg), "Test case: %s", tc->name);

        TEST_ASSERT_EQUAL_INT32_MESSAGE(tc->expected_status, status, msg);

        if (tc->expected_status == 0) {
            TEST_ASSERT_EQUAL_INT32_MESSAGE(tc->expected_quotient, quotient, msg);
            TEST_ASSERT_EQUAL_INT32_MESSAGE(tc->expected_remainder, remainder, msg);
        }
    }
}

/* 另一种参数化方式：使用宏 */
#define DEFINE_DIVISION_TEST(name, divd, divs, exp_q, exp_r) \
    void test_division_##name(void) { \
        int32_t q, r; \
        int32_t s = safe_divide(divd, divs, &q, &r); \
        TEST_ASSERT_EQUAL_INT32(0, s); \
        TEST_ASSERT_EQUAL_INT32(exp_q, q); \
        TEST_ASSERT_EQUAL_INT32(exp_r, r); \
    }

DEFINE_DIVISION_TEST(ten_by_two, 10, 2, 5, 0)
DEFINE_DIVISION_TEST(ten_by_three, 10, 3, 3, 1)
DEFINE_DIVISION_TEST(negative_by_positive, -10, 3, -3, -1)
```

### 2.3 边界值分析

边界值是错误高发区域，应重点测试：

```c
/* 边界值分析示例：缓冲区操作 */
#define BUFFER_SIZE 64

/* 被测函数 */
int32_t buffer_write(buffer_t* buf, const uint8_t* data, size_t len);

/* 边界值测试 */
void test_buffer_write_boundaries(void)
{
    buffer_t buf;
    uint8_t data[BUFFER_SIZE * 2];

    /* 边界1: 空缓冲区写入0字节 */
    buffer_init(&buf, BUFFER_SIZE);
    TEST_ASSERT_EQUAL(0, buffer_write(&buf, data, 0));
    TEST_ASSERT_EQUAL(0, buffer_available(&buf));

    /* 边界2: 空缓冲区写入1字节 */
    buffer_reset(&buf);
    TEST_ASSERT_EQUAL(1, buffer_write(&buf, data, 1));
    TEST_ASSERT_EQUAL(1, buffer_available(&buf));

    /* 边界3: 写入容量减1 */
    buffer_reset(&buf);
    TEST_ASSERT_EQUAL(BUFFER_SIZE - 1,
                      buffer_write(&buf, data, BUFFER_SIZE - 1));

    /* 边界4: 恰好填满缓冲区 */
    buffer_reset(&buf);
    TEST_ASSERT_EQUAL(BUFFER_SIZE,
                      buffer_write(&buf, data, BUFFER_SIZE));
    TEST_ASSERT_EQUAL(0, buffer_free_space(&buf));

    /* 边界5: 尝试超容写入 */
    buffer_reset(&buf);
    TEST_ASSERT_EQUAL(BUFFER_SIZE,
                      buffer_write(&buf, data, BUFFER_SIZE + 1));

    /* 边界6: 满缓冲区继续写入 */
    buffer_reset(&buf);
    buffer_write(&buf, data, BUFFER_SIZE);  /* 填满 */
    TEST_ASSERT_EQUAL(0, buffer_write(&buf, data, 1));  /* 应失败 */

    buffer_cleanup(&buf);
}
```

**典型边界值**:

- 数值范围: MIN, MIN+1, 典型值, MAX-1, MAX
- 数组/缓冲区: 0, 1, n-1, n, n+1
- 字符串: 空串, 1字符, 最大长度-1, 最大长度, 最大长度+1

### 2.4 等价类划分

等价类划分减少测试用例数量同时保持覆盖：

```c
/* 等价类划分示例：输入验证 */
/* 被测函数：验证年龄输入，有效范围 18-120 */
bool is_valid_age(int32_t age);

/* 等价类划分 */
typedef enum {
    INVALID_TOO_SMALL,    /* < 18 */
    VALID_RANGE,          /* 18-120 */
    INVALID_TOO_LARGE,    /* > 120 */
    INVALID_NEGATIVE,     /* < 0 */
    INVALID_NON_NUMERIC   /* 非数值 - 不同类型 */
} age_equivalence_class_t;

/* 测试用例设计（每个等价类至少一个代表） */
void test_age_validation_equivalence(void)
{
    /* 无效等价类1: 小于下限 */
    TEST_ASSERT_FALSE(is_valid_age(17));   /* 边界-1 */
    TEST_ASSERT_FALSE(is_valid_age(0));    /* 典型值 */
    TEST_ASSERT_FALSE(is_valid_age(-10));  /* 负数 */

    /* 有效等价类: 正常范围 */
    TEST_ASSERT_TRUE(is_valid_age(18));    /* 边界（下限） */
    TEST_ASSERT_TRUE(is_valid_age(25));    /* 典型值 */
    TEST_ASSERT_TRUE(is_valid_age(120));   /* 边界（上限） */

    /* 无效等价类2: 大于上限 */
    TEST_ASSERT_FALSE(is_valid_age(121));  /* 边界+1 */
    TEST_ASSERT_FALSE(is_valid_age(999));  /* 典型值 */
}

/* 复杂等价类划分：多条件组合 */
/* 用户注册：用户名3-20字符，密码8-32字符 */
typedef struct {
    const char* username;
    const char* password;
    bool expected_valid;
    const char* description;
} registration_test_case_t;

static const registration_test_case_t reg_tests[] = {
    /* 有效等价类 */
    {"user123",   "Pass1234!",   true,  "both_valid"},

    /* 用户名无效 */
    {"ab",        "Pass1234!",   false, "username_too_short"},
    {"user...",   "Pass1234!",   false, "username_too_long"},
    {"",          "Pass1234!",   false, "username_empty"},

    /* 密码无效 */
    {"user123",   "short1!",     false, "password_too_short"},
    {"user123",   "verylongpasswordthatexceeds32chars!", false, "password_too_long"},
    {"user123",   "",            false, "password_empty"},

    /* 两者都无效 */
    {"ab",        "short",       false, "both_invalid"},
};
```

---

## 3. Mock 技术

### 3.1 函数打桩

函数打桩替换依赖函数实现：

```c
/* 被测模块: data_processor.c */
/* 依赖的外部函数 */
extern int32_t database_query(const char* sql, void* result);
extern int32_t network_send(const uint8_t* data, size_t len);
extern int32_t log_write(log_level_t level, const char* msg);

int32_t process_and_send(const char* query)
{
    char result[256];

    if (database_query(query, result) != 0) {
        log_write(LOG_ERROR, "Query failed");
        return -1;
    }

    if (network_send((uint8_t*)result, strlen(result)) != 0) {
        log_write(LOG_ERROR, "Send failed");
        return -2;
    }

    log_write(LOG_INFO, "Process completed");
    return 0;
}

/* === 测试桩实现 === */
/* test_stubs.c */

/* 桩状态跟踪 */
static struct {
    int32_t query_return;
    int32_t send_return;
    char last_query[256];
    uint8_t last_sent_data[256];
    size_t last_sent_len;
    uint32_t query_call_count;
    uint32_t send_call_count;
    uint32_t log_call_count;
} stub_state;

void stub_reset(void)
{
    memset(&stub_state, 0, sizeof(stub_state));
    stub_state.query_return = 0;
    stub_state.send_return = 0;
}

/* 数据库查询桩 */
int32_t database_query(const char* sql, void* result)
{
    stub_state.query_call_count++;
    strncpy(stub_state.last_query, sql, sizeof(stub_state.last_query) - 1);

    if (stub_state.query_return == 0 && result != NULL) {
        /* 模拟返回数据 */
        strcpy((char*)result, "mock_result_data");
    }

    return stub_state.query_return;
}

/* 网络发送桩 */
int32_t network_send(const uint8_t* data, size_t len)
{
    stub_state.send_call_count++;

    if (len < sizeof(stub_state.last_sent_data)) {
        memcpy(stub_state.last_sent_data, data, len);
    }
    stub_state.last_sent_len = len;

    return stub_state.send_return;
}

/* 日志桩 */
int32_t log_write(log_level_t level, const char* msg)
{
    stub_state.log_call_count++;
    (void)level;
    (void)msg;
    return 0;
}

/* 桩配置接口 */
void stub_set_query_return(int32_t ret)
{
    stub_state.query_return = ret;
}

void stub_set_send_return(int32_t ret)
{
    stub_state.send_return = ret;
}

uint32_t stub_get_query_call_count(void)
{
    return stub_state.query_call_count;
}

const char* stub_get_last_query(void)
{
    return stub_state.last_query;
}

/* 测试用例 */
void test_process_success(void)
{
    stub_reset();

    int32_t result = process_and_send("SELECT * FROM users");

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1, stub_get_query_call_count());
    TEST_ASSERT_EQUAL_STRING("SELECT * FROM users", stub_get_last_query());
}

void test_process_query_fails(void)
{
    stub_reset();
    stub_set_query_return(-1);  /* 模拟查询失败 */

    int32_t result = process_and_send("INVALID SQL");

    TEST_ASSERT_EQUAL(-1, result);
    TEST_ASSERT_EQUAL(1, stub_state.log_call_count);
}
```

### 3.2 弱符号替换

使用 GCC 弱符号实现运行时替换：

```c
/* 弱符号定义: weak_deps.h */
#ifndef WEAK_DEPS_H
#define WEAK_DEPS_H

#include <stdint.h>
#include <stddef.h>

/* 弱符号属性 */
#ifdef __GNUC__
#define WEAK __attribute__((weak))
#else
#define WEAK
#endif

/* 弱符号声明 */
int32_t WEAK hardware_read_sensor(uint8_t sensor_id);
int32_t WEAK hardware_write_actuator(uint8_t actuator_id, int32_t value);
uint32_t WEAK system_get_timestamp_ms(void);

#endif /* WEAK_DEPS_H */

/* 默认弱实现: weak_deps.c */
#include "weak_deps.h"

int32_t __attribute__((weak)) hardware_read_sensor(uint8_t sensor_id)
{
    (void)sensor_id;
    return 0;  /* 默认返回值 */
}

int32_t __attribute__((weak)) hardware_write_actuator(uint8_t actuator_id, int32_t value)
{
    (void)actuator_id;
    (void)value;
    return 0;
}

uint32_t __attribute__((weak)) system_get_timestamp_ms(void)
{
    return 0;
}

/* === 测试中的强符号替换 === */
/* test_mocks.c */

static int32_t mock_sensor_values[16];
static uint32_t mock_timestamp = 0;

/* 强符号替换弱符号 */
int32_t hardware_read_sensor(uint8_t sensor_id)
{
    if (sensor_id < 16) {
        return mock_sensor_values[sensor_id];
    }
    return -1;
}

uint32_t system_get_timestamp_ms(void)
{
    return mock_timestamp;
}

void mock_set_sensor(uint8_t id, int32_t value)
{
    if (id < 16) {
        mock_sensor_values[id] = value;
    }
}

void mock_advance_time(uint32_t ms)
{
    mock_timestamp += ms;
}

/* 被测代码自动使用强符号实现 */
void test_temperature_control(void)
{
    mock_set_sensor(0, 2500);  /* 25.00度 */
    mock_set_sensor(1, 2600);  /* 26.00度 */
    mock_timestamp = 0;

    /* 调用被测函数 - 使用 Mock */
    temperature_controller_init();
    temperature_controller_update();

    TEST_ASSERT_TRUE(temperature_controller_is_active());
}
```

### 3.3 链接期替换

使用链接器在链接时替换实现：

```c
/* 原始实现: real_hal.c (生产代码) */
int32_t hal_spi_transfer(uint8_t* tx, uint8_t* rx, size_t len)
{
    /* 真实硬件操作 */
    /* ... */
    return 0;
}

/* Mock 实现: mock_hal.c (测试代码) */
#include "hal_interface.h"

typedef struct {
    uint8_t expected_tx[256];
    size_t expected_len;
    uint8_t response_rx[256];
    int32_t return_value;
    bool verify_transmitted;
} spi_mock_expectation_t;

static spi_mock_expectation_t spi_expectations[10];
static int spi_expectation_index = 0;
static int spi_call_count = 0;

void mock_spi_set_expectation(const uint8_t* tx, size_t len,
                               const uint8_t* rx, int32_t ret)
{
    if (spi_expectation_index < 10) {
        spi_mock_expectation_t* exp = &spi_expectations[spi_expectation_index++];
        memcpy(exp->expected_tx, tx, len);
        exp->expected_len = len;
        if (rx != NULL) {
            memcpy(exp->response_rx, rx, len);
        }
        exp->return_value = ret;
    }
}

void mock_spi_verify(void)
{
    TEST_ASSERT_EQUAL(spi_expectation_index, spi_call_count);
}

void mock_spi_reset(void)
{
    memset(spi_expectations, 0, sizeof(spi_expectations));
    spi_expectation_index = 0;
    spi_call_count = 0;
}

/* Mock 实现 */
int32_t hal_spi_transfer(uint8_t* tx, uint8_t* rx, size_t len)
{
    TEST_ASSERT_LESS_THAN(spi_expectation_index, spi_call_count);

    spi_mock_expectation_t* exp = &spi_expectations[spi_call_count++];

    /* 验证发送数据 */
    if (exp->verify_transmitted) {
        TEST_ASSERT_EQUAL_UINT8_ARRAY(exp->expected_tx, tx, len);
    }

    /* 填充接收数据 */
    if (rx != NULL) {
        memcpy(rx, exp->response_rx, len);
    }

    return exp->return_value;
}

/* Makefile 链接控制 */
/*
生产构建:
gcc -c real_hal.c -o real_hal.o
gcc real_hal.o main.o -o app

测试构建:
gcc -c mock_hal.c -o mock_hal.o
gcc mock_hal.o test_main.o test_cases.o unity.o -o test_app
*/
```

### 3.4 行为验证

验证函数调用行为而非返回值：

```c
/* 行为验证示例 */
typedef struct {
    /* 调用记录 */
    struct {
        const char* func_name;
        int32_t arg1;
        int32_t arg2;
        uint32_t timestamp;
    } call_log[100];
    uint32_t call_count;

    /* 参数捕获 */
    struct {
        void* captured_ptr;
        size_t captured_size;
    } last_allocation;

    /* 调用顺序 */
    uint32_t expected_sequence[10];
    uint32_t actual_sequence[10];
    uint32_t sequence_index;
} behavior_tracker_t;

static behavior_tracker_t tracker;

void tracker_reset(void)
{
    memset(&tracker, 0, sizeof(tracker));
}

/* 带行为跟踪的 Mock 分配器 */
void* mock_malloc(size_t size)
{
    tracker.call_log[tracker.call_count].func_name = "malloc";
    tracker.call_log[tracker.call_count].arg1 = (int32_t)size;
    tracker.call_log[tracker.call_count].timestamp = get_tick_count();
    tracker.call_count++;

    void* ptr = malloc(size);
    tracker.last_allocation.captured_ptr = ptr;
    tracker.last_allocation.captured_size = size;

    return ptr;
}

void mock_free(void* ptr)
{
    tracker.call_log[tracker.call_count].func_name = "free";
    tracker.call_log[tracker.call_count].arg1 = (int32_t)(uintptr_t)ptr;
    tracker.call_log[tracker.call_count].timestamp = get_tick_count();
    tracker.call_count++;

    free(ptr);
}

/* 行为验证断言 */
void assert_malloc_called_with_size(size_t expected_size)
{
    bool found = false;
    for (uint32_t i = 0; i < tracker.call_count; i++) {
        if (strcmp(tracker.call_log[i].func_name, "malloc") == 0 &&
            tracker.call_log[i].arg1 == (int32_t)expected_size) {
            found = true;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(found, "Expected malloc with specific size not called");
}

void assert_call_sequence(const char** expected_funcs, uint32_t count)
{
    TEST_ASSERT_EQUAL(count, tracker.call_count);
    for (uint32_t i = 0; i < count; i++) {
        TEST_ASSERT_EQUAL_STRING(expected_funcs[i], tracker.call_log[i].func_name);
    }
}

void assert_no_memory_leaks(void)
{
    /* 验证每个 malloc 都有对应的 free */
    int32_t balance = 0;
    for (uint32_t i = 0; i < tracker.call_count; i++) {
        if (strcmp(tracker.call_log[i].func_name, "malloc") == 0) {
            balance++;
        } else if (strcmp(tracker.call_log[i].func_name, "free") == 0) {
            balance--;
        }
    }
    TEST_ASSERT_EQUAL(0, balance);
}

/* 测试用例 */
void test_buffer_lifecycle(void)
{
    tracker_reset();

    buffer_t* buf = buffer_create(100);

    /* 验证分配行为 */
    assert_malloc_called_with_size(sizeof(buffer_t) + 100);

    buffer_write(buf, "test", 4);
    buffer_destroy(buf);

    /* 验证调用顺序 */
    const char* expected[] = {"malloc", "free"};
    assert_call_sequence(expected, 2);

    /* 验证无内存泄漏 */
    assert_no_memory_leaks();
}
```

---

## 4. 覆盖率

### 4.1 语句覆盖

语句覆盖确保每行代码至少执行一次：

```c
/* 被测代码 */
int32_t classify_number(int32_t n)
{
    if (n > 0) {              /* 语句1 */
        return 1;             /* 语句2 */
    } else if (n < 0) {       /* 语句3 */
        return -1;            /* 语句4 */
    } else {
        return 0;             /* 语句5 */
    }
}

/* 达到 100% 语句覆盖的测试集 */
void test_statement_coverage(void)
{
    /* 覆盖语句1和语句2 */
    TEST_ASSERT_EQUAL(1, classify_number(5));

    /* 覆盖语句3和语句4 */
    TEST_ASSERT_EQUAL(-1, classify_number(-3));

    /* 覆盖语句5 */
    TEST_ASSERT_EQUAL(0, classify_number(0));
}

/* 覆盖率计算:
 * 执行语句数 / 总语句数 = 5/5 = 100%
 */
```

### 4.2 分支覆盖

分支覆盖确保每个判断的 True/False 都执行：

```c
/* 被测代码 */
bool check_range(int32_t value, int32_t min, int32_t max)
{
    if (value >= min && value <= max) {  /* 两个条件，4种组合 */
        return true;
    }
    return false;
}

/* 分支覆盖测试 */
void test_branch_coverage(void)
{
    /* 分支: value >= min = T, value <= max = T */
    TEST_ASSERT_TRUE(check_range(5, 0, 10));

    /* 分支: value >= min = F (短路，不评估第二个) */
    TEST_ASSERT_FALSE(check_range(-1, 0, 10));

    /* 分支: value >= min = T, value <= max = F */
    TEST_ASSERT_FALSE(check_range(15, 0, 10));

    /* 边界分支: value == min */
    TEST_ASSERT_TRUE(check_range(0, 0, 10));

    /* 边界分支: value == max */
    TEST_ASSERT_TRUE(check_range(10, 0, 10));
}

/* 分支覆盖率计算:
 * 执行分支数 / 总分支数 = ?
 * 实际代码被编译器优化为:
 *   if (value < min) return false;
 *   if (value > max) return false;
 *   return true;
 * 共4个分支，需要全部覆盖
 */
```

### 4.3 MC/DC 覆盖

MC/DC (Modified Condition/Decision Coverage) 是航空安全标准 DO-178C 要求：

```c
/* 被测代码: 多条件判断 */
bool enable_motor(bool power_ok, bool emergency_stop_clear, bool temperature_ok)
{
    return power_ok && emergency_stop_clear && temperature_ok;
}

/* MC/DC 要求:
 * 1. 每个条件的每个可能结果至少出现一次
 * 2. 每个条件独立影响判断结果
 * 3. 每个判断的每个结果至少出现一次
 *
 * 对于 N 个条件，最少需要 N+1 个测试用例
 * 3个条件 -> 最少4个测试用例
 */

/* MC/DC 覆盖测试集 */
void test_mcdc_coverage(void)
{
    /* 测试用例1: 全为真，结果真 */
    /* 证明 power_ok 影响结果 */
    TEST_ASSERT_TRUE(enable_motor(true, true, true));

    /* 测试用例2: power_ok=F，结果假 */
    /* 证明 power_ok 独立影响结果 */
    TEST_ASSERT_FALSE(enable_motor(false, true, true));

    /* 测试用例3: emergency_stop_clear=F，结果假 */
    /* 证明 emergency_stop_clear 独立影响结果 */
    TEST_ASSERT_FALSE(enable_motor(true, false, true));

    /* 测试用例4: temperature_ok=F，结果假 */
    /* 证明 temperature_ok 独立影响结果 */
    TEST_ASSERT_FALSE(enable_motor(true, true, false));

    /* 验证: 每个条件改变时，输出相应改变，证明独立性 */
}

/* MC/DC 真值表验证 */
/*
 * 条件: P=power_ok, E=emergency_stop_clear, T=temperature_ok
 *
 * 用例 | P | E | T | 结果 | 说明
 * -----|---|---|---|------|------------------
 *  1   | 1 | 1 | 1 |  1   | 基线
 *  2   | 0 | 1 | 1 |  0   | P 独立影响
 *  3   | 1 | 0 | 1 |  0   | E 独立影响
 *  4   | 1 | 1 | 0 |  0   | T 独立影响
 *
 * 每个条件都有两次出现，一次为真一次为假，且该条件是唯一改变的因素
 */
```

### 4.4 覆盖率工具使用

**gcov/lcov 使用示例**:

```bash
# 编译时启用覆盖率
# 编译选项: -fprofile-arcs -ftest-coverage
gcc -fprofile-arcs -ftest-coverage -O0 -g \
    test_runner.c calculator.c unity.c \
    -o test_runner

# 运行测试
./test_runner

# 生成覆盖率报告
gcov calculator.c

# 使用 lcov 生成 HTML 报告
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info

# 生成 HTML
genhtml coverage.info --output-directory coverage_html

# 查看报告
# coverage_html/index.html
```

**Makefile 示例**:

```makefile
# 覆盖率构建目标
COVERAGE_CFLAGS = -fprofile-arcs -ftest-coverage -O0 -g
COVERAGE_LDFLAGS = -lgcov

.PHONY: coverage

coverage: CFLAGS += $(COVERAGE_CFLAGS)
coverage: LDFLAGS += $(COVERAGE_LDFLAGS)
coverage: test_runner
 @echo "Running tests with coverage..."
 @./test_runner
 @echo "Generating coverage report..."
 @gcov $(SRC_FILES)
 @lcov --capture --directory . --output-file coverage.info
 @genhtml coverage.info --output-directory coverage_report
 @echo "Coverage report: coverage_report/index.html"

coverage_clean:
 rm -f *.gcov *.gcda *.gcno coverage.info
 rm -rf coverage_report
```

---

## 5. 测试驱动开发

### 5.1 TDD 流程

TDD (Test-Driven Development) 的基本流程：

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│   1. 编写失败    │ --> │   2. 编写通过    │ --> │   3. 重构代码    │
│     的测试      │     │    的代码       │     │                │
└─────────────────┘     └─────────────────┘     └─────────────────┘
         ^                                               │
         └───────────────────────────────────────────────┘
                          循环
```

### 5.2 红绿重构循环

```c
/* TDD 示例：实现一个栈 */

/* ========== 第1轮：基本功能 ========== */

/* 红色阶段：编写失败的测试 */
void test_stack_should_push_and_pop(void)
{
    stack_t stack;
    stack_init(&stack);

    stack_push(&stack, 42);
    int32_t value;
    stack_pop(&stack, &value);

    TEST_ASSERT_EQUAL(42, value);  /* 失败：栈未实现 */
}

/* 绿色阶段：最简实现 */
typedef struct {
    int32_t data;
    bool has_data;
} stack_t;

void stack_init(stack_t* s) {
    s->has_data = false;
}

int32_t stack_push(stack_t* s, int32_t v) {
    s->data = v;
    s->has_data = true;
    return 0;
}

int32_t stack_pop(stack_t* s, int32_t* v) {
    if (s->has_data) {
        *v = s->data;
        s->has_data = false;
        return 0;
    }
    return -1;
}

/* 重构阶段：代码已足够简单，无需重构 */

/* ========== 第2轮：支持多个元素 ========== */

/* 红色阶段 */
void test_stack_should_handle_multiple_values(void)
{
    stack_t stack;
    stack_init(&stack);

    stack_push(&stack, 10);
    stack_push(&stack, 20);
    stack_push(&stack, 30);

    int32_t v1, v2, v3;
    stack_pop(&stack, &v1);
    stack_pop(&stack, &v2);
    stack_pop(&stack, &v3);

    TEST_ASSERT_EQUAL(30, v1);  /* LIFO */
    TEST_ASSERT_EQUAL(20, v2);
    TEST_ASSERT_EQUAL(10, v3);
}

/* 绿色阶段：扩展实现 */
#define STACK_CAPACITY 16

typedef struct {
    int32_t data[STACK_CAPACITY];
    uint32_t count;
} stack_t;

void stack_init(stack_t* s) {
    s->count = 0;
}

int32_t stack_push(stack_t* s, int32_t v) {
    if (s->count >= STACK_CAPACITY) {
        return -1;
    }
    s->data[s->count++] = v;
    return 0;
}

int32_t stack_pop(stack_t* s, int32_t* v) {
    if (s->count == 0) {
        return -1;
    }
    *v = s->data[--s->count];
    return 0;
}

/* ========== 第3轮：边界条件 ========== */

/* 红色阶段 */
void test_stack_should_handle_empty_pop(void)
{
    stack_t stack;
    stack_init(&stack);

    int32_t value;
    int32_t result = stack_pop(&stack, &value);

    TEST_ASSERT_EQUAL(-1, result);  /* 应返回错误 */
}

void test_stack_should_handle_overflow(void)
{
    stack_t stack;
    stack_init(&stack);

    /* 填充栈 */
    for (int32_t i = 0; i < STACK_CAPACITY; i++) {
        TEST_ASSERT_EQUAL(0, stack_push(&stack, i));
    }

    /* 尝试超容 */
    TEST_ASSERT_EQUAL(-1, stack_push(&stack, 999));
}

/* 绿色阶段：实现已在上一阶段完成 */
/* 验证所有测试通过 */

/* ========== 重构阶段 ========== */
/* 提取魔术数字 */
/* 统一错误码 */
/* 添加内联文档 */

typedef enum {
    STACK_OK = 0,
    STACK_ERROR_EMPTY = -1,
    STACK_ERROR_FULL = -2
} stack_status_t;

#define STACK_MAX_CAPACITY 16

typedef struct {
    int32_t data[STACK_MAX_CAPACITY];
    uint32_t count;
} stack_t;

/**
 * @brief 初始化栈
 * @param s 栈实例指针
 * @pre s != NULL
 */
static inline void stack_init(stack_t* s) {
    s->count = 0;
}

/**
 * @brief 压栈
 * @param s 栈实例指针
 * @param v 要压入的值
 * @return STACK_OK 或 STACK_ERROR_FULL
 */
static inline stack_status_t stack_push(stack_t* s, int32_t v) {
    if (s->count >= STACK_MAX_CAPACITY) {
        return STACK_ERROR_FULL;
    }
    s->data[s->count++] = v;
    return STACK_OK;
}

/**
 * @brief 弹栈
 * @param s 栈实例指针
 * @param v 输出参数，存储弹出的值
 * @return STACK_OK 或 STACK_ERROR_EMPTY
 */
static inline stack_status_t stack_pop(stack_t* s, int32_t* v) {
    if (s->count == 0) {
        return STACK_ERROR_EMPTY;
    }
    *v = s->data[--s->count];
    return STACK_OK;
}
```

---

## 6. 深入理解

### 6.1 形式化定义

**测试充分性形式化定义**:

设程序 P 包含语句集合 S = {s₁, s₂, ..., sₙ}，测试套件 T = {t₁, t₂, ..., tₘ}。

**定义 1 (语句覆盖充分性)**:

```
Cover_stmt(T, P) = |{s ∈ S : ∃t ∈ T, 执行(t, P) 覆盖 s}| / |S| ≥ θ
```

其中 θ 为覆盖阈值，通常 θ = 1.0 (100%)。

**定义 2 (分支覆盖充分性)**:

```
设 B = {b₁, b₂, ..., bₖ} 为程序中所有分支的集合
Cover_branch(T, P) = |{b ∈ B : T 覆盖 b 的 True 和 False}| / |B| ≥ θ
```

**定义 3 (MC/DC 覆盖充分性)**:

```
对于决策 D = c₁ ∘ c₂ ∘ ... ∘ cₙ (其中 ∘ ∈ {&&, ||})
T 满足 MC/DC 当且仅当:
∀cᵢ ∈ D: ∃t₁, t₂ ∈ T:
  1. t₁(cᵢ) = true, t₂(cᵢ) = false
  2. t₁ 和 t₂ 在其他所有条件 cⱼ (j≠i) 上取值相同
  3. D(t₁) ≠ D(t₂)
```

### 6.2 测试充分性

**覆盖率计算数学公式**:

```
语句覆盖率 = (已执行语句数 / 可执行语句总数) × 100%

分支覆盖率 = (已执行分支数 / 总分支数) × 100%

条件覆盖率 = (已评估条件结果数 / 所有条件可能结果总数) × 100%

MC/DC 覆盖率 = (满足独立影响证明的条件数 / 总条件数) × 100%

函数覆盖率 = (已调用函数数 / 总函数数) × 100%

行覆盖率 = (已执行代码行数 / 总代码行数) × 100%
```

**测试充分性层次**:

```
┌─────────────────────────────────────────────────────────────┐
│                     形式化验证 (Formal Verification)         │
│                         100% 正确性                          │
├─────────────────────────────────────────────────────────────┤
│                     MC/DC 覆盖                               │
│                   DO-178C A级要求                            │
├─────────────────────────────────────────────────────────────┤
│                     分支覆盖 + 条件覆盖                       │
│                   高安全要求系统                             │
├─────────────────────────────────────────────────────────────┤
│                     语句覆盖                                 │
│                   基础质量要求                               │
├─────────────────────────────────────────────────────────────┤
│                     无测试                                   │
│                   无法验证正确性                             │
└─────────────────────────────────────────────────────────────┘
```

### 6.3 层次映射

**单元测试与集成测试关系**:

```
┌────────────────────────────────────────────────────────────┐
│                     系统测试层                              │
│           (System Testing - 端到端验证)                      │
├────────────────────────────────────────────────────────────┤
│                     集成测试层                              │
│    (Integration Testing - 模块间接口验证)                   │
│                                                            │
│    关注点:                                                  │
│    - 模块间数据流                                            │
│    - 接口契约                                                │
│    - 时序依赖                                                │
├────────────────────────────────────────────────────────────┤
│                     单元测试层                              │
│    (Unit Testing - 函数/模块级验证)                         │
│                                                            │
│    关注点:                                                  │
│    - 功能正确性                                              │
│    - 边界条件                                                │
│    - 错误处理                                                │
│    - 覆盖率                                                  │
├────────────────────────────────────────────────────────────┤
│                     被测代码                                │
│    (SUT - System Under Test)                                │
└────────────────────────────────────────────────────────────┘

测试金字塔:
    /\
   /  \      系统测试  (少量)
  /____\
 /      \    集成测试  (中等)
/________\
/          \  单元测试  (大量)
/____________\
```

**测试与持续集成关系**:

```
┌──────────────────────────────────────────────────────────────┐
│                     持续集成流水线                            │
│                                                              │
│  [代码提交] --> [编译检查] --> [静态分析] --> [单元测试]       │
│                                    │                         │
│                                    ▼                         │
│                              ┌─────────────┐                 │
│                              │ 覆盖率阈值   │                 │
│                              │   检查      │                 │
│                              └──────┬──────┘                 │
│                                     │                        │
│                              ┌──────▼──────┐                 │
│                              │  < 阈值?    │                 │
│                              │  是 → 失败  │                 │
│                              │  否 → 继续  │                 │
│                              └──────┬──────┘                 │
│                                     ▼                        │
│  [集成测试] --> [性能测试] --> [部署] --> [监控]              │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

## 7. 思维表征

### 7.1 测试策略决策树

```
                                    开始测试规划
                                         │
                    ┌────────────────────┼────────────────────┐
                    ▼                    ▼                    ▼
               代码复杂度              安全等级              资源约束
            ┌────┴────┐           ┌────┴────┐           ┌────┴────┐
            ▼         ▼           ▼         ▼           ▼         ▼
         简单(1-10) 复杂(>10)   SIL-A/B    SIL-C/D     受限      充足
            │         │           │         │           │         │
            ▼         ▼           ▼         ▼           ▼         ▼
       基础测试    全面测试    分支覆盖    MC/DC     Unity    Check/
       (80%)      (100%)      (100%)     (100%)    框架     CMocka
            │         │           │         │           │         │
            └─────────┴───────────┴─────────┴───────────┴─────────┘
                                    │
                                    ▼
                            ┌───────────────┐
                            │  测试框架选择  │
                            │  + 覆盖目标    │
                            └───────────────┘
                                    │
                    ┌───────────────┼───────────────┐
                    ▼               ▼               ▼
                 需要Mock       执行速度         报告格式
               ┌────┴────┐     ┌────┴────┐     ┌────┴────┐
               ▼         ▼     ▼         ▼     ▼         ▼
              是         否    快         慢   简单      丰富
               │         │     │         │     │         │
               ▼         ▼     ▼         ▼     ▼         ▼
            CMocka     Unity  本地      目标   文本      XML/
            /CMock              执行     执行          HTML
```

### 7.2 测试框架对比矩阵

```
┌─────────────────┬─────────────┬─────────────┬─────────────┬─────────────┐
│     特性        │    Unity    │   CMocka    │    Check    │   自定义     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 代码大小        │     ★★★     │     ★★☆     │     ★★☆     │     ★★★     │
│ (越小越好)      │   ~5KB      │   ~15KB     │   ~20KB     │   按需       │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 功能丰富度      │     ★★☆     │     ★★★     │     ★★★     │     ★☆☆     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 学习曲线        │     ★★★     │     ★★☆     │     ★★☆     │     ★★★     │
│ (越平缓越好)    │   简单       │   中等       │   中等       │   可控       │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ Mock支持        │     ★☆☆     │     ★★★     │     ★☆☆     │     ★★☆     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 进程隔离        │     ☆☆☆     │     ☆☆☆     │     ★★★     │     ☆☆☆     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 输出格式        │     ★★☆     │     ★★★     │     ★★★     │     ★☆☆     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 嵌入式适用      │     ★★★     │     ★★☆     │     ★☆☆     │     ★★★     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 社区活跃度      │     ★★★     │     ★★☆     │     ★★☆     │     ☆☆☆     │
├─────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤
│ 推荐场景        │  嵌入式/     │  通用桌面/   │  复杂业务/   │  特殊平台/   │
│                 │  资源受限    │  需要Mock   │  需要隔离   │  最小依赖    │
└─────────────────┴─────────────┴─────────────┴─────────────┴─────────────┘
```

### 7.3 覆盖率类型层次图

```
                            ┌─────────────────┐
                            │    路径覆盖      │
                            │  (Path Coverage) │
                            │  所有执行路径    │
                            └────────┬────────┘
                                     │
                                     │  包含
                                     ▼
                          ┌──────────────────────┐
                          │      MC/DC 覆盖       │
                          │ (Modified Condition/  │
                          │   Decision Coverage)  │
                          │ 每个条件独立影响决策  │
                          └──────────┬───────────┘
                                     │
                                     │  包含
                                     ▼
                          ┌──────────────────────┐
                          │      条件覆盖         │
                          │ (Condition Coverage)  │
                          │ 每个布尔子表达式结果  │
                          └──────────┬───────────┘
                                     │
                                     │  包含
                                     ▼
                          ┌──────────────────────┐
                          │      分支覆盖         │
                          │ (Branch Coverage)     │
                          │ 每个判断的真/假分支   │
                          └──────────┬───────────┘
                                     │
                                     │  包含
                                     ▼
                          ┌──────────────────────┐
                          │      语句覆盖         │
                          │ (Statement Coverage)  │
                          │ 每行代码至少执行一次  │
                          └──────────┬───────────┘
                                     │
                                     │  包含
                                     ▼
                          ┌──────────────────────┐
                          │      函数覆盖         │
                          │ (Function Coverage)   │
                          │ 每个函数至少调用一次  │
                          └──────────┬───────────┘
                                     │
                                     │  包含
                                     ▼
                          ┌──────────────────────┐
                          │      行覆盖           │
                          │   (Line Coverage)     │
                          │ 每行源代码至少一次    │
                          └──────────────────────┘

层次说明:
=========
难度/成本:  行 < 函数 < 语句 < 分支 < 条件 < MC/DC < 路径
有效性:     行 < 函数 < 语句 < 分支 < 条件 < MC/DC < 路径

工业标准:
- DO-178C Level A: 需要 MC/DC (100%)
- DO-178C Level B: 需要分支覆盖 (100%)
- DO-178C Level C: 需要语句覆盖 (100%)
- ISO 26262 ASIL D: 推荐 MC/DC
- IEC 61508 SIL 4: 推荐 MC/DC
```

---

## 8. 总结

C 语言单元测试是保障代码质量的核心实践。关键要点：

**测试框架选择**:

- Unity: 嵌入式首选，零依赖
- CMocka: 需要内置 Mock 时
- Check: 需要进程隔离和超时控制

**测试设计原则**:

- 遵循 AAA 模式保持结构清晰
- 参数化测试减少重复代码
- 边界值和等价类划分确保全面覆盖

**Mock 技术**:

- 函数打桩: 最灵活，需维护状态
- 弱符号: 编译器支持，自动替换
- 链接期替换: 构建系统控制

**覆盖率策略**:

- 语句覆盖: 基础要求 (≥80%)
- 分支覆盖: 安全关键 (100%)
- MC/DC: 航空/汽车最高安全等级

**TDD 实践**:

- 红绿重构循环
- 小步快跑，持续验证
- 测试即文档

---

**参考资源**:

- Unity: <https://github.com/ThrowTheSwitch/Unity>
- CMocka: <https://git.cryptomilk.org/projects/cmocka.git>
- Check: <https://libcheck.github.io/check/>
- gcov: <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>
- DO-178C: Software Considerations in Airborne Systems
