---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# MISRA C:2023 Rules 19.1-19.3 - 预处理指令

> #include、宏定义、条件编译

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 19.1 | #include | Required | 头文件包含规范 |
| Rule 19.2 | 宏定义 | Required | 宏定义安全 |
| Rule 19.3 | 条件编译 | Required | 条件编译完整性 |

---

## Rule 19.1 - #include指令

### 规则原文

> **Rule 19.1 (Required)**: #include directives shall only use the <filename> or "filename" syntax.
>
> **中文**: #include指令只能使用<filename>或"filename"语法。

### 合规示例

```c
/* ✅ 合规 - 系统头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ✅ 合规 - 项目头文件 */
#include "config.h"
#include "utils.h"
#include "module/driver.h"

/* ✅ 合规 - 相对路径 */
#include "../include/common.h"
#include "./local.h"

/* ❌ 违反 - 宏扩展 */
#define HEADER "file.h"
#include HEADER  /* 违反 */

/* ❌ 违反 - 拼接 */
#include FILENAME  /* 宏扩展 */
```

### 头文件搜索路径

```c
/* 系统头文件 - 在系统目录搜索 */
#include <stdio.h>

/* 本地头文件 - 先在当前目录搜索 */
#include "myheader.h"
```

---

## Rule 19.2 - 宏定义

### 规则原文

> **Rule 19.2 (Required)**: Macros shall be defined and used safely.
>
> **中文**: 宏应安全地定义和使用。

### 违反示例

```c
/* ❌ 违反 - 参数无括号 */
#define SQUARE(x) x * x
SQUARE(a + b);  /* a + b * a + b，不是(a+b)*(a+b) */

/* ❌ 违反 - 宏体无括号 */
#define SUM(a, b) a + b
SUM(x, y) * 2;  /* x + y * 2，不是(x+y)*2 */

/* ❌ 违反 - 多语句宏无do-while */
#define SWAP(a, b) \
    temp = (a); \
    (a) = (b); \
    (b) = temp;

if (condition)
    SWAP(x, y);  /* 只有第一条在if中！ */

/* ❌ 违反 - 自增/自减在宏参数 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int m = MAX(x++, y++);  /* x++或y++可能执行两次！*/
```

### 合规示例

```c
/* ✅ 合规 - 完全括号化 */
#define SQUARE(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))

/* ✅ 合规 - 多语句宏用do-while(0) */
#define SWAP(type, a, b) do { \
    type temp = (a); \
    (a) = (b); \
    (b) = temp; \
} while(0)

/* 使用 */
if (condition) {
    SWAP(int, x, y);  /* 正确 */
}

/* ✅ 合规 - 避免副作用的参数 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* 安全使用 */
int x_val = x++;
int y_val = y++;
int m = MAX(x_val, y_val);

/* ✅ 合规 - 常量定义 */
#define BUFFER_SIZE 1024
#define MAX_RETRIES 3
#define VERSION_STRING "1.0.0"

/* ✅ 合规 - 条件编译宏 */
#define FEATURE_ENABLED 1
#if FEATURE_ENABLED
    /* ... */
#endif

/* ✅ 合规 - 头文件保护 */
#ifndef MODULE_H
#define MODULE_H
/* 内容 */
#endif /* MODULE_H */
```

---

## Rule 19.3 - 条件编译

### 规则原文

> **Rule 19.3 (Required)**: Conditional compilation directives shall be used correctly.
>
> **中文**: 条件编译指令应正确使用。

### 合规示例

```c
/* ✅ 合规 - 完整的条件编译 */
#ifdef FEATURE_X
    /* 代码 */
#elif defined(FEATURE_Y)
    /* 代码 */
#else
    /* 默认代码 */
#endif

/* ✅ 合规 - 头文件保护 */
#ifndef UNIQUE_HEADER_H
#define UNIQUE_HEADER_H

/* 内容 */

#endif /* UNIQUE_HEADER_H */

/* ✅ 合规 - 平台检测 */
#if defined(_WIN32)
    /* Windows代码 */
#elif defined(__linux__)
    /* Linux代码 */
#elif defined(__APPLE__)
    /* macOS代码 */
#else
    #error "Unsupported platform"
#endif

/* ✅ 合规 - 版本检查 */
#if __STDC_VERSION__ >= 201112L
    /* C11代码 */
#elif __STDC_VERSION__ >= 199901L
    /* C99代码 */
#else
    /* C89代码 */
#endif

/* ✅ 合规 - 静态断言 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
    #include <assert.h>
    static_assert(sizeof(int) == 4, "int must be 32-bit");
#endif
```

### 条件编译最佳实践

```c
/* 配置文件示例：config.h */

#ifndef CONFIG_H
#define CONFIG_H

/* 调试级别 */
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_ERROR
#endif

/* 功能开关 */
#define ENABLE_FEATURE_A 1
#define ENABLE_FEATURE_B 0

/* 平台抽象 */
#if defined(_WIN32)
    #define PLATFORM_PATH_SEP '\\'
#elif defined(__unix__) || defined(__APPLE__)
    #define PLATFORM_PATH_SEP '/'
#else
    #define PLATFORM_PATH_SEP '/'
#endif

/* 编译器特性 */
#if defined(__GNUC__) || defined(__clang__)
    #define UNUSED __attribute__((unused))
    #define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define UNUSED
    #define NORETURN __declspec(noreturn)
#else
    #define UNUSED
    #define NORETURN
#endif

#endif /* CONFIG_H */
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 19


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
