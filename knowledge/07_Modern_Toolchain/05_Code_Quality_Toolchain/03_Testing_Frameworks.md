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
}

void test_divide(void) {
    TEST_ASSERT_EQUAL(2, divide(6, 3));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_divide);
    return UNITY_END();
}
```

### CMake集成

```cmake
enable_testing()

add_executable(test_math test_math.c)
target_link_libraries(test_math unity mathlib)

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

static void test_add(void **state) {
    (void) state;
    assert_int_equal(add(2, 3), 5);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_add),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

---

**← [返回工具链主页](../README.md)**
