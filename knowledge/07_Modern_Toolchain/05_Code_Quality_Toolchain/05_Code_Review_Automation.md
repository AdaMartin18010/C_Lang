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

**← [返回工具链主页](../README.md)**
