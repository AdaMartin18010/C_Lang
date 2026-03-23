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
﻿# MISRA C:2023 Rules 20.1-20.x - 标准库

> 标准库函数安全使用

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 20.1 | 边界检查 | Required | 边界检查函数 |
| Rule 20.2 | 动态内存 | Required | malloc/free安全 |
| Rule 20.3 | 错误处理 | Required | errno使用 |
| Rule 20.4 | 线程安全 | Required | 线程安全函数 |
| Rule 20.5 | 废弃函数 | Required | 不使用废弃函数 |

---

## Rule 20.1 - 边界检查函数

### 规则原文

> **Rule 20.1 (Required)**: The validity of values passed to library functions shall be checked.
>
> **中文**: 应检查传递给库函数的值的有效性。

### 安全标准库使用

```c
/* ✅ 安全字符串函数 */
#include <string.h>

/* 使用带长度限制的函数 */
char dest[100];
strncpy(dest, src, sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';

/* 使用strnlen */
size_t len = strnlen(src, MAX_LEN);

/* ✅ 安全格式化 */
#include <stdio.h>

char buf[256];
snprintf(buf, sizeof(buf), "%s: %d", name, value);

/* ✅ 安全内存操作 */
#include <string.h>

/* 检查重叠（memmove可处理重叠）*/
if (dest < src || dest >= src + n) {
    memcpy(dest, src, n);
} else {
    memmove(dest, src, n);
}
```

---

## Rule 20.2 - 动态内存

### 规则原文

> **Rule 20.2 (Required)**: Dynamic memory allocation shall be used safely.
>
> **中文**: 动态内存分配应安全使用。

### 安全内存管理

```c
/* ✅ 检查返回值 */
void *ptr = malloc(size);
if (ptr == NULL) {
    /* 错误处理 */
    return ERROR_NO_MEMORY;
}

/* ✅ 零长度分配 */
if (size == 0) {
    /* 处理或返回错误 */
    return;
}
void *ptr = malloc(size);

/* ✅ 避免内存泄漏 */
void process(void)
{
    char *buffer = malloc(BUF_SIZE);
    if (buffer == NULL) {
        return;
    }

    /* 使用buffer */

    free(buffer);  /* 总是释放 */
}

/* ✅ 配对使用 */
void *create_object(void)
{
    return malloc(sizeof(Object));
}

void destroy_object(void *obj)
{
    free(obj);
}
```

---

## Rule 20.3 - 错误处理

### 规则原文

> **Rule 20.3 (Required)**: errno shall be used correctly.
>
> **中文**: errno应正确使用。

### 合规示例

```c
#include <errno.h>
#include <math.h>

/* ✅ 清除errno */
errno = 0;
double result = strtod(str, &endptr);
if (errno == ERANGE) {
    /* 溢出或下溢 */
}

/* ✅ 检查函数返回值 */
FILE *fp = fopen("file", "r");
if (fp == NULL) {
    if (errno == ENOENT) {
        /* 文件不存在 */
    } else if (errno == EACCES) {
        /* 权限不足 */
    }
}
```

---

## Rule 20.4 - 线程安全

### 规则原文

> **Rule 20.4 (Required)**: Thread-safe library functions shall be used in multi-threaded environments.
>
> **中文**: 多线程环境中应使用线程安全的库函数。

### 线程安全函数

```c
/* ✅ 使用线程安全版本 */
#include <string.h>

/* strtok_r是线程安全的，strtok不是 */
char *saveptr;
char *token = strtok_r(str, delim, &saveptr);

/* ✅ 使用本地时间函数 */
#include <time.h>

/* localtime_r是线程安全的 */
struct tm result;
localtime_r(&time_val, &result);

/* ✅ 使用可重入函数 */
#include <stdlib.h>

/* rand_r是线程安全的，rand不是 */
unsigned int seed = time(NULL);
int value = rand_r(&seed);
```

---

## Rule 20.5 - 废弃函数

### 规则原文

> **Rule 20.5 (Required)**: Obsolete or deprecated library functions shall not be used.
>
> **中文**: 不得使用废弃或弃用的库函数。

### 废弃函数替代

| 废弃函数 | 替代函数 | 原因 |
|:---------|:---------|:-----|
| `gets()` | `fgets()` | 缓冲区溢出 |
| `strcpy()` | `strncpy()` | 缓冲区溢出 |
| `strcat()` | `strncat()` | 缓冲区溢出 |
| `sprintf()` | `snprintf()` | 缓冲区溢出 |
| `vsprintf()` | `vsnprintf()` | 缓冲区溢出 |
| `rand()` | `rand_r()` / 更好的RNG | 质量差，非线程安全 |

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 20


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


---

## 深入理解

### 核心原理

深入探讨相关技术原理和实现细节。

### 实现机制

`c
// 概念示例
void concept_demo() {
    // 实现逻辑
}
``n

### 实践应用

- 应用案例1
- 应用案例2
- 应用案例3

### 最佳实践

1. 掌握基础概念
2. 理解实现原理
3. 应用最佳实践

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
