# SEI CERT C Coding Standard (2024 Edition)


---

## 📑 目录

- [SEI CERT C Coding Standard (2024 Edition)](#sei-cert-c-coding-standard-2024-edition)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [标准架构](#标准架构)
    - [规则分类体系](#规则分类体系)
    - [优先级矩阵](#优先级矩阵)
  - [C23 语言特性更新 (2024)](#c23-语言特性更新-2024)
    - [新增安全特性支持](#新增安全特性支持)
    - [废弃危险特性](#废弃危险特性)
  - [核心规则详解](#核心规则详解)
    - [1. 内存管理规则 (MEM)](#1-内存管理规则-mem)
      - [MEM30-C: 不要访问已释放的内存](#mem30-c-不要访问已释放的内存)
      - [MEM31-C: 动态内存分配后必须释放](#mem31-c-动态内存分配后必须释放)
    - [2. 字符串处理规则 (STR)](#2-字符串处理规则-str)
      - [STR30-C: 不要试图修改字符串字面量](#str30-c-不要试图修改字符串字面量)
      - [STR31-C: 确保字符串以 null 结尾](#str31-c-确保字符串以-null-结尾)
    - [3. 整数运算规则 (INT)](#3-整数运算规则-int)
      - [INT30-C: 确保无符号整数运算不回绕](#int30-c-确保无符号整数运算不回绕)
      - [INT32-C: 确保有符号整数运算不溢出](#int32-c-确保有符号整数运算不溢出)
    - [4. 输入验证规则 (ENV/FIO)](#4-输入验证规则-envfio)
      - [ENV33-C: 小心执行外部程序](#env33-c-小心执行外部程序)
      - [FIO30-C: 排除用户输入中的格式字符串](#fio30-c-排除用户输入中的格式字符串)
    - [5. 并发规则 (CON)](#5-并发规则-con)
      - [CON30-C: 正确使用互斥量](#con30-c-正确使用互斥量)
  - [静态分析工具集成](#静态分析工具集成)
    - [CodeQL CERT C 规则集](#codeql-cert-c-规则集)
    - [主要 CERT C 规则覆盖](#主要-cert-c-规则覆盖)
  - [与 ISO/IEC TS 17961 映射](#与-isoiec-ts-17961-映射)
  - [2024 版更新要点](#2024-版更新要点)
  - [参考资源](#参考资源)


---

## 概述

SEI CERT C Coding Standard 是由卡内基梅隆大学软件工程研究所 (SEI) 维护的C语言安全编码标准，最新版本为 2024 版。
该标准通过规范编码实践来消除可能导致安全漏洞的不安全编码行为和未定义行为。

**标准地位**: 该标准已成为 ISO/IEC TS 17961 (Secure C) 国际标准的基准，被 Cisco、Oracle 等大型企业广泛采用，并被 GitHub CodeQL 等自动化分析工具集成。

---

## 标准架构

### 规则分类体系

| 类别 | 范围 | 优先级 | 数量 |
|------|------|--------|------|
| **Rules** (规则) | 强制性要求，必须遵守 | L1/L2/L3 | ~100条 |
| **Recommendations** (建议) | 推荐性最佳实践 | N/A | ~150条 |

### 优先级矩阵

```
严重等级评估:
┌─────────────────┬─────────────────┬─────────────────┐
│     严重/高度    │     中低/高度    │     低/高度     │
│    优先级:1     │    优先级:2     │    优先级:3     │
├─────────────────┼─────────────────┼─────────────────┤
│     严重/中度    │     中低/中度    │     低/中度     │
│    优先级:4     │    优先级:6     │    优先级:8     │
├─────────────────┼─────────────────┼─────────────────┤
│     严重/低度    │     中低/低度    │     低/低度     │
│    优先级:5     │    优先级:7     │    优先级:9     │
└─────────────────┴─────────────────┴─────────────────┘

评估维度:
- 严重性: 违反规则导致安全漏洞的严重程度
- 可能性: 违反规则导致漏洞的概率
- 成本: 修复违反问题的成本
```

---

## C23 语言特性更新 (2024)

### 新增安全特性支持

```c
// C23 constexpr - 编译时常量
constexpr int MAX_BUFFER_SIZE = 1024;  // 编译期确定，避免运行时错误

// C23 nullptr 替代 NULL
void* p = nullptr;  // 类型安全的空指针

// C23 属性语法标准化
[[nodiscard]] int critical_operation(void);  // 强制检查返回值
[[maybe_unused]] int debug_var = 0;          // 显式标记可能未使用

// C23 typeof 运算符
typeof(int) x;  // 类型推导，减少类型错误
typeof_unqual(const int) y;  // 移除类型限定符
```

### 废弃危险特性

| 特性 | 状态 | 替代方案 |
|------|------|----------|
| `gets()` | 已移除 | `fgets()` |
| K&R 函数声明 | 已废弃 | 原型声明 |
| 隐式函数声明 | 已移除 | 显式声明 |
| `noreturn` 宏 | 废弃 | `[[noreturn]]` 属性 |

---

## 核心规则详解

### 1. 内存管理规则 (MEM)

#### MEM30-C: 不要访问已释放的内存

```c
// 不合规代码 - Use-After-Free
void process_data(char* data) {
    free(data);
    // 错误: 访问已释放内存
    if (data[0] == 'A') {  // UAF 漏洞!
        // ...
    }
}

// 合规代码
void process_data_safe(char* data) {
    if (data[0] == 'A') {  // 先访问，后释放
        // ...
    }
    free(data);
    data = NULL;  // 置空，避免悬挂指针
}
```

#### MEM31-C: 动态内存分配后必须释放

```c
// 不合规代码 - 内存泄漏
int* create_buffer(void) {
    int* buf = malloc(100 * sizeof(int));
    if (buf == NULL) return NULL;

    if (some_error_condition()) {
        return NULL;  // 泄漏!
    }
    return buf;
}

// 合规代码
int* create_buffer_safe(void) {
    int* buf = malloc(100 * sizeof(int));
    if (buf == NULL) return NULL;

    if (some_error_condition()) {
        free(buf);  // 正确释放
        return NULL;
    }
    return buf;
}
```

### 2. 字符串处理规则 (STR)

#### STR30-C: 不要试图修改字符串字面量

```c
// 不合规代码 - 未定义行为
void modify_string(void) {
    char* str = "hello";  // 字符串字面量
    str[0] = 'H';         // 崩溃或不可预测行为
}

// 合规代码
void safe_string_modify(void) {
    char str[] = "hello";  // 字符数组，可修改
    str[0] = 'H';          // 安全

    // 或使用动态分配
    char* str2 = strdup("hello");
    if (str2 != NULL) {
        str2[0] = 'H';
        free(str2);
    }
}
```

#### STR31-C: 确保字符串以 null 结尾

```c
// 不合规代码 - 非空终止字符串
void copy_string(char* src, char* dest, size_t n) {
    strncpy(dest, src, n);  // 如果 src >= n，dest 可能不终止
    // 未显式设置终止符
}

// 合规代码
void copy_string_safe(const char* src, char* dest, size_t n) {
    if (src == NULL || dest == NULL || n == 0) return;

    strncpy(dest, src, n - 1);  // 留一个位置给 '\0'
    dest[n - 1] = '\0';         // 确保终止
}
```

### 3. 整数运算规则 (INT)

#### INT30-C: 确保无符号整数运算不回绕

```c
#include <stdint.h>
#include <limits.h>

// 不合规代码 - 整数回绕
uint32_t add_sizes(uint32_t a, uint32_t b) {
    return a + b;  // 可能回绕!
}

// 合规代码 - 溢出检测
#include <stdbool.h>

bool add_sizes_safe(uint32_t a, uint32_t b, uint32_t* result) {
    if (a > UINT32_MAX - b) {
        return false;  // 溢出
    }
    *result = a + b;
    return true;
}

// C23 使用标准检查函数
#if __STDC_VERSION__ >= 202311L
#include <stdckdint.h>

bool add_sizes_c23(uint32_t a, uint32_t b, uint32_t* result) {
    return ckd_add(result, a, b);  // 标准溢出检测
}
#endif
```

#### INT32-C: 确保有符号整数运算不溢出

```c
// 不合规代码 - 有符号溢出 (UB)
int multiply(int a, int b) {
    return a * b;  // 未定义行为如果溢出
}

// 合规代码
#include <limits.h>

bool multiply_safe(int a, int b, int* result) {
    if (a > 0) {
        if (b > 0) {
            if (a > (INT_MAX / b)) return false;
        } else {
            if (b < (INT_MIN / a)) return false;
        }
    } else {
        if (b > 0) {
            if (a < (INT_MIN / b)) return false;
        } else {
            if (a != 0 && b < (INT_MAX / a)) return false;
        }
    }
    *result = a * b;
    return true;
}
```

### 4. 输入验证规则 (ENV/FIO)

#### ENV33-C: 小心执行外部程序

```c
// 不合规代码 - 命令注入
void run_command(const char* user_input) {
    char cmd[256];
    sprintf(cmd, "ls %s", user_input);  // 注入风险!
    system(cmd);
}

// 合规代码 - 使用 exec 系列函数
#include <unistd.h>
#include <sys/wait.h>

bool run_command_safe(const char* path, char* const args[]) {
    pid_t pid = fork();
    if (pid == -1) return false;

    if (pid == 0) {
        // 子进程
        execv(path, args);
        _exit(127);  // 如果 exec 失败
    }

    // 父进程等待
    int status;
    return waitpid(pid, &status, 0) != -1 && WIFEXITED(status) && WEXITSTATUS(status) == 0;
}
```

#### FIO30-C: 排除用户输入中的格式字符串

```c
// 不合规代码 - 格式字符串攻击
void log_message(const char* user_msg) {
    printf(user_msg);  // 如果 user_msg 包含 %s %n 等，危险!
}

// 合规代码
void log_message_safe(const char* user_msg) {
    printf("%s", user_msg);  // 使用固定格式字符串
}

// 更好：使用 fputs
void log_message_best(const char* user_msg) {
    fputs(user_msg, stdout);
}
```

### 5. 并发规则 (CON)

#### CON30-C: 正确使用互斥量

```c
#include <threads.h>  // C11/C23 线程支持

// 不合规代码 - 数据竞争
static int shared_counter = 0;

void increment_unsafe(void) {
    shared_counter++;  // 非原子操作，数据竞争!
}

// 合规代码
static mtx_t counter_mutex;
static int shared_counter_safe = 0;

bool increment_safe(void) {
    if (mtx_lock(&counter_mutex) != thrd_success) {
        return false;
    }
    shared_counter_safe++;
    mtx_unlock(&counter_mutex);
    return true;
}

// C23 使用原子操作
#if __STDC_VERSION__ >= 202311L
#include <stdatomic.h>

static _Atomic int atomic_counter = 0;

void increment_atomic(void) {
    atomic_fetch_add(&atomic_counter, 1, memory_order_relaxed);
}
#endif
```

---

## 静态分析工具集成

### CodeQL CERT C 规则集

```yaml
# GitHub Actions 配置示例
name: CERT C Security Scan
on: [push, pull_request]

jobs:
  analyze:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Initialize CodeQL
        uses: github/codeql-action/init@v3
        with:
          languages: c-cpp
          queries: security-extended,security-and-quality

      - name: Build
        run: make

      - name: Perform CodeQL Analysis
        uses: github/codeql-action/analyze@v3
```

### 主要 CERT C 规则覆盖

| 规则ID | 描述 | 严重性 |
|--------|------|--------|
| `cpp/cert/atoi-used` | 使用 atoi() 而非 strtol() | 中 |
| `cpp/cert/dangerous-function-overflow` | 危险函数溢出检查 | 高 |
| `cpp/cert/information-leak` | 信息泄露检测 | 中 |
| `cpp/cert/non-constant-format` | 非常量格式字符串 | 高 |
| `cpp/cert/null-pointer-deref` | 空指针解引用 | 高 |
| `cpp/cert/use-after-free` | 释放后使用 | 高 |

---

## 与 ISO/IEC TS 17961 映射

SEI CERT C 标准与 ISO/IEC TS 17961 国际标准有完整的映射关系：

| ISO 17961 规则 | CERT C 规则 | 描述 |
|----------------|-------------|------|
| 5.1 - Calling `system()` | ENV33-C | 环境命令执行安全 |
| 5.2 - Buffer overflow | STR31-C, STR32-C | 缓冲区溢出防护 |
| 5.3 - Buffer overflow from tainted data | STR31-C | 污染数据输入检查 |
| 5.4 - Integer overflow | INT30-C, INT32-C | 整数溢出防护 |
| 5.5 - Memory management | MEM30-C, MEM31-C | 内存管理安全 |

**合规性说明**: 符合 MISRA C:2025 的代码自动满足 ISO/IEC TS 17961 要求。

---

## 2024 版更新要点

1. **C23 标准集成**: 全面支持 C23 安全特性
2. **内存安全强化**: 新增对 `memset_explicit` 等安全函数的指导
3. **原子操作指南**: 更新 C11/C23 原子操作最佳实践
4. **AI 辅助验证**: 支持基于 LLM 的代码安全分析训练数据
5. **内存安全语言对比**: 增加 Rust 内存安全模型对比分析

---

## 参考资源

- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c)
- [SEI CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/display/cplusplus)
- [ISO/IEC TS 17961:2013](https://www.iso.org/standard/61134.html)
- [Robert Seacord - Effective C, 2nd Edition](https://www.informit.com/store/effective-c-9780138374269)
- [GitHub CodeQL CERT C Queries](https://github.com/github/codeql/tree/main/cpp/ql/src/cert)

---

*最后更新: 2026-03-18 | 版本: 2024 Edition*:


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
