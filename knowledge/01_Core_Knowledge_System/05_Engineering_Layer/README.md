# C 语言软件工程层指南

## 概述

软件工程是将系统化、规范化、可量化的方法应用于软件开发的过程。对于 C 语言项目，良好的软件工程实践尤为重要，因为 C 语言的底层特性使得错误代价更高、调试更困难。本指南详细介绍 C 语言项目的项目结构设计、版本控制工作流、代码审查和持续集成等工程实践。

## 项目结构设计

### 标准目录布局

```
myproject/
├── CMakeLists.txt          # 主构建配置
├── Makefile               # 简单项目备用
├── README.md              # 项目说明
├── LICENSE                # 许可证
├── .gitignore            # Git 忽略配置
├── .clang-format         # 代码格式化配置
├── .clang-tidy           # 静态分析配置
├── docs/                 # 文档
│   ├── api/              # API 文档
│   ├── design/           # 设计文档
│   └── examples/         # 使用示例
├── include/              # 公共头文件
│   └── myproject/        # 项目命名空间
│       ├── core.h
│       ├── utils.h
│       └── version.h
├── src/                  # 源代码
│   ├── core/             # 核心模块
│   │   ├── module_a.c
│   │   └── module_a.h
│   ├── utils/            # 工具模块
│   │   ├── string_utils.c
│   │   └── string_utils.h
│   └── main.c            # 主程序入口
├── tests/                # 测试代码
│   ├── unit/             # 单元测试
│   │   ├── test_core.c
│   │   └── test_utils.c
│   ├── integration/      # 集成测试
│   └── fixtures/         # 测试数据
├── third_party/          # 第三方依赖
│   └── README.md         # 说明依赖管理方式
├── scripts/              # 工具脚本
│   ├── build.sh
│   ├── test.sh
│   └── lint.sh
├── config/               # 配置文件模板
├── data/                 # 数据文件
└── tools/                # 开发工具
```

### 模块化设计原则

```c
// include/myproject/module.h - 模块公共接口
#ifndef MYPROJECT_MODULE_H
#define MYPROJECT_MODULE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 前向声明不透明结构体
typedef struct module_ctx module_ctx_t;

// 错误码定义
typedef enum {
    MODULE_OK = 0,
    MODULE_ERROR_INVALID_PARAM = -1,
    MODULE_ERROR_NO_MEMORY = -2,
    MODULE_ERROR_IO = -3,
    MODULE_ERROR_TIMEOUT = -4
} module_error_t;

// 配置选项
typedef struct {
    size_t buffer_size;
    int timeout_ms;
    bool enable_logging;
    const char* log_file;
} module_config_t;

// 生命周期管理
module_ctx_t* module_create(const module_config_t* config);
void module_destroy(module_ctx_t* ctx);

// 核心操作
module_error_t module_open(module_ctx_t* ctx, const char* path);
module_error_t module_close(module_ctx_t* ctx);
module_error_t module_process(module_ctx_t* ctx, const void* input,
                               size_t input_len, void* output,
                               size_t* output_len);

// 状态查询
const char* module_error_string(module_error_t error);
bool module_is_ready(const module_ctx_t* ctx);

// 版本信息
const char* module_version(void);

#ifdef __cplusplus
}
#endif

#endif // MYPROJECT_MODULE_H
```

```c
// src/core/module.c - 模块实现
#include "myproject/module.h"
#include <stdlib.h>
#include <string.h>

// 实现细节对外隐藏
struct module_ctx {
    module_config_t config;
    bool is_open;
    void* internal_buffer;
    // ... 其他私有字段
};

module_ctx_t* module_create(const module_config_t* config) {
    if (!config) return NULL;

    module_ctx_t* ctx = calloc(1, sizeof(module_ctx_t));
    if (!ctx) return NULL;

    ctx->config = *config;
    ctx->internal_buffer = malloc(config->buffer_size);

    if (!ctx->internal_buffer) {
        free(ctx);
        return NULL;
    }

    return ctx;
}

void module_destroy(module_ctx_t* ctx) {
    if (ctx) {
        free(ctx->internal_buffer);
        free(ctx);
    }
}

// ... 其他实现
```

### 版本管理

```c
// include/myproject/version.h
#ifndef MYPROJECT_VERSION_H
#define MYPROJECT_VERSION_H

#define MYPROJECT_VERSION_MAJOR 1
#define MYPROJECT_VERSION_MINOR 2
#define MYPROJECT_VERSION_PATCH 3
#define MYPROJECT_VERSION_TWEAK 0

#define MYPROJECT_VERSION_STRING "1.2.3"

#define MYPROJECT_VERSION(major, minor, patch) \
    ((major) * 1000000 + (minor) * 1000 + (patch))

#define MYPROJECT_VERSION_CURRENT \
    MYPROJECT_VERSION(MYPROJECT_VERSION_MAJOR, \
                      MYPROJECT_VERSION_MINOR, \
                      MYPROJECT_VERSION_PATCH)

// 版本检查宏
#define MYPROJECT_VERSION_AT_LEAST(major, minor, patch) \
    (MYPROJECT_VERSION_CURRENT >= MYPROJECT_VERSION(major, minor, patch))

#endif
```

