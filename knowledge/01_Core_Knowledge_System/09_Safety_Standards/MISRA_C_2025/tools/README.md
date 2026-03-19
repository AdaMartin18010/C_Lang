# MISRA C:2025 工具支持

> 静态分析工具链配置与使用指南
>
> ⚠️ **注意**: 本文档基于预期更新编制，具体工具支持情况请参考官方发布

---

## 概述

本文档介绍支持 MISRA C:2025 的静态分析工具链配置和使用方法。

---

## 工具支持状态 [待官方文档确认]

### 商业工具

| 工具 | 厂商 | MISRA C:2025支持 | 预计发布时间 | 状态 |
|:-----|:-----|:----------------:|:------------|:----:|
| Helix QAC | Perforce | [待官方文档确认] | 2025 Q2-Q3 | ⏳ |
| PC-lint Plus | Gimpel | [待官方文档确认] | 2025 Q2-Q3 | ⏳ |
| Coverity | Synopsys | [待官方文档确认] | 2025 Q2-Q3 | ⏳ |
| Polyspace | MathWorks | [待官方文档确认] | 2025 Q2-Q3 | ⏳ |
| Understand | SciTools | [待官方文档确认] | 待定 | ⏳ |

### 开源工具

| 工具 | 许可 | MISRA C:2025支持 | 备注 |
|:-----|:-----|:----------------:|:-----|
| Cppcheck | GPL | [待社区更新] | 依赖社区贡献 |
| Clang Static Analyzer | Apache 2.0 | [待官方文档确认] | 随Clang更新 |
| Sparse | MIT | [待社区更新] | Linux内核工具 |

---

## 编译器C23支持 [待官方文档确认]

### GCC

```bash
# GCC 14+ 预期支持C23 [待确认]
gcc -std=c23 -pedantic -c file.c

# 或
gcc -std=gnu23 -c file.c
```

### Clang

```bash
# Clang 18+ 预期支持C23 [待确认]
clang -std=c23 -pedantic -c file.c
```

### MSVC

```bash
# Visual Studio 2022 17.x+ [待确认]
cl /std:c23 file.c
```

---

## 配置文件模板 [待官方文档确认]

### Helix QAC 配置

```python
# qac_misra_c2025.conf [待官方文档确认]
# 预期配置模板

STANDARD MISRA_C_2025
LANGUAGE C23

# 规则集配置
RULESET ALL

# 排除规则 (根据项目需要)
# SUPPRESS Rule-X.Y

# 输出格式
FORMAT HTML, JSON, TEXT
```

### PC-lint Plus 配置

```c
// co-misra-c2025.lnt [待官方文档确认]
// 预期配置文件

// 标准选择
-std=c23

// MISRA C:2025规则集
+misra-c2025

// 输出格式
-hF1            // 完整文件名
"format=%f:%l:%c: %t %n: %m"  // 自定义格式
```

---

## 文档目录

- [合规性指南](./docs/MISRA_COMPLIANCE.md) - 合规流程文档
- [偏离管理](./docs/DEVIATIONS.md) - 偏离申请和管理
- [审计日志](./docs/AUDIT_LOG.md) - 审计记录模板

---

**最后更新**: 2026-03-19

**状态**: 等待MISRA C:2025官方发布和工具厂商更新


---

## 深入理解

### 核心概念

本主题的核心概念包括：基础理论、实现机制、实际应用。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 学习建议

1. 先理解基础概念
2. 再进行实践练习
3. 最后深入源码

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
