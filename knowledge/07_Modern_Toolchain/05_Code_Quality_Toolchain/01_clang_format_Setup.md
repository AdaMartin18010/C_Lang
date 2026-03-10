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

# Windows
# 随LLVM安装
```

### 基础用法

```bash
# 格式化文件
clang-format -i main.c

# 格式化目录
find . -name "*.c" -o -name "*.h" | xargs clang-format -i

# 检查格式 (CI用)
clang-format --dry-run --Werror main.c
```

### 配置文件

**.clang-format**:

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
```

### Git钩子

**.git/hooks/pre-commit**:

```bash
#!/bin/bash
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')
for file in $files; do
    clang-format -i "$file"
    git add "$file"
done
```

---

**← [返回工具链主页](../README.md)**