## 版本控制工作流

### Git 分支策略 (Git Flow)

```bash
# 主要分支
main        # 生产环境代码，始终可发布
develop     # 开发分支，日常开发

# 辅助分支
feature/*   # 功能分支，从 develop 创建
release/*   # 发布分支，从 develop 创建
hotfix/*    # 热修复分支，从 main 创建

# 工作流示例

# 1. 开始新功能
git checkout -b feature/user-authentication develop

# 2. 开发完成后合并回 develop
git checkout develop
git merge --no-ff feature/user-authentication
git branch -d feature/user-authentication

# 3. 准备发布
git checkout -b release/v1.2.0 develop
# 修改版本号，最后调整...
git commit -am "Bump version to 1.2.0"

# 4. 发布完成，合并到 main 和 develop
git checkout main
git merge --no-ff release/v1.2.0
git tag -a v1.2.0 -m "Release version 1.2.0"

git checkout develop
git merge --no-ff release/v1.2.0
git branch -d release/v1.2.0
```

### 提交信息规范

```markdown
# 格式：<type>(<scope>): <subject>
#      <BLANK LINE>
#      <body>
#      <BLANK LINE>
#      <footer>

# Type 类型
- feat: 新功能
- fix: 修复
- docs: 文档
- style: 格式（不影响代码运行）
- refactor: 重构
- test: 测试
- chore: 构建过程或辅助工具变动

# 示例
git commit -m "feat(auth): add JWT token validation

- Implement HS256 signature verification
- Add token expiration check
- Update error messages

Closes #123"
```

### 版本标记脚本

```bash
#!/bin/bash
# scripts/bump_version.sh

set -e

OLD_VERSION=$1
NEW_VERSION=$2

if [ -z "$NEW_VERSION" ]; then
    echo "Usage: $1 <old_version> <new_version>"
    exit 1
fi

# 更新版本头文件
sed -i "s/VERSION_STRING \"$OLD_VERSION\"/VERSION_STRING \"$NEW_VERSION\"/" \
    include/myproject/version.h

# 更新 CMakeLists.txt
sed -i "s/VERSION $OLD_VERSION/VERSION $NEW_VERSION/" CMakeLists.txt

# 更新 CHANGELOG
date_str=$(date +%Y-%m-%d)
echo "## [$NEW_VERSION] - $date_str

### Added
-

### Changed
-

### Fixed
-

" | cat - CHANGELOG.md > temp && mv temp CHANGELOG.md

git add -A
git commit -m "chore(release): bump version to $NEW_VERSION"
git tag -a "v$NEW_VERSION" -m "Release version $NEW_VERSION"

echo "Version bumped to $NEW_VERSION"
echo "Run 'git push origin main --tags' to publish"
```

## 代码审查

### 审查清单

```markdown
## C 代码审查清单

### 功能正确性
- [ ] 需求实现完整
- [ ] 边界条件处理正确
- [ ] 错误路径测试充分
- [ ] 资源释放正确

### 代码质量
- [ ] 命名清晰有意义
- [ ] 函数长度适中（<50行）
- [ ] 圈复杂度 <10
- [ ] 无重复代码
- [ ] 适当的注释

### 安全性
- [ ] 无缓冲区溢出风险
- [ ] 输入验证完整
- [ ] 无格式化字符串漏洞
- [ ] 敏感数据正确处理

### 性能
- [ ] 无不必要的内存分配
- [ ] 算法复杂度合理
- [ ] 缓存友好

### 可移植性
- [ ] 使用标准 C
- [ ] 字节序处理正确
- [ ] 平台相关代码隔离
```

### 审查工具脚本

```bash
#!/bin/bash
# scripts/review_check.sh

echo "=== 代码审查自动化检查 ==="

# 1. 静态分析
echo "[*] 运行静态分析..."
clang-tidy src/**/*.c --

# 2. 代码格式检查
echo "[*] 检查代码格式..."
find src -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror

# 3. 编译警告
echo "[*] 编译检查..."
mkdir -p build && cd build
cmake .. -DCMAKE_C_FLAGS="-Wall -Wextra -Werror"
make 2>&1 | tee build.log

# 4. 测试覆盖
echo "[*] 测试覆盖率..."
make test
gcovr -r .. --html --html-details -o coverage.html

# 5. 代码度量
echo "[*] 代码度量分析..."
python3 ../scripts/metrics_analysis.py ../src/

echo "=== 检查完成 ==="
```

### Pull Request 模板

```markdown
## 描述
<!-- 描述改动内容 -->

## 类型
- [ ] Bug 修复
- [ ] 新功能
- [ ] 性能优化
- [ ] 文档更新
- [ ] 重构

## 检查清单
- [ ] 代码通过静态分析
- [ ] 所有测试通过
- [ ] 新增测试覆盖新代码
- [ ] 文档已更新

## 测试
<!-- 如何测试这些改动 -->

## 相关 Issue
Closes #(issue number)
```

## 持续集成

