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

### 使用

```bash
# 扫描构建
scan-build cmake -B build
scan-build cmake --build build

# HTML报告
scan-build -o report cmake --build build
```

### 检查器

```bash
# 列出所有检查器
scan-build --help-checkers

# 启用特定检查器
scan-build -enable-checker security .
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
```

### CI集成

```yaml
# .github/workflows/static-analysis.yml
- name: Cppcheck
  run: |
    cppcheck --error-exitcode=1 \
      --enable=all \
      --suppress=missingIncludeSystem \
      src/
```

---

**← [返回工具链主页](../README.md)**
