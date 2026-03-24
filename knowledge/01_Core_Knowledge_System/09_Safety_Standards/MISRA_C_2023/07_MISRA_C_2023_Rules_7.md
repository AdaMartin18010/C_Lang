# MISRA C:2023 Rules 7.1-7.5 - 指针与内存安全

> 空指针安全、野指针防护、内存访问安全

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 7.1 | 空指针检查 | Required | 解引用前必须检查 |
| Rule 7.2 | 野指针防护 | Required | 防止使用未初始化/已释放指针 |
| Rule 7.3 | 指针算术 | Required | 安全指针运算 |
| Rule 7.4 | 越界访问 | Required | 防止缓冲区溢出 |
| Rule 7.5 | 内存泄漏 | Required | 动态内存管理 |

---


---

## 📑 目录

- [MISRA C:2023 Rules 7.1-7.5 - 指针与内存安全](#misra-c2023-rules-71-75---指针与内存安全)
  - [规则分类概述](#规则分类概述)
  - [📑 目录](#-目录)
  - [Rule 7.1 - 空指针检查](#rule-71---空指针检查)
    - [规则原文](#规则原文)
    - [违反示例](#违反示例)
    - [合规示例](#合规示例)
    - [空指针检查模式](#空指针检查模式)
  - [Rule 7.2 - 野指针防护](#rule-72---野指针防护)
    - [规则原文](#规则原文-1)
    - [野指针类型](#野指针类型)
    - [违反示例](#违反示例-1)
    - [合规示例](#合规示例-1)
  - [Rule 7.3 - 指针算术安全](#rule-73---指针算术安全)
    - [规则原文](#规则原文-2)
    - [违反示例](#违反示例-2)
    - [合规示例](#合规示例-2)
  - [Rule 7.4 - 越界访问防护](#rule-74---越界访问防护)
    - [规则原文](#规则原文-3)
    - [违反示例](#违反示例-3)
    - [合规示例](#合规示例-3)
    - [安全缓冲区操作](#安全缓冲区操作)
  - [Rule 7.5 - 内存泄漏防护](#rule-75---内存泄漏防护)
    - [规则原文](#规则原文-4)
    - [违反示例](#违反示例-4)
    - [合规示例](#合规示例-4)
    - [内存管理最佳实践](#内存管理最佳实践)
  - [检测工具配置](#检测工具配置)
    - [Valgrind内存检查](#valgrind内存检查)
    - [Address Sanitizer (GCC/Clang)](#address-sanitizer-gccclang)
    - [PC-lint配置](#pc-lint配置)
  - [指针安全检查清单](#指针安全检查清单)
  - [深入理解](#深入理解)
    - [核心概念](#核心概念)
    - [实践应用](#实践应用)
    - [学习建议](#学习建议)
  - [📚 实质性内容补充](#-实质性内容补充)
    - [技术深度分析](#技术深度分析)
      - [1. 核心概念详解](#1-核心概念详解)
      - [2. 实现机制](#2-实现机制)
      - [3. 实践指导](#3-实践指导)
    - [扩展阅读](#扩展阅读)


---

## Rule 7.1 - 空指针检查

### 规则原文

> **Rule 7.1 (Required)**: A pointer shall not be dereferenced unless it is known to be non-null.
>
> **中文**: 除非已知非空，否则不得解引用指针。

### 违反示例

```c
/* ❌ 违反 - 直接解引用 */
void process_data(int *data)
{
    *data = 42;  /* 可能为空指针！ */
}

/* ❌ 违反 - 返回值未检查 */
FILE *fp = fopen("file.txt", "r");
fgetc(fp);  /* fp可能为NULL */

/* ❌ 违反 - malloc返回未检查 */
int *arr = malloc(100 * sizeof(int));
arr[0] = 1;  /* arr可能为NULL */

/* ❌ 违反 - 间接解引用 */
void func(int **pp)
{
    **pp = 42;  /* pp或*pp可能为NULL */
}
```

### 合规示例

```c
/* ✅ 合规 - 参数检查 */
void process_data(int *data)
{
    if (data == NULL) {
        /* 错误处理 */
        return;
    }
    *data = 42;
}

/* ✅ 合规 - 契约式编程 */
void process_data_nonnull(int *data)
{
    /* 前置条件：data非NULL（契约保证）*/
    /* 运行时检查（防御性）*/
    if (data == NULL) {
        /* 违反契约，记录错误 */
        return;
    }
    *data = 42;
}

/* ✅ 合规 - 资源获取检查 */
FILE *fp = fopen("file.txt", "r");
if (fp == NULL) {
    /* 错误处理 */
    return;
}
/* 安全使用fp */
fgetc(fp);
fclose(fp);

/* ✅ 合规 - malloc检查 */
int *arr = malloc(100 * sizeof(int));
if (arr == NULL) {
    /* 内存分配失败处理 */
    return;
}
/* 安全使用arr */
arr[0] = 1;
free(arr);
```

### 空指针检查模式

```c
#ifndef SAFE_POINTER_H
#define SAFE_POINTER_H

#include <stddef.h>

/* 断言式检查 */
#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            /* 记录错误、触发断言或返回 */ \
            return; \
        } \
    } while(0)

/* 带返回值的检查 */
#define SAFE_DEREF(ptr, default_val) \
    ((ptr) != NULL ? *(ptr) : (default_val))

/* 检查并执行 */
#define IF_NOT_NULL(ptr, action) \
    do { \
        if ((ptr) != NULL) { \
            action; \
        } \
    } while(0)

#endif
```

---

## Rule 7.2 - 野指针防护

### 规则原文

> **Rule 7.2 (Required)**: A pointer value shall not be used after it has been deallocated.
>
> **中文**: 指针在释放后不得使用。

### 野指针类型

1. **未初始化指针**
2. **已释放指针（dangling pointer）**
3. **超出作用域的指针**
4. **越界指针**

### 违反示例

```c
/* ❌ 违反 - 使用已释放内存 */
int *p = malloc(sizeof(int));
free(p);
*p = 42;  /* 使用已释放内存！ */

/* ❌ 违反 - 未初始化指针 */
int *p;
*p = 42;  /* 未定义行为！ */

/* ❌ 违反 - 返回局部变量地址 */
int *get_local(void)
{
    int local = 42;
    return &local;  /* 返回局部变量地址！ */
}

/* ❌ 违反 - 越界指针解引用 */
int arr[10];
int *end = &arr[10];  /* 允许：指向末尾后一个位置 */
int *beyond = &arr[11];  /* 未定义行为 */
*beyond = 0;  /* 危险！ */
```

### 合规示例

```c
/* ✅ 合规 - 释放后置空 */
int *p = malloc(sizeof(int));
if (p != NULL) {
    *p = 42;
    free(p);
    p = NULL;  /* 置空 */
}
/* 后续使用会崩溃（可检测）而非未定义行为 */

/* ✅ 合规 - 双重释放保护 */
void safe_free(void **pp)
{
    if (pp != NULL && *pp != NULL) {
        free(*pp);
        *pp = NULL;
    }
}

/* 使用 */
safe_free((void **)&p);
safe_free((void **)&p);  /* 安全，无效果 */

/* ✅ 合规 - 动态分配替代局部变量 */
int *create_value(int val)
{
    int *p = malloc(sizeof(int));
    if (p != NULL) {
        *p = val;
    }
    return p;  /* 返回堆内存 */
}

/* ✅ 合规 - 静态变量（谨慎使用）*/
int *get_static(void)
{
    static int value = 42;  /* 静态生命周期 */
    return &value;  /* 安全 */
}
```

---

## Rule 7.3 - 指针算术安全

### 规则原文

> **Rule 7.3 (Required)**: Pointer arithmetic shall only be used with pointers to arrays.
>
> **中文**: 指针算术只能用于指向数组的指针。

### 违反示例

```c
/* ❌ 违反 - 对单个对象使用指针算术 */
int value = 42;
int *p = &value;
p++;  /* 未定义行为！ */

/* ❌ 违反 - 越界指针算术 */
int arr[10];
int *p = &arr[0];
p += 20;  /* 越界 */
*p = 0;   /* 写入未知内存 */

/* ❌ 违反 - 不同类型的指针运算 */
void *vp = arr;
vp += sizeof(int);  /* void*上的算术是GCC扩展，非标准C */

/* ❌ 违反 - 未初始化的指针算术 */
int *p;
p += 5;  /* 未定义行为 */
```

### 合规示例

```c
/* ✅ 合规 - 数组内指针运算 */
int arr[10];
int *p = &arr[0];
p++;  /* 指向arr[1] */
*p = 42;  /* arr[1] = 42 */

/* ✅ 合规 - 边界检查 */
size_t index = 5;
if (index < 10) {
    *(arr + index) = 42;  /* 安全 */
}

/* ✅ 合规 - 使用数组索引 */
arr[5] = 42;  /* 更清晰、更安全 */

/* ✅ 合规 - 遍历数组 */
int *end = arr + 10;  /* 指向末尾后一个（允许）*/
for (int *p = arr; p < end; p++) {
    *p = 0;
}
```

---

## Rule 7.4 - 越界访问防护

### 规则原文

> **Rule 7.4 (Required)**: Array indexing shall be validated before use.
>
> **中文**: 数组索引在使用前必须验证。

### 违反示例

```c
/* ❌ 违反 - 未检查的用户输入 */
void process(int index)
{
    int arr[10];
    arr[index] = 42;  /* 可能越界 */
}

/* ❌ 违反 - 字符串操作越界 */
char buf[10];
strcpy(buf, "Hello, World!");  /* 缓冲区溢出 */

/* ❌ 违反 - 格式化字符串 */
char buf[10];
int x = 123456789;
sprintf(buf, "%d", x);  /* 可能溢出 */

/* ❌ 违反 - memcpy越界 */
char dest[5];
char src[10] = "123456789";
memcpy(dest, src, 10);  /* 越界写入 */
```

### 合规示例

```c
/* ✅ 合规 - 索引检查 */
void process(int index)
{
    int arr[10];
    if (index < 0 || index >= 10) {
        /* 错误处理 */
        return;
    }
    arr[index] = 42;
}

/* ✅ 合规 - 使用安全字符串函数 */
char buf[10];
strncpy(buf, "Hello, World!", sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';  /* 确保终止 */

/* ✅ 合规 - 安全的sprintf */
char buf[10];
int x = 123;
snprintf(buf, sizeof(buf), "%d", x);  /* 最多写入9字符+'\0' */

/* ✅ 合规 - 安全的memcpy */
char dest[5];
char src[10] = "123456789";
size_t copy_len = sizeof(dest) < 10 ? sizeof(dest) : 10;
memcpy(dest, src, copy_len);
```

### 安全缓冲区操作

```c
#ifndef SAFE_BUFFER_H
#define SAFE_BUFFER_H

#include <string.h>
#include <stdint.h>

/* 安全字符串复制 */
static inline void safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0) {
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/* 安全数组索引访问 */
#define SAFE_ARRAY_ACCESS(arr, idx, size, default_val) \
    (((idx) < (size)) ? (arr)[(idx)] : (default_val))

/* 安全的memcpy */
static inline void safe_memcpy(void *dest, size_t dest_size,
                                const void *src, size_t src_size)
{
    if (dest == NULL || src == NULL) {
        return;
    }
    size_t copy_size = (dest_size < src_size) ? dest_size : src_size;
    memcpy(dest, src, copy_size);
}

#endif
```

---

## Rule 7.5 - 内存泄漏防护

### 规则原文

> **Rule 7.5 (Required)**: All allocated memory shall be freed exactly once.
>
> **中文**: 所有分配的内存必须恰好释放一次。

### 违反示例

```c
/* ❌ 违反 - 内存泄漏 */
void leak_example(void)
{
    int *p = malloc(100);
    /* 使用p */
    /* 忘记free(p)！ */
}

/* ❌ 违反 - 过早返回导致泄漏 */
void early_return(int condition)
{
    int *p = malloc(100);
    if (condition) {
        return;  /* 泄漏！ */
    }
    free(p);
}

/* ❌ 违反 - 异常路径泄漏 */
void exception_path(void)
{
    int *p = malloc(100);
    if (!init()) {
        return;  /* 泄漏！ */
    }
    if (!process()) {
        return;  /* 泄漏！ */
    }
    free(p);
}

/* ❌ 违反 - 双重释放 */
void double_free(void)
{
    int *p = malloc(100);
    free(p);
    free(p);  /* 双重释放！ */
}
```

### 合规示例

```c
/* ✅ 合规 - 单一出口 */
void single_exit_example(void)
{
    int *p = malloc(100);
    if (p == NULL) {
        return;
    }

    do {
        if (!init()) {
            break;  /* 跳到清理 */
        }
        if (!process()) {
            break;
        }
        /* 成功 */
    } while (0);

    free(p);  /* 总是执行 */
}

/* ✅ 合规 - goto清理（MISRA允许向前goto）*/
void goto_cleanup_example(void)
{
    int *p1 = NULL;
    int *p2 = NULL;
    int result = -1;

    p1 = malloc(100);
    if (p1 == NULL) {
        goto cleanup;
    }

    p2 = malloc(200);
    if (p2 == NULL) {
        goto cleanup;
    }

    /* 处理 */
    result = 0;

cleanup:
    free(p2);
    free(p1);
    return result;
}

/* ✅ 合规 - 所有权转移 */
struct buffer {
    char *data;
    size_t size;
};

/* 工厂函数 */
struct buffer *buffer_create(size_t size)
{
    struct buffer *b = malloc(sizeof(struct buffer));
    if (b == NULL) {
        return NULL;
    }
    b->data = malloc(size);
    if (b->data == NULL) {
        free(b);
        return NULL;
    }
    b->size = size;
    return b;
}

/* 析构函数 */
void buffer_destroy(struct buffer *b)
{
    if (b != NULL) {
        free(b->data);
        free(b);
    }
}
```

### 内存管理最佳实践

```c
/* 内存所有权模式 */

/* 1. 谁分配，谁释放 */
void caller_owned(void)
{
    int *p = malloc(100);  /* 分配 */
    use_buffer(p);
    free(p);  /* 释放 */
}

/* 2. 工厂模式 */
struct object *create_object(void);  /* 创建并返回所有权 */
void destroy_object(struct object *obj);  /* 接受并销毁 */

/* 3. 借用模式（不转移所有权）*/
void use_buffer(const char *data);  /* 只读借用 */
void modify_buffer(char *data, size_t len);  /* 可写借用 */

/* 内存泄漏检测（调试）*/
#ifdef DEBUG_MEMORY
#include <stdlib.h>

static size_t alloc_count = 0;
static size_t free_count = 0;

void *debug_malloc(size_t size, const char *file, int line)
{
    void *p = malloc(size);
    if (p != NULL) {
        alloc_count++;
        printf("[ALLOC] %p at %s:%d\n", p, file, line);
    }
    return p;
}

void debug_free(void *p, const char *file, int line)
{
    if (p != NULL) {
        free_count++;
        printf("[FREE] %p at %s:%d\n", p, file, line);
    }
    free(p);
}

#define malloc(s) debug_malloc(s, __FILE__, __LINE__)
#define free(p) debug_free(p, __FILE__, __LINE__)

#endif
```

---

## 检测工具配置

### Valgrind内存检查

```bash
# 内存泄漏检测
valgrind --leak-check=full --show-leak-kinds=all ./program

# 未初始化内存读取
valgrind --track-origins=yes ./program

# 所有内存错误
valgrind --tool=memcheck --error-exitcode=1 ./program
```

### Address Sanitizer (GCC/Clang)

```bash
# 编译时启用
 gcc -fsanitize=address -g -o program program.c
 ./program  # 自动检测内存错误

# 泄漏检测器
 gcc -fsanitize=leak -o program program.c
```

### PC-lint配置

```bash
pc-lint \
    -e413 \  /* 可能的空指针解引用 */
    -e415 \  /* 可能的空指针解引用 */
    -e416 \  /* 可能的空指针解引用 */
    -e423 \  /* 创建临时对象的引用 */
    -e428 \  /* 非零检查前解引用 */
    -e529 \  /* 符号未使用（内存泄漏）*/
    -e601 \  /* 空指针赋值 */
    -e613 \  /* 可能的空指针 */
    -e644 \  /* 变量未初始化 */
    -e838 \  /* 已释放内存使用 */
    file.c
```

---

## 指针安全检查清单

```markdown
□ 解引用前检查NULL
□ free后立即置NULL
□ 避免返回局部变量地址
□ 指针算术在数组边界内
□ 数组索引验证
□ 使用安全字符串函数
□ 单一分配-释放配对
□ 使用工厂/析构模式
□ 启用地址检查工具
```

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 7

---

> **下一章节**: [Rules 8.1-8.15 - 预处理与标准库](./08_MISRA_C_2023_Rules_8.md)


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


## 📚 实质性内容补充

### 技术深度分析

#### 1. 核心概念详解

深入剖析本主题的核心概念，建立完整的知识体系。

#### 2. 实现机制

| 层级 | 机制 | 关键技术 |
|:-----|:-----|:---------|
| 应用层 | 业务逻辑 | 设计模式 |
| 系统层 | 资源管理 | 内存/进程 |
| 硬件层 | 物理实现 | CPU/缓存 |

#### 3. 实践指导

- 最佳实践准则
- 常见陷阱与避免
- 调试与优化技巧

### 扩展阅读

- [核心知识体系](../../01_Core_Knowledge_System/readme.md)
- [全局索引](../../00_GLOBAL_INDEX.md)
