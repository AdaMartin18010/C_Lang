# Rust与C的FFI互操作与安全对比指南

> 本文档全面介绍Rust与C语言之间的FFI（Foreign Function Interface）互操作机制，深入对比两种语言的内存安全模型，并提供从实际迁移策略到完整项目示例的全面指导。

## 目录

- [Rust与C的FFI互操作与安全对比指南](#rust与c的ffi互操作与安全对比指南)
  - [目录](#目录)
  - [1. Rust语言概述与C的对比](#1-rust语言概述与c的对比)
    - [1.1 语言设计理念对比](#11-语言设计理念对比)
    - [1.2 代码风格对比](#12-代码风格对比)
    - [1.3 编译模型对比](#13-编译模型对比)
  - [2. 内存安全模型对比](#2-内存安全模型对比)
    - [2.1 C语言的内存模型](#21-c语言的内存模型)
    - [2.2 Rust的所有权系统](#22-rust的所有权系统)
    - [2.3 内存安全对比总结](#23-内存安全对比总结)
    - [2.4 FFI边界的安全挑战](#24-ffi边界的安全挑战)
  - [3. FFI基础](#3-ffi基础)
    - [3.1 C ABI与调用约定](#31-c-abi与调用约定)
    - [3.2 #\[no\_mangle\]属性](#32-no_mangle属性)
    - [3.3 类型映射](#33-类型映射)
    - [3.4 repr(C)属性](#34-reprc属性)
  - [4. 从C调用Rust函数](#4-从c调用rust函数)
    - [4.1 基本步骤](#41-基本步骤)
    - [4.2 完整示例：Rust库](#42-完整示例rust库)
    - [4.3 C端调用代码](#43-c端调用代码)
  - [5. 从Rust调用C函数](#5-从rust调用c函数)
    - [5.1 基本步骤](#51-基本步骤)
    - [5.2 完整示例：Rust调用C库](#52-完整示例rust调用c库)
  - [6. 复杂数据类型传递](#6-复杂数据类型传递)
    - [6.1 结构体传递](#61-结构体传递)
    - [6.2 联合体（Union）处理](#62-联合体union处理)
  - [7. 回调函数处理](#7-回调函数处理)
    - [7.1 Rust调用C的回调](#71-rust调用c的回调)
    - [7.2 C调用Rust的回调](#72-c调用rust的回调)
  - [8. 错误处理对比](#8-错误处理对比)
    - [8.1 C语言的错误处理模式](#81-c语言的错误处理模式)
    - [8.2 Rust的错误处理模式](#82-rust的错误处理模式)
    - [8.3 错误处理对比表](#83-错误处理对比表)
  - [9. 并发安全对比](#9-并发安全对比)
    - [9.1 C语言的并发编程](#91-c语言的并发编程)
    - [9.2 Rust的并发安全](#92-rust的并发安全)
    - [9.3 并发安全对比表](#93-并发安全对比表)
  - [10. 逐步迁移策略](#10-逐步迁移策略)
    - [10.1 迁移策略概述](#101-迁移策略概述)
    - [10.2 具体迁移模式](#102-具体迁移模式)
    - [10.3 迁移检查清单](#103-迁移检查清单)
    - [10.4 风险管理](#104-风险管理)
  - [11. Cargo与Make/CMake集成](#11-cargo与makecmake集成)
    - [11.1 Cargo作为构建工具](#111-cargo作为构建工具)
    - [11.2 Make与Cargo集成](#112-make与cargo集成)
    - [11.3 CMake与Cargo集成](#113-cmake与cargo集成)
    - [11.4 混合项目结构](#114-混合项目结构)
  - [12. 实际项目：混合项目完整示例](#12-实际项目混合项目完整示例)
    - [12.1 项目概述](#121-项目概述)
    - [12.2 完整项目代码](#122-完整项目代码)
  - [13. 性能对比基准测试](#13-性能对比基准测试)
    - [13.1 基准测试设计](#131-基准测试设计)
    - [13.2 性能对比总结](#132-性能对比总结)
    - [13.3 FFI性能开销](#133-ffi性能开销)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. Rust语言概述与C的对比

### 1.1 语言设计理念对比

| 特性 | C语言 | Rust语言 |
|------|-------|----------|
| **诞生时间** | 1972年 | 2010年（1.0版2015年） |
| **设计哲学** | 信任程序员，提供最大灵活性 | 通过编译时检查保证安全 |
| **内存管理** | 手动管理 | 所有权系统+借用检查器 |
| **类型系统** | 静态弱类型 | 静态强类型+类型推断 |
| **空指针** | 允许 | 使用Option<T>替代 |
| **数据竞争** | 运行时检测 | 编译时禁止 |
| **包管理** | 无内置（依赖系统包管理器） | Cargo（内置） |
| **标准库** | 小巧精悍 | 丰富现代 |
| **编译目标** | 原生机器码 | LLVM IR（可编译到多种目标） |
| **FFI支持** | 原生C ABI | 通过extern关键字 |

### 1.2 代码风格对比

**C语言示例：**

```c
/* C语言: 计算两数之和的函数 */
#include <stdio.h>
#include <stdlib.h>

/* 函数声明 */
int add(int a, int b);

/* 结构体定义 */
typedef struct {
    int x;
    int y;
} Point;

/* 函数实现 */
int add(int a, int b) {
    return a + b;
}

/* 主函数 */
int main(void) {
    /* 变量声明 */
    int result;
    Point *p = malloc(sizeof(Point));

    /* 检查内存分配 */
    if (p == NULL) {
        fprintf(stderr, "内存分配失败\n");
        return 1;
    }

    /* 初始化 */
    p->x = 10;
    p->y = 20;

    /* 函数调用 */
    result = add(p->x, p->y);
    printf("结果: %d\n", result);

    /* 手动释放内存 */
    free(p);

    return 0;
}
```

**Rust等效代码：**

```rust
// Rust语言: 计算两数之和的函数

/// 表示二维点的结构体
#[derive(Debug)]
struct Point {
    x: i32,
    y: i32,
}

/// 计算两数之和的函数
///
/// # 参数
/// - `a`: 第一个加数
/// - `b`: 第二个加数
///
/// # 返回值
/// 两数之和
fn add(a: i32, b: i32) -> i32 {
    a + b  // 表达式自动返回，无需return
}

fn main() {
    // 使用堆分配（Box）
    let p = Box::new(Point { x: 10, y: 20 });

    // 函数调用
    let result = add(p.x, p.y);
    println!("结果: {}", result);

    // 内存自动释放，无需手动free
    // 当p离开作用域时，Box会自动释放内存
}
```

### 1.3 编译模型对比

**C编译流程：**

```
源代码 (.c) → 预处理 → 编译 → 汇编 → 链接 → 可执行文件
     ↓           ↓        ↓       ↓       ↓
   gcc -E     gcc -S    gcc -c   ld     ./a.out
```

**Rust编译流程：**

```
源代码 (.rs) → 词法分析 → HIR → MIR → LLVM IR → 目标代码 → 链接
     ↓                                       ↓
   rustc                                cargo build
```

Rust使用LLVM作为后端，因此可以与C代码共享相同的ABI（应用程序二进制接口），这是实现FFI互操作的基础。

---

## 2. 内存安全模型对比

### 2.1 C语言的内存模型

C语言将内存管理完全交给程序员，提供最大的灵活性但代价是安全风险。

**C内存管理示例：**

```c
/* C语言内存管理示例 - 展示了常见内存安全问题 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 内存泄漏示例 */
void memory_leak_example(void) {
    /* 分配内存但未释放 */
    char *buffer = malloc(1024);
    if (buffer == NULL) return;

    strcpy(buffer, "这段内存永远不会被释放");
    /* 函数返回时buffer丢失，造成内存泄漏 */
}

/* 悬垂指针示例 */
void dangling_pointer_example(void) {
    char *dangling;

    {
        char local[] = "局部变量";
        dangling = local;  /* 指向局部变量 */
    } /* local在这里被销毁 */

    /* 危险！dangling现在是悬垂指针 */
    printf("%s\n", dangling);  /* 未定义行为 */
}

/* 缓冲区溢出示例 */
void buffer_overflow_example(void) {
    char buffer[10];
    /* 危险：字符串长度超过缓冲区 */
    strcpy(buffer, "这是一个非常长的字符串，会溢出缓冲区");
}

/* 双重释放示例 */
void double_free_example(void) {
    char *ptr = malloc(100);
    if (ptr == NULL) return;

    free(ptr);
    /* 危险：重复释放同一内存 */
    free(ptr);  /* 未定义行为 */
}

/* 使用已释放内存示例 */
void use_after_free_example(void) {
    int *data = malloc(sizeof(int) * 10);
    if (data == NULL) return;

    data[0] = 42;
    free(data);

    /* 危险：访问已释放的内存 */
    printf("%d\n", data[0]);  /* 未定义行为 */
}
```

### 2.2 Rust的所有权系统

Rust通过所有权（Ownership）、借用（Borrowing）和生命周期（Lifetimes）三大机制在编译时保证内存安全。

**Rust所有权示例：**

```rust
// Rust所有权系统示例

fn main() {
    // ===== 所有权转移 =====
    {
        let s1 = String::from("Rust所有权");
        let s2 = s1;  // 所有权从s1转移到s2

        // println!("{}", s1);  // 错误！s1不再有效
        println!("{}", s2);     // 正确：s2拥有数据
    } // s2在这里被销毁，内存自动释放

    // ===== 借用（Borrowing） =====
    {
        let s = String::from("借用示例");

        // 不可变借用
        let len = calculate_length(&s);
        println!("'{}' 的长度是 {}", s, len);  // s仍然有效

        // 可变借用
        let mut s_mut = String::from("可变");
        change(&mut s_mut);
        println!("修改后: {}", s_mut);
    }

    // ===== 生命周期确保引用有效 =====
    {
        let r;
        {
            let x = 5;
            // r = &x;  // 错误！x的生命周期不够长
        }
        // println!("r: {}", r);  // 编译错误

        let y = 10;
        r = &y;  // 正确：r和y有相同的生命周期
        println!("r: {}", r);
    }
}

/// 接受不可变引用，不获取所有权
fn calculate_length(s: &String) -> usize {
    s.len()  // 返回长度，但不消耗s
}

/// 接受可变引用，可以修改数据
fn change(s: &mut String) {
    s.push_str(" 借用成功");
}
```

### 2.3 内存安全对比总结

| 安全问题 | C语言 | Rust |
|----------|-------|------|
| **空指针解引用** | 运行时崩溃（可能） | 编译错误（Option<T>强制处理） |
| **悬垂指针** | 允许，未定义行为 | 编译错误（借用检查器阻止） |
| **缓冲区溢出** | 允许，安全漏洞 | 编译错误（边界检查）或运行时panic |
| **数据竞争** | 允许，难以调试 | 编译错误（借用规则阻止） |
| **内存泄漏** | 可能发生 | 可能发生（但更安全） |
| **双重释放** | 允许，未定义行为 | 编译错误（所有权系统阻止） |
| **使用已释放内存** | 允许，安全漏洞 | 编译错误（生命周期检查） |

### 2.4 FFI边界的安全挑战

当Rust与C通过FFI互操作时，Rust的内存安全保证在边界处暂时失效。这是因为C代码不受Rust编译器控制。

```rust
// FFI边界的安全挑战示例
use std::os::raw::c_char;
use std::ffi::CStr;

extern "C" {
    // 从C库导入的函数
    fn c_function_returns_pointer() -> *const c_char;
    fn c_function_takes_pointer(ptr: *const c_char);
}

/// 安全封装：将C指针转换为Rust字符串
///
/// # 安全性
/// 这个函数是unsafe的，因为：
/// 1. 我们不知道C函数返回的指针是否有效
/// 2. 我们不知道指针指向的数据生命周期有多长
/// 3. 数据可能不是有效的UTF-8
pub unsafe fn safe_wrapper() -> Option<String> {
    // 调用C函数（unsafe块）
    let ptr = unsafe { c_function_returns_pointer() };

    // 检查空指针
    if ptr.is_null() {
        return None;
    }

    // 转换为CStr（unsafe，但比原始指针安全）
    let c_str = unsafe { CStr::from_ptr(ptr) };

    // 转换为Rust String
    c_str.to_str().ok().map(|s| s.to_string())
}
```

---

## 3. FFI基础

### 3.1 C ABI与调用约定

ABI（Application Binary Interface）定义了函数如何在二进制层面被调用，包括：

- 参数传递方式（寄存器/栈）
- 返回值处理
- 栈清理责任
- 名称修饰（Name Mangling）规则

**Rust支持的调用约定：**

```rust
// Rust支持多种调用约定
extern "C" fn c_calling_convention() {}
extern "stdcall" fn stdcall_convention() {}  // Windows API
extern "fastcall" fn fastcall_convention() {}
extern "system" fn system_convention() {}     // 平台默认系统调用
extern "cdecl" fn cdecl_convention() {}       // C默认

// 默认Rust调用约定（不稳定，可能变化）
fn rust_convention() {}
```

### 3.2 #[no_mangle]属性

Rust编译器默认会对函数名进行修饰（Name Mangling）以支持函数重载等特性。为了与C代码链接，需要禁用名称修饰。

```rust
// Rust端：导出函数给C使用

/// 这个函数名会被编译器修改，不适合FFI
pub fn rust_function() {}

/// 这个函数名保持原样，C代码可以通过这个名字找到它
#[no_mangle]
pub extern "C" fn exported_to_c() {
    println!("从C调用的Rust函数");
}

/// 带参数的导出函数
#[no_mangle]
pub extern "C" fn add_numbers(a: i32, b: i32) -> i32 {
    a + b
}
```

对应的C头文件：

```c
/* C头文件：rust_functions.h */
#ifndef RUST_FUNCTIONS_H
#define RUST_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* 声明从Rust导出的函数 */
void exported_to_c(void);
int add_numbers(int a, int b);

#ifdef __cplusplus
}
#endif

#endif /* RUST_FUNCTIONS_H */
```

### 3.3 类型映射

Rust与C之间的类型映射是FFI的基础：

| C类型 | Rust类型 | 说明 |
|-------|----------|------|
| `void` | `()` | 单元类型 |
| `char` | `c_char` | 有符号/无符号取决于平台 |
| `short` | `c_short` | 16位有符号整数 |
| `int` | `c_int` | 32位有符号整数 |
| `long` | `c_long` | 32或64位，取决于平台 |
| `long long` | `c_longlong` | 64位有符号整数 |
| `unsigned char` | `c_uchar` | 8位无符号整数 |
| `unsigned short` | `c_ushort` | 16位无符号整数 |
| `unsigned int` | `c_uint` | 32位无符号整数 |
| `unsigned long` | `c_ulong` | 32或64位无符号 |
| `float` | `c_float` | 32位浮点数 |
| `double` | `c_double` | 64位浮点数 |
| `void*` | `*mut c_void` | 通用可变指针 |
| `const void*` | `*const c_void` | 通用常量指针 |
| `char*` | `*mut c_char` | C字符串（可变） |
| `const char*` | `*const c_char` | C字符串（常量） |
| `size_t` | `usize` | 无符号指针大小整数 |
| `ptrdiff_t` | `isize` | 有符号指针大小整数 |

**完整类型映射示例：**

```rust
// Rust FFI类型映射完整示例
use std::os::raw::*;

extern "C" {
    // 基本整数类型
    fn c_int_function(i: c_int) -> c_int;
    fn c_long_function(l: c_long) -> c_long;
    fn c_short_function(s: c_short) -> c_short;

    // 无符号整数
    fn c_uint_function(u: c_uint) -> c_uint;
    fn c_ulong_function(ul: c_ulong) -> c_ulong;

    // 浮点数
    fn c_float_function(f: c_float) -> c_float;
    fn c_double_function(d: c_double) -> c_double;

    // 字符和字符串
    fn c_char_function(c: c_char) -> c_char;
    fn c_string_function(s: *const c_char) -> *const c_char;

    // 指针类型
    fn c_void_pointer(ptr: *mut c_void) -> *mut c_void;
    fn c_const_pointer(ptr: *const c_void) -> *const c_void;

    // 平台相关类型
    fn c_size_t_function(sz: usize) -> usize;
    fn c_ptrdiff_t_function(pd: isize) -> isize;
}
```

### 3.4 repr(C)属性

为了让Rust的结构体与C兼容，需要使用`#[repr(C)]`属性：

```rust
// 使用#[repr(C)]确保布局与C兼容

/// 这个结构体的内存布局与C兼容
#[repr(C)]
pub struct Point {
    pub x: f64,
    pub y: f64,
}

/// C风格的结构体，包含指针
#[repr(C)]
pub struct Person {
    pub name: *const c_char,  // C字符串指针
    pub age: c_int,
}

/// 带有数组的C兼容结构体
#[repr(C)]
pub struct Buffer {
    pub data: [u8; 1024],  // 固定大小数组
    pub len: usize,
}

/// 带有枚举的结构体
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub enum Status {
    Ok = 0,
    Error = 1,
    Warning = 2,
}

#[repr(C)]
pub struct Result {
    pub status: Status,
    pub value: c_double,
}
```

对应的C结构体：

```c
/* 对应的C结构体定义 */
#ifndef RUST_STRUCTS_H
#define RUST_STRUCTS_H

/* 点结构体 */
typedef struct {
    double x;
    double y;
} Point;

/* 人员结构体 */
typedef struct {
    const char *name;
    int age;
} Person;

/* 缓冲区结构体 */
typedef struct {
    unsigned char data[1024];
    size_t len;
} Buffer;

/* 枚举 */
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_WARNING = 2
} Status;

/* 结果结构体 */
typedef struct {
    Status status;
    double value;
} Result;

#endif
```

---

## 4. 从C调用Rust函数

### 4.1 基本步骤

从C调用Rust函数的完整流程：

1. **创建Rust库项目**
2. **设置crate类型为cdylib或staticlib**
3. **使用`#[no_mangle]`和`extern "C"`导出函数**
4. **编译Rust库**
5. **在C中链接并调用**

### 4.2 完整示例：Rust库

**Cargo.toml配置：**

```toml
[package]
name = "rust_math_lib"
version = "0.1.0"
edition = "2021"

[lib]
# cdylib: 生成动态链接库（.so/.dll/.dylib）
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# staticlib: 生成静态链接库（.a/.lib）
crate-type = ["cdylib", "staticlib"]

[dependencies]
```

**src/lib.rs：**

```rust
//! Rust数学库 - 导出给C使用
//!
//! 这个库演示了如何从C调用Rust函数

use std::os::raw::{c_char, c_int, c_double};
use std::ffi::{CStr, CString};
use std::ptr;

// ============================================
// 基本数学函数
// ============================================

/// 计算两个整数的和
///
/// # 参数
/// * `a` - 第一个整数
/// * `b` - 第二个整数
///
/// # 返回值
/// 两数之和
#[no_mangle]
pub extern "C" fn rust_add(a: c_int, b: c_int) -> c_int {
    a + b
}

/// 计算两个浮点数的乘积
#[no_mangle]
pub extern "C" fn rust_multiply(a: c_double, b: c_double) -> c_double {
    a * b
}

/// 计算阶乘
///
/// # 安全性
/// 对于大数可能溢出，但Rust会在调试模式下检查
#[no_mangle]
pub extern "C" fn rust_factorial(n: c_int) -> c_int {
    if n < 0 {
        return -1;  // 错误：负数阶乘
    }

    let mut result = 1i64;
    for i in 1..=n as i64 {
        result *= i;
    }

    // 检查溢出
    if result > c_int::MAX as i64 {
        return -1;  // 溢出错误
    }

    result as c_int
}

// ============================================
// 字符串处理
// ============================================

/// 处理C字符串并返回结果
///
/// # 参数
/// * `input` - 输入的C字符串（以null结尾）
///
/// # 返回值
/// 新分配的C字符串，调用者负责释放
///
/// # 安全性
/// 这是一个unsafe函数，因为涉及原始指针操作
#[no_mangle]
pub extern "C" fn rust_process_string(input: *const c_char) -> *mut c_char {
    // 检查空指针
    if input.is_null() {
        return ptr::null_mut();
    }

    // 安全地转换C字符串
    let c_str = unsafe { CStr::from_ptr(input) };

    // 转换为Rust字符串
    let str_slice = match c_str.to_str() {
        Ok(s) => s,
        Err(_) => return ptr::null_mut(),  // 无效的UTF-8
    };

    // 处理字符串（转换为大写）
    let processed = str_slice.to_uppercase();

    // 添加前缀
    let result = format!("[PROCESSED BY RUST] {}", processed);

    // 转换回C字符串
    match CString::new(result) {
        Ok(c_string) => c_string.into_raw(),  // 转移所有权给C
        Err(_) => ptr::null_mut(),
    }
}

/// 释放由Rust分配的字符串
///
/// # 安全性
/// 这个函数是unsafe的，因为：
/// 1. 指针必须由rust_process_string返回
/// 2. 指针不能被释放两次
/// 3. 释放后指针不能再被使用
#[no_mangle]
pub unsafe extern "C" fn rust_free_string(s: *mut c_char) {
    if !s.is_null() {
        // 重新获取CString的所有权，然后让它自动drop
        let _ = CString::from_raw(s);
    }
}

// ============================================
// 数据结构操作
// ============================================

/// 与C兼容的点结构体
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct Point2D {
    pub x: c_double,
    pub y: c_double,
}

/// 计算两点之间的距离
#[no_mangle]
pub extern "C" fn rust_point_distance(p1: Point2D, p2: Point2D) -> c_double {
    let dx = p2.x - p1.x;
    let dy = p2.y - p1.y;

    (dx * dx + dy * dy).sqrt()
}

/// 计算向量长度
#[no_mangle]
pub extern "C" fn rust_vector_magnitude(p: *const Point2D) -> c_double {
    // 检查空指针
    if p.is_null() {
        return -1.0;  // 错误指示
    }

    // 安全地解引用指针
    let point = unsafe { &*p };

    (point.x * point.x + point.y * point.y).sqrt()
}

/// 批量处理点数组
///
/// # 参数
/// * `points` - 点数组指针
/// * `count` - 数组元素数量
/// * `results` - 结果数组指针（由调用者分配）
///
/// # 返回值
/// 成功处理的元素数量
#[no_mangle]
pub extern "C" fn rust_process_points(
    points: *const Point2D,
    count: usize,
    results: *mut c_double,
) -> usize {
    // 参数验证
    if points.is_null() || results.is_null() || count == 0 {
        return 0;
    }

    // 安全地创建切片
    let points_slice = unsafe { std::slice::from_raw_parts(points, count) };
    let results_slice = unsafe { std::slice::from_raw_parts_mut(results, count) };

    // 处理每个点
    for (i, point) in points_slice.iter().enumerate() {
        results_slice[i] = (point.x * point.x + point.y * point.y).sqrt();
    }

    count
}

// ============================================
// 状态管理和错误处理
// ============================================

/// 错误代码枚举
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub enum ErrorCode {
    Success = 0,
    NullPointer = 1,
    InvalidArgument = 2,
    OutOfMemory = 3,
    ComputationError = 4,
}

/// 计算结果结构体
#[repr(C)]
pub struct ComputeResult {
    pub error_code: ErrorCode,
    pub value: c_double,
    pub message: *const c_char,
}

/// 安全除法
///
/// # 参数
/// * `numerator` - 分子
/// * `denominator` - 分母
/// * `result` - 结果输出参数
///
/// # 返回值
/// 错误代码
#[no_mangle]
pub extern "C" fn rust_safe_divide(
    numerator: c_double,
    denominator: c_double,
    result: *mut c_double,
) -> ErrorCode {
    // 检查输出参数
    if result.is_null() {
        return ErrorCode::NullPointer;
    }

    // 检查除零
    if denominator == 0.0 {
        return ErrorCode::InvalidArgument;
    }

    // 执行计算
    unsafe {
        *result = numerator / denominator;
    }

    ErrorCode::Success
}

/// 获取错误消息（静态字符串，不需要释放）
#[no_mangle]
pub extern "C" fn rust_error_message(code: ErrorCode) -> *const c_char {
    let message: &'static [u8] = match code {
        ErrorCode::Success => b"操作成功完成\0",
        ErrorCode::NullPointer => b"空指针错误\0",
        ErrorCode::InvalidArgument => b"无效参数\0",
        ErrorCode::OutOfMemory => b"内存不足\0",
        ErrorCode::ComputationError => b"计算错误\0",
    };

    message.as_ptr() as *const c_char
}

// ============================================
// 内存管理工具
// ============================================

/// 分配指定大小的内存块
///
/// # 参数
/// * `size` - 所需字节数
///
/// # 返回值
/// 分配的内存指针，失败返回NULL
#[no_mangle]
pub extern "C" fn rust_alloc(size: usize) -> *mut u8 {
    if size == 0 {
        return ptr::null_mut();
    }

    // 使用Vec分配内存
    let mut buffer = vec![0u8; size];
    let ptr = buffer.as_mut_ptr();

    // 防止Vec被drop（内存泄漏故意）
    // 实际上我们会使用Box或特定分配器
    std::mem::forget(buffer);

    ptr
}

/// 释放由rust_alloc分配的内存
///
/// # 安全性
/// 指针必须由rust_alloc返回，且只能释放一次
#[no_mangle]
pub unsafe extern "C" fn rust_free(ptr: *mut u8, size: usize) {
    if !ptr.is_null() && size > 0 {
        // 重新构建Vec以便正确释放
        let _ = Vec::from_raw_parts(ptr, 0, size);
    }
}
```

### 4.3 C端调用代码

**rust_math_lib.h：**

```c
/* Rust数学库C头文件 */
#ifndef RUST_MATH_LIB_H
#define RUST_MATH_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* ============================================
   类型定义
   ============================================ */

/* 点结构体 */
typedef struct {
    double x;
    double y;
} Point2D;

/* 错误代码 */
typedef enum {
    SUCCESS = 0,
    NULL_POINTER = 1,
    INVALID_ARGUMENT = 2,
    OUT_OF_MEMORY = 3,
    COMPUTATION_ERROR = 4
} ErrorCode;

/* ============================================
   基本数学函数
   ============================================ */

/* 计算两个整数的和 */
int rust_add(int a, int b);

/* 计算两个浮点数的乘积 */
double rust_multiply(double a, double b);

/* 计算阶乘 */
int rust_factorial(int n);

/* ============================================
   字符串处理
   ============================================ */

/* 处理字符串并返回新字符串 */
char* rust_process_string(const char* input);

/* 释放Rust分配的字符串 */
void rust_free_string(char* s);

/* ============================================
   数据结构操作
   ============================================ */

/* 计算两点之间的距离 */
double rust_point_distance(Point2D p1, Point2D p2);

/* 计算向量长度 */
double rust_vector_magnitude(const Point2D* p);

/* 批量处理点数组 */
size_t rust_process_points(
    const Point2D* points,
    size_t count,
    double* results
);

/* ============================================
   错误处理
   ============================================ */

/* 安全除法 */
ErrorCode rust_safe_divide(
    double numerator,
    double denominator,
    double* result
);

/* 获取错误消息 */
const char* rust_error_message(ErrorCode code);

/* ============================================
   内存管理
   ============================================ */

/* 分配内存 */
unsigned char* rust_alloc(size_t size);

/* 释放内存 */
void rust_free(unsigned char* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* RUST_MATH_LIB_H */
```

**main.c：**

```c
/* C程序：调用Rust数学库 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rust_math_lib.h"

/* 测试基本数学函数 */
void test_basic_math(void) {
    printf("\n=== 基本数学函数测试 ===\n");

    /* 加法 */
    int sum = rust_add(10, 20);
    printf("rust_add(10, 20) = %d\n", sum);

    /* 乘法 */
    double product = rust_multiply(3.14, 2.0);
    printf("rust_multiply(3.14, 2.0) = %.4f\n", product);

    /* 阶乘 */
    int fact = rust_factorial(5);
    printf("rust_factorial(5) = %d\n", fact);

    /* 错误情况 */
    int bad_fact = rust_factorial(-1);
    printf("rust_factorial(-1) = %d (错误)\n", bad_fact);
}

/* 测试字符串处理 */
void test_string_processing(void) {
    printf("\n=== 字符串处理测试 ===\n");

    const char* input = "Hello, FFI World!";
    char* result = rust_process_string(input);

    if (result != NULL) {
        printf("输入: %s\n", input);
        printf("输出: %s\n", result);

        /* 重要：释放Rust分配的内存 */
        rust_free_string(result);
    } else {
        printf("字符串处理失败\n");
    }

    /* 测试空指针 */
    char* null_result = rust_process_string(NULL);
    if (null_result == NULL) {
        printf("正确处理空指针输入\n");
    }
}

/* 测试数据结构 */
void test_data_structures(void) {
    printf("\n=== 数据结构测试 ===\n");

    /* 测试点距离 */
    Point2D p1 = {0.0, 0.0};
    Point2D p2 = {3.0, 4.0};
    double dist = rust_point_distance(p1, p2);
    printf("点(%.1f, %.1f)到点(%.1f, %.1f)的距离 = %.4f\n",
           p1.x, p1.y, p2.x, p2.y, dist);

    /* 测试向量长度 */
    Point2D p = {3.0, 4.0};
    double mag = rust_vector_magnitude(&p);
    printf("向量(%.1f, %.1f)的长度 = %.4f\n", p.x, p.y, mag);

    /* 测试空指针处理 */
    double null_mag = rust_vector_magnitude(NULL);
    printf("空指针向量的长度 = %.1f (错误指示)\n", null_mag);
}

/* 测试批量处理 */
void test_batch_processing(void) {
    printf("\n=== 批量处理测试 ===\n");

    /* 创建点数组 */
    Point2D points[] = {
        {3.0, 4.0},
        {5.0, 12.0},
        {8.0, 15.0},
        {7.0, 24.0}
    };
    size_t count = sizeof(points) / sizeof(points[0]);

    /* 分配结果数组 */
    double* results = malloc(count * sizeof(double));
    if (results == NULL) {
        printf("内存分配失败\n");
        return;
    }

    /* 调用Rust函数批量处理 */
    size_t processed = rust_process_points(points, count, results);
    printf("处理了 %zu/%zu 个点\n", processed, count);

    /* 输出结果 */
    for (size_t i = 0; i < processed; i++) {
        printf("点[%zu] (%5.1f, %5.1f) 的模 = %6.4f\n",
               i, points[i].x, points[i].y, results[i]);
    }

    free(results);
}

/* 测试错误处理 */
void test_error_handling(void) {
    printf("\n=== 错误处理测试 ===\n");

    double result;
    ErrorCode code;

    /* 正常除法 */
    code = rust_safe_divide(10.0, 2.0, &result);
    printf("10.0 / 2.0 = %.2f (代码: %d, %s)\n",
           result, code, rust_error_message(code));

    /* 除零错误 */
    code = rust_safe_divide(10.0, 0.0, &result);
    printf("10.0 / 0.0 错误 (代码: %d, %s)\n",
           code, rust_error_message(code));

    /* 空指针错误 */
    code = rust_safe_divide(10.0, 2.0, NULL);
    printf("空指针结果 (代码: %d, %s)\n",
           code, rust_error_message(code));

    /* 所有错误消息 */
    printf("\n所有错误消息:\n");
    for (int i = 0; i <= 4; i++) {
        printf("  %d: %s\n", i, rust_error_message((ErrorCode)i));
    }
}

/* 主函数 */
int main(void) {
    printf("Rust数学库C调用示例\n");
    printf("====================\n");

    test_basic_math();
    test_string_processing();
    test_data_structures();
    test_batch_processing();
    test_error_handling();

    printf("\n所有测试完成!\n");
    return 0;
}
```

---

## 5. 从Rust调用C函数

### 5.1 基本步骤

从Rust调用C函数的流程：

1. **创建Rust项目**
2. **声明C函数的extern块**
3. **使用build.rs或链接属性指定库**
4. **安全封装C函数**
5. **调用**

### 5.2 完整示例：Rust调用C库

假设我们有一个C数学库`libcmath.a`：

**c_math_lib.h：**

```c
/* C数学库头文件 */
#ifndef C_MATH_LIB_H
#define C_MATH_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* 基本数学运算 */
double c_add(double a, double b);
double c_subtract(double a, double b);
double c_multiply(double a, double b);
double c_divide(double a, double b, int* error);

/* 统计函数 */
double c_mean(const double* values, int count);
double c_variance(const double* values, int count);
double c_standard_deviation(const double* values, int count);

/* 几何函数 */
double c_distance_2d(double x1, double y1, double x2, double y2);
double c_distance_3d(double x1, double y1, double z1,
                     double x2, double y2, double z2);

/* 字符串工具 */
char* c_reverse_string(const char* str);
void c_free_string(char* str);

/* 内存分配器 */
void* c_allocate(size_t size);
void c_deallocate(void* ptr);

/* 回调函数类型 */
typedef double (*OperationFunc)(double, double);

/* 使用回调的函数 */
double c_apply_operation(double a, double b, OperationFunc op);

#ifdef __cplusplus
}
#endif

#endif /* C_MATH_LIB_H */
```

**c_math_lib.c：**

```c
/* C数学库实现 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "c_math_lib.h"

/* 基本数学运算 */
double c_add(double a, double b) {
    return a + b;
}

double c_subtract(double a, double b) {
    return a - b;
}

double c_multiply(double a, double b) {
    return a * b;
}

double c_divide(double a, double b, int* error) {
    if (error) *error = 0;

    if (b == 0.0) {
        if (error) *error = 1;
        return 0.0;
    }

    return a / b;
}

/* 统计函数 */
double c_mean(const double* values, int count) {
    if (count <= 0 || values == NULL) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += values[i];
    }

    return sum / count;
}

double c_variance(const double* values, int count) {
    if (count <= 1 || values == NULL) {
        return 0.0;
    }

    double mean = c_mean(values, count);
    double sum_squared_diff = 0.0;

    for (int i = 0; i < count; i++) {
        double diff = values[i] - mean;
        sum_squared_diff += diff * diff;
    }

    return sum_squared_diff / (count - 1);
}

double c_standard_deviation(const double* values, int count) {
    return sqrt(c_variance(values, count));
}

/* 几何函数 */
double c_distance_2d(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

double c_distance_3d(double x1, double y1, double z1,
                     double x2, double y2, double z2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dz = z2 - z1;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

/* 字符串工具 */
char* c_reverse_string(const char* str) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = strlen(str);
    char* reversed = (char*)malloc(len + 1);

    if (reversed == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        reversed[i] = str[len - 1 - i];
    }
    reversed[len] = '\0';

    return reversed;
}

void c_free_string(char* str) {
    free(str);
}

/* 内存分配器 */
void* c_allocate(size_t size) {
    return malloc(size);
}

void c_deallocate(void* ptr) {
    free(ptr);
}

/* 回调函数 */
double c_apply_operation(double a, double b, OperationFunc op) {
    if (op == NULL) {
        return 0.0;
    }
    return op(a, b);
}
```

**Rust端代码：**

**Cargo.toml：**

```toml
[package]
name = "rust_caller"
version = "0.1.0"
edition = "2021"

[dependencies]

[build-dependencies]
cc = "1.0"  # 用于编译C代码
```

**build.rs：**

```rust
// build.rs - 构建脚本，用于编译和链接C库
use std::env;
use std::path::PathBuf;

fn main() {
    // 获取项目目录
    let project_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());

    // 编译C代码为静态库
    cc::Build::new()
        .file("c_src/c_math_lib.c")
        .include("c_src")
        .compile("c_math_lib");

    // 链接数学库
    println!("cargo:rustc-link-lib=m");

    // 如果C代码变化，重新构建
    println!("cargo:rerun-if-changed=c_src/c_math_lib.c");
    println!("cargo:rerun-if-changed=c_src/c_math_lib.h");
}
```

**src/main.rs：**

```rust
//! Rust调用C数学库示例
//!
//! 演示如何安全地从Rust调用C函数

use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_double, c_int, c_void};

// ============================================
// FFI声明 - 告诉Rust C函数的存在
// ============================================

/// C数学库的FFI接口
///
/// 注意：这个块中的所有函数都是unsafe的
extern "C" {
    // 基本数学运算
    fn c_add(a: c_double, b: c_double) -> c_double;
    fn c_subtract(a: c_double, b: c_double) -> c_double;
    fn c_multiply(a: c_double, b: c_double) -> c_double;
    fn c_divide(a: c_double, b: c_double, error: *mut c_int) -> c_double;

    // 统计函数
    fn c_mean(values: *const c_double, count: c_int) -> c_double;
    fn c_variance(values: *const c_double, count: c_int) -> c_double;
    fn c_standard_deviation(values: *const c_double, count: c_int) -> c_double;

    // 几何函数
    fn c_distance_2d(x1: c_double, y1: c_double, x2: c_double, y2: c_double) -> c_double;
    fn c_distance_3d(
        x1: c_double, y1: c_double, z1: c_double,
        x2: c_double, y2: c_double, z2: c_double
    ) -> c_double;

    // 字符串工具
    fn c_reverse_string(str: *const c_char) -> *mut c_char;
    fn c_free_string(str: *mut c_char);

    // 内存分配器
    fn c_allocate(size: usize) -> *mut c_void;
    fn c_deallocate(ptr: *mut c_void);

    // 回调函数
    type OperationFunc = unsafe extern "C" fn(c_double, c_double) -> c_double;
    fn c_apply_operation(a: c_double, b: c_double, op: OperationFunc) -> c_double;
}

// ============================================
// 安全封装层
// ============================================

/// C数学库的安全Rust接口
pub mod safe_c_math {
    use super::*;

    /// 错误类型
    #[derive(Debug, Clone, Copy, PartialEq)]
    pub enum MathError {
        DivisionByZero,
        InvalidInput,
        NullPointer,
    }

    impl std::fmt::Display for MathError {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            match self {
                MathError::DivisionByZero => write!(f, "除零错误"),
                MathError::InvalidInput => write!(f, "无效输入"),
                MathError::NullPointer => write!(f, "空指针"),
            }
        }
    }

    impl std::error::Error for MathError {}

    /// 安全加法
    pub fn add(a: f64, b: f64) -> f64 {
        unsafe { c_add(a, b) }
    }

    /// 安全减法
    pub fn subtract(a: f64, b: f64) -> f64 {
        unsafe { c_subtract(a, b) }
    }

    /// 安全乘法
    pub fn multiply(a: f64, b: f64) -> f64 {
        unsafe { c_multiply(a, b) }
    }

    /// 安全除法，返回Result类型
    pub fn divide(a: f64, b: f64) -> Result<f64, MathError> {
        if b == 0.0 {
            return Err(MathError::DivisionByZero);
        }

        let mut error: c_int = 0;
        let result = unsafe { c_divide(a, b, &mut error) };

        if error != 0 {
            Err(MathError::DivisionByZero)
        } else {
            Ok(result)
        }
    }

    /// 计算平均值
    pub fn mean(values: &[f64]) -> Result<f64, MathError> {
        if values.is_empty() {
            return Err(MathError::InvalidInput);
        }

        let result = unsafe { c_mean(values.as_ptr(), values.len() as c_int) };
        Ok(result)
    }

    /// 计算方差
    pub fn variance(values: &[f64]) -> Result<f64, MathError> {
        if values.len() < 2 {
            return Err(MathError::InvalidInput);
        }

        let result = unsafe { c_variance(values.as_ptr(), values.len() as c_int) };
        Ok(result)
    }

    /// 计算标准差
    pub fn standard_deviation(values: &[f64]) -> Result<f64, MathError> {
        if values.len() < 2 {
            return Err(MathError::InvalidInput);
        }

        let result = unsafe { c_standard_deviation(values.as_ptr(), values.len() as c_int) };
        Ok(result)
    }

    /// 计算2D距离
    pub fn distance_2d(x1: f64, y1: f64, x2: f64, y2: f64) -> f64 {
        unsafe { c_distance_2d(x1, y1, x2, y2) }
    }

    /// 计算3D距离
    pub fn distance_3d(x1: f64, y1: f64, z1: f64, x2: f64, y2: f64, z2: f64) -> f64 {
        unsafe { c_distance_3d(x1, y1, z1, x2, y2, z2) }
    }

    /// 反转字符串
    pub fn reverse_string(input: &str) -> Result<String, MathError> {
        // 转换为C字符串
        let c_input = CString::new(input).map_err(|_| MathError::InvalidInput)?;

        // 调用C函数
        let c_result = unsafe { c_reverse_string(c_input.as_ptr()) };

        if c_result.is_null() {
            return Err(MathError::NullPointer);
        }

        // 转换为Rust字符串
        let result = unsafe {
            let c_str = CStr::from_ptr(c_result);
            let string_result = c_str.to_string_lossy().into_owned();

            // 释放C分配的内存
            c_free_string(c_result);

            string_result
        };

        Ok(result)
    }
}

// ============================================
// 回调函数示例
// ============================================

/// Rust回调函数，将被C调用
unsafe extern "C" fn rust_add_callback(a: c_double, b: c_double) -> c_double {
    a + b
}

unsafe extern "C" fn rust_max_callback(a: c_double, b: c_double) -> c_double {
    if a > b { a } else { b }
}

/// 使用回调的安全封装
pub fn apply_operation(a: f64, b: f64, op: fn(f64, f64) -> f64) -> f64 {
    // 注意：这里我们使用固定的C兼容回调
    // 在实际应用中，可能需要更复杂的处理
    let c_result = unsafe {
        c_apply_operation(a, b, rust_add_callback)
    };
    c_result
}

// ============================================
// 测试代码
// ============================================

fn main() {
    use safe_c_math::*;

    println!("Rust调用C数学库示例");
    println!("====================\n");

    // 基本运算测试
    println!("=== 基本运算 ===");
    println!("add(10.0, 20.0) = {}", add(10.0, 20.0));
    println!("subtract(30.0, 15.0) = {}", subtract(30.0, 15.0));
    println!("multiply(5.0, 6.0) = {}", multiply(5.0, 6.0));

    match divide(10.0, 2.0) {
        Ok(result) => println!("divide(10.0, 2.0) = {}", result),
        Err(e) => println!("除法错误: {}", e),
    }

    match divide(10.0, 0.0) {
        Ok(result) => println!("divide(10.0, 0.0) = {}", result),
        Err(e) => println!("除法错误（预期）: {}", e),
    }

    // 统计函数测试
    println!("\n=== 统计函数 ===");
    let data = vec![1.0, 2.0, 3.0, 4.0, 5.0];
    println!("数据: {:?}", data);

    match mean(&data) {
        Ok(result) => println!("平均值 = {:.4}", result),
        Err(e) => println!("错误: {}", e),
    }

    match variance(&data) {
        Ok(result) => println!("方差 = {:.4}", result),
        Err(e) => println!("错误: {}", e),
    }

    match standard_deviation(&data) {
        Ok(result) => println!("标准差 = {:.4}", result),
        Err(e) => println!("错误: {}", e),
    }

    // 几何函数测试
    println!("\n=== 几何函数 ===");
    println!("distance_2d(0,0到3,4) = {:.4}", distance_2d(0.0, 0.0, 3.0, 4.0));
    println!("distance_3d(0,0,0到1,1,1) = {:.4}", distance_3d(0.0, 0.0, 0.0, 1.0, 1.0, 1.0));

    // 字符串处理测试
    println!("\n=== 字符串处理 ===");
    match reverse_string("Hello, Rust!") {
        Ok(result) => println!("'Hello, Rust!' 反转 = '{}'", result),
        Err(e) => println!("错误: {}", e),
    }

    // 回调测试
    println!("\n=== 回调函数 ===");
    let callback_result = apply_operation(10.0, 20.0, |a, b| a + b);
    println!("通过回调调用加法: {}", callback_result);

    println!("\n所有测试完成!");
}
```

---

## 6. 复杂数据类型传递

### 6.1 结构体传递

**C端结构体和函数：**


```c
/* complex_types.h - C端复杂数据类型 */
#ifndef COMPLEX_TYPES_H
#define COMPLEX_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
   嵌套结构体
   ============================================ */

typedef struct {
    double real;
    double imag;
} Complex;

typedef struct {
    Complex top_left;
    Complex bottom_right;
} Rectangle;

typedef struct {
    Rectangle bounds;
    char* name;
    int id;
} Region;

/* ============================================
   动态数组结构
   ============================================ */

typedef struct {
    double* data;
    size_t capacity;
    size_t length;
} DoubleVector;

/* 向量操作 */
DoubleVector* vector_create(size_t initial_capacity);
void vector_destroy(DoubleVector* vec);
int vector_push(DoubleVector* vec, double value);
double vector_get(const DoubleVector* vec, size_t index);
int vector_set(DoubleVector* vec, size_t index, double value);

/* ============================================
   链表结构
   ============================================ */

typedef struct ListNode {
    int value;
    struct ListNode* next;
} ListNode;

typedef struct {
    ListNode* head;
    size_t length;
} LinkedList;

/* 链表操作 */
LinkedList* list_create(void);
void list_destroy(LinkedList* list);
int list_append(LinkedList* list, int value);
int list_prepend(LinkedList* list, int value);
int list_remove(LinkedList* list, int value);
size_t list_length(const LinkedList* list);
int* list_to_array(const LinkedList* list, size_t* count);

/* ============================================
   不透明指针（Opaque Pointer）模式
   ============================================ */

/* 隐藏实现细节的句柄 */
typedef struct Database* DatabaseHandle;
typedef struct Transaction* TransactionHandle;

/* 数据库操作 */
DatabaseHandle db_open(const char* path);
void db_close(DatabaseHandle db);
int db_execute(DatabaseHandle db, const char* sql);
TransactionHandle db_begin_transaction(DatabaseHandle db);
int db_commit(TransactionHandle tx);
void db_rollback(TransactionHandle tx);

#ifdef __cplusplus
}
#endif

#endif /* COMPLEX_TYPES_H */
```

**Rust端声明和封装：**

```rust
//! Rust与C复杂数据类型互操作

use std::os::raw::{c_char, c_double, c_int};
use std::ffi::{CStr, CString};
use std::ptr::{self, NonNull};

// ============================================
// FFI声明
// ============================================

/// 复数结构体（与C完全兼容）
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct Complex {
    pub real: c_double,
    pub imag: c_double,
}

/// 矩形结构体
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct Rectangle {
    pub top_left: Complex,
    pub bottom_right: Complex,
}

/// 区域结构体（包含指针）
#[repr(C)]
pub struct Region {
    pub bounds: Rectangle,
    pub name: *mut c_char,
    pub id: c_int,
}

/// 动态数组结构体（不透明指针）
#[repr(C)]
pub struct DoubleVector {
    pub data: *mut c_double,
    pub capacity: usize,
    pub length: usize,
}

/// 链表节点
#[repr(C)]
pub struct ListNode {
    pub value: c_int,
    pub next: *mut ListNode,
}

/// 链表结构体
#[repr(C)]
pub struct LinkedList {
    pub head: *mut ListNode,
    pub length: usize,
}

/// 不透明数据库句柄
#[repr(transparent)]
pub struct DatabaseHandle(pub *mut c_void);

/// 不透明事务句柄
#[repr(transparent)]
pub struct TransactionHandle(pub *mut c_void);

extern "C" {
    // 复数操作
    fn complex_add(a: Complex, b: Complex) -> Complex;
    fn complex_multiply(a: Complex, b: Complex) -> Complex;
    fn complex_magnitude(c: Complex) -> c_double;

    // 向量操作
    fn vector_create(initial_capacity: usize) -> *mut DoubleVector;
    fn vector_destroy(vec: *mut DoubleVector);
    fn vector_push(vec: *mut DoubleVector, value: c_double) -> c_int;
    fn vector_get(vec: *const DoubleVector, index: usize) -> c_double;
    fn vector_set(vec: *mut DoubleVector, index: usize, value: c_double) -> c_int;

    // 链表操作
    fn list_create() -> *mut LinkedList;
    fn list_destroy(list: *mut LinkedList);
    fn list_append(list: *mut LinkedList, value: c_int) -> c_int;
    fn list_prepend(list: *mut LinkedList, value: c_int) -> c_int;
    fn list_remove(list: *mut LinkedList, value: c_int) -> c_int;
    fn list_length(list: *const LinkedList) -> usize;
    fn list_to_array(list: *const LinkedList, count: *mut usize) -> *mut c_int;

    // 数据库操作（不透明指针）
    fn db_open(path: *const c_char) -> DatabaseHandle;
    fn db_close(db: DatabaseHandle);
    fn db_execute(db: DatabaseHandle, sql: *const c_char) -> c_int;
    fn db_begin_transaction(db: DatabaseHandle) -> TransactionHandle;
    fn db_commit(tx: TransactionHandle) -> c_int;
    fn db_rollback(tx: TransactionHandle);
}

// ============================================
// 安全Rust封装
// ============================================

/// 复数的安全封装
pub mod complex {
    use super::*;

    impl Complex {
        /// 创建新的复数
        pub fn new(real: f64, imag: f64) -> Self {
            Complex { real, imag }
        }

        /// 加法
        pub fn add(self, other: Complex) -> Complex {
            Complex {
                real: self.real + other.real,
                imag: self.imag + other.imag,
            }
        }

        /// 乘法
        pub fn multiply(self, other: Complex) -> Complex {
            Complex {
                real: self.real * other.real - self.imag * other.imag,
                imag: self.real * other.imag + self.imag * other.real,
            }
        }

        /// 模
        pub fn magnitude(self) -> f64 {
            (self.real * self.real + self.imag * self.imag).sqrt()
        }
    }

    impl std::fmt::Display for Complex {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            if self.imag >= 0.0 {
                write!(f, "{} + {}i", self.real, self.imag)
            } else {
                write!(f, "{} - {}i", self.real, -self.imag)
            }
        }
    }
}

/// 动态向量的安全封装（RAII模式）
pub struct Vector {
    ptr: NonNull<DoubleVector>,
}

impl Vector {
    /// 创建新向量
    pub fn new(initial_capacity: usize) -> Option<Self> {
        let ptr = unsafe { vector_create(initial_capacity) };
        NonNull::new(ptr).map(|p| Vector { ptr: p })
    }

    /// 添加元素
    pub fn push(&mut self, value: f64) -> Result<(), &'static str> {
        let result = unsafe { vector_push(self.ptr.as_ptr(), value) };
        if result == 0 {
            Ok(())
        } else {
            Err("添加元素失败")
        }
    }

    /// 获取元素
    pub fn get(&self, index: usize) -> Option<f64> {
        if index >= self.len() {
            return None;
        }
        Some(unsafe { vector_get(self.ptr.as_ptr(), index) })
    }

    /// 设置元素
    pub fn set(&mut self, index: usize, value: f64) -> Result<(), &'static str> {
        if index >= self.len() {
            return Err("索引越界");
        }
        let result = unsafe { vector_set(self.ptr.as_ptr(), index, value) };
        if result == 0 {
            Ok(())
        } else {
            Err("设置元素失败")
        }
    }

    /// 获取长度
    pub fn len(&self) -> usize {
        unsafe { (*self.ptr.as_ptr()).length }
    }

    /// 检查是否为空
    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    /// 转换为Vec
    pub fn to_vec(&self) -> Vec<f64> {
        let len = self.len();
        let mut result = Vec::with_capacity(len);
        for i in 0..len {
            if let Some(val) = self.get(i) {
                result.push(val);
            }
        }
        result
    }
}

impl Drop for Vector {
    fn drop(&mut self) {
        unsafe { vector_destroy(self.ptr.as_ptr()) }
    }
}

impl std::fmt::Debug for Vector {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_list().entries(self.to_vec()).finish()
    }
}

/// 链表的安全封装
pub struct List {
    ptr: NonNull<LinkedList>,
}

impl List {
    /// 创建新链表
    pub fn new() -> Option<Self> {
        let ptr = unsafe { list_create() };
        NonNull::new(ptr).map(|p| List { ptr: p })
    }

    /// 在末尾添加元素
    pub fn append(&mut self, value: i32) -> Result<(), &'static str> {
        let result = unsafe { list_append(self.ptr.as_ptr(), value) };
        if result == 0 {
            Ok(())
        } else {
            Err("添加元素失败")
        }
    }

    /// 在头部添加元素
    pub fn prepend(&mut self, value: i32) -> Result<(), &'static str> {
        let result = unsafe { list_prepend(self.ptr.as_ptr(), value) };
        if result == 0 {
            Ok(())
        } else {
            Err("添加元素失败")
        }
    }

    /// 移除元素
    pub fn remove(&mut self, value: i32) -> bool {
        let result = unsafe { list_remove(self.ptr.as_ptr(), value) };
        result == 0
    }

    /// 获取长度
    pub fn len(&self) -> usize {
        unsafe { list_length(self.ptr.as_ptr()) }
    }

    /// 转换为Vec
    pub fn to_vec(&self) -> Vec<i32> {
        let mut count: usize = 0;
        let arr = unsafe { list_to_array(self.ptr.as_ptr(), &mut count) };

        if arr.is_null() {
            return Vec::new();
        }

        let slice = unsafe { std::slice::from_raw_parts(arr, count) };
        let result = slice.to_vec();

        // 释放C分配的数组
        unsafe { libc::free(arr as *mut c_void) };

        result
    }
}

impl Drop for List {
    fn drop(&mut self) {
        unsafe { list_destroy(self.ptr.as_ptr()) }
    }
}

impl Default for List {
    fn default() -> Self {
        List::new().expect("创建链表失败")
    }
}

impl std::fmt::Debug for List {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_list().entries(self.to_vec()).finish()
    }
}

/// 数据库的安全封装（不透明指针）
pub struct Database {
    handle: DatabaseHandle,
}

impl Database {
    /// 打开数据库
    pub fn open(path: &str) -> Result<Self, &'static str> {
        let c_path = CString::new(path).map_err(|_| "无效路径")?;
        let handle = unsafe { db_open(c_path.as_ptr()) };

        if handle.0.is_null() {
            Err("打开数据库失败")
        } else {
            Ok(Database { handle })
        }
    }

    /// 执行SQL
    pub fn execute(&self, sql: &str) -> Result<(), &'static str> {
        let c_sql = CString::new(sql).map_err(|_| "无效SQL")?;
        let result = unsafe { db_execute(self.handle, c_sql.as_ptr()) };

        if result == 0 {
            Ok(())
        } else {
            Err("执行SQL失败")
        }
    }

    /// 开始事务
    pub fn begin_transaction(&self) -> Result<Transaction, &'static str> {
        let handle = unsafe { db_begin_transaction(self.handle) };

        if handle.0.is_null() {
            Err("开始事务失败")
        } else {
            Ok(Transaction {
                handle,
                db: self,
            })
        }
    }
}

impl Drop for Database {
    fn drop(&mut self) {
        unsafe { db_close(self.handle) }
    }
}

/// 事务的安全封装
pub struct Transaction<'db> {
    handle: TransactionHandle,
    db: &'db Database,
}

impl<'db> Transaction<'db> {
    /// 提交事务
    pub fn commit(self) -> Result<(), &'static str> {
        let result = unsafe { db_commit(self.handle) };

        // 转移所有权，防止double-free
        std::mem::forget(self);

        if result == 0 {
            Ok(())
        } else {
            Err("提交事务失败")
        }
    }

    /// 回滚事务
    pub fn rollback(self) {
        unsafe { db_rollback(self.handle) }
        // 转移所有权
        std::mem::forget(self);
    }
}

impl<'db> Drop for Transaction<'db> {
    fn drop(&mut self) {
        // 如果事务没有被显式提交或回滚，自动回滚
        unsafe { db_rollback(self.handle) }
    }
}

// ============================================
// 使用示例
// ============================================

fn main() {
    use complex::*;

    println!("复杂数据类型传递示例\n");

    // 复数运算
    println!("=== 复数运算 ===");
    let c1 = Complex::new(3.0, 4.0);
    let c2 = Complex::new(1.0, 2.0);
    println!("c1 = {}", c1);
    println!("c2 = {}", c2);
    println!("c1 + c2 = {}", c1.add(c2));
    println!("c1 * c2 = {}", c1.multiply(c2));
    println!("|c1| = {}", c1.magnitude());

    // 动态向量
    println!("\n=== 动态向量 ===");
    let mut vec = Vector::new(10).expect("创建向量失败");
    for i in 0..5 {
        vec.push(i as f64 * 1.5).expect("添加元素失败");
    }
    println!("向量: {:?}", vec);
    println!("vec[2] = {:?}", vec.get(2));
    vec.set(2, 100.0).expect("设置元素失败");
    println!("设置后 vec[2] = {:?}", vec.get(2));

    // 链表
    println!("\n=== 链表 ===");
    let mut list = List::new().expect("创建链表失败");
    list.append(10).unwrap();
    list.append(20).unwrap();
    list.prepend(5).unwrap();
    println!("链表: {:?}", list);
    list.remove(10);
    println!("移除10后: {:?}", list);

    println!("\n所有示例完成!");
}
```

### 6.2 联合体（Union）处理

**C端联合体：**

```c
/* union_example.h */
#ifndef UNION_EXAMPLE_H
#define UNION_EXAMPLE_H

#include <stdint.h>

typedef enum {
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOLEAN
} ValueType;

typedef struct {
    ValueType type;
    union {
        int64_t int_value;
        double float_value;
        char* string_value;
        int bool_value;
    } data;
} Value;

/* 构造函数 */
Value* value_create_integer(int64_t v);
Value* value_create_float(double v);
Value* value_create_string(const char* v);
Value* value_create_boolean(int v);

/* 析构函数 */
void value_destroy(Value* v);

/* 访问函数 */
int64_t value_get_integer(const Value* v);
double value_get_float(const Value* v);
const char* value_get_string(const Value* v);
int value_get_boolean(const Value* v);

/* 工具函数 */
void value_print(const Value* v);

#endif
```

**Rust端处理：**

```rust
//! Rust处理C联合体

use std::os::raw::{c_char, c_int};
use std::ffi::{CStr, CString};

// ============================================
// 类型定义
// ============================================

/// 值类型枚举
#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq)]
pub enum ValueType {
    Integer = 0,
    Float = 1,
    String = 2,
    Boolean = 3,
}

/// 联合体数据
#[repr(C)]
pub union ValueData {
    pub int_value: i64,
    pub float_value: f64,
    pub string_value: *mut c_char,
    pub bool_value: c_int,
}

/// 值结构体
#[repr(C)]
pub struct Value {
    pub type_: ValueType,
    pub data: ValueData,
}

extern "C" {
    fn value_create_integer(v: i64) -> *mut Value;
    fn value_create_float(v: f64) -> *mut Value;
    fn value_create_string(v: *const c_char) -> *mut Value;
    fn value_create_boolean(v: c_int) -> *mut Value;
    fn value_destroy(v: *mut Value);
    fn value_get_integer(v: *const Value) -> i64;
    fn value_get_float(v: *const Value) -> f64;
    fn value_get_string(v: *const Value) -> *const c_char;
    fn value_get_boolean(v: *const Value) -> c_int;
    fn value_print(v: *const Value);
}

// ============================================
// 安全Rust封装
// ============================================

/// 值的安全封装
pub struct ValueWrapper {
    ptr: *mut Value,
}

impl ValueWrapper {
    /// 创建整数
    pub fn new_integer(v: i64) -> Option<Self> {
        let ptr = unsafe { value_create_integer(v) };
        if ptr.is_null() {
            None
        } else {
            Some(ValueWrapper { ptr })
        }
    }

    /// 创建浮点数
    pub fn new_float(v: f64) -> Option<Self> {
        let ptr = unsafe { value_create_float(v) };
        if ptr.is_null() {
            None
        } else {
            Some(ValueWrapper { ptr })
        }
    }

    /// 创建字符串
    pub fn new_string(v: &str) -> Option<Self> {
        let c_str = CString::new(v).ok()?;
        let ptr = unsafe { value_create_string(c_str.as_ptr()) };
        if ptr.is_null() {
            None
        } else {
            Some(ValueWrapper { ptr })
        }
    }

    /// 创建布尔值
    pub fn new_boolean(v: bool) -> Option<Self> {
        let ptr = unsafe { value_create_boolean(v as c_int) };
        if ptr.is_null() {
            None
        } else {
            Some(ValueWrapper { ptr })
        }
    }

    /// 获取类型
    pub fn value_type(&self) -> ValueType {
        unsafe { (*self.ptr).type_ }
    }

    /// 获取整数值
    pub fn as_integer(&self) -> Option<i64> {
        if self.value_type() == ValueType::Integer {
            Some(unsafe { value_get_integer(self.ptr) })
        } else {
            None
        }
    }

    /// 获取浮点值
    pub fn as_float(&self) -> Option<f64> {
        if self.value_type() == ValueType::Float {
            Some(unsafe { value_get_float(self.ptr) })
        } else {
            None
        }
    }

    /// 获取字符串值
    pub fn as_string(&self) -> Option<String> {
        if self.value_type() == ValueType::String {
            let c_str = unsafe { value_get_string(self.ptr) };
            if c_str.is_null() {
                None
            } else {
                unsafe { CStr::from_ptr(c_str) }
                    .to_str()
                    .ok()
                    .map(|s| s.to_string())
            }
        } else {
            None
        }
    }

    /// 获取布尔值
    pub fn as_boolean(&self) -> Option<bool> {
        if self.value_type() == ValueType::Boolean {
            Some(unsafe { value_get_boolean(self.ptr) != 0 })
        } else {
            None
        }
    }

    /// 打印值
    pub fn print(&self) {
        unsafe { value_print(self.ptr) }
    }
}

impl Drop for ValueWrapper {
    fn drop(&mut self) {
        unsafe { value_destroy(self.ptr) }
    }
}

impl std::fmt::Debug for ValueWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self.value_type() {
            ValueType::Integer => write!(f, "Integer({:?})", self.as_integer()),
            ValueType::Float => write!(f, "Float({:?})", self.as_float()),
            ValueType::String => write!(f, "String({:?})", self.as_string()),
            ValueType::Boolean => write!(f, "Boolean({:?})", self.as_boolean()),
        }
    }
}

fn main() {
    println!("C联合体Rust封装示例\n");

    // 创建各种类型的值
    let int_val = ValueWrapper::new_integer(42).expect("创建失败");
    let float_val = ValueWrapper::new_float(3.14159).expect("创建失败");
    let string_val = ValueWrapper::new_string("Hello from Rust!").expect("创建失败");
    let bool_val = ValueWrapper::new_boolean(true).expect("创建失败");

    println!("整数值: {:?}", int_val);
    println!("浮点值: {:?}", float_val);
    println!("字符串值: {:?}", string_val);
    println!("布尔值: {:?}", bool_val);

    // 通过C函数打印
    println!("\n通过C函数打印:");
    int_val.print();
    float_val.print();
    string_val.print();
    bool_val.print();
}
```

---

## 7. 回调函数处理

### 7.1 Rust调用C的回调

```rust
//! Rust向C传递回调函数

use std::os::raw::{c_double, c_int, c_void};

// ============================================
// FFI声明
// ============================================

/// C回调函数类型
type CCallback = unsafe extern "C" fn(c_int, *mut c_void) -> c_int;

/// 另一个C回调函数类型（用于计算）
type CComputeCallback = unsafe extern "C" fn(c_double, *mut c_void) -> c_double;

extern "C" {
    /// 使用回调处理数组
    fn c_process_array(
        data: *const c_int,
        len: usize,
        callback: CCallback,
        user_data: *mut c_void,
    ) -> c_int;

    /// 使用回调过滤数组
    fn c_filter_array(
        data: *const c_double,
        len: usize,
        result: *mut c_double,
        predicate: unsafe extern "C" fn(c_double, *mut c_void) -> c_int,
        user_data: *mut c_void,
    ) -> usize;

    /// 迭代器风格的回调
    fn c_for_each(
        data: *const c_double,
        len: usize,
        callback: unsafe extern "C" fn(c_double, usize, *mut c_void),
        user_data: *mut c_void,
    );

    /// 排序使用比较函数
    fn c_sort_custom(
        data: *mut c_double,
        len: usize,
        compare: unsafe extern "C" fn(*const c_void, *const c_void) -> c_int,
    );
}

// ============================================
// 安全封装和示例
// ============================================

/// 处理数组，对每个元素应用闭包
pub fn process_array<F>(data: &[i32], mut f: F) -> Result<(), &'static str>
where
    F: FnMut(i32) -> i32,
{
    // 我们需要将闭包转换为C可调用的形式
    // 这需要一个trampoline函数

    struct CallbackData<'a, F: FnMut(i32) -> i32> {
        closure: &'a mut F,
    }

    unsafe extern "C" fn trampoline<F>(
        value: c_int,
        user_data: *mut c_void,
    ) -> c_int
    where
        F: FnMut(i32) -> i32,
    {
        let data = &mut *(user_data as *mut CallbackData<F>);
        (data.closure)(value as i32) as c_int
    }

    let mut callback_data = CallbackData { closure: &mut f };

    let result = unsafe {
        c_process_array(
            data.as_ptr(),
            data.len(),
            trampoline::<F>,
            &mut callback_data as *mut _ as *mut c_void,
        )
    };

    if result == 0 {
        Ok(())
    } else {
        Err("处理失败")
    }
}

/// 使用更高级的技术：Box<dyn FnMut>
pub struct CallbackRegistry {
    // 在实际实现中，这里可能需要全局存储或其他机制
}

/// 更实用的方法：使用泛型转换器
pub fn filter_array<F>(data: &[f64], predicate: F) -> Vec<f64>
where
    F: Fn(f64) -> bool,
{
    data.iter()
        .filter(|&&x| predicate(x))
        .copied()
        .collect()
}

/// 将Rust闭包转换为C回调（简化版）
pub fn for_each<F>(data: &[f64], mut f: F)
where
    F: FnMut(f64, usize),
{
    for (i, &value) in data.iter().enumerate() {
        f(value, i);
    }
}

// ============================================
// 使用示例
// ============================================

fn main() {
    println!("回调函数处理示例\n");

    // 简单闭包示例
    let data = vec![1.0, 2.0, 3.0, 4.0, 5.0];

    println!("原始数据: {:?}", data);

    // 过滤示例
    let filtered: Vec<f64> = filter_array(&data, |x| x > 2.0);
    println!("大于2.0的元素: {:?}", filtered);

    // for_each示例
    println!("\n迭代输出:");
    for_each(&data, |value, index| {
        println!("  [{}] = {}", index, value);
    });

    // 捕获环境的闭包
    let threshold = 3.0;
    let filtered2: Vec<f64> = filter_array(&data, |x| x > threshold);
    println!("\n大于{}的元素: {:?}", threshold, filtered2);
}
```

### 7.2 C调用Rust的回调

**Rust端（导出回调）：**

```rust
//! 导出Rust函数供C作为回调使用

use std::os::raw::{c_double, c_int, c_void};

// ============================================
// 导出函数（C可调用的回调）
// ============================================

/// 简单的回调函数：判断是否为偶数
#[no_mangle]
pub extern "C" fn rust_is_even(value: c_int, _user_data: *mut c_void) -> c_int {
    if value % 2 == 0 { 1 } else { 0 }
}

/// 回调函数：平方
#[no_mangle]
pub extern "C" fn rust_square(value: c_double, _user_data: *mut c_void) -> c_double {
    value * value
}

/// 回调函数：带状态的累加器
#[no_mangle]
pub extern "C" fn rust_accumulator(
    value: c_double,
    user_data: *mut c_void,
) -> c_double {
    if user_data.is_null() {
        return 0.0;
    }

    // 将user_data转换为累加器指针
    let accumulator = unsafe { &mut *(user_data as *mut c_double) };
    *accumulator += value;
    *accumulator
}

/// 回调函数：查找最大值
#[no_mangle]
pub extern "C" fn rust_max_finder(
    a: *const c_void,
    b: *const c_void,
) -> c_int {
    if a.is_null() || b.is_null() {
        return 0;
    }

    let a_val = unsafe { *(a as *const c_double) };
    let b_val = unsafe { *(b as *const c_double) };

    if a_val > b_val {
        -1
    } else if a_val < b_val {
        1
    } else {
        0
    }
}

/// 多用途处理回调
#[no_mangle]
pub extern "C" fn rust_processor(
    value: c_double,
    index: usize,
    user_data: *mut c_void,
) {
    if user_data.is_null() {
        return;
    }

    // 假设user_data指向一个处理上下文
    println!("Rust处理器: [{}] = {}", index, value);
}

// ============================================
// 注册和管理回调
// ============================================

/// 回调表结构（用于C注册Rust回调）
#[repr(C)]
pub struct CallbackTable {
    pub predicate: Option<unsafe extern "C" fn(c_int, *mut c_void) -> c_int>,
    pub transform: Option<unsafe extern "C" fn(c_double, *mut c_void) -> c_double>,
    pub compare: Option<unsafe extern "C" fn(*const c_void, *const c_void) -> c_int>,
}

/// 注册回调表
#[no_mangle]
pub extern "C" fn rust_register_callbacks(table: *mut CallbackTable) {
    if table.is_null() {
        return;
    }

    unsafe {
        (*table).predicate = Some(rust_is_even);
        (*table).transform = Some(rust_square);
        (*table).compare = Some(rust_max_finder);
    }
}

/// 使用注册的回调处理数据
#[no_mangle]
pub extern "C" fn rust_process_with_callbacks(
    data: *const c_double,
    len: usize,
    table: *const CallbackTable,
    results: *mut c_double,
) -> c_int {
    if data.is_null() || table.is_null() || results.is_null() || len == 0 {
        return -1;
    }

    unsafe {
        let data_slice = std::slice::from_raw_parts(data, len);
        let results_slice = std::slice::from_raw_parts_mut(results, len);

        if let Some(transform) = (*table).transform {
            for (i, &value) in data_slice.iter().enumerate() {
                results_slice[i] = transform(value, std::ptr::null_mut());
            }
        }
    }

    0
}
```

**C端使用：**

```c
/* callback_user.c - 使用Rust回调 */
#include <stdio.h>
#include <stdlib.h>

/* Rust导出的回调函数 */
extern int rust_is_even(int value, void* user_data);
extern double rust_square(double value, void* user_data);
extern int rust_max_finder(const void* a, const void* b);

/* 回调表结构 */
typedef struct {
    int (*predicate)(int, void*);
    double (*transform)(double, void*);
    int (*compare)(const void*, const void*);
} CallbackTable;

/* Rust函数 */
extern void rust_register_callbacks(CallbackTable* table);
extern int rust_process_with_callbacks(
    const double* data,
    size_t len,
    const CallbackTable* table,
    double* results
);

/* 使用Rust回调的C函数 */
void test_rust_callbacks(void) {
    printf("=== 测试Rust回调 ===\n");

    /* 注册回调 */
    CallbackTable table = {0};
    rust_register_callbacks(&table);

    /* 使用predicate回调 */
    printf("测试 rust_is_even:\n");
    for (int i = 0; i < 5; i++) {
        int result = rust_is_even(i, NULL);
        printf("  rust_is_even(%d) = %d\n", i, result);
    }

    /* 使用transform回调 */
    printf("\n测试 rust_square:\n");
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double results[5];

    rust_process_with_callbacks(data, 5, &table, results);
    printf("  原始: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", data[i]);
    printf("\n  平方: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", results[i]);
    printf("\n");

    /* 使用compare回调排序 */
    printf("\n测试 rust_max_finder (排序):\n");
    double unsorted[] = {3.5, 1.2, 4.8, 2.1, 5.0};
    printf("  排序前: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", unsorted[i]);

    qsort(unsorted, 5, sizeof(double), rust_max_finder);

    printf("\n  排序后: ");
    for (int i = 0; i < 5; i++) printf("%.1f ", unsorted[i]);
    printf("\n");
}

int main(void) {
    test_rust_callbacks();
    return 0;
}
```

---

## 8. 错误处理对比

### 8.1 C语言的错误处理模式

C语言通常使用以下几种错误处理方式：

```c
/* C语言错误处理模式示例 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* ============================================
   模式1: 返回错误码，输出参数返回结果
   ============================================ */

typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR = -1,
    ERR_INVALID_ARG = -2,
    ERR_OUT_OF_MEMORY = -3,
    ERR_OVERFLOW = -4,
    ERR_NOT_FOUND = -5
} ErrorCode;

/* 返回错误码，通过指针输出结果 */
ErrorCode safe_divide(int a, int b, int* result) {
    if (result == NULL) {
        return ERR_NULL_PTR;
    }
    if (b == 0) {
        return ERR_INVALID_ARG;
    }
    *result = a / b;
    return ERR_OK;
}

/* ============================================
   模式2: 返回NULL表示错误
   ============================================ */

char* create_buffer(size_t size) {
    char* buffer = malloc(size);
    if (buffer == NULL) {
        /* 设置全局错误码 */
        errno = ENOMEM;
        return NULL;
    }
    return buffer;
}

/* ============================================
   模式3: 返回特殊值表示错误
   ============================================ */

double safe_sqrt(double x) {
    if (x < 0) {
        return -1.0;  /* 特殊值表示错误 */
    }
    /* 实际计算... */
    return x;  /* 简化 */
}

/* ============================================
   模式4: 使用全局错误变量
   ============================================ */

static ErrorCode last_error = ERR_OK;
static char error_message[256] = {0};

void set_error(ErrorCode code, const char* msg) {
    last_error = code;
    if (msg != NULL) {
        strncpy(error_message, msg, sizeof(error_message) - 1);
        error_message[sizeof(error_message) - 1] = '\0';
    }
}

ErrorCode get_last_error(char* msg, size_t msg_size) {
    if (msg != NULL && msg_size > 0) {
        strncpy(msg, error_message, msg_size - 1);
        msg[msg_size - 1] = '\0';
    }
    return last_error;
}

/* ============================================
   模式5: goto错误处理
   ============================================ */

int complex_operation(void) {
    int result = -1;
    char* buffer1 = NULL;
    char* buffer2 = NULL;
    FILE* file = NULL;

    buffer1 = malloc(1024);
    if (buffer1 == NULL) {
        set_error(ERR_OUT_OF_MEMORY, "无法分配buffer1");
        goto cleanup;
    }

    buffer2 = malloc(2048);
    if (buffer2 == NULL) {
        set_error(ERR_OUT_OF_MEMORY, "无法分配buffer2");
        goto cleanup;
    }

    file = fopen("data.txt", "r");
    if (file == NULL) {
        set_error(ERR_NOT_FOUND, "无法打开文件");
        goto cleanup;
    }

    /* 执行操作... */
    result = 0;  /* 成功 */

cleanup:
    if (file != NULL) fclose(file);
    free(buffer2);
    free(buffer1);
    return result;
}

/* ============================================
   模式6: 错误跳转（setjmp/longjmp）
   ============================================ */

#include <setjmp.h>

static jmp_buf error_jump;
static ErrorCode jump_error_code;

void throw_error(ErrorCode code) {
    jump_error_code = code;
    longjmp(error_jump, 1);
}

int risky_operation(void) {
    if (setjmp(error_jump) != 0) {
        /* 错误处理 */
        printf("捕获到错误: %d\n", jump_error_code);
        return -1;
    }

    /* 可能抛出错误的操作 */
    char* ptr = malloc(100);
    if (ptr == NULL) {
        throw_error(ERR_OUT_OF_MEMORY);
    }

    free(ptr);
    return 0;
}
```

### 8.2 Rust的错误处理模式

Rust使用Result和Option类型进行类型安全的错误处理：

```rust
//! Rust错误处理模式

use std::error::Error;
use std::fmt;
use std::io;

// ============================================
// 自定义错误类型
// ============================================

#[derive(Debug)]
pub enum MathError {
    DivisionByZero,
    NegativeSquareRoot(f64),
    Overflow,
    InvalidInput(String),
}

impl fmt::Display for MathError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            MathError::DivisionByZero => write!(f, "除零错误"),
            MathError::NegativeSquareRoot(x) => {
                write!(f, "负数不能开平方: {}", x)
            }
            MathError::Overflow => write!(f, "数值溢出"),
            MathError::InvalidInput(msg) => write!(f, "无效输入: {}", msg),
        }
    }
}

impl Error for MathError {}

// ============================================
// Result<T, E> 使用示例
// ============================================

/// 安全除法，返回Result
pub fn safe_divide(a: f64, b: f64) -> Result<f64, MathError> {
    if b == 0.0 {
        Err(MathError::DivisionByZero)
    } else {
        Ok(a / b)
    }
}

/// 安全开方
pub fn safe_sqrt(x: f64) -> Result<f64, MathError> {
    if x < 0.0 {
        Err(MathError::NegativeSquareRoot(x))
    } else {
        Ok(x.sqrt())
    }
}

/// 链式错误传播
pub fn calculate_quadratic(a: f64, b: f64, c: f64) -> Result<(f64, f64), MathError> {
    let discriminant = b * b - 4.0 * a * c;
    let sqrt_disc = safe_sqrt(discriminant)?;  // ?操作符传播错误

    let x1 = (-b + sqrt_disc) / (2.0 * a);
    let x2 = (-b - sqrt_disc) / (2.0 * a);

    Ok((x1, x2))
}

// ============================================
// Option<T> 使用示例
// ============================================

/// 查找数组中的最大值
pub fn find_max(numbers: &[i32]) -> Option<&i32> {
    numbers.iter().max()
}

/// 使用Option链式操作
pub fn get_config_value(key: &str) -> Option<String> {
    // 模拟配置查找
    let configs = vec![
        ("host", "localhost"),
        ("port", "8080"),
    ];

    configs
        .iter()
        .find(|(k, _)| *k == key)
        .map(|(_, v)| v.to_string())
}

// ============================================
// FFI中的错误处理
// ============================================

use std::os::raw::{c_char, c_int};
use std::ffi::CStr;

/// C兼容的错误码
#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq)]
pub enum CErrorCode {
    Success = 0,
    NullPointer = -1,
    InvalidArg = -2,
    OutOfMemory = -3,
    ComputationError = -4,
}

/// 将Rust Result转换为C错误码
pub fn result_to_c_error<T>(result: Result<T, MathError>) -> CErrorCode {
    match result {
        Ok(_) => CErrorCode::Success,
        Err(e) => match e {
            MathError::DivisionByZero => CErrorCode::InvalidArg,
            MathError::NegativeSquareRoot(_) => CErrorCode::InvalidArg,
            MathError::Overflow => CErrorCode::ComputationError,
            MathError::InvalidInput(_) => CErrorCode::InvalidArg,
        },
    }
}

/// C可调用的安全除法
#[no_mangle]
pub extern "C" fn rust_safe_divide_ffi(
    a: f64,
    b: f64,
    result: *mut f64,
) -> CErrorCode {
    if result.is_null() {
        return CErrorCode::NullPointer;
    }

    match safe_divide(a, b) {
        Ok(val) => {
            unsafe { *result = val }
            CErrorCode::Success
        }
        Err(_) => CErrorCode::ComputationError,
    }
}

// ============================================
// 错误组合和处理多个错误
// ============================================

use std::collections::HashMap;

/// 组合多个可能失败的操作
pub fn process_data(inputs: &[f64]) -> Result<Vec<f64>, Vec<MathError>> {
    let mut results = Vec::new();
    let mut errors = Vec::new();

    for &input in inputs {
        match safe_sqrt(input) {
            Ok(val) => results.push(val),
            Err(e) => errors.push(e),
        }
    }

    if errors.is_empty() {
        Ok(results)
    } else {
        Err(errors)
    }
}

/// 使用anyhow风格（简化版）
pub type AnyResult<T> = Result<T, Box<dyn Error>>;

pub fn complex_operation() -> AnyResult<()> {
    let file = std::fs::File::open("config.txt")?;
    let data: f64 = "not_a_number".parse()?;
    let result = safe_divide(data, 0.0)?;

    println!("结果: {}", result);
    Ok(())
}

// ============================================
// 对比示例
// ============================================

fn main() {
    println!("Rust错误处理示例\n");

    // Result基本使用
    println!("=== Result基本使用 ===");
    match safe_divide(10.0, 2.0) {
        Ok(result) => println!("10 / 2 = {}", result),
        Err(e) => println!("错误: {}", e),
    }

    match safe_divide(10.0, 0.0) {
        Ok(result) => println!("10 / 0 = {}", result),
        Err(e) => println!("错误（预期）: {}", e),
    }

    // ?操作符和链式调用
    println!("\n=== 链式错误传播 ===");
    match calculate_quadratic(1.0, -5.0, 6.0) {
        Ok((x1, x2)) => println!("方程根: x1={}, x2={}", x1, x2),
        Err(e) => println!("错误: {}", e),
    }

    // Option使用
    println!("\n=== Option使用 ===");
    let numbers = vec![3, 1, 4, 1, 5, 9];
    match find_max(&numbers) {
        Some(max) => println!("最大值: {}", max),
        None => println!("空数组"),
    }

    // 处理多个错误
    println!("\n=== 处理多个错误 ===");
    let inputs = vec![4.0, -1.0, 9.0, -4.0, 16.0];
    match process_data(&inputs) {
        Ok(results) => println!("处理结果: {:?}", results),
        Err(errors) => {
            println!("遇到 {} 个错误:", errors.len());
            for e in errors {
                println!("  - {}", e);
            }
        }
    }

    // 对比：Rust vs C错误处理
    println!("\n=== Rust vs C错误处理对比 ===");
    println!("C语言:");
    println!("  - 使用返回值或全局变量传递错误信息");
    println!("  - 容易遗漏错误检查");
    println!("  - 运行时才能发现错误");
    println!("Rust:");
    println!("  - 使用Result<T, E>类型强制处理错误");
    println!("  - ?操作符简化错误传播");
    println!("  - 编译时保证错误被处理");
}
```

### 8.3 错误处理对比表

| 特性 | C语言 | Rust |
|------|-------|------|
| **错误类型** | 整数错误码、特殊返回值 | Result<T, E>枚举 |
| **错误传播** | 手动检查每个调用 | `?`操作符自动传播 |
| **错误组合** | 困难，通常用全局变量 | 直接组合多个Result |
| **可选值** | NULL指针、特殊值 | Option<T>类型 |
| **强制处理** | 否 | 是（必须解包或传播） |
| **错误信息** | 通常需要额外查找 | 类型中包含完整信息 |
| **性能开销** | 最小 | 零成本抽象 |
| **调试支持** | 运行时调试 | 编译时类型检查 |

---

## 9. 并发安全对比

### 9.1 C语言的并发编程

C语言标准不直接支持线程，通常使用POSIX线程（pthreads）或平台特定API：

```c
/* C语言并发示例（POSIX线程） */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* 共享数据 - 需要手动同步 */
int shared_counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/* 线程函数 */
void* increment_counter(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < 1000; i++) {
        /* 手动加锁 */
        pthread_mutex_lock(&counter_mutex);

        /* 临界区 */
        shared_counter++;

        /* 手动解锁 */
        pthread_mutex_unlock(&counter_mutex);
    }

    printf("线程 %d 完成\n", thread_id);
    return NULL;
}

/* 无锁（数据竞争示例） */
void* unsafe_increment(void* arg) {
    for (int i = 0; i < 1000; i++) {
        /* 危险！没有同步 */
        shared_counter++;
    }
    return NULL;
}

/* 条件变量示例 */
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
int ready = 0;

void* producer(void* arg) {
    sleep(1);  /* 模拟工作 */

    pthread_mutex_lock(&cond_mutex);
    ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&cond_mutex);

    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&cond_mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &cond_mutex);
    }
    pthread_mutex_unlock(&cond_mutex);

    printf("消费者收到信号\n");
    return NULL;
}

int main(void) {
    pthread_t threads[4];
    int thread_ids[4] = {0, 1, 2, 3};

    printf("C语言并发测试\n");
    printf("============\n");

    /* 创建线程 */
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, increment_counter, &thread_ids[i]);
    }

    /* 等待线程完成 */
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("最终计数器值: %d（预期: 4000）\n", shared_counter);

    /* 条件变量测试 */
    printf("\n条件变量测试:\n");
    pthread_t prod, cons;
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    pthread_join(cons, NULL);
    pthread_join(prod, NULL);

    /* 清理 */
    pthread_mutex_destroy(&counter_mutex);
    pthread_mutex_destroy(&cond_mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
```

### 9.2 Rust的并发安全

Rust通过所有权系统在编译时防止数据竞争：

```rust
//! Rust并发编程示例

use std::sync::{Arc, Mutex, RwLock, mpsc};
use std::thread;
use std::time::Duration;

// ============================================
// 基本线程创建
// ============================================

fn basic_threads() {
    println!("=== 基本线程 ===");

    let handle = thread::spawn(|| {
        for i in 1..=5 {
            println!("子线程: {}", i);
            thread::sleep(Duration::from_millis(10));
        }
    });

    for i in 1..=3 {
        println!("主线程: {}", i);
        thread::sleep(Duration::from_millis(10));
    }

    handle.join().unwrap();
}

// ============================================
// 使用Mutex进行同步
// ============================================

fn mutex_example() {
    println!("\n=== Mutex同步 ===");

    // Arc（原子引用计数）允许多个线程共享所有权
    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for i in 0..4 {
        let counter = Arc::clone(&counter);
        let handle = thread::spawn(move || {
            for _ in 0..1000 {
                // 自动加锁，退出作用域自动解锁
                let mut num = counter.lock().unwrap();
                *num += 1;
            }
            println!("线程 {} 完成", i);
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("最终计数器值: {}（预期: 4000）", *counter.lock().unwrap());
}

// ============================================
// 读写锁（RwLock）
// ============================================

fn rwlock_example() {
    println!("\n=== RwLock ===");

    let data = Arc::new(RwLock::new(vec![1, 2, 3]));

    // 多个读线程
    let mut read_handles = vec![];
    for i in 0..3 {
        let data = Arc::clone(&data);
        let handle = thread::spawn(move || {
            let values = data.read().unwrap();
            println!("读线程 {}: {:?}", i, *values);
        });
        read_handles.push(handle);
    }

    // 一个写线程
    let data_clone = Arc::clone(&data);
    let write_handle = thread::spawn(move || {
        let mut values = data_clone.write().unwrap();
        values.push(4);
        println!("写线程: 添加了4");
    });

    for handle in read_handles {
        handle.join().unwrap();
    }
    write_handle.join().unwrap();

    println!("最终数据: {:?}", *data.read().unwrap());
}

// ============================================
// 通道（Channel）通信
// ============================================

fn channel_example() {
    println!("\n=== 通道通信 ===");

    // 创建通道
    let (tx, rx) = mpsc::channel();

    // 生产者线程
    let tx1 = tx.clone();
    thread::spawn(move || {
        let vals = vec!["hello", "from", "thread"];
        for val in vals {
            tx1.send(val).unwrap();
            thread::sleep(Duration::from_millis(100));
        }
    });

    // 另一个生产者
    thread::spawn(move || {
        let vals = vec!["more", "messages"];
        for val in vals {
            tx.send(val).unwrap();
            thread::sleep(Duration::from_millis(100));
        }
    });

    // 主线程作为消费者
    drop(tx);  // 关闭发送端

    for received in rx {
        println!("收到: {}", received);
    }
}

// ============================================
// 编译时防止数据竞争
// ============================================

fn compile_time_safety() {
    println!("\n=== 编译时安全保证 ===");

    /* 以下代码如果取消注释，编译会失败：

    let data = vec![1, 2, 3];

    let handle = thread::spawn(|| {
        println!("{:?}", data);  // 错误：data的生命周期不够长
    });

    drop(data);  // 错误：数据已被move到线程

    handle.join().unwrap();
    */

    /* 正确的方式：使用Arc共享所有权 */
    let data = Arc::new(vec![1, 2, 3]);
    let data_clone = Arc::clone(&data);

    let handle = thread::spawn(move || {
        println!("线程中的数据: {:?}", data_clone);
    });

    println!("主线程中的数据: {:?}", data);
    handle.join().unwrap();

    /* 可变共享的错误尝试：

    let mut data = vec![1, 2, 3];

    let handle1 = thread::spawn(|| {
        data.push(4);  // 错误：不能在不同线程间共享可变引用
    });

    let handle2 = thread::spawn(|| {
        data.push(5);  // 错误：不能在不同线程间共享可变引用
    });
    */

    /* 正确的方式：使用Mutex */
    let data = Arc::new(Mutex::new(vec![1, 2, 3]));
    let data1 = Arc::clone(&data);
    let data2 = Arc::clone(&data);

    let handle1 = thread::spawn(move || {
        data1.lock().unwrap().push(4);
    });

    let handle2 = thread::spawn(move || {
        data2.lock().unwrap().push(5);
    });

    handle1.join().unwrap();
    handle2.join().unwrap();

    println!("修改后的数据: {:?}", *data.lock().unwrap());
}

// ============================================
// Send和Sync trait
// ============================================

/// 只有实现了Send的类型才能跨线程传递所有权
/// 只有实现了Sync的类型才能跨线程共享引用
fn trait_bounds() {
    println!("\n=== Send和Sync trait ===");

    // Rc不是Send，不能跨线程
    // let rc = std::rc::Rc::new(5);
    // let handle = thread::spawn(move || {
    //     println!("{}", rc);  // 编译错误
    // });

    // Arc是Send和Sync，可以跨线程
    let arc = Arc::new(5);
    let handle = thread::spawn(move || {
        println!("Arc值: {}", arc);
    });
    handle.join().unwrap();

    println!("Rc不是Send: 不能跨线程传递");
    println!("Arc是Send+Sync: 可以跨线程安全共享");
    println!("RefCell不是Sync: 不能跨线程共享引用");
    println!("Mutex<T>是Sync（如果T是Send）: 提供内部可变性");
}

// ============================================
// FFI中的并发考虑
// ============================================

use std::os::raw::c_void;

extern "C" {
    fn c_thread_safe_function(data: *mut c_void);
}

/// 安全地调用可能多线程的C函数
pub fn call_c_from_thread() {
    let data = Arc::new(Mutex::new(0));
    let data_clone = Arc::clone(&data);

    let handle = thread::spawn(move || {
        // 确保C函数调用是线程安全的
        // 如果需要，在调用前加锁
        let mut value = data_clone.lock().unwrap();
        // unsafe { c_thread_safe_function(&mut *value as *mut _ as *mut c_void) };
        *value += 1;
    });

    handle.join().unwrap();
    println!("值: {}", *data.lock().unwrap());
}

fn main() {
    println!("Rust并发编程示例\n");

    basic_threads();
    mutex_example();
    rwlock_example();
    channel_example();
    compile_time_safety();
    trait_bounds();
    call_c_from_thread();

    println!("\n=== Rust vs C并发对比 ===");
    println!("C语言:");
    println!("  - 数据竞争在编译时不被发现");
    println!("  - 手动管理锁，容易遗漏");
    println!("  - 运行时错误难以调试");
    println!("  - 死锁、竞态条件常见问题");
    println!("Rust:");
    println!("  - 编译时防止数据竞争");
    println!("  - 所有权系统自动管理同步");
    println!("  - Send/Sync trait保证线程安全");
    println!("  - 锁与数据绑定，防止忘记解锁");
}
```

### 9.3 并发安全对比表

| 特性 | C语言 | Rust |
|------|-------|------|
| **线程创建** | pthread_create | thread::spawn |
| **互斥锁** | pthread_mutex_t | std::sync::Mutex |
| **数据竞争检测** | 运行时（可能遗漏） | 编译时 |
| **死锁防护** | 无 | 无（但锁与数据绑定） |
| **忘记解锁** | 可能 | 不可能（RAII） |
| **跨线程数据** | 手动管理 | Send/Sync trait |
| **性能** | 原生 | 零成本抽象 |
| **错误发现时间** | 运行时 | 编译时 |

---

## 10. 逐步迁移策略

### 10.1 迁移策略概述

从C迁移到Rust是一个渐进过程，不是全有或全无的选择。以下是推荐的迁移策略：

```
迁移路线图:

Phase 1: 新功能用Rust开发
  ↓ 通过FFI与现有C代码集成
Phase 2: 识别并隔离关键模块
  ↓ 逐步用Rust重写
Phase 3: 核心库迁移
  ↓ 保持C API兼容
Phase 4: 完全迁移
  ↓ 可选：保留C API供外部使用
Phase 5: 纯Rust项目
```

### 10.2 具体迁移模式

**模式1：Rust库 + C主程序**

```rust
// 适用于：新功能开发，保持现有C代码不变
// Rust作为库，C程序调用

// lib.rs - 导出新功能
#[no_mangle]
pub extern "C" fn new_rust_feature(input: *const c_char) -> *mut c_char {
    // 实现新功能
}
```

**模式2：C库 + Rust主程序**

```rust
// 适用于：逐步替换C代码，Rust作为主程序
// 逐步将C函数迁移到Rust

// 继续使用C库，逐步重写
extern "C" {
    fn legacy_c_function();
}

fn main() {
    unsafe { legacy_c_function() };
}
```

**模式3：模块化迁移**

```rust
// 适用于：大型项目，按模块逐步迁移

// 定义清晰的模块边界
pub mod parser {
    // 先从parser模块开始
}

pub mod networking {
    // 然后是网络模块
}

pub mod storage {
    // 最后是存储模块
}
```

### 10.3 迁移检查清单

```markdown
## C到Rust迁移检查清单

### 准备工作
- [ ] 识别关键模块和依赖关系
- [ ] 建立CI/CD管道支持双语言构建
- [ ] 创建全面的测试套件（作为安全网）
- [ ] 准备回滚计划

### 第一阶段：建立边界
- [ ] 定义清晰的C API边界
- [ ] 为Rust代码创建FFI层
- [ ] 编写C头文件对应的Rust绑定
- [ ] 测试FFI边界的正确性

### 第二阶段：逐步迁移
- [ ] 选择风险较低的模块开始
- [ ] 保持原有C API不变
- [ ] 为Rust代码编写单元测试
- [ ] 集成测试验证功能等价性

### 第三阶段：优化和清理
- [ ] 移除不再需要的C代码
- [ ] 优化Rust API设计
- [ ] 添加Rust特有的功能（迭代器等）
- [ ] 文档更新

### 验证
- [ ] 功能测试通过
- [ ] 性能测试对比
- [ ] 内存安全测试
- [ ] 边界情况测试
```

### 10.4 风险管理

| 风险 | 缓解策略 |
|------|----------|
| **FFI边界错误** | 全面测试，使用sanitizer |
| **性能退化** | 基准测试，识别热点 |
| **编译时间增加** | 模块化，并行编译 |
| **团队学习曲线** | 培训，代码审查 |
| **依赖管理** | 使用cargo，文档化 |

---

## 11. Cargo与Make/CMake集成

### 11.1 Cargo作为构建工具

**Cargo.toml配置：**

```toml
[package]
name = "mixed_project"
version = "0.1.0"
edition = "2021"
authors = ["Your Name <you@example.com>"]

[lib]
name = "mixed_lib"
crate-type = ["staticlib", "cdylib", "rlib"]

[[bin]]
name = "mixed_app"
path = "src/main.rs"

[dependencies]
libc = "0.2"

[build-dependencies]
cc = "1.0"

[profile.release]
opt-level = 3
lto = true
```

**build.rs（构建脚本）：**

```rust
// build.rs - 编译C代码并链接
use std::env;
use std::path::PathBuf;

fn main() {
    // 获取项目目录
    let project_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let c_src_dir = project_dir.join("c_src");

    // 收集所有C源文件
    let c_files = vec![
        c_src_dir.join("math.c"),
        c_src_dir.join("utils.c"),
        c_src_dir.join("parser.c"),
    ];

    // 使用cc crate编译C代码
    let mut build = cc::Build::new();

    build
        .include(&c_src_dir)
        .include(c_src_dir.join("include"))
        .flag_if_supported("-Wall")
        .flag_if_supported("-Wextra")
        .flag_if_supported("-std=c11");

    // 添加所有C文件
    for file in &c_files {
        build.file(file);
    }

    build.compile("c_components");

    // 链接其他系统库
    println!("cargo:rustc-link-lib=m");
    println!("cargo:rustc-link-lib=pthread");

    // 重新构建触发条件
    for file in &c_files {
        println!("cargo:rerun-if-changed={}", file.display());
    }
    println!("cargo:rerun-if-changed={}", c_src_dir.join("include").display());
}
```

### 11.2 Make与Cargo集成

**Makefile示例：**

```makefile
# Makefile - 集成Cargo和Make

# 配置
RUST_TARGET_DIR := target
CARGO := cargo
RELEASE_DIR := $(RUST_TARGET_DIR)/release
DEBUG_DIR := $(RUST_TARGET_DIR)/debug

# 检测平台
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIB_EXT := .so
    STATIC_EXT := .a
endif
ifeq ($(UNAME_S),Darwin)
    LIB_EXT := .dylib
    STATIC_EXT := .a
endif
ifeq ($(OS),Windows_NT)
    LIB_EXT := .dll
    STATIC_EXT := .lib
endif

# 目标
.PHONY: all build release test clean install

all: build

# 调试构建
build:
 $(CARGO) build
 @echo "构建完成: $(DEBUG_DIR)"

# 发布构建
release:
 $(CARGO) build --release
 @echo "发布构建完成: $(RELEASE_DIR)"

# 运行测试
test:
 $(CARGO) test
 $(CARGO) test --release

# 运行示例
run: build
 $(CARGO) run

run-release: release
 $(CARGO) run --release

# 生成C头文件
headers:
 @echo "生成C头文件..."
 cbindgen --crate mixed_project --output include/mixed_lib.h

# 清理
clean:
 $(CARGO) clean
 rm -rf build/

# 安装
install: release
 install -d $(DESTDIR)/usr/lib
 install -d $(DESTDIR)/usr/include
 install $(RELEASE_DIR)/libmixed_lib$(STATIC_EXT) $(DESTDIR)/usr/lib/
 install include/*.h $(DESTDIR)/usr/include/

# 帮助
help:
 @echo "可用目标:"
 @echo "  make build     - 调试构建"
 @echo "  make release   - 发布构建"
 @echo "  make test      - 运行测试"
 @echo "  make run       - 运行程序"
 @echo "  make headers   - 生成C头文件"
 @echo "  make clean     - 清理构建文件"
 @echo "  make install   - 安装库和头文件"
```

### 11.3 CMake与Cargo集成

**CMakeLists.txt示例：**

```cmake
# CMakeLists.txt - 集成CMake和Cargo

cmake_minimum_required(VERSION 3.12)
project(MixedProject VERSION 0.1.0 LANGUAGES C CXX)

# 查找Cargo
find_program(CARGO cargo)
if(NOT CARGO)
    message(FATAL_ERROR "Cargo not found")
endif()

# 配置选项
set(RUST_BUILD_TYPE "release" CACHE STRING "Rust build type")
set_property(CACHE RUST_BUILD_TYPE PROPERTY STRINGS "debug" "release")

# Rust目标目录
set(RUST_TARGET_DIR ${CMAKE_BINARY_DIR}/rust)

# 添加自定义命令构建Rust代码
add_custom_command(
    OUTPUT ${RUST_TARGET_DIR}/${RUST_BUILD_TYPE}/libmixed_lib.a
    COMMAND ${CMAKE_COMMAND} -E env
        CARGO_TARGET_DIR=${RUST_TARGET_DIR}
        ${CARGO} build
        $<$<STREQUAL:${RUST_BUILD_TYPE},release>:--release>
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building Rust library"
    VERBATIM
)

# 创建导入库目标
add_library(rust_lib STATIC IMPORTED GLOBAL)
add_dependencies(rust_lib rust_lib_build)

set_target_properties(rust_lib PROPERTIES
    IMPORTED_LOCATION ${RUST_TARGET_DIR}/${RUST_BUILD_TYPE}/libmixed_lib.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/include
)

# 自定义目标
add_custom_target(rust_lib_build
    DEPENDS ${RUST_TARGET_DIR}/${RUST_BUILD_TYPE}/libmixed_lib.a
)

# C可执行文件
add_executable(c_app
    c_src/main.c
    c_src/app.c
)

target_link_libraries(c_app
    rust_lib
    pthread
    dl
    m
)

# 测试
enable_testing()
add_test(NAME rust_tests
    COMMAND ${CARGO} test --release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# 安装
install(TARGETS c_app
    RUNTIME DESTINATION bin
)

install(FILES ${RUST_TARGET_DIR}/${RUST_BUILD_TYPE}/libmixed_lib.a
    DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
```

### 11.4 混合项目结构

```
mixed_project/
├── Cargo.toml           # Rust项目配置
├── build.rs             # Rust构建脚本
├── Makefile             # Make构建配置
├── CMakeLists.txt       # CMake配置
├── src/
│   ├── lib.rs           # Rust库入口
│   ├── main.rs          # Rust可执行文件
│   └── ffi/
│       ├── mod.rs       # FFI模块
│       └── bindings.rs  # C绑定
├── c_src/
│   ├── include/
│   │   └── mixed_lib.h  # C头文件
│   ├── math.c           # C数学函数
│   ├── utils.c          # C工具函数
│   └── main.c           # C主程序
├── tests/
│   ├── rust_tests.rs    # Rust测试
│   └── c_tests.c        # C测试
└── examples/
    ├── rust_example.rs  # Rust示例
    └── c_example.c      # C示例
```

---

## 12. 实际项目：混合项目完整示例

### 12.1 项目概述

我们将创建一个混合的图像处理库，其中核心算法用Rust实现（利用内存安全），而I/O和界面用C实现。

### 12.2 完整项目代码

**Cargo.toml：**

```toml
[package]
name = "image_processor"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["cdylib", "staticlib"]

[dependencies]
libc = "0.2"

[build-dependencies]
cc = "1.0"

[[bin]]
name = "processor_cli"
path = "src/cli.rs"
```

**build.rs：**

```rust
use std::env;
use std::path::PathBuf;

fn main() {
    let project_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let c_src = project_dir.join("c_src");

    // 编译C I/O代码
    cc::Build::new()
        .file(c_src.join("io.c"))
        .file(c_src.join("format.c"))
        .include(c_src.join("include"))
        .compile("c_io");

    println!("cargo:rerun-if-changed=c_src/");
}
```

**src/lib.rs（Rust核心库）：**

```rust
//! 图像处理库 - Rust核心实现

use std::os::raw::{c_char, c_int, c_uchar};
use std::slice;

// ============================================
// 核心数据结构
// ============================================

/// 图像结构体（C兼容）
#[repr(C)]
pub struct Image {
    pub width: usize,
    pub height: usize,
    pub channels: usize,
    pub data: *mut c_uchar,
}

/// 处理选项
#[repr(C)]
pub struct ProcessOptions {
    pub brightness: f32,
    pub contrast: f32,
    pub blur_radius: f32,
    pub grayscale: bool,
}

// ============================================
// 核心处理函数
// ============================================

/// 调整亮度
///
/// # 安全性
/// 调用者必须确保image指针有效，且数据缓冲区足够大
#[no_mangle]
pub extern "C" fn img_adjust_brightness(image: *mut Image, factor: f32) -> c_int {
    if image.is_null() {
        return -1;
    }

    let img = unsafe { &mut *image };

    if img.data.is_null() || img.width == 0 || img.height == 0 {
        return -1;
    }

    let total_pixels = img.width * img.height * img.channels;
    let pixels = unsafe { slice::from_raw_parts_mut(img.data, total_pixels) };

    // Rust实现：安全的像素处理
    for pixel in pixels.iter_mut() {
        let adjusted = (*pixel as f32 * factor).min(255.0).max(0.0);
        *pixel = adjusted as c_uchar;
    }

    0
}

/// 转换为灰度
#[no_mangle]
pub extern "C" fn img_to_grayscale(image: *mut Image) -> c_int {
    if image.is_null() {
        return -1;
    }

    let img = unsafe { &mut *image };

    if img.channels < 3 {
        return -1;  // 已经是灰度或无效
    }

    let pixel_count = img.width * img.height;

    // 原地转换为灰度
    for i in 0..pixel_count {
        let idx = i * img.channels;
        let pixels = unsafe { slice::from_raw_parts_mut(img.data, pixel_count * img.channels) };

        let r = pixels[idx] as f32;
        let g = pixels[idx + 1] as f32;
        let b = pixels[idx + 2] as f32;

        // 标准灰度转换公式
        let gray = (0.299 * r + 0.587 * g + 0.114 * b) as c_uchar;

        pixels[idx] = gray;
        pixels[idx + 1] = gray;
        pixels[idx + 2] = gray;
    }

    0
}

/// 应用高斯模糊
#[no_mangle]
pub extern "C" fn img_gaussian_blur(image: *mut Image, radius: f32) -> c_int {
    if image.is_null() || radius <= 0.0 {
        return -1;
    }

    let img = unsafe { &mut *image };
    let (width, height, channels) = (img.width, img.height, img.channels);
    let pixel_count = width * height;

    // 创建临时缓冲区
    let mut temp_buffer = vec![0u8; pixel_count * channels];
    let src = unsafe { slice::from_raw_parts(img.data, pixel_count * channels) };

    // 简单的盒式模糊（简化版高斯模糊）
    let radius_int = radius as isize;

    for y in 0..height {
        for x in 0..width {
            for c in 0..channels {
                let mut sum = 0u32;
                let mut count = 0u32;

                for dy in -radius_int..=radius_int {
                    for dx in -radius_int..=radius_int {
                        let nx = (x as isize + dx).max(0).min((width - 1) as isize) as usize;
                        let ny = (y as isize + dy).max(0).min((height - 1) as isize) as usize;

                        let idx = (ny * width + nx) * channels + c;
                        sum += src[idx] as u32;
                        count += 1;
                    }
                }

                let idx = (y * width + x) * channels + c;
                temp_buffer[idx] = (sum / count) as c_uchar;
            }
        }
    }

    // 复制回原始缓冲区
    let dst = unsafe { slice::from_raw_parts_mut(img.data, pixel_count * channels) };
    dst.copy_from_slice(&temp_buffer);

    0
}

/// 批量处理多张图片
#[no_mangle]
pub extern "C" fn img_batch_process(
    images: *mut *mut Image,
    count: usize,
    options: *const ProcessOptions,
) -> c_int {
    if images.is_null() || options.is_null() || count == 0 {
        return -1;
    }

    let opts = unsafe { &*options };
    let img_ptrs = unsafe { slice::from_raw_parts(images, count) };

    for &img_ptr in img_ptrs {
        if img_ptr.is_null() {
            continue;
        }

        // 应用亮度调整
        if opts.brightness != 1.0 {
            img_adjust_brightness(img_ptr, opts.brightness);
        }

        // 应用对比度调整
        if opts.contrast != 1.0 {
            // 简化实现：调用亮度调整作为示例
            let _ = img_adjust_brightness(img_ptr, opts.contrast);
        }

        // 应用模糊
        if opts.blur_radius > 0.0 {
            img_gaussian_blur(img_ptr, opts.blur_radius);
        }

        // 转换为灰度
        if opts.grayscale {
            img_to_grayscale(img_ptr);
        }
    }

    0
}

/// 获取库版本
#[no_mangle]
pub extern "C" fn img_get_version() -> *const c_char {
    b"1.0.0\0".as_ptr() as *const c_char
}

/// 获取错误消息
#[no_mangle]
pub extern "C" fn img_get_error_message(code: c_int) -> *const c_char {
    match code {
        0 => b"Success\0".as_ptr(),
        -1 => b"Invalid argument\0".as_ptr(),
        -2 => b"Out of memory\0".as_ptr(),
        -3 => b"Unsupported format\0".as_ptr(),
        _ => b"Unknown error\0".as_ptr(),
    } as *const c_char
}
```

**c_src/include/image_processor.h：**

```c
/* image_processor.h - Rust图像处理库C接口 */
#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* 图像结构体 */
typedef struct {
    size_t width;
    size_t height;
    size_t channels;
    uint8_t* data;
} Image;

/* 处理选项 */
typedef struct {
    float brightness;
    float contrast;
    float blur_radius;
    int grayscale;
} ProcessOptions;

/* 核心处理函数 */
int img_adjust_brightness(Image* image, float factor);
int img_to_grayscale(Image* image);
int img_gaussian_blur(Image* image, float radius);
int img_batch_process(Image** images, size_t count, const ProcessOptions* options);

/* 工具函数 */
const char* img_get_version(void);
const char* img_get_error_message(int code);

/* C端辅助函数 */
Image* img_create(size_t width, size_t height, size_t channels);
void img_destroy(Image* image);
int img_load(const char* filename, Image** out_image);
int img_save(const char* filename, const Image* image);

#ifdef __cplusplus
}
#endif

#endif
```

**c_src/io.c（C端I/O实现）：**

```c
/* io.c - C端图像I/O实现 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

/* 简化的PPM图像加载 */
Image* img_create(size_t width, size_t height, size_t channels) {
    Image* img = malloc(sizeof(Image));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = calloc(width * height * channels, 1);

    if (!img->data) {
        free(img);
        return NULL;
    }

    return img;
}

void img_destroy(Image* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

int img_load(const char* filename, Image** out_image) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return -1;

    /* 简化的P6 PPM格式加载 */
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P6") != 0) {
        fclose(fp);
        return -1;
    }

    int width, height, maxval;
    if (fscanf(fp, "%d %d %d", &width, &height, &maxval) != 3) {
        fclose(fp);
        return -1;
    }

    fgetc(fp);  /* 消耗换行符 */

    Image* img = img_create(width, height, 3);
    if (!img) {
        fclose(fp);
        return -2;
    }

    if (fread(img->data, 1, width * height * 3, fp) != (size_t)(width * height * 3)) {
        img_destroy(img);
        fclose(fp);
        return -1;
    }

    fclose(fp);
    *out_image = img;
    return 0;
}

int img_save(const char* filename, const Image* image) {
    if (!image || !image->data) return -1;

    FILE* fp = fopen(filename, "wb");
    if (!fp) return -1;

    fprintf(fp, "P6\n%zu %zu\n255\n", image->width, image->height);
    fwrite(image->data, 1, image->width * image->height * image->channels, fp);

    fclose(fp);
    return 0;
}
```

**c_src/main.c（C主程序）：**

```c
/* main.c - 图像处理程序主入口 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_processor.h"

void print_usage(const char* program) {
    printf("Usage: %s <input.ppm> <output.ppm> [options]\n", program);
    printf("Options:\n");
    printf("  -b <factor>  Brightness adjustment (0.0-2.0, default: 1.0)\n");
    printf("  -g           Convert to grayscale\n");
    printf("  -r <radius>  Gaussian blur radius (default: 0.0)\n");
    printf("  -v           Show version\n");
    printf("  -h           Show help\n");
}

int main(int argc, char** argv) {
    printf("Image Processor v%s\n", img_get_version());
    printf("Powered by Rust + C\n\n");

    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    /* 解析选项 */
    ProcessOptions opts = {
        .brightness = 1.0f,
        .contrast = 1.0f,
        .blur_radius = 0.0f,
        .grayscale = 0
    };

    int input_idx = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            return 0;
        }
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            opts.brightness = atof(argv[++i]);
        }
        if (strcmp(argv[i], "-g") == 0) {
            opts.grayscale = 1;
        }
        if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            opts.blur_radius = atof(argv[++i]);
        }
    }

    /* 查找输入和输出文件 */
    if (argc < 3 || argv[argc-2][0] == '-' || argv[argc-1][0] == '-') {
        printf("Error: Missing input or output file\n");
        return 1;
    }

    const char* input_file = argv[argc-2];
    const char* output_file = argv[argc-1];

    /* 加载图像 */
    printf("Loading: %s\n", input_file);
    Image* image = NULL;
    int result = img_load(input_file, &image);

    if (result != 0) {
        printf("Error loading image: %s\n", img_get_error_message(result));
        return 1;
    }

    printf("Image: %zux%zu, %zu channels\n",
           image->width, image->height, image->channels);

    /* 应用处理 */
    printf("Processing...\n");

    Image* images[] = { image };
    result = img_batch_process(images, 1, &opts);

    if (result != 0) {
        printf("Error processing: %s\n", img_get_error_message(result));
        img_destroy(image);
        return 1;
    }

    /* 保存结果 */
    printf("Saving: %s\n", output_file);
    result = img_save(output_file, image);

    if (result != 0) {
        printf("Error saving image\n");
        img_destroy(image);
        return 1;
    }

    /* 清理 */
    img_destroy(image);

    printf("Done!\n");
    return 0;
}
```

**src/cli.rs（Rust CLI）：**

```rust
//! Rust CLI - 纯Rust实现的命令行工具

use std::env;
use std::process;

fn main() {
    println!("Image Processor CLI (Rust)");
    println!("Version: 1.0.0\n");

    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        print_usage(&args[0]);
        process::exit(1);
    }

    // 解析命令
    match args[1].as_str() {
        "--version" | "-v" => {
            println!("1.0.0");
        }
        "--help" | "-h" => {
            print_usage(&args[0]);
        }
        "process" => {
            if args.len() < 4 {
                eprintln!("Error: Missing input or output file");
                process::exit(1);
            }
            println!("Processing {} -> {}", args[2], args[3]);
            println!("This would call the Rust library functions");
        }
        _ => {
            eprintln!("Unknown command: {}", args[1]);
            print_usage(&args[0]);
            process::exit(1);
        }
    }
}

fn print_usage(program: &str) {
    println!("Usage: {} <command> [options]", program);
    println!();
    println!("Commands:");
    println!("  process <input> <output>  Process an image");
    println!("  --version, -v             Show version");
    println!("  --help, -h                Show help");
}
```

**tests/integration_tests.rs：**

```rust
//! 集成测试

#[cfg(test)]
mod tests {
    use std::os::raw::{c_int, c_uchar};
    use std::slice;

    // 模拟FFI调用
    #[repr(C)]
    struct TestImage {
        width: usize,
        height: usize,
        channels: usize,
        data: *mut c_uchar,
    }

    #[test]
    fn test_brightness_adjustment() {
        let mut data: Vec<u8> = vec![100, 150, 200];

        // 模拟亮度调整
        let factor = 1.5f32;
        for pixel in data.iter_mut() {
            let adjusted = (*pixel as f32 * factor).min(255.0) as u8;
            *pixel = adjusted;
        }

        assert_eq!(data, vec![150, 225, 255]);
    }

    #[test]
    fn test_grayscale_conversion() {
        // RGB值
        let r = 255u8;
        let g = 128u8;
        let b = 64u8;

        // 灰度转换公式
        let gray = (0.299 * r as f32 + 0.587 * g as f32 + 0.114 * b as f32) as u8;

        // 预期值：约144
        assert!(gray > 140 && gray < 150);
    }
}
```

**Makefile（完整版）：**

```makefile
.PHONY: all build release test clean run install

RUST := cargo
BUILD_DIR := target

all: build

build:
 $(RUST) build
 $(CC) -c c_src/io.c -o $(BUILD_DIR)/io.o -Ic_src/include
 $(CC) c_src/main.c $(BUILD_DIR)/io.o \
     -L$(BUILD_DIR)/debug \
     -limage_processor \
     -o $(BUILD_DIR)/processor_c \
     -pthread -ldl -lm

release:
 $(RUST) build --release
 $(CC) -O2 -c c_src/io.c -o $(BUILD_DIR)/io.o -Ic_src/include
 $(CC) -O2 c_src/main.c $(BUILD_DIR)/io.o \
     -L$(BUILD_DIR)/release \
     -limage_processor \
     -o $(BUILD_DIR)/processor_c \
     -pthread -ldl -lm

test:
 $(RUST) test

run: build
 ./$(BUILD_DIR)/processor_c

run-rust: build
 $(RUST) run

clean:
 $(RUST) clean
 rm -f $(BUILD_DIR)/*.o
 rm -f $(BUILD_DIR)/processor_c

install: release
 sudo install $(BUILD_DIR)/release/libimage_processor.a /usr/local/lib/
 sudo install c_src/include/image_processor.h /usr/local/include/

help:
 @echo "Available targets:"
 @echo "  build    - Debug build"
 @echo "  release  - Release build"
 @echo "  test     - Run tests"
 @echo "  run      - Run C version"
 @echo "  run-rust - Run Rust version"
 @echo "  clean    - Clean build files"
 @echo "  install  - Install library"
```

---

## 13. 性能对比基准测试

### 13.1 基准测试设计

```rust
//! 性能基准测试 - Rust vs C

use std::time::{Duration, Instant};

// ============================================
// 测试数据结构
// ============================================

const TEST_SIZES: [usize; 5] = [100, 1000, 10000, 100000, 1000000];

// ============================================
// 数值计算测试
// ============================================

/// 向量点积（Rust实现）
fn vector_dot_product_rust(a: &[f64], b: &[f64]) -> f64 {
    a.iter().zip(b.iter()).map(|(x, y)| x * y).sum()
}

/// 向量点积（手动循环，模拟C风格）
fn vector_dot_product_manual(a: &[f64], b: &[f64]) -> f64 {
    let mut sum = 0.0;
    for i in 0..a.len() {
        sum += a[i] * b[i];
    }
    sum
}

/// 矩阵乘法（Rust实现）
fn matrix_multiply_rust(a: &[f64], b: &[f64], n: usize) -> Vec<f64> {
    let mut result = vec![0.0; n * n];

    for i in 0..n {
        for j in 0..n {
            let mut sum = 0.0;
            for k in 0..n {
                sum += a[i * n + k] * b[k * n + j];
            }
            result[i * n + j] = sum;
        }
    }

    result
}

// ============================================
// 字符串处理测试
// ============================================

/// 字符串反转（Rust迭代器）
fn reverse_string_rust(s: &str) -> String {
    s.chars().rev().collect()
}

/// 字符串反转（手动，模拟C风格）
fn reverse_string_manual(s: &str) -> String {
    let chars: Vec<char> = s.chars().collect();
    let mut result = String::with_capacity(s.len());

    for i in (0..chars.len()).rev() {
        result.push(chars[i]);
    }

    result
}

// ============================================
// 排序测试
// ============================================

/// 快速排序（Rust实现）
fn quicksort_rust(arr: &mut [i32]) {
    if arr.len() <= 1 {
        return;
    }

    let pivot = arr[arr.len() / 2];
    let mut left = 0;
    let mut right = arr.len() - 1;

    while left <= right {
        while arr[left] < pivot {
            left += 1;
        }
        while arr[right] > pivot {
            right -= 1;
        }
        if left <= right {
            arr.swap(left, right);
            left += 1;
            if right > 0 {
                right -= 1;
            }
        }
    }

    let len = arr.len();
    if right > 0 {
        quicksort_rust(&mut arr[0..=right]);
    }
    if left < len {
        quicksort_rust(&mut arr[left..len]);
    }
}

// ============================================
// 内存操作测试
// ============================================

/// 内存拷贝（Rust实现）
fn copy_memory_rust(src: &[u8], dst: &mut [u8]) {
    dst.copy_from_slice(src);
}

/// 查找最大值（Rust实现）
fn find_max_rust(arr: &[i32]) -> Option<i32> {
    arr.iter().copied().max()
}

// ============================================
// 基准测试框架
// ============================================

struct BenchmarkResult {
    name: String,
    size: usize,
    iterations: usize,
    total_time: Duration,
    avg_time: Duration,
}

fn benchmark<F>(name: &str, size: usize, iterations: usize, f: F) -> BenchmarkResult
where
    F: Fn(),
{
    let start = Instant::now();

    for _ in 0..iterations {
        f();
    }

    let total_time = start.elapsed();
    let avg_time = total_time / iterations as u32;

    BenchmarkResult {
        name: name.to_string(),
        size,
        iterations,
        total_time,
        avg_time,
    }
}

fn format_duration(d: Duration) -> String {
    if d.as_secs() > 0 {
        format!("{:.3}s", d.as_secs_f64())
    } else if d.as_millis() > 0 {
        format!("{}ms", d.as_millis())
    } else if d.as_micros() > 0 {
        format!("{}μs", d.as_micros())
    } else {
        format!("{}ns", d.as_nanos())
    }
}

// ============================================
// 运行基准测试
// ============================================

fn main() {
    println!("Rust vs C 性能基准测试");
    println!("======================\n");

    let mut results: Vec<BenchmarkResult> = Vec::new();

    // 向量点积测试
    println!("--- 向量点积测试 ---");
    for &size in &TEST_SIZES {
        let a: Vec<f64> = (0..size).map(|x| x as f64).collect();
        let b: Vec<f64> = (0..size).map(|x| (size - x) as f64).collect();

        let iterations = if size < 10000 { 10000 } else { 1000 };

        let result = benchmark(
            "Rust迭代器",
            size,
            iterations,
            || { let _: f64 = vector_dot_product_rust(&a, &b); }
        );
        results.push(result);

        let result = benchmark(
            "手动循环",
            size,
            iterations,
            || { let _: f64 = vector_dot_product_manual(&a, &b); }
        );
        results.push(result);
    }

    // 字符串处理测试
    println!("\n--- 字符串处理测试 ---");
    let test_string = "Hello, World! This is a test string for benchmarking.".repeat(1000);

    let result = benchmark(
        "Rust迭代器反转",
        test_string.len(),
        1000,
        || { let _ = reverse_string_rust(&test_string); }
    );
    results.push(result);

    let result = benchmark(
        "手动反转",
        test_string.len(),
        1000,
        || { let _ = reverse_string_manual(&test_string); }
    );
    results.push(result);

    // 排序测试
    println!("\n--- 排序测试 ---");
    for &size in &[100, 1000, 10000] {
        let iterations = if size < 1000 { 100 } else { 10 };

        let result = benchmark(
            "快速排序",
            size,
            iterations,
            || {
                let mut arr: Vec<i32> = (0..size as i32).rev().collect();
                quicksort_rust(&mut arr);
            }
        );
        results.push(result);
    }

    // 打印结果
    println!("\n=== 测试结果汇总 ===");
    println!("{:<20} {:<12} {:<12} {:<15} {:<15}",
             "测试名称", "数据大小", "迭代次数", "总时间", "平均时间");
    println!("{}", "-".repeat(80));

    for result in &results {
        println!("{:<20} {:<12} {:<12} {:<15} {:<15}",
            result.name,
            result.size,
            result.iterations,
            format_duration(result.total_time),
            format_duration(result.avg_time)
        );
    }

    // 对比总结
    println!("\n=== 性能对比总结 ===");
    println!("Rust编译器优化:");
    println!("  - LLVM后端提供与C相当的优化水平");
    println!("  - 迭代器通常被优化为与手动循环相同的性能");
    println!("  - 边界检查在发布模式下可被优化掉");
    println!();
    println!("典型性能对比（相对于优化过的C代码）:");
    println!("  - 数值计算: 100-105%（基本相同）");
    println!("  - 字符串处理: 95-100%（有时略慢）");
    println!("  - 内存操作: 100%（使用unsafe时）");
    println!("  - 抽象开销: 0%（零成本抽象）");
}
```

### 13.2 性能对比总结

| 测试类型 | C性能 | Rust性能 | 备注 |
|----------|-------|----------|------|
| **数值计算** | 基准 | 100-102% | LLVM优化相当 |
| **内存拷贝** | 基准 | 100% | 相同机器码 |
| **字符串处理** | 基准 | 95-100% | UTF-8验证略有开销 |
| **动态分配** | 基准 | 98-100% | jemalloc与malloc相当 |
| **递归算法** | 基准 | 100% | 尾递归优化 |
| **虚函数/Trait** | 基准 | 100% | 去虚化优化 |

### 13.3 FFI性能开销

| 调用方式 | 开销 | 说明 |
|----------|------|------|
| **直接调用** | 0 | 同语言内联 |
| **FFI调用（无转换）** | ~5-10ns | 仅函数调用开销 |
| **FFI + 类型转换** | ~20-50ns | 包括字符串转换等 |
| **批量FFI调用** | 可忽略 | 平摊开销 |

---

## 总结

本文档全面介绍了Rust与C之间的FFI互操作机制，包括：

1. **语言对比**：Rust通过所有权系统提供编译时内存安全保证
2. **FFI基础**：extern "C"、#[no_mangle]、类型映射
3. **双向调用**：从C调用Rust和从Rust调用C的完整流程
4. **复杂类型**：结构体、联合体、动态数组的处理
5. **回调函数**：安全地在两种语言间传递函数指针
6. **错误处理**：Result vs 返回值的模式对比
7. **并发安全**：编译时防止数据竞争
8. **迁移策略**：逐步从C迁移到Rust的方法
9. **构建集成**：Cargo、Make、CMake的集成方案
10. **完整项目**：图像处理混合项目示例
11. **性能基准**：Rust与C的性能对比

通过FFI，开发者可以在保持现有C代码投资的同时，逐步引入Rust的内存安全特性，实现渐进式现代化。

---

*文档版本: 1.0*
*最后更新: 2026年3月*
*作者: Rust & C FFI 指南*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
