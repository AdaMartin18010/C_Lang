# <stdckdint.h> 完全参考手册 (C23)

> **标准**: ISO/IEC 9899:2024 | **头文件**: `<stdckdint.h>` | **难度**: ⭐⭐⭐⭐

---

## 目录

- [\<stdckdint.h\> 完全参考手册 (C23)](#stdckdinth-完全参考手册-c23)
  - [目录](#目录)
  - [一、概述](#一概述)
    - [1.1 安全整数运算需求](#11-安全整数运算需求)
    - [1.2 与传统方法的对比](#12-与传统方法的对比)
  - [二、核心 API](#二核心-api)
    - [2.1 ckd\_add - 检查加法](#21-ckd_add---检查加法)
    - [2.2 ckd\_sub - 检查减法](#22-ckd_sub---检查减法)
    - [2.3 ckd\_mul - 检查乘法](#23-ckd_mul---检查乘法)
  - [三、形式化语义](#三形式化语义)
  - [四、与 Zig 对比](#四与-zig-对比)
  - [五、实战案例](#五实战案例)

---

## 一、概述

### 1.1 安全整数运算需求

```c
// C23 引入 <stdckdint.h> 解决整数溢出问题
// 整数溢出是安全漏洞的主要来源之一

#include <stdckdint.h>
#include <stdio.h>
#include <limits.h>

int main(void) {
    int result;

    // 安全加法
    bool overflow = ckd_add(&result, INT_MAX, 1);
    if (overflow) {
        printf("Overflow detected!\n");
        // result 包含回绕值，但不使用
    }

    return 0;
}
```

### 1.2 与传统方法的对比

```c
// 方法 1: 传统手动检查 (容易出错)
int unsafe_add(int a, int b) {
    if (a > 0 && b > INT_MAX - a) {
        // overflow
    }
    return a + b;
}

// 方法 2: 内置函数 (GCC/Clang 特定)
int gcc_add(int a, int b) {
    int result;
    bool overflow = __builtin_add_overflow(a, b, &result);
    // ...
}

// 方法 3: C23 标准方法 (可移植)
int c23_add(int a, int b) {
    int result;
    bool overflow = ckd_add(&result, a, b);
    // ...
}
```

---

## 二、核心 API

### 2.1 ckd_add - 检查加法

```c
#include <stdckdint.h>
#include <stdbool.h>

// 完整类型覆盖
bool ckd_add(int *result, int a, int b);
bool ckd_add(long *result, long a, long b);
bool ckd_add(long long *result, long long a, long long b);

// 无符号版本
bool ckd_add(unsigned int *result, unsigned int a, unsigned int b);
bool ckd_add(unsigned long *result, unsigned long a, unsigned long b);
bool ckd_add(unsigned long long *result, unsigned long long a, unsigned long long b);

// 使用示例
void ckd_add_examples(void) {
    int result;

    // 正常加法
    bool overflow1 = ckd_add(&result, 100, 200);
    // overflow1 = false, result = 300

    // 溢出检测 (有符号)
    bool overflow2 = ckd_add(&result, INT_MAX, 1);
    // overflow2 = true, result = INT_MIN (回绕)

    // 溢出检测 (无符号)
    unsigned int uresult;
    bool overflow3 = ckd_add(&uresult, UINT_MAX, 1);
    // overflow3 = true, uresult = 0
}
```

### 2.2 ckd_sub - 检查减法

```c
#include <stdckdint.h>

bool ckd_sub(int *result, int a, int b);
bool ckd_sub(long *result, long a, long b);
bool ckd_sub(long long *result, long long a, long long b);

// 使用示例
void ckd_sub_examples(void) {
    int result;

    // 正常减法
    ckd_sub(&result, 100, 50);  // result = 50, return false

    // 下溢检测
    bool underflow = ckd_sub(&result, INT_MIN, 1);
    // underflow = true, result = INT_MAX

    // 无符号减法溢出
    unsigned int uresult;
    bool overflow = ckd_sub(&uresult, 0U, 1U);
    // overflow = true, uresult = UINT_MAX
}
```

### 2.3 ckd_mul - 检查乘法

```c
#include <stdckdint.h>

bool ckd_mul(int *result, int a, int b);
bool ckd_mul(long *result, long a, long b);
bool ckd_mul(long long *result, long long a, long long b);

// 使用示例
void ckd_mul_examples(void) {
    int result;

    // 正常乘法
    ckd_mul(&result, 100, 200);  // result = 20000

    // 溢出检测
    bool overflow = ckd_mul(&result, INT_MAX, 2);
    // overflow = true

    // 安全分配计算
    size_t total_size;
    size_t count = 1000000;
    size_t element_size = sizeof(int);

    if (ckd_mul(&total_size, count, element_size)) {
        // 溢出处理
        fprintf(stderr, "Size overflow!\n");
        return;
    }

    int* arr = malloc(total_size);
}
```

---

## 三、形式化语义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                    <stdckdint.h> 形式化语义                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ckd_add(result, a, b):                                                      │
│  ─────────────────────                                                       │
│  设 sum = (intmax_t)a + (intmax_t)b  // 提升为最大类型计算                   │
│                                                                              │
│  如果 sum ∈ [INT_MIN, INT_MAX]:                                              │
│      *result = (int)sum;                                                     │
│      return false;                                                           │
│  否则:                                                                       │
│      *result = (int)(sum % 2^32);  // 回绕                                   │
│      return true;                                                            │
│                                                                              │
│  ckd_sub(result, a, b):                                                      │
│  ─────────────────────                                                       │
│  设 diff = (intmax_t)a - (intmax_t)b                                         │
│                                                                              │
│  如果 diff ∈ [INT_MIN, INT_MAX]:                                             │
│      *result = (int)diff;                                                    │
│      return false;                                                           │
│  否则:                                                                       │
│      *result = (int)(diff % 2^32);                                           │
│      return true;                                                            │
│                                                                              │
│  ckd_mul(result, a, b):                                                      │
│  ─────────────────────                                                       │
│  设 prod = (intmax_t)a * (intmax_t)b                                         │
│                                                                              │
│  如果 prod ∈ [INT_MIN, INT_MAX]:                                             │
│      *result = (int)prod;                                                    │
│      return false;                                                           │
│  否则:                                                                       │
│      *result = (int)(prod % 2^32);                                           │
│      return true;                                                            │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 四、与 Zig 对比

```zig
const std = @import("std");

// Zig 内置溢出检测
fn zig_overflow_detection() void {
    const a: i32 = 100;
    const b: i32 = 200;

    // 方法 1: 使用内置函数
    const sum, const overflowed = @addWithOverflow(a, b);
    if (overflowed) {
        // 处理溢出
    }

    // 方法 2: 饱和运算
    const saturated = a +| b;  // 溢出时取最大值

    // 方法 3: 回绕运算 (显式)
    const wrapped = a +% b;    // 溢出时回绕

    // 方法 4: 严格模式 (Debug 模式会 panic)
    const strict = a + b;      // 溢出时 panic (Debug)
}

// Zig 乘法检查
fn safeMultiply(a: usize, b: usize) !usize {
    const result, const overflow = @mulWithOverflow(a, b);
    if (overflow) {
        return error.Overflow;
    }
    return result;
}
```

---

## 五、实战案例

```c
// 案例 1: 安全数组分配
#include <stdckdint.h>
#include <stdlib.h>
#include <stdio.h>

void* safe_malloc_array(size_t nmemb, size_t size) {
    size_t total;
    if (ckd_mul(&total, nmemb, size)) {
        fprintf(stderr, "Array size overflow\n");
        return NULL;
    }
    return malloc(total);
}

// 案例 2: 金融计算
#include <stdckdint.h>
#include <stdint.h>

typedef struct {
    int64_t cents;
} Money;

bool money_add(Money* result, Money a, Money b) {
    return ckd_add(&result->cents, a.cents, b.cents);
}

// 案例 3: 图像处理 (尺寸计算)
#include <stdckdint.h>
#include <stdbool.h>

typedef struct {
    int width, height;
} ImageSize;

bool image_buffer_size(ImageSize size, int channels, size_t* out_size) {
    size_t pixel_count;
    if (ckd_mul(&pixel_count, size.width, size.height)) {
        return false;  // 溢出
    }
    if (ckd_mul(out_size, pixel_count, channels)) {
        return false;  // 溢出
    }
    return true;
}

// 案例 4: 网络协议 (序列号计算)
#include <stdckdint.h>
#include <stdint.h>

uint32_t safe_sequence_add(uint32_t seq, uint32_t offset) {
    uint32_t result;
    // 无符号溢出在序列号中是预期的
    ckd_add(&result, seq, offset);
    return result;
}
```

---

> **参考**: ISO/IEC 9899:2024 7.20 | **WG14 N3080**


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
