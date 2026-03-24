# 代码审查自动化

> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L3 进阶
> **预估学习时间**: 2-3小时

---

## 📋 审查流程

### GitHub Flow

```
创建分支 → 提交代码 → 创建PR → 自动检查 → 人工审查 → 合并
```

### 自动化检查清单

- [ ] CI构建通过
- [ ] 测试通过
- [ ] 静态分析无错误
- [ ] 代码格式化检查
- [ ] 覆盖率不下降
- [ ] 安全扫描通过

---


---

## 📑 目录

- [代码审查自动化](#代码审查自动化)
  - [📋 审查流程](#-审查流程)
    - [GitHub Flow](#github-flow)
    - [自动化检查清单](#自动化检查清单)
  - [📑 目录](#-目录)
  - [🚀 GitHub Actions集成](#-github-actions集成)
  - [🦊 GitLab CI/CD集成](#-gitlab-cicd集成)
  - [📊 代码覆盖率集成](#-代码覆盖率集成)
    - [CMake + gcov + Codecov配置](#cmake--gcov--codecov配置)
    - [GitHub Actions + Codecov](#github-actions--codecov)
    - [覆盖率阈值设置](#覆盖率阈值设置)
  - [🔍 自动化代码审查工具](#-自动化代码审查工具)
    - [SonarQube集成](#sonarqube集成)
    - [CodeClimate配置](#codeclimate配置)
  - [🔒 依赖安全扫描](#-依赖安全扫描)
    - [Snyk集成](#snyk集成)
    - [Dependabot配置](#dependabot配置)
  - [📝 PR模板配置](#-pr模板配置)
    - [GitHub PR模板](#github-pr模板)
    - [GitLab MR模板](#gitlab-mr模板)
  - [✅ 代码审查最佳实践](#-代码审查最佳实践)
    - [审查者指南](#审查者指南)
    - [提交者指南](#提交者指南)
    - [审查效率技巧](#审查效率技巧)
  - [🔐 审查权限和工作流设置](#-审查权限和工作流设置)
    - [GitHub分支保护规则](#github分支保护规则)
    - [CODEOWNERS配置](#codeowners配置)
    - [合并策略配置](#合并策略配置)
  - [🚨 常见审查问题自动检测规则](#-常见审查问题自动检测规则)
    - [自定义Lint规则](#自定义lint规则)
    - [禁止函数检测脚本](#禁止函数检测脚本)
    - [Clang-Tidy检测规则](#clang-tidy检测规则)
    - [代码异味检测](#代码异味检测)
  - [📋 审查清单模板](#-审查清单模板)
    - [C代码审查清单](#c代码审查清单)
  - [🛠️ 自动化审查脚本集合](#️-自动化审查脚本集合)
    - [完整的CI/CD审查脚本](#完整的cicd审查脚本)
    - [预提交钩子配置](#预提交钩子配置)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 🚀 GitHub Actions集成

```yaml
name: PR Checks

on:
  pull_request:
    branches: [main]

jobs:
  check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Build
        run: cmake -B build && cmake --build build

      - name: Test
        run: ctest --test-dir build

      - name: Format Check
        run: |
          find . -name "*.c" | xargs clang-format -i
          git diff --exit-code

      - name: Static Analysis
        run: cppcheck --error-exitcode=1 src/
```

---

## 🦊 GitLab CI/CD集成

```yaml
stages:
  - build
  - test
  - analysis
  - coverage

variables:
  GIT_DEPTH: 10

build:
  stage: build
  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Debug
    - cmake --build build --parallel
  artifacts:
    paths:
      - build/

test:
  stage: test
  dependencies:
    - build
  script:
    - ctest --test-dir build --output-on-failure
  coverage: '/TOTAL.*\s+(\d+%)$/'

clang_format:
  stage: analysis
  script:
    - find . -name "*.c" -o -name "*.h" | xargs clang-format -i
    - git diff --exit-code
  allow_failure: false

cppcheck:
  stage: analysis
  script:
    - cppcheck --enable=all --error-exitcode=1 --inline-suppr src/
  allow_failure: true

coverage_report:
  stage: coverage
  dependencies:
    - build
  script:
    - cmake --build build --target coverage
    - lcov --capture --directory build --output-file coverage.info
    - lcov --remove coverage.info '/usr/*' --output-file coverage.info
    - genhtml coverage.info --output-directory coverage_report
  coverage: '/lines:.*\s+(\d+\.\d+%)/'
  artifacts:
    paths:
      - coverage_report/
    expire_in: 1 week
```

---

## 📊 代码覆盖率集成

### CMake + gcov + Codecov配置

```cmake
# CMakeLists.txt

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

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage)
    endif()
endif()

# 添加覆盖率目标
add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
    COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --html-details -o ${CMAKE_BINARY_DIR}/coverage/index.html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
```

### GitHub Actions + Codecov

```yaml
- name: Generate Coverage
  run: |
    cmake -B build -DENABLE_COVERAGE=ON
    cmake --build build
    ctest --test-dir build

- name: Upload to Codecov
  uses: codecov/codecov-action@v3
  with:
    files: build/coverage.info
    flags: unittests
    name: codecov-umbrella
    fail_ci_if_error: true
    verbose: true
```

### 覆盖率阈值设置

```yaml
# codecov.yml
coverage:
  status:
    project:
      default:
        target: 80%
        threshold: 2%
    patch:
      default:
        target: 90%
        threshold: 1%
```

---

## 🔍 自动化代码审查工具

### SonarQube集成

```yaml
# .github/workflows/sonar.yml
name: SonarQube Analysis

on:
  pull_request:
    branches: [main]
  push:
    branches: [main]

jobs:
  sonar:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
        with:
          fetch-depth: 0

      - name: Install SonarScanner
        run: |
          wget https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-5.0.1.3006-linux.zip
          unzip sonar-scanner-cli-*.zip

      - name: Build with Wrapper
        run: |
          cmake -B build
          build-wrapper-linux-x86-64 --out-dir bw-output cmake --build build

      - name: Run Analysis
        env:
          SONAR_TOKEN: ${{ secrets.SONAR_TOKEN }}
        run: |
          sonar-scanner/bin/sonar-scanner \
            -Dsonar.projectKey=myproject \
            -Dsonar.sources=src \
            -Dsonar.cfamily.build-wrapper-output=bw-output \
            -Dsonar.host.url=https://sonarqube.example.com
```

### CodeClimate配置

```yaml
# .codeclimate.yml
version: "2"
checks:
  argument-count:
    enabled: true
    config:
      threshold: 4
  complex-logic:
    enabled: true
    config:
      threshold: 4
  method-lines:
    enabled: true
    config:
      threshold: 50
  duplication:
    enabled: true
    config:
      languages:
        - c
plugins:
  cppcheck:
    enabled: true
  clang-tidy:
    enabled: true
```

---

## 🔒 依赖安全扫描

### Snyk集成

```yaml
# .github/workflows/snyk.yml
name: Snyk Security Scan

on:
  pull_request:
    branches: [main]
  schedule:
    - cron: '0 6 * * 1'

jobs:
  security:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Run Snyk to check for vulnerabilities
        uses: snyk/actions/c@master
        env:
          SNYK_TOKEN: ${{ secrets.SNYK_TOKEN }}
        with:
          args: --severity-threshold=high
```

### Dependabot配置

```yaml
# .github/dependabot.yml
version: 2
updates:
  - package-ecosystem: "github-actions"
    directory: "/"
    schedule:
      interval: "weekly"
    open-pull-requests-limit: 10

  - package-ecosystem: "pip"
    directory: "/"
    schedule:
      interval: "weekly"
    open-pull-requests-limit: 5
```

---

## 📝 PR模板配置

### GitHub PR模板

创建文件 `.github/pull_request_template.md`：

```markdown
## 变更描述
<!-- 简要描述本次变更的内容 -->

## 类型
- [ ] Bug修复
- [ ] 新功能
- [ ] 性能优化
- [ ] 代码重构
- [ ] 文档更新

## 检查清单
- [ ] 代码通过所有自动化测试
- [ ] 新增代码有对应的单元测试
- [ ] 代码符合项目编码规范
- [ ] 文档已更新
- [ ] 变更已记录在CHANGELOG中

## 相关Issue
Fixes #(issue编号)
```

### GitLab MR模板

创建文件 `.gitlab/merge_request_templates/default.md`：

```markdown
## 变更摘要
<!-- 一句话描述本次MR的目的 -->

## 详细变更
<!-- 详细列出所做的更改 -->

## 测试情况
- [ ] 本地测试通过
- [ ] CI/CD流水线通过
- [ ] 代码审查完成

## 审查者注意
<!-- 需要审查者特别关注的部分 -->
```

---

## ✅ 代码审查最佳实践

### 审查者指南

1. **及时响应**: PR创建后24小时内开始审查
2. **聚焦重点**: 优先审查安全性和核心业务逻辑
3. **建设性反馈**: 使用建议性语言，解释原因
4. **分批审查**: 大型PR分多次审查，避免疲劳

### 提交者指南

1. **小而精**: 单个PR控制在500行以内
2. **自测充分**: 提交前确保所有本地测试通过
3. **清晰描述**: PR描述清楚说明变更内容和原因
4. **积极回应**: 及时响应审查意见并修改

### 审查效率技巧

```bash
# 使用gh CLI快速审查
gb pr checkout 123              # 检出PR分支
gb pr review 123 --approve      # 批准PR
gb pr review 123 --request-changes --body "需要修改..."

# 查看PR变更统计
gb pr diff 123 --stat
```

---

## 🔐 审查权限和工作流设置

### GitHub分支保护规则

```yaml
# 通过API或UI配置
branch-protection:
  main:
    required_status_checks:
      strict: true
      contexts:
        - "CI / Build"
        - "CI / Test"
        - "Code Coverage"
    required_pull_request_reviews:
      required_approving_review_count: 2
      dismiss_stale_reviews: true
      require_code_owner_reviews: true
    enforce_admins: false
    restrictions:
      users: []
      teams: ["maintainers"]
```

### CODEOWNERS配置

```
# .github/CODEOWNERS
# 全局默认审查者
* @team/lead-developers

# 核心模块需要架构师审查
/src/core/ @architect @lead-developer
/src/security/ @security-team

# 文档变更
/docs/ @docs-team

# CI/CD配置
/.github/workflows/ @devops-team
```

### 合并策略配置

```yaml
# 推荐配置
merge-strategy:
  allow_merge_commit: false      # 禁止普通合并
  allow_squash_merge: true       # 允许压缩合并
  allow_rebase_merge: true       # 允许变基合并
  delete_branch_on_merge: true   # 合并后删除分支
```

---

## 🚨 常见审查问题自动检测规则

### 自定义Lint规则

```yaml
# .github/linters/.cspell.json
{
  "words": ["malloc", "calloc", "realloc", "free"],
  "flagWords": ["strcpy", "gets", "sprintf"],
  "ignorePaths": ["build/", "third_party/"]
}
```

### 禁止函数检测脚本

```bash
#!/bin/bash
# scripts/banned_functions_check.sh

BANNED_FUNCS="strcpy|gets|sprintf|scanf|system"
FILES=$(find src -name "*.c")

for file in $FILES; do
    matches=$(grep -n -E "\b($BANNED_FUNCS)\s*\(" "$file" || true)
    if [ -n "$matches" ]; then
        echo "ERROR: Found banned functions in $file:"
        echo "$matches"
        exit 1
    fi
done
```

### Clang-Tidy检测规则

```yaml
# .clang-tidy
Checks: >
  bugprone-*,
  cppcoreguidelines-*,
  modernize-*,
  performance-*,
  portability-*,
  readability-*,
  -cppcoreguidelines-avoid-magic-numbers,
  -readability-named-parameter

CheckOptions:
  - key:   readability-function-cognitive-complexity.Threshold
    value: '15'
  - key:   cppcoreguidelines-special-member-functions.AllowSoleDefaultDtor
    value: '0'
  - key:   readability-identifier-naming.FunctionCase
    value: 'lower_case'
```

### 代码异味检测

```yaml
# .github/workflows/code-smell.yml
name: Code Smell Detection

on: [pull_request]

jobs:
  detect:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Check TODO/FIXME comments
        run: |
          COUNT=$(grep -r "TODO\|FIXME" src/ --include="*.c" --include="*.h" | wc -l)
          if [ $COUNT -gt 10 ]; then
            echo "Warning: Found $COUNT TODO/FIXME comments"
          fi

      - name: Check function length
        run: |
          python3 scripts/check_function_length.py --max-lines 50 src/

      - name: Check include guards
        run: |
          python3 scripts/check_include_guards.py src/
```

---

## 📋 审查清单模板

### C代码审查清单

**功能性**:

- [ ] 功能实现正确
- [ ] 边界条件处理
- [ ] 错误处理完善

**安全性**:

- [ ] 无缓冲区溢出风险
- [ ] 输入验证
- [ ] 资源正确释放

**代码质量**:

- [ ] 命名清晰
- [ ] 函数长度适中
- [ ] 注释充分

---

## 🛠️ 自动化审查脚本集合

### 完整的CI/CD审查脚本

```bash
#!/bin/bash
# ci-code-review.sh - 完整的CI代码审查脚本
# 用法: ./ci-code-review.sh [目标目录]

set -e  # 遇到错误立即退出

TARGET_DIR="${1:-.}"
EXIT_CODE=0

echo "=========================================="
echo "     C项目自动化代码审查"
echo "=========================================="
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查工具是否安装
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}✗ $1 未安装${NC}"
        return 1
    else
        echo -e "${GREEN}✓ $1 已安装${NC}"
        return 0
    fi
}

echo "【1/7】检查依赖工具..."
check_tool clang-format
check_tool clang-tidy
check_tool cppcheck || true

# 1. 格式化检查
echo ""
echo "【2/7】代码格式检查..."
if command -v clang-format &> /dev/null; then
    # 查找所有C/C++源文件
    FILES=$(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/third_party/*")

    FORMAT_ERRORS=0
    for file in $FILES; do
        if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
            echo -e "${RED}  格式错误: $file${NC}"
            FORMAT_ERRORS=$((FORMAT_ERRORS + 1))
        fi
    done

    if [ $FORMAT_ERRORS -eq 0 ]; then
        echo -e "${GREEN}✓ 所有文件格式正确${NC}"
    else
        echo -e "${RED}✗ 发现 $FORMAT_ERRORS 个文件格式不正确${NC}"
        echo "  运行 'clang-format -i <file>' 修复"
        EXIT_CODE=1
    fi
fi

# 2. 静态分析 - Cppcheck
echo ""
echo "【3/7】Cppcheck静态分析..."
if command -v cppcheck &> /dev/null; then
    if cppcheck --enable=all --error-exitcode=1 \
                --suppress=missingIncludeSystem \
                --suppress=unusedFunction \
                --inline-suppr \
                -I "$TARGET_DIR/include" \
                "$TARGET_DIR/src" 2>&1; then
        echo -e "${GREEN}✓ Cppcheck检查通过${NC}"
    else
        echo -e "${RED}✗ Cppcheck发现问题${NC}"
        EXIT_CODE=1
    fi
else
    echo -e "${YELLOW}⚠ Cppcheck未安装，跳过${NC}"
fi

# 3. 禁止函数检查
echo ""
echo "【4/7】禁止函数检查..."
BANNED_FUNCS="strcpy|gets|sprintf|scanf|system|strcat"
BANNED_COUNT=0

FILES=$(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/third_party/*")

for file in $FILES; do
    matches=$(grep -n -E "\b($BANNED_FUNCS)\s*\(" "$file" 2>/dev/null || true)
    if [ -n "$matches" ]; then
        echo -e "${RED}  禁止函数发现于 $file:${NC}"
        echo "$matches" | head -5
        BANNED_COUNT=$((BANNED_COUNT + 1))
    fi
done

if [ $BANNED_COUNT -eq 0 ]; then
    echo -e "${GREEN}✓ 未发现禁止函数${NC}"
else
    EXIT_CODE=1
fi

# 4. TODO/FIXME检查
echo ""
echo "【5/7】TODO/FIXME注释检查..."
TODO_COUNT=$(grep -r "TODO\|FIXME" "$TARGET_DIR/src" --include="*.c" --include="*.h" 2>/dev/null | wc -l)

if [ "$TODO_COUNT" -gt 10 ]; then
    echo -e "${YELLOW}⚠ 发现 $TODO_COUNT 个TODO/FIXME注释（建议: <=10）${NC}"
else
    echo -e "${GREEN}✓ TODO/FIXME注释数量: $TODO_COUNT${NC}"
fi

# 5. 文件头版权检查
echo ""
echo "【6/7】文件头版权检查..."
COPYRIGHT_MISSING=0

for file in $FILES; do
    if ! head -10 "$file" | grep -q "Copyright\|LICENSE\|@file"; then
        COPYRIGHT_MISSING=$((COPYRIGHT_MISSING + 1))
    fi
done

if [ $COPYRIGHT_MISSING -eq 0 ]; then
    echo -e "${GREEN}✓ 所有文件包含版权信息${NC}"
else
    echo -e "${YELLOW}⚠ $COPYRIGHT_MISSING 个文件缺少版权头${NC}"
fi

# 6. 行长度检查
echo ""
echo "【7/7】行长度检查（最大100字符）..."
LONG_LINES=$(find "$TARGET_DIR" -type f \( -name "*.c" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/third_party/*" -exec awk 'length > 100 {print FILENAME":"NR":"length}' {} \; | head -20)

if [ -z "$LONG_LINES" ]; then
    echo -e "${GREEN}✓ 所有行长度符合要求${NC}"
else
    echo -e "${YELLOW}⚠ 发现超长行:${NC}"
    echo "$LONG_LINES"
fi

echo ""
echo "=========================================="
if [ $EXIT_CODE -eq 0 ]; then
    echo -e "${GREEN}     所有检查通过！✓${NC}"
else
    echo -e "${RED}     发现代码质量问题，请修复${NC}"
fi
echo "=========================================="

exit $EXIT_CODE
```

### 预提交钩子配置

```bash
#!/bin/bash
# .git/hooks/pre-commit - Git预提交钩子

echo "Running pre-commit checks..."

# 运行自动格式化
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$' || true)

if [ -z "$STAGED_FILES" ]; then
    exit 0
fi

# 检查格式
for file in $STAGED_FILES; do
    if command -v clang-format &> /dev/null; then
        if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
            echo "Error: $file is not properly formatted"
            echo "Run: clang-format -i $file"
            exit 1
        fi
    fi
done

# 检查是否有禁止函数
if grep -n -E "\b(strcpy|gets|sprintf|scanf|system)\s*\(" $STAGED_FILES; then
    echo "Error: Found banned functions in staged files"
    exit 1
fi

echo "Pre-commit checks passed!"
```

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
