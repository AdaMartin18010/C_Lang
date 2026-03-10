# clang-format代码格式化配置

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

## 🚀 快速开始

### 安装

```bash
# macOS
brew install clang-format

# Linux
sudo apt-get install clang-format

# Windows (随LLVM安装)
choco install llvm
```

### 基础用法

```bash
# 格式化文件
clang-format -i main.c

# 格式化目录
find . -name "*.c" -o -name "*.h" | xargs clang-format -i

# 检查格式 (CI用)
clang-format --dry-run --Werror main.c

# 查看格式化后的输出(不保存)
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

**← [返回工具链主页](../README.md)**
