# 代码覆盖率指南

> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L3 进阶
> **预估学习时间**: 2-3小时

---

## 📋 覆盖率工具

| 工具 | 编译器 | 报告格式 |
|:-----|:-------|:---------|
| **gcov/lcov** | GCC | HTML |
| **llvm-cov** | Clang | HTML/文本 |
| **gcovr** | GCC | XML/HTML/Cobertura |

---


---

## 📑 目录

- [代码覆盖率指南](#代码覆盖率指南)
  - [📋 覆盖率工具](#-覆盖率工具)
  - [📑 目录](#-目录)
  - [🚀 使用gcov/lcov](#-使用gcovlcov)
    - [编译带覆盖率](#编译带覆盖率)
    - [生成HTML报告](#生成html报告)
    - [CMake集成](#cmake集成)
  - [🚀 使用llvm-cov (Clang)](#-使用llvm-cov-clang)
  - [🚀 CI集成](#-ci集成)
    - [GitHub Actions](#github-actions)
    - [覆盖率阈值检查](#覆盖率阈值检查)
  - [📊 覆盖率指标](#-覆盖率指标)
  - [✅ 最佳实践](#-最佳实践)
  - [📈 代码覆盖率深度实践](#-代码覆盖率深度实践)
    - [1. gcov/lcov 完整工作流](#1-gcovlcov-完整工作流)
    - [2. CMake完整集成](#2-cmake完整集成)
    - [3. Clang/llvm-cov 详细使用](#3-clangllvm-cov-详细使用)
    - [4. 覆盖率指标详解](#4-覆盖率指标详解)
    - [5. CI/CD集成完整配置](#5-cicd集成完整配置)
    - [6. 覆盖率阈值与质量门禁](#6-覆盖率阈值与质量门禁)
    - [7. 高级技巧](#7-高级技巧)
  - [✅ 最佳实践总结](#-最佳实践总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 🚀 使用gcov/lcov

### 编译带覆盖率

```bash
# 编译
gcc -fprofile-arcs -ftest-coverage -g main.c utils.c -o myapp

# 运行
./myapp

# 生成覆盖率报告
gcov main.c utils.c
```

### 生成HTML报告

```bash
# 收集数据
lcov --capture --directory . --output-file coverage.info

# 过滤系统文件
lcov --remove coverage.info '/usr/*' --output-file coverage.info

# 生成HTML
genhtml coverage.info --output-directory coverage_report

# 打开报告
open coverage_report/index.html
```

### CMake集成

```cmake
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
endif()

# 添加覆盖率目标
add_custom_target(coverage
    COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
```

---

## 🚀 使用llvm-cov (Clang)

```bash
# 编译
clang -fprofile-instr-generate -fcoverage-mapping main.c -o myapp

# 运行
./myapp

# 生成报告
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov report ./myapp -instr-profile=default.profdata

# HTML报告
llvm-cov show ./myapp -instr-profile=default.profdata \
    --format=html -output-dir=coverage_report
```

---

## 🚀 CI集成

### GitHub Actions

```yaml
- name: Build with Coverage
  run: |
    cmake -B build -DENABLE_COVERAGE=ON
    cmake --build build

- name: Run Tests
  run: ctest --test-dir build

- name: Generate Coverage
  run: |
    cd build
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info

- name: Upload to Codecov
  uses: codecov/codecov-action@v3
  with:
    files: ./build/coverage.info
    fail_ci_if_error: true
```

### 覆盖率阈值检查

```bash
# 检查覆盖率是否达到80%
lcov --summary coverage.info | grep "lines" | awk '{print $2}' | sed 's/%//' | \
  awk '{if ($1 < 80) exit 1}'
```

---

## 📊 覆盖率指标

| 指标 | 说明 | 目标 |
|:-----|:-----|:----:|
| 行覆盖率 | 执行的代码行比例 | >80% |
| 函数覆盖率 | 执行的函数比例 | >80% |
| 分支覆盖率 | 执行的分支比例 | >70% |
| 条件覆盖率 | 执行的条件比例 | >70% |

---

## ✅ 最佳实践

- [ ] 集成到CI流程
- [ ] 设置覆盖率阈值
- [ ] 定期查看覆盖率报告
- [ ] 关注未覆盖的关键路径
- [ ] 使用lcov过滤无关文件

---

## 📈 代码覆盖率深度实践

### 1. gcov/lcov 完整工作流

**基础编译与运行**:

```bash
# 启用覆盖率编译选项
gcc -fprofile-arcs -ftest-coverage -g -O0 \
    -o myprogram \
    main.c utils.c parser.c

# 运行程序（生成.gcda文件）
./myprogram

# 查看原始gcov数据
gcov main.c
# 输出: main.c.gcov (包含每行执行次数)
```

**gcov输出解读**:

```
# main.c.gcov
        -:    1:#include <stdio.h>
        -:    2:
        1:    3:int main() {
        1:    4:    printf("Hello\n");
    #####:    5:    if (0) {           // 从未执行
    #####:    6:        printf("Never\n");
        -:    7:    }
        1:    8:    return 0;
        -:    9:}
```

**lcov完整流程**:

```bash
#!/bin/bash
# coverage.sh - 完整的覆盖率收集脚本

set -e

# 目录配置
BUILD_DIR="build"
COVERAGE_DIR="$BUILD_DIR/coverage"
REPORT_DIR="$COVERAGE_DIR/report"

# 创建目录
mkdir -p "$REPORT_DIR"

# 初始化lcov
echo "Initializing coverage data..."
lcov --directory "$BUILD_DIR" --zerocounters

# 运行测试
echo "Running tests..."
ctest --test-dir "$BUILD_DIR" --output-on-failure

# 收集数据
echo "Collecting coverage data..."
lcov \
    --capture \
    --directory "$BUILD_DIR" \
    --output-file "$COVERAGE_DIR/coverage.raw.info" \
    --rc lcov_branch_coverage=1

# 过滤数据
echo "Filtering coverage data..."
lcov \
    --remove "$COVERAGE_DIR/coverage.raw.info" \
    '/usr/*' \
    '*/tests/*' \
    '*/third_party/*' \
    '*.h' \
    --output-file "$COVERAGE_DIR/coverage.info" \
    --rc lcov_branch_coverage=1

# 生成摘要
echo "Coverage Summary:"
lcov --summary "$COVERAGE_DIR/coverage.info"

# 生成HTML报告
echo "Generating HTML report..."
genhtml \
    "$COVERAGE_DIR/coverage.info" \
    --output-directory "$REPORT_DIR" \
    --branch-coverage \
    --highlight \
    --legend \
    --title "My Project Coverage"

echo "Report generated at: $REPORT_DIR/index.html"
```

### 2. CMake完整集成

```cmake
# CMakeLists.txt - 覆盖率完整配置

cmake_minimum_required(VERSION 3.13)
project(MyProject C)

# 选项
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(COVERAGE_BRANCH "Enable branch coverage" ON)
option(COVERAGE_HTML "Generate HTML report" ON)
option(COVERAGE_XML "Generate XML report for CI" ON)

# 覆盖率配置
if(ENABLE_COVERAGE)
    # 检查编译器
    if(NOT CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "Coverage requires GCC or Clang")
    endif()

    # 检查工具
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    find_program(GCOVR_PATH gcovr)

    if(NOT LCOV_PATH)
        message(WARNING "lcov not found, coverage targets disabled")
    endif()

    # 添加编译选项
    add_compile_options(
        -O0          # 禁用优化，确保准确的行号
        -g           # 调试信息
        --coverage   # 覆盖率插桩
    )

    add_link_options(--coverage)

    # 创建统一的目标
    add_custom_target(coverage
        COMMENT "Running coverage analysis..."
    )

    # lcov目标
    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage-lcov
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV_PATH}
                --capture
                --directory .
                --output-file coverage.info
                --rc lcov_branch_coverage=${COVERAGE_BRANCH}
            COMMAND ${LCOV_PATH}
                --remove coverage.info
                '/usr/*' '/opt/*' '*/tests/*' '*/test/*' '*.h'
                --output-file coverage.filtered.info
                --rc lcov_branch_coverage=${COVERAGE_BRANCH}
            COMMAND ${LCOV_PATH} --summary coverage.filtered.info
            COMMAND ${GENHTML_PATH}
                coverage.filtered.info
                --output-directory coverage-html
                --branch-coverage
                --highlight
                --legend
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating lcov coverage report"
        )
        add_dependencies(coverage coverage-lcov)
    endif()

    # gcovr目标 (XML输出，用于CI)
    if(GCOVR_PATH AND COVERAGE_XML)
        add_custom_target(coverage-xml
            COMMAND ${GCOVR_PATH}
                -r ${CMAKE_SOURCE_DIR}
                --xml
                --output coverage.xml
                --exclude-directories 'tests|test|third_party'
                ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating XML coverage report"
        )
        add_dependencies(coverage coverage-xml)
    endif()

    # HTML报告 (gcovr)
    if(GCOVR_PATH AND COVERAGE_HTML)
        add_custom_target(coverage-html
            COMMAND ${GCOVR_PATH}
                -r ${CMAKE_SOURCE_DIR}
                --html
                --html-details
                --output coverage/index.html
                --exclude-directories 'tests|test|third_party'
                ${CMAKE_BINARY_DIR}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating gcovr HTML report"
        )
        add_dependencies(coverage coverage-html)
    endif()

    # 快速检查目标
    add_custom_target(coverage-quick
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${LCOV_PATH}
            --capture
            --directory .
            --output-file coverage.quick.info
        COMMAND ${LCOV_PATH} --summary coverage.quick.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Quick coverage check"
    )
endif()

# 添加库和测试
add_library(mylib STATIC src/mylib.c)
target_include_directories(mylib PUBLIC include)

enable_testing()
add_subdirectory(tests)
```

### 3. Clang/llvm-cov 详细使用

**编译与运行**:

```bash
# 使用Clang编译
clang -fprofile-instr-generate \
      -fcoverage-mapping \
      -g -O0 \
      -o myprogram \
      main.c utils.c

# 运行程序
LLVM_PROFILE_FILE="myprogram.profraw" ./myprogram

# 或运行多个测试，生成多个profile文件
for test in tests/*; do
    LLVM_PROFILE_FILE="${test}.profraw" ./$test
done

# 合并profile文件
llvm-profdata merge \
    -sparse \
    *.profraw \
    -o combined.profdata
```

**生成报告**:

```bash
# 文本报告
llvm-cov report \
    ./myprogram \
    -instr-profile=combined.profdata \
    -ignore-filename-regex='(tests|/usr)' \
    -show-functions

# 详细报告
llvm-cov show \
    ./myprogram \
    -instr-profile=combined.profdata \
    -format=html \
    -output-dir=coverage-report \
    -show-branches=count \
    -show-expansions \
    -show-instantiations \
    -show-regions \
    -ignore-filename-regex='(tests|/usr)'
```

**与CMake集成**:

```cmake
if(ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Clang使用不同的覆盖率选项
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}
        -fprofile-instr-generate
        -fcoverage-mapping")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
        -fprofile-instr-generate")

    find_program(LLVM_PROFDATA llvm-profdata)
    find_program(LLVM_COV llvm-cov)

    if(LLVM_PROFDATA AND LLVM_COV)
        add_custom_target(coverage-clang
            COMMAND ${CMAKE_COMMAND} -E env
                LLVM_PROFILE_DIR=${CMAKE_BINARY_DIR}/profiles
                ${CMAKE_CTEST_COMMAND}
            COMMAND ${LLVM_PROFDATA} merge
                -sparse ${CMAKE_BINARY_DIR}/profiles/*.profraw
                -o ${CMAKE_BINARY_DIR}/default.profdata
            COMMAND ${LLVM_COV} report
                -instr-profile=${CMAKE_BINARY_DIR}/default.profdata
                ${CMAKE_BINARY_DIR}/test-runner
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()
endif()
```

### 4. 覆盖率指标详解

```
┌────────────────────────────────────────────────────────────────┐
│                       覆盖率指标说明                            │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│  1. 行覆盖率 (Line Coverage)                                    │
│     公式: 执行的代码行数 / 可执行代码总行数                      │
│     示例: 100行代码中80行被执行 = 80%                           │
│     重要性: ⭐⭐⭐⭐⭐                                            │
│                                                                │
│  2. 函数覆盖率 (Function Coverage)                              │
│     公式: 调用的函数数 / 总函数数                               │
│     示例: 20个函数中16个被调用 = 80%                            │
│     重要性: ⭐⭐⭐⭐                                              │
│                                                                │
│  3. 分支覆盖率 (Branch Coverage)                                │
│     公式: 执行的分支数 / 总分支数                               │
│     示例: if-else有2个分支，只执行了if = 50%                    │
│     重要性: ⭐⭐⭐⭐⭐                                            │
│                                                                │
│  4. 条件覆盖率 (Condition Coverage)                             │
│     公式: 评估的条件结果数 / 可能的条件结果数                    │
│     示例: if (a && b) 有4种组合，执行了2种 = 50%               │
│     重要性: ⭐⭐⭐⭐                                              │
│                                                                │
│  5. MC/DC (Modified Condition/Decision Coverage)               │
│     要求: 每个条件独立影响决策结果                               │
│     主要用于: 航空、汽车等安全关键领域                           │
│     重要性: ⭐⭐⭐⭐⭐ (DO-178C要求)                              │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

**代码示例 - 不同覆盖率指标**:

```c
int calculate(int a, int b, int op) {   // 函数入口
    int result = 0;                      // 行覆盖率: 1

    if (op == 1) {                       // 分支1: true/false
        result = a + b;                  // 行覆盖率: ?
    } else if (op == 2) {                // 分支2: true/false
        result = a - b;                  // 行覆盖率: ?
    } else if (op == 3 && b != 0) {      // 条件: 4种组合
        result = a / b;                  // 行覆盖率: ?
    } else {
        result = -1;                     // 默认分支
    }

    return result;                       // 行覆盖率: 1
}
```

### 5. CI/CD集成完整配置

**GitHub Actions**:

```yaml
# .github/workflows/coverage.yml
name: Code Coverage

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  coverage:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y lcov gcovr cmake

    - name: Configure with coverage
      run: |
        cmake -B build \
          -DCMAKE_BUILD_TYPE=Debug \
          -DENABLE_COVERAGE=ON \
          -DCMAKE_C_COMPILER=gcc

    - name: Build
      run: cmake --build build --parallel

    - name: Run tests
      run: ctest --test-dir build --output-on-failure

    - name: Generate coverage report (lcov)
      run: |
        cd build
        lcov --capture --directory . --output-file coverage.info
        lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
        lcov --summary coverage.info

    - name: Generate XML report (gcovr)
      run: |
        cd build
        gcovr -r .. --xml --output coverage.xml .

    - name: Check coverage threshold
      run: |
        cd build
        COVERAGE=$(lcov --summary coverage.info | grep "lines" | grep -oP '\d+\.?\d+' | head -1)
        echo "Line coverage: $COVERAGE%"
        if (( $(echo "$COVERAGE < 80" | bc -l) )); then
          echo "Coverage below threshold (80%)"
          exit 1
        fi

    - name: Upload to Codecov
      uses: codecov/codecov-action@v3
      with:
        files: ./build/coverage.info
        fail_ci_if_error: true
        verbose: true

    - name: Upload HTML report
      uses: actions/upload-artifact@v4
      with:
        name: coverage-report
        path: build/coverage-html/
```

**GitLab CI**:

```yaml
# .gitlab-ci.yml
coverage:
  stage: test
  image: gcc:latest
  before_script:
    - apt-get update && apt-get install -y lcov cmake
  script:
    - cmake -B build -DENABLE_COVERAGE=ON
    - cmake --build build
    - ctest --test-dir build
    - cd build && lcov --capture --directory . --output-file coverage.info
    - lcov --remove coverage.info '/usr/*' --output-file coverage.info
    - genhtml coverage.info --output-directory coverage
  coverage: '/lines.*: (\d+\.?\d+)%/'
  artifacts:
    paths:
      - build/coverage/
    reports:
      coverage_report:
        coverage_format: cobertura
        path: build/coverage.xml
  only:
    - merge_requests
    - main
```

### 6. 覆盖率阈值与质量门禁

```bash
#!/bin/bash
# check-coverage.sh

COVERAGE_FILE="$1"
LINE_THRESHOLD=${2:-80}
BRANCH_THRESHOLD=${3:-70}

# 提取覆盖率数据
LINE_COVERAGE=$(lcov --summary "$COVERAGE_FILE" | grep "lines" | grep -oP '\d+\.?\d+' | head -1)
FUNC_COVERAGE=$(lcov --summary "$COVERAGE_FILE" | grep "functions" | grep -oP '\d+\.?\d+' | head -1)
BRANCH_COVERAGE=$(lcov --summary "$COVERAGE_FILE" | grep "branches" | grep -oP '\d+\.?\d+' | head -1)

echo "================================"
echo "Coverage Report"
echo "================================"
echo "Line Coverage:    ${LINE_COVERAGE}% (threshold: ${LINE_THRESHOLD}%)"
echo "Function Coverage: ${FUNC_COVERAGE}%"
echo "Branch Coverage:  ${BRANCH_COVERAGE}% (threshold: ${BRANCH_THRESHOLD}%)"
echo "================================"

# 检查阈值
EXIT_CODE=0

if (( $(echo "$LINE_COVERAGE < $LINE_THRESHOLD" | bc -l) )); then
    echo "❌ Line coverage below threshold!"
    EXIT_CODE=1
else
    echo "✅ Line coverage OK"
fi

if [ -n "$BRANCH_COVERAGE" ] && (( $(echo "$BRANCH_COVERAGE < $BRANCH_THRESHOLD" | bc -l) )); then
    echo "❌ Branch coverage below threshold!"
    EXIT_CODE=1
else
    echo "✅ Branch coverage OK"
fi

exit $EXIT_CODE
```

### 7. 高级技巧

**增量覆盖率**:

```bash
# 只检查变更文件的覆盖率
git diff --name-only HEAD~1 | grep '\.c$' > changed_files.txt

# 生成仅包含变更文件的覆盖率报告
gcovr --include-from-file changed_files.txt --html -o partial-coverage.html
```

**排除特定代码块**:

```c
// 使用宏排除不可达代码
#ifdef ENABLE_COVERAGE
#define COVERAGE_EXCLUDE_START
#define COVERAGE_EXCLUDE_END
#else
#define COVERAGE_EXCLUDE_START /* GCOV_EXCL_START */
#define COVERAGE_EXCLUDE_END   /* GCOV_EXCL_STOP */
#endif

// 标记不需要覆盖的代码（如错误处理）
COVERAGE_EXCLUDE_START
void panic(const char* msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();  // 永远不会返回
}
COVERAGE_EXCLUDE_END
```

**条件编译支持**:

```cmake
# 针对不同编译器的覆盖率选项
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(COVERAGE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(COVERAGE_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
endif()
```

---

## ✅ 最佳实践总结

- [ ] 集成到CI流程
- [ ] 设置覆盖率阈值
- [ ] 定期查看覆盖率报告
- [ ] 关注未覆盖的关键路径
- [ ] 使用lcov过滤无关文件
- [ ] 区分单元测试和集成测试覆盖率
- [ ] 设置增量覆盖率检查
- [ ] 文档化覆盖率策略

---

**← [返回工具链主页](../README.md)**


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
