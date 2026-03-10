# C语言静态分析深度指南

> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L3 进阶
> **预估学习时间**: 4-6小时

---

## 📋 静态分析工具概览

| 工具 | 类型 | 特点 |
|:-----|:-----|:-----|
| **Clang Static Analyzer** | 编译器内置 | 深度分析，误报少 |
| **Cppcheck** | 独立工具 | 快速，易集成 |
| **PC-lint/Flexelint** | 商业工具 | MISRA支持 |
| **SonarQube** | 平台级 | 持续监控 |
| **Coverity** | 商业级 | 企业首选 |

---

## 🚀 Clang Static Analyzer

### 使用scan-build

```bash
# 扫描构建
scan-build cmake -B build
scan-build cmake --build build

# HTML报告
scan-build -o report cmake --build build

# 检查特定检查器
scan-build --help-checkers
scan-build -enable-checker security .
```

### 常用检查器

```bash
# 启用所有检查器
scan-build -enable-checker alpha .

# 特定检查器
scan-build -enable-checker alpha.core.NullDereference \
           -enable-checker alpha.security.ArrayBound \
           cmake --build build
```

### 与CMake集成

```cmake
option(ENABLE_ANALYZER "Enable static analyzer" OFF)

if(ENABLE_ANALYZER)
    set(CMAKE_C_COMPILER_LAUNCHER "scan-build")
endif()
```

---

## 🚀 Cppcheck

### 基础用法

```bash
# 基本检查
cppcheck --enable=all src/

# 严格检查
cppcheck --enable=all --inconclusive --std=c11 src/

# 生成XML报告
cppcheck --enable=all --xml --xml-version=2 src/ 2> report.xml

# 检查特定问题
cppcheck --enable=warning,performance,portability,style src/
```

### 抑制警告

```bash
# 命令行抑制
cppcheck --suppress=missingIncludeSystem src/

# 使用抑制文件
cppcheck --suppressions-list=suppressions.txt src/
```

**suppressions.txt**:

```
missingIncludeSystem
unusedFunction:src/test/*.c
```

### CI集成

```yaml
# .github/workflows/static-analysis.yml
- name: Cppcheck
  run: |
    cppcheck --error-exitcode=1 \
      --enable=all \
      --suppress=missingIncludeSystem \
      --suppress=unusedFunction \
      --inline-suppr \
      src/
```

---

## 🚀 Clang-Tidy

### 配置.clang-tidy

```yaml
Checks: >
  bugprone-*,
  clang-analyzer-*,
  cppcoreguidelines-*,
  performance-*,
  portability-*,
  readability-*,
  -cppcoreguidelines-avoid-magic-numbers

CheckOptions:
  - key:   readability-function-cognitive-complexity.Threshold
    value: '25'
```

### 使用

```bash
# 检查文件
clang-tidy src/*.c -- -Iinclude

# 修复问题
clang-tidy src/*.c -fix -- -Iinclude
```

---

## 📊 检查规则分类

| 类别 | 说明 | 工具 |
|:-----|:-----|:-----|
| Bug检测 | 空指针、数组越界 | Clang, Cppcheck |
| 性能 | 低效操作 | Cppcheck, Clang-Tidy |
| 可移植性 | 平台相关代码 | Cppcheck |
| 代码风格 | 命名、格式 | Clang-Tidy |
| 安全 | 缓冲区溢出 | Clang, Cppcheck |

---

## ✅ 集成建议

- [ ] 本地开发时使用Clang-Tidy
- [ ] CI中使用Cppcheck
- [ ] 定期运行深度扫描
- [ ] 维护抑制列表
- [ ] 跟踪误报率

---

**← [返回工具链主页](../README.md)**