### GitHub Actions 完整配置

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        compiler: [gcc, clang]
        build_type: [Debug, Release]
        exclude:
          - os: windows-latest
            compiler: clang

    steps:
      - uses: actions/checkout@v3

      - name: Setup compiler
        shell: bash
        run: |
          if [ "${{ matrix.compiler }}" = "gcc" ]; then
            echo "CC=gcc" >> $GITHUB_ENV
          else
            echo "CC=clang" >> $GITHUB_ENV
          fi

      - name: Install dependencies (Ubuntu)
        if: matrix.os == 'ubuntu-latest'
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake valgrind

      - name: Configure
        run: |
          cmake -B build \
            -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
            -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic"

      - name: Build
        run: cmake --build build --config ${{ matrix.build_type }}

      - name: Test
        run: ctest --test-dir build -C ${{ matrix.build_type }} --output-on-failure

      - name: Memory Check (Linux only)
        if: matrix.os == 'ubuntu-latest' && matrix.build_type == 'Debug'
        run: |
          valgrind --leak-check=full --error-exitcode=1 \
            ./build/test/unit_tests

  static-analysis:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Install tools
        run: |
          sudo apt-get update
          sudo apt-get install -y clang-tidy cppcheck clang-format

      - name: Run clang-tidy
        run: |
          find src -name "*.c" | xargs clang-tidy -- \
            -Isrc -Iinclude -std=c11 2>&1 | tee clang-tidy-report.txt

      - name: Run cppcheck
        run: |
          cppcheck --enable=all --xml --xml-version=2 \
            --output-file=cppcheck-report.xml src/

      - name: Check formatting
        run: |
          find src -name "*.c" -o -name "*.h" | \
            xargs clang-format --dry-run --Werror

      - name: Upload reports
        uses: actions/upload-artifact@v3
        with:
          name: static-analysis-reports
          path: |
            clang-tidy-report.txt
            cppcheck-report.xml

  code-metrics:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3

      - name: Setup Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.10'

      - name: Run metrics
        run: |
          pip install lizard
          lizard src/ -o metrics.txt
          cat metrics.txt

      - name: Check complexity threshold
        run: |
          python3 scripts/check_complexity.py metrics.txt --threshold 10
```

### 预提交钩子

```bash
#!/bin/bash
# .git/hooks/pre-commit

set -e

echo "Running pre-commit checks..."

# 获取暂存文件
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$' || true)

if [ -z "$STAGED_FILES" ]; then
    exit 0
fi

echo "[*] 格式化检查..."
echo "$STAGED_FILES" | xargs clang-format --dry-run --Werror

echo "[*] 静态分析..."
echo "$STAGED_FILES" | xargs clang-tidy -- -Isrc -Iinclude

echo "[*] 快速编译测试..."
mkdir -p build
cd build
cmake .. > /dev/null 2>&1
make -j4 > /dev/null 2>&1

echo "Pre-commit checks passed!"
```

### 质量门禁配置

```yaml
# sonar-project.properties
sonar.projectKey=myproject
sonar.projectName=My C Project
sonar.projectVersion=1.0

sonar.sources=src
sonar.tests=tests
sonar.inclusions=**/*.c,**/*.h

sonar.cfamily.build-wrapper-output=build/bw-output
sonar.cfamily.gcov.reportsPath=build/coverage

# 质量门禁
sonar.qualitygate.wait=true

# 覆盖度要求
sonar.coverage.exclusions=tests/**,**/test_*
```

## 文档生成

### Doxygen 配置

```doxygen
# Doxyfile 关键配置
PROJECT_NAME     = "MyProject"
PROJECT_VERSION  = 1.2.3
INPUT            = src include docs
FILE_PATTERNS    = *.c *.h *.md
RECURSIVE        = YES
GENERATE_HTML    = YES
GENERATE_LATEX   = NO
EXTRACT_ALL      = YES
EXTRACT_PRIVATE  = NO
WARN_IF_UNDOCUMENTED = YES
```

### 文档注释规范

```c
/**
 * @file module.h
 * @brief 核心模块接口
 * @author Your Name
 * @date 2024
 * @copyright MIT License
 */

/**
 * @brief 创建模块上下文
 *
 * @param config 模块配置，不能为 NULL
 * @return 模块上下文指针，失败返回 NULL
 *
 * @note 返回的上下文必须用 module_destroy 释放
 *
 * @code
 * module_config_t cfg = {
 *     .buffer_size = 1024,
 *     .timeout_ms = 5000
 * };
 * module_ctx_t* ctx = module_create(&cfg);
 * if (!ctx) {
 *     // 错误处理
 * }
 * @endcode
 */
module_ctx_t* module_create(const module_config_t* config);

/**
 * @brief 错误码枚举
 */
typedef enum {
    MODULE_OK = 0,           /**< 成功 */
    MODULE_ERROR_PARAM = -1, /**< 无效参数 */
    MODULE_ERROR_NOMEM = -2, /**< 内存不足 */
} module_error_t;
```

通过遵循这些软件工程实践，可以确保 C 语言项目具有良好的可维护性、可靠性和可扩展性。
