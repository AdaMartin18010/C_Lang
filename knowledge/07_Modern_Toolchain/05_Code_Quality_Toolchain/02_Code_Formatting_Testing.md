# 代码格式化与测试框架

> **层级定位**: 07_Modern_Toolchain > 05_Code_Quality_Toolchain
> **难度级别**: L3 (中级)
> **前置知识**: 构建系统基础
> **预估学习时间**: 1-2 小时

---

## 模块概述

统一的代码格式和自动化测试是保证代码质量的基础。
本指南介绍C语言项目的格式化工具和测试框架配置。

---

## 学习目标

1. **配置代码格式化** - clang-format、indent
2. **设置自动化格式化** - 提交前检查、CI验证
3. **选择和配置测试框架** - Unity、Check、CMocka
4. **集成测试到构建流程** - CMake集成、覆盖率

---

## 一、代码格式化

### 1.1 clang-format配置

```yaml
# .clang-format
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
---
Language: C
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100

# 对齐
AlignAfterOpenBracket: Align
AlignConsecutiveMacros: true
AlignConsecutiveAssignments: true
AlignConsecutiveDeclarations: true
AlignEscapedNewlines: Right
AlignOperands: true
AlignTrailingComments: true

# 换行
AllowAllArgumentsOnNextLine: false
AllowAllParametersOfDeclarationOnASingleLine: false
AllowShortBlocksOnASingleLine: Never
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false

# 大括号
BreakBeforeBraces: Attach
BraceWrapping:
  AfterFunction: false
  AfterControlStatement: false
  AfterEnum: false
  AfterStruct: false
  AfterUnion: false
  BeforeElse: false

# 其他
IndentCaseLabels: true
IndentGotoLabels: true
MaxEmptyLinesToKeep: 1
PointerAlignment: Right
SortIncludes: true
SpaceAfterCStyleCast: false
SpaceBeforeAssignmentOperators: true
SpaceBeforeParens: ControlStatements
SpacesInParentheses: false
SpacesInSquareBrackets: false
---
```

### 1.2 格式化脚本

```bash
#!/bin/bash
# scripts/format.sh

set -e

# 查找所有C/C++文件
FILES=$(find src include tests -name "*.c" -o -name "*.h" | grep -v "third_party")

echo "Formatting C files..."

# 检查clang-format
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found"
    exit 1
fi

# 格式化模式
if [ "$1" == "--check" ]; then
    # 仅检查，不修改
    echo "Checking formatting..."
    clang-format --dry-run --Werror $FILES
else
    # 实际格式化
    echo "Applying formatting..."
    clang-format -i $FILES
fi

echo "Done!"
```

### 1.3 Git预提交钩子

```bash
#!/bin/bash
# .git/hooks/pre-commit

# 获取要提交的C文件
FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')

if [ -z "$FILES" ]; then
    exit 0
fi

echo "Running pre-commit checks..."

# 检查格式化
for file in $FILES; do
    if [ -f "$file" ]; then
        # 检查是否需要格式化
        diff -q <(cat "$file") <(clang-format "$file") > /dev/null
        if [ $? -ne 0 ]; then
            echo "Error: $file is not properly formatted"
            echo "Run: clang-format -i $file"
            exit 1
        fi
    fi
done

# 运行静态分析
# cppcheck --error-exitcode=1 $FILES

echo "Pre-commit checks passed!"
```

---

## 二、测试框架

### 2.1 Unity测试框架

```c
// tests/test_example.c
#include "unity.h"
#include "mylib.h"

void setUp(void) {
    // 每个测试前执行
}

void tearDown(void) {
    // 每个测试后执行
}

void test_addition(void) {
    TEST_ASSERT_EQUAL(4, add(2, 2));
    TEST_ASSERT_EQUAL(0, add(0, 0));
    TEST_ASSERT_EQUAL(-5, add(-2, -3));
}

void test_subtraction(void) {
    TEST_ASSERT_EQUAL(2, subtract(5, 3));
    TEST_ASSERT_EQUAL(-5, subtract(0, 5));
}

void test_null_pointer(void) {
    int* ptr = NULL;
    TEST_ASSERT_NULL(ptr);
}

void test_string_equal(void) {
    TEST_ASSERT_EQUAL_STRING("hello", "hello");
    TEST_ASSERT_NOT_EQUAL("hello", "world");
}

// 测试入口
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_addition);
    RUN_TEST(test_subtraction);
    RUN_TEST(test_null_pointer);
    RUN_TEST(test_string_equal);

    return UNITY_END();
}
```

### 2.2 CMake集成Unity

