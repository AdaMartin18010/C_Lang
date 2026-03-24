# clang-format代码格式化配置


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L3 进阶

---
> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L2 应用
> **预估学习时间**: 2-3小时

---

## 📋 clang-format简介

自动化代码格式化工具：

- 统一代码风格
- 减少风格争论
- 与CI/CD集成

---


---

## 📑 目录

- [clang-format代码格式化配置](#clang-format代码格式化配置)
  - [📋 clang-format简介](#-clang-format简介)
  - [📑 目录](#-目录)
  - [🚀 快速开始](#-快速开始)
    - [安装](#安装)
    - [基础用法](#基础用法)
  - [⚙️ 配置文件](#️-配置文件)
    - [.clang-format](#clang-format)
    - [常用风格](#常用风格)
  - [🔧 集成方案](#-集成方案)
    - [Git钩子](#git钩子)
    - [VS Code集成](#vs-code集成)
    - [CI检查](#ci检查)
  - [📚 高级配置](#-高级配置)
    - [条件编译处理](#条件编译处理)
    - [包含文件排序](#包含文件排序)
  - [✅ 最佳实践](#-最佳实践)
  - [🔧 clang-format 高级配置详解](#-clang-format-高级配置详解)
    - [1. 完整配置文件模板](#1-完整配置文件模板)
    - [2. 团队规范配置策略](#2-团队规范配置策略)
    - [3. IDE集成详细配置](#3-ide集成详细配置)
    - [4. Git Hooks详细配置](#4-git-hooks详细配置)
    - [5. CI/CD集成详细配置](#5-cicd集成详细配置)
    - [6. 格式化脚本工具](#6-格式化脚本工具)
    - [7. 风格迁移与兼容性](#7-风格迁移与兼容性)
  - [✅ 最佳实践总结](#-最佳实践总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 🚀 快速开始

### 安装

```bash
## macOS

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../README.md](../../README.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/README.md](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/README.md](../../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

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
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/README.md](../../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/README.md](../../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/README.md](../../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/README.md](../../04_Industrial_Scenarios/README.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/README.md) |

---

brew install clang-format

## Linux
sudo apt-get install clang-format

## Windows (随LLVM安装)
choco install llvm
```

### 基础用法

```bash
## 格式化文件
clang-format -i main.c

## 格式化目录
find . -name "*.c" -o -name "*.h" | xargs clang-format -i

## 检查格式 (CI用)
clang-format --dry-run --Werror main.c

## 查看格式化后的输出(不保存)
clang-format main.c
```

---

## ⚙️ 配置文件

### .clang-format

```yaml
Language: C
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 80
AllowShortFunctionsOnASingleLine: Empty
BreakBeforeBraces: Attach
PointerAlignment: Right
SortIncludes: true
IncludeBlocks: Regroup
SpacesBeforeTrailingComments: 2
AlignConsecutiveMacros: true
AlignConsecutiveAssignments: false
AlignConsecutiveDeclarations: false
BreakStringLiterals: true
ReflowComments: true
```

### 常用风格

| 风格 | 说明 |
|:-----|:-----|
| LLVM | LLVM项目风格 |
| Google | Google风格 |
| Chromium | Chromium风格 |
| Mozilla | Mozilla风格 |
| WebKit | WebKit风格 |
| Microsoft | Microsoft风格 |
| GNU | GNU风格 |

---

## 🔧 集成方案

### Git钩子

**.git/hooks/pre-commit**:

```bash
#!/bin/bash
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')
for file in $files; do
    if [ -f "$file" ]; then
        clang-format -i "$file"
        git add "$file"
    fi
done
```

### VS Code集成

settings.json:

```json
{
  "editor.formatOnSave": true,
  "[c]": {
    "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
  }
}
```

### CI检查

```yaml
- name: Format Check
  run: |
    find src tests -name "*.c" -o -name "*.h" | \
      xargs clang-format --dry-run --Werror
```

---

## 📚 高级配置

### 条件编译处理

```yaml
IfMacros:
  - KJ_IF_MAYBE
  - KJ_IF_SOME
StatementMacros:
  - Q_UNUSED
  - QT_REQUIRE_VERSION
```

### 包含文件排序

```yaml
IncludeCategories:
  - Regex: '^"(llvm|llvm-c|clang|clang-c)/'
    Priority: 2
  - Regex: '^(<|"(gtest|gmock|isl|json)/)'
    Priority: 3
  - Regex: '<[[:alnum:].]+>'
    Priority: 4
  - Regex: '.*'
    Priority: 1
```

---

## ✅ 最佳实践

- [ ] 团队统一配置
- [ ] 提交前自动格式化
- [ ] CI中检查格式
- [ ] 配置文件纳入版本控制
- [ ] 定期审查格式规则

---

## 🔧 clang-format 高级配置详解

### 1. 完整配置文件模板

```yaml
## =============================================================================
## C语言项目clang-format配置
## =============================================================================

## 语言标准
Language: C
Standard: Latest

## 基础风格
BasedOnStyle: LLVM

## =============================================================================
## 缩进与空白
## =============================================================================

## 缩进宽度
IndentWidth: 4
TabWidth: 4
UseTab: Never
ContinuationIndentWidth: 4

## 列限制
ColumnLimit: 100

## 自动检测行尾换行符
DeriveLineEnding: false
UseCRLF: false

## 最大空行数
MaxEmptyLinesToKeep: 2

## 保持行尾空白
KeepEmptyLinesAtTheStartOfBlocks: false

## =============================================================================
## 大括号风格
## =============================================================================

BreakBeforeBraces: Custom
BraceWrapping:
  AfterCaseLabel: false
  AfterClass: false
  AfterControlStatement: Never
  AfterEnum: false
  AfterFunction: false
  AfterNamespace: false
  AfterObjCDeclaration: false
  AfterStruct: false
  AfterUnion: false
  AfterExternBlock: false
  BeforeCatch: false
  BeforeElse: false
  BeforeLambdaBody: false
  BeforeWhile: false
  IndentBraces: false
  SplitEmptyFunction: true
  SplitEmptyRecord: true
  SplitEmptyNamespace: true

## =============================================================================
## 对齐选项
## =============================================================================

## 对齐连续赋值
AlignConsecutiveAssignments:
  Enabled: false
  AcrossEmptyLines: false
  AcrossComments: false
  AlignCompound: false
  PadOperators: true

## 对齐连续声明
AlignConsecutiveDeclarations:
  Enabled: false
  AcrossEmptyLines: false
  AcrossComments: false
  AlignFunctionPointers: false

## 对齐连续宏定义
AlignConsecutiveMacros:
  Enabled: true
  AcrossEmptyLines: true
  AcrossComments: true

## 对齐转义换行
AlignEscapedNewlines: Right

## 对齐操作数
AlignOperands: Align

## 对齐尾随注释
AlignTrailingComments:
  Kind: Always
  OverEmptyLines: 0

## 数组初始化对齐
AlignArrayOfStructures: Right

## =============================================================================
## 指针与引用
## =============================================================================

PointerAlignment: Right
ReferenceAlignment: Pointer
SpaceAroundPointerQualifiers: Default

## =============================================================================
## 函数与参数
## =============================================================================

## 允许短函数在一行
AllowShortBlocksOnASingleLine: Empty
AllowShortCaseLabelsOnASingleLine: false
AllowShortEnumsOnASingleLine: false
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: Never
AllowShortLambdasOnASingleLine: All
AllowShortLoopsOnASingleLine: false

## 函数参数换行
BinPackArguments: false
BinPackParameters: false

## 参数打包方式
PackConstructorInitializers: Never

## 函数声明返回类型换行
AlwaysBreakAfterReturnType: None

## 函数调用参数
AllowAllArgumentsOnNextLine: true
AllowAllParametersOfDeclarationOnANextLine: true

## =============================================================================
## 换行与分割
## =============================================================================

## 二元操作符换行
BreakBeforeBinaryOperators: None

## 概念声明换行
BreakBeforeConceptDeclarations: Always

## 模板声明换行
BreakBeforeTernaryOperators: true

## 字符串换行
BreakStringLiterals: true

## 继承列表换行
BreakInheritanceList: BeforeColon

## 构造初始化器换行
BreakConstructorInitializers: BeforeColon

## 长字符串分割
SplitEmptyFunction: false
SplitEmptyRecord: false
SplitEmptyNamespace: false

## =============================================================================
## 包含文件
## =============================================================================

SortIncludes: CaseSensitive
IncludeBlocks: Regroup

IncludeCategories:
  # 系统头文件
  - Regex:           '^<[a-z_]+\.h>'
    Priority:        4
    SortPriority:    0
    CaseSensitive:   false
  - Regex:           '^<sys/[a-z_]+\.h>'
    Priority:        4
    SortPriority:    1
    CaseSensitive:   false
  - Regex:           '^<[a-z/_]+\.h>'
    Priority:        4
    SortPriority:    2
    CaseSensitive:   false
  # 第三方库
  - Regex:           '^<[^/]+>'
    Priority:        3
    SortPriority:    0
    CaseSensitive:   false
  # 项目内部头文件
  - Regex:           '^"[^/]+"'
    Priority:        2
    SortPriority:    0
    CaseSensitive:   false
  - Regex:           '^".*"'
    Priority:        1
    SortPriority:    0
    CaseSensitive:   false

## =============================================================================
## 注释
## =============================================================================

ReflowComments: true

## 注释对齐
AlignTrailingComments:
  Kind: Always
  OverEmptyLines: 0

## =============================================================================
## 空白字符
## =============================================================================

## 空格选项
SpaceAfterCStyleCast: false
SpaceAfterLogicalNot: false
SpaceBeforeAssignmentOperators: true
SpaceBeforeCaseColon: false
SpaceBeforeParens: ControlStatements
SpaceBeforeRangeBasedForLoopColon: true
SpaceBeforeSquareBrackets: false
SpaceInEmptyBlock: false
SpaceInEmptyParentheses: false
SpacesBeforeTrailingComments: 2
SpacesInAngles: Never
SpacesInCStyleCastParentheses: false
SpacesInConditionalStatement: false
SpacesInContainerLiterals: false
SpacesInParentheses: false
SpacesInSquareBrackets: false

## 行尾注释前的空格
SpacesBeforeTrailingComments: 2

## =============================================================================
## 其他选项
## =============================================================================

## 插入尾行注释分隔符
InsertTrailingCommas: None

## 修复命名空间注释
FixNamespaceComments: true

## 短命名空间行数
ShortNamespaceLines: 1

## 紧凑命名空间
CompactNamespaces: false

## 属性换行
BreakAfterAttributes: Leave

## 行尾字符
LineEnding: LF

## 删除多余空白
RemoveBracesLLVM: false
RemoveParentheses: Leave
RemoveSemicolon: false

## 要求每个表达式在单独一行
AllowAllParametersOfDeclarationOnANextLine: true
ExperimentalAutoDetectBinPacking: false
```

### 2. 团队规范配置策略

**项目级配置**:

```yaml
## .clang-format (项目根目录)
## 这是项目的主要配置文件

## 基于LLVM风格，但根据团队需求定制
BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 100
```

**子目录覆盖配置**:

```yaml
## third_party/.clang-format
## 第三方代码使用不同规则

## 禁用格式化
DisableFormat: true
SortIncludes: false
```

```yaml
## tests/.clang-format
## 测试代码允许更宽松的格式

BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 120
AllowShortFunctionsOnASingleLine: All
```

**Git属性配置**:

```gitattributes
## .gitattributes
## 标记clang-format配置为导出忽略
.clang-format export-ignore

## 确保C文件使用LF换行
*.c text eol=lf
*.h text eol=lf
```

### 3. IDE集成详细配置

**VS Code完整配置**:

```json
// .vscode/settings.json
{
    // 格式化设置
    "editor.formatOnSave": true,
    "editor.formatOnPaste": true,
    "editor.formatOnType": false,

    // C/C++特定设置
    "[c]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd",
        "editor.tabSize": 4,
        "editor.insertSpaces": true,
        "editor.detectIndentation": false,
        "editor.rulers": [100],
        "editor.wordWrap": "wordWrapColumn",
        "editor.wordWrapColumn": 100
    },
    "[cpp]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd",
        "editor.tabSize": 4,
        "editor.insertSpaces": true,
        "editor.detectIndentation": false,
        "editor.rulers": [100],
        "editor.wordWrap": "wordWrapColumn",
        "editor.wordWrapColumn": 100
    },

    // clangd设置
    "clangd.path": "/usr/bin/clangd",
    "clangd.arguments": [
        "--background-index",
        "--compile-commands-dir=build",
        "--clang-tidy",
        "--header-insertion=iwyu"
    ],

    // 文件关联
    "files.associations": {
        "*.h": "c",
        "*.c": "c"
    },

    // 排除文件
    "files.exclude": {
        "**/build/**": true,
        "**/.cache/**": true
    },

    // 搜索排除
    "search.exclude": {
        "**/build/**": true,
        "**/third_party/**": true
    }
}
```

**CLion配置**:

```
Settings → Editor → Code Style → C/C++
├── Tabs and Indents
│   ├── Tab size: 4
│   ├── Indent: 4
│   └── Continuation indent: 4
├── Spaces
│   ├── After keywords: ☑
│   └── Within: 按需选择
├── Wrapping and Braces
│   ├── Hard wrap at: 100
│   └── 大括号风格: Attach
├── Imports
│   └── Sort imports: ☑
└── 启用clang-format:
    └── 使用.clang-format文件
```

**Vim/Neovim配置**:

```vim
" .vimrc 或 init.vim

" 自动格式化
augroup ClangFormat
  autocmd!
  autocmd BufWritePre *.c,*.h :silent !clang-format -i %
  autocmd BufWritePre *.c,*.h :edit!
augroup END

" 手动格式化快捷键
nnoremap <leader>f :!clang-format -i %<CR>

" 使用ale插件进行实时检查
let g:ale_c_clangformat_executable = 'clang-format'
let g:ale_c_clangformat_options = '-style=file'
let g:ale_fixers = {
\   'c': ['clang-format'],
\   'cpp': ['clang-format']
\}
let g:ale_fix_on_save = 1
```

### 4. Git Hooks详细配置

**预提交钩子 (Pre-commit)**:

```bash
#!/bin/bash
## .git/hooks/pre-commit

set -e

echo "Running pre-commit checks..."

## 获取暂存区的C文件
FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$' || true)

if [ -z "$FILES" ]; then
    echo "No C files to check."
    exit 0
fi

echo "Checking formatting for:"
echo "$FILES"

## 检查clang-format是否安装
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format not found"
    exit 1
fi

## 检查格式
FORMATTING_NEEDED=0
for FILE in $FILES; do
    if [ -f "$FILE" ]; then
        # 比较原始文件和格式化后的文件
        if ! clang-format --dry-run --Werror "$FILE" 2>/dev/null; then
            echo "Formatting needed: $FILE"
            FORMATTING_NEEDED=1
        fi
    fi
done

if [ $FORMATTING_NEEDED -eq 1 ]; then
    echo ""
    echo "ERROR: Code formatting issues found!"
    echo "Run the following to fix:"
    echo "  clang-format -i $FILES"
    echo ""
    echo "Or use the format script:"
    echo "  ./scripts/format-code.sh"
    exit 1
fi

echo "Formatting check passed!"
exit 0
```

**提交信息钩子 (Commit-msg)**:

```bash
#!/bin/bash
## .git/hooks/commit-msg

## 读取提交信息
COMMIT_MSG_FILE=$1
COMMIT_MSG=$(cat "$COMMIT_MSG_FILE")

## 检查提交信息格式
## 格式: type(scope): subject
## 示例: feat(parser): add error recovery

PATTERN="^(feat|fix|docs|style|refactor|test|chore)(\([a-z-]+\))?: .+"

if ! echo "$COMMIT_MSG" | grep -qE "$PATTERN"; then
    echo "ERROR: Invalid commit message format."
    echo ""
    echo "Expected format: type(scope): subject"
    echo ""
    echo "Types:"
    echo "  feat:     新功能"
    echo "  fix:      修复bug"
    echo "  docs:     文档更新"
    echo "  style:    代码格式（不影响功能）"
    echo "  refactor: 重构"
    echo "  test:     测试相关"
    echo "  chore:    构建/工具变更"
    echo ""
    echo "Example: feat(parser): add error recovery"
    exit 1
fi

exit 0
```

### 5. CI/CD集成详细配置

**GitHub Actions完整工作流**:

```yaml
## .github/workflows/code-format.yml
name: Code Format Check

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  format-check:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Install clang-format
      run: |
        sudo apt-get update
        sudo apt-get install -y clang-format-17
        sudo ln -sf /usr/bin/clang-format-17 /usr/bin/clang-format

    - name: Check formatting
      run: |
        find src tests include -name "*.c" -o -name "*.h" | \
          xargs clang-format --dry-run --Werror 2>&1 | tee format-issues.txt

    - name: Upload format report
      if: failure()
      uses: actions/upload-artifact@v4
      with:
        name: format-issues
        path: format-issues.txt

    - name: Suggest fixes
      if: failure()
      run: |
        echo "::error::Code formatting issues found!"
        echo "To fix locally, run:"
        echo "  find src tests include -name '*.c' -o -name '*.h' | xargs clang-format -i"

  format-apply:
    runs-on: ubuntu-latest
    if: github.event_name == 'pull_request'
    steps:
    - uses: actions/checkout@v4
      with:
        token: ${{ secrets.GITHUB_TOKEN }}

    - name: Install clang-format
      run: |
        sudo apt-get update
        sudo apt-get install -y clang-format-17

    - name: Apply formatting
      run: |
        find src tests include -name "*.c" -o -name "*.h" | \
          xargs clang-format -i

    - name: Check for changes
      id: changes
      run: |
        if git diff --quiet; then
          echo "changed=false" >> $GITHUB_OUTPUT
        else
          echo "changed=true" >> $GITHUB_OUTPUT
        fi

    - name: Commit changes
      if: steps.changes.outputs.changed == 'true'
      run: |
        git config user.name "github-actions"
        git config user.email "actions@github.com"
        git add -A
        git commit -m "style: auto-format code with clang-format"
        git push
```

**GitLab CI配置**:

```yaml
## .gitlab-ci.yml
stages:
  - lint
  - build
  - test

variables:
  CLANG_FORMAT_VERSION: "17"

format-check:
  stage: lint
  image: silkeh/clang:$CLANG_FORMAT_VERSION
  script:
    - find src tests include -name "*.c" -o -name "*.h" | \
        xargs clang-format --dry-run --Werror
  allow_failure: false

format-apply:
  stage: lint
  image: silkeh/clang:$CLANG_FORMAT_VERSION
  script:
    - find src tests include -name "*.c" -o -name "*.h" | \
        xargs clang-format -i
    - git diff --exit-code || (
        git config user.email "ci@gitlab.com" &&
        git config user.name "CI Bot" &&
        git add -A &&
        git commit -m "Apply clang-format [ci skip]" &&
        git push origin HEAD:$CI_COMMIT_REF_NAME
      )
  only:
    - merge_requests
```

### 6. 格式化脚本工具

```bash
#!/bin/bash
## scripts/format-code.sh
## 代码格式化脚本

set -e

## 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

## 默认配置
DRY_RUN=false
CHECK_ONLY=false
FILES=""

## 使用说明
usage() {
    echo "Usage: $0 [OPTIONS] [FILES...]"
    echo ""
    echo "Options:"
    echo "  -c, --check       仅检查格式，不修改文件"
    echo "  -d, --dry-run     显示将要进行的更改"
    echo "  -h, --help        显示帮助信息"
    echo ""
    echo "Examples:"
    echo "  $0                格式化所有源文件"
    echo "  $0 -c             检查所有文件格式"
    echo "  $0 src/main.c     格式化特定文件"
}

## 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -c|--check)
            CHECK_ONLY=true
            shift
            ;;
        -d|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -*)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
        *)
            FILES="$FILES $1"
            shift
            ;;
    esac
done

## 检查clang-format
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}Error: clang-format not found${NC}"
    echo "Please install clang-format"
    exit 1
fi

## 检查配置文件
if [ ! -f ".clang-format" ]; then
    echo -e "${YELLOW}Warning: .clang-format not found in current directory${NC}"
fi

## 确定要格式化的文件
if [ -z "$FILES" ]; then
    # 查找所有C源文件
    FILES=$(find src tests include -name "*.c" -o -name "*.h" 2>/dev/null || true)

    if [ -z "$FILES" ]; then
        echo -e "${YELLOW}No C files found in src/, tests/, or include/${NC}"
        echo "Please specify files manually or run from project root"
        exit 0
    fi
fi

## 执行格式化
if [ "$CHECK_ONLY" = true ]; then
    echo "Checking code format..."
    ERRORS=0
    for FILE in $FILES; do
        if [ -f "$FILE" ]; then
            if ! clang-format --dry-run --Werror "$FILE" 2>/dev/null; then
                echo -e "${RED}✗ $FILE${NC}"
                ERRORS=$((ERRORS + 1))
            else
                echo -e "${GREEN}✓ $FILE${NC}"
            fi
        fi
    done

    if [ $ERRORS -gt 0 ]; then
        echo ""
        echo -e "${RED}Found $ERRORS file(s) with formatting issues${NC}"
        echo "Run '$0' to fix them"
        exit 1
    else
        echo -e "${GREEN}All files are properly formatted!${NC}"
        exit 0
    fi
elif [ "$DRY_RUN" = true ]; then
    echo "Dry run - showing changes:"
    for FILE in $FILES; do
        if [ -f "$FILE" ]; then
            clang-format --dry-run "$FILE"
        fi
    done
else
    echo "Formatting code..."
    for FILE in $FILES; do
        if [ -f "$FILE" ]; then
            clang-format -i "$FILE"
            echo -e "${GREEN}✓ $FILE${NC}"
        fi
    done
    echo -e "${GREEN}Formatting complete!${NC}"
fi
```

### 7. 风格迁移与兼容性

**从其他风格迁移**:

```bash
#!/bin/bash
## migrate-style.sh
## 从一种风格迁移到另一种风格

OLD_STYLE="GNU"
NEW_STYLE="LLVM"

echo "Migrating from $OLD_STYLE to $NEW_STYLE..."

## 备份
tar czf code-backup-$(date +%Y%m%d).tar.gz src/ tests/ include/

## 使用旧风格格式化（标准化当前代码）
find src tests include -name "*.c" -o -name "*.h" | \
    xargs clang-format -i --style=$OLD_STYLE

## 更新配置文件
cat > .clang-format << EOF
BasedOnStyle: $NEW_STYLE
IndentWidth: 4
ColumnLimit: 100
EOF

## 使用新风格重新格式化
find src tests include -name "*.c" -o -name "*.h" | \
    xargs clang-format -i

echo "Migration complete. Review changes before committing."
```

---

## ✅ 最佳实践总结

- [ ] 团队统一配置
- [ ] 提交前自动格式化
- [ ] CI中检查格式
- [ ] 配置文件纳入版本控制
- [ ] 定期审查格式规则
- [ ] 提供IDE配置指导
- [ ] 编写格式化脚本工具
- [ ] 文档化风格规范
- [ ] 培训团队成员
- [ ] 处理遗留代码策略

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
