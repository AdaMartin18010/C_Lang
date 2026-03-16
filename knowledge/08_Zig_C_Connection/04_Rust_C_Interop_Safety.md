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