```cmake
# CMakeLists.txt 测试部分

include(FetchContent)

# 获取Unity
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(unity)

# 测试可执行文件
function(add_unity_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name}
        PRIVATE
            mylib
            unity
    )
    target_include_directories(${test_name}
        PRIVATE
            ${unity_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/include
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# 添加测试
add_unity_test(test_math tests/test_math.c)
add_unity_test(test_string tests/test_string.c)
add_unity_test(test_fsm tests/test_fsm.c)

# 内存检查
find_program(VALGRIND valgrind)
if(VALGRIND)
    add_test(NAME test_valgrind
        COMMAND ${VALGRIND}
            --leak-check=full
            --error-exitcode=1
            $<TARGET_FILE:test_math>
    )
endif()
```

### 2.3 测试固件(Fixtures)

```c
// tests/test_with_fixture.c
#include "unity.h"
#include "database.h"

typedef struct {
    Database* db;
    char* temp_file;
} TestFixture;

static TestFixture fixture;

void setUp(void) {
    // 创建临时数据库
    fixture.temp_file = tempnam(NULL, "test_db_");
    fixture.db = db_open(fixture.temp_file);
    TEST_ASSERT_NOT_NULL(fixture.db);
}

void tearDown(void) {
    // 清理
    db_close(fixture.db);
    remove(fixture.temp_file);
    free(fixture.temp_file);
}

void test_insert_and_retrieve(void) {
    Record rec = {.id = 1, .name = "Test"};

    TEST_ASSERT_TRUE(db_insert(fixture.db, &rec));

    Record* result = db_get(fixture.db, 1);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(1, result->id);
    TEST_ASSERT_EQUAL_STRING("Test", result->name);
}

void test_delete_record(void) {
    Record rec = {.id = 1, .name = "ToDelete"};
    db_insert(fixture.db, &rec);

    TEST_ASSERT_TRUE(db_delete(fixture.db, 1));
    TEST_ASSERT_NULL(db_get(fixture.db, 1));
}
```

---

## 三、测试最佳实践

### 3.1 测试金字塔

```
┌─────────────────────────────────────────────────────────────┐
│                      测试金字塔                              │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│        ┌─────────────┐                                       │
│        │  E2E Tests  │  少量, 验证完整流程                   │
│        │    (10%)    │                                       │
│        └──────┬──────┘                                       │
│               │                                              │
│        ┌──────┴──────┐                                       │
│        │Integration  │  验证组件交互                         │
│        │  Tests(20%) │                                       │
│        └──────┬──────┘                                       │
│               │                                              │
│        ┌──────┴──────┐                                       │
│        │  Unit Tests │  大量, 验证函数行为                   │
│        │    (70%)    │                                       │
│        └─────────────┘                                       │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 测试命名规范

```c
// 良好命名
void test_functionName_condition_expectedResult(void)
void test_add_positiveNumbers_returnsSum(void)
void test_parse_nullInput_returnsError(void)
void test_buffer_overflow_protectionTriggered(void)

// 测试分组
// test_math.c - 数学函数测试
// test_string.c - 字符串处理测试
// test_fsm.c - 状态机测试
// test_integration.c - 集成测试
```

### 3.3 边界条件测试

```c
void test_boundary_conditions(void) {
    // 空值
    TEST_ASSERT_NULL(process(NULL));

    // 最小值
    TEST_ASSERT_EQUAL(0, factorial(0));

    // 最大值
    int max = INT_MAX;
    TEST_ASSERT_EQUAL(INT_MAX, max);

    // 边界值
    char buffer[10];
    TEST_ASSERT_FALSE(copy_string(buffer, sizeof(buffer), "12345678901"));
    TEST_ASSERT_TRUE(copy_string(buffer, sizeof(buffer), "123456789"));
}
```

---

## 四、CI/CD集成

```yaml
# .github/workflows/quality.yml
name: Code Quality

on: [push, pull_request]

jobs:
  formatting:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install clang-format
        run: sudo apt-get install -y clang-format

      - name: Check formatting
        run: |
          find src include tests -name "*.c" -o -name "*.h" | \
          xargs clang-format --dry-run --Werror

  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]

    steps:
      - uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B build -DBUILD_TESTS=ON

      - name: Build
        run: cmake --build build

      - name: Test
        run: ctest --test-dir build --output-on-failure

  coverage:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get install -y lcov

      - name: Build with coverage
        run: |
          cmake -B build -DENABLE_COVERAGE=ON -DBUILD_TESTS=ON
          cmake --build build

      - name: Run tests
        run: ctest --test-dir build

      - name: Generate coverage report
        run: |
          cmake --build build --target coverage

      - name: Upload coverage
        uses: codecov/codecov-action@v3
        with:
          files: build/coverage.info
```

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L3 (深化)
