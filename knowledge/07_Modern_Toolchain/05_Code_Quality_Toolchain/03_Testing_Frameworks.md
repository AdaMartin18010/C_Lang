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

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../readme.md](../../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 现代工具链  
**难度等级**: L2-L4  
**前置依赖**: 核心知识体系  
**后续延伸**: 工程实践

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/readme.md](../../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构
```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/readme.md) |

---

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

**← [返回工具链主页](../readme.md)**


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
