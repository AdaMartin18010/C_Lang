# C语言测试框架指南

> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L3 进阶
> **预估学习时间**: 4-6小时

---

## 📋 测试框架对比

| 框架 | 特点 | 适用场景 |
|:-----|:-----|:---------|
| **Unity** | 轻量，嵌入式友好 | 嵌入式/资源受限 |
| **CMocka** | 标准C，mock支持 | 系统编程 |
| **Google Test** | C++，功能丰富 | 可与C混合 |
| **Check** | 标准C，多进程 | 并发测试 |
| **Criterion** | 现代C，直观 | 新项目 |

---

## 🚀 Unity测试框架

### 示例

```c
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
```

### CMake集成

```cmake
enable_testing()

# 添加Unity
add_subdirectory(third_party/unity)

# 测试可执行文件
add_executable(test_math test_math.c)
target_link_libraries(test_math unity mathlib)

# 添加测试
add_test(NAME MathTest COMMAND test_math)
```

---

## 🚀 CMocka测试框架

### 示例

```c
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

// 被测函数
static int add(int a, int b) {
    return a + b;
}

// 测试函数
static void test_add_positive(void **state) {
    (void) state;
    assert_int_equal(add(2, 3), 5);
}

static void test_add_negative(void **state) {
    (void) state;
    assert_int_equal(add(-2, -3), -5);
}

static void test_add_zero(void **state) {
    (void) state;
    assert_int_equal(add(0, 0), 0);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add_positive),
        cmocka_unit_test(test_add_negative),
        cmocka_unit_test(test_add_zero),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

### Mock示例

```c
#include <cmocka.h>

// Mock函数
int __wrap_external_api(void) {
    return mock_type(int);
}

static void test_with_mock(void **state) {
    (void) state;

    // 设置mock返回值
    will_return(__wrap_external_api, 42);

    // 调用被测函数
    int result = function_under_test();

    assert_int_equal(result, 42);
}
```

---

## 🚀 Criterion测试框架

### 示例

```c
#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "math_lib.h"

Test(math, add) {
    cr_assert(eq(int, add(2, 3), 5));
    cr_assert(eq(int, add(-2, 3), 1));
}

Test(math, divide, .description = "division tests") {
    cr_assert(eq(int, divide(6, 3), 2));
}

Test(math, divide_by_zero, .signal = SIGFPE) {
    divide(1, 0);  // 期望触发浮点异常
}

// 参数化测试
ParameterizedTestParameters(math, add_param) {
    static int params[] = {{1, 2, 3}, {2, 3, 5}, {0, 0, 0}};
    return cr_make_param_array(int[3], params, 3);
}

ParameterizedTest(int *params, math, add_param) {
    cr_assert(eq(int, add(params[0], params[1]), params[2]));
}
```

---

## 📚 测试组织

### 测试目录结构

```text
tests/
├── CMakeLists.txt
├── test_main.c        # 主测试入口
├── test_math.c        # 数学库测试
├── test_string.c      # 字符串测试
└── fixtures/          # 测试数据
    └── data.txt
```

### 测试命名规范

```c
// 格式: test_<模块>_<功能>_<场景>
void test_math_add_positive(void);
void test_math_add_overflow(void);
void test_string_copy_null_terminator(void);
```

---

## ✅ 测试最佳实践

- [ ] 每个函数至少一个测试
- [ ] 测试边界条件
- [ ] 测试错误处理
- [ ] 使用有意义的测试名
- [ ] 独立测试，无依赖
- [ ] 快速执行

---

**← [返回工具链主页](../README.md)**
