# C23 标准库完整参考

> **定位**: 核心知识体系 / 标准库层 | **标准**: ISO/IEC 9899:2024 (C23)

---

## 目录

- [C23 标准库完整参考](#c23-标准库完整参考)
  - [目录](#目录)
  - [C23 标准库概述](#c23-标准库概述)
    - [核心改进](#核心改进)
    - [标准版本宏](#标准版本宏)
  - [新增头文件](#新增头文件)
    - [`<stdbit.h>` - 位和字节操作 (C23)](#stdbith---位和字节操作-c23)
    - [`<stdckdint.h>` - 整数溢出检查 (C23)](#stdckdinth---整数溢出检查-c23)
  - [更新的头文件](#更新的头文件)
    - [`<string.h>` 新增函数](#stringh-新增函数)
    - [`<stdlib.h>` 新增函数](#stdlibh-新增函数)
    - [`<stdio.h>` 更新](#stdioh-更新)
  - [弃用的特性](#弃用的特性)
  - [学习路径](#学习路径)
    - [初学者路径](#初学者路径)
    - [进阶路径](#进阶路径)
    - [专家路径](#专家路径)
  - [参考资料](#参考资料)

---

## C23 标准库概述

C23 (ISO/IEC 9899:2024) 是 C 语言的最新标准，于 2024年10月31日 正式发布。标准库引入了多项重要改进：

### 核心改进

| 特性 | 说明 | 头文件 |
|:-----|:-----|:-------|
| **nullptr** | 类型安全的空指针常量 | 语言关键字 |
| **constexpr** | 编译期计算支持 | 语言关键字 |
| **typeof** | 类型推断 | 语言关键字 |
| **位精确整数** | `_BitInt(N)` 类型 | 语言特性 |
| **位操作** | 标准位操作函数 | `<stdbit.h>` |
| **整数溢出检查** | 安全算术运算 | `<stdckdint.h>` |
| **内存清除** | 敏感数据安全擦除 | `<string.h>` |

### 标准版本宏

```c
// C23 标准版本标识
#define __STDC_VERSION__ 202311L

// 各头文件版本宏
#define __STDC_VERSION_STDBIT_H__ 202311L
#define __STDC_VERSION_STDCKDINT_H__ 202311L
```

---

## 新增头文件

### `<stdbit.h>` - 位和字节操作 (C23)

提供可移植的位操作函数，替代编译器特定扩展。

```c
#include <stdbit.h>

// 端序检测
#if __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_LITTLE__
    // 小端系统
#elif __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__
    // 大端系统
#endif

// 位计数函数
unsigned int count = stdc_count_ones(0b10101111);  // 返回 6
unsigned int zeros = stdc_leading_zeros(0x08);      // 返回 28 (32位系统)

// 位宽度计算
unsigned int width = stdc_bit_width(255);  // 返回 8
unsigned int ceil = stdc_bit_ceil(17);     // 返回 32 (大于等于17的最小2的幂)
```

**完整函数列表**:

| 函数 | 描述 |
|:-----|:-----|
| `stdc_leading_zeros` | 计算前导零位数 |
| `stdc_leading_ones` | 计算前导1位数 |
| `stdc_trailing_zeros` | 计算尾随零位数 |
| `stdc_trailing_ones` | 计算尾随1位数 |
| `stdc_first_leading_zero` | 查找第一个前导零位置 |
| `stdc_first_leading_one` | 查找第一个前导1位置 |
| `stdc_first_trailing_zero` | 查找第一个尾随零位置 |
| `stdc_first_trailing_one` | 查找第一个尾随1位置 |
| `stdc_count_zeros` | 计算零位总数 |
| `stdc_count_ones` (popcount) | 计算1位总数 |
| `stdc_has_single_bit` | 检查是否为2的幂 |
| `stdc_bit_width` | 计算表示值所需位数 |
| `stdc_bit_floor` | 不大于值的最大2的幂 |
| `stdc_bit_ceil` | 不小于值的最小2的幂 |

### `<stdckdint.h>` - 整数溢出检查 (C23)

提供安全的整数算术运算，检测溢出/下溢。

```c
#include <stdckdint.h>
#include <stdio.h>

int main(void) {
    int result;
    bool overflow = ckd_add(&result, INT_MAX, 1);

    if (overflow) {
        printf("加法溢出!\n");
    } else {
        printf("结果: %d\n", result);
    }

    return 0;
}
```

**核心宏**:

| 宏 | 描述 |
|:--|:----|
| `ckd_add(result, a, b)` | 安全加法，检测溢出 |
| `ckd_sub(result, a, b)` | 安全减法，检测溢出 |
| `ckd_mul(result, a, b)` | 安全乘法，检测溢出 |

---

## 更新的头文件

### `<string.h>` 新增函数

```c
// C23 新增：安全内存清除（防止优化消除）
void memset_explicit(void *s, int c, size_t n);

// 用法：安全擦除敏感数据
char password[64];
// ... 使用密码 ...
memset_explicit(password, 0, sizeof(password));  // 保证被擦除
```

### `<stdlib.h>` 新增函数

```c
// C23 新增（已广泛作为扩展支持）
char *strdup(const char *s);      // 分配并复制字符串
char *strndup(const char *s, size_t n);  // 分配并复制n个字符

// 内存拷贝扩展
void *memccpy(void *dest, const void *src, int c, size_t n);
```

### `<stdio.h>` 更新

```c
// C23 支持二进制输出格式
printf("%b\n", 42);   // 输出: 101010

// 数字分隔符
int x = 1'000'000;    // 更易读的大数字面量
```

---

## 弃用的特性

C23 正式弃用以下特性：

| 特性 | 替代方案 | 状态 |
|:-----|:---------|:-----|
| `<stdnoreturn.h>` | `[[noreturn]]` 属性 | 已弃用 |
| `<stdalign.h>` | `alignas`/`alignof` 关键字 | 已弃用 |
| `<stdbool.h>` | `bool`/`true`/`false` 关键字 | 已弃用 |
| `asctime()` | `asctime_s()` | 已弃用 |
| `ctime()` | `ctime_s()` | 已弃用 |

---

## 学习路径

### 初学者路径

1. 从 C17 基础开始
2. 学习 `nullptr` 和 `constexpr`
3. 掌握 `<stdbit.h>` 基本用法
4. 了解 `typeof` 类型推断

### 进阶路径

1. 深入 `<stdckdint.h>` 安全算术
2. 掌握 `_BitInt` 位精确整数
3. 学习属性语法 `[[...]]`
4. 理解内存安全改进

### 专家路径

1. 形式化语义分析
2. 编译器实现细节
3. 跨标准兼容性设计
4. 安全关键系统应用

---

## 参考资料

- [ISO/IEC 9899:2024 - C23 标准](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3220.pdf)
- [cppreference - C23](https://en.cppreference.com/w/c/23)
- [Modern C Programming - Springer](https://www.springer.com/gp/book/9783031453601)

---

> **最后更新**: 2026-03-13 | **状态**: 完整 | **C23 合规**: ✅
