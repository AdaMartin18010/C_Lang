# C/Rust FFI 最佳实践指南

> **层级定位**: 15 Memory Safe Languages / 01 Rust vs C / 07 FFI Best Practices
> **文档类型**: 技术实践指南
> **目标读者**: 系统工程师、互操作开发者、迁移工程师
> **最后更新**: 2026-03-18

---

## 📑 目录

- [C/Rust FFI 最佳实践指南](#crust-ffi-最佳实践指南)
  - [📑 目录](#-目录)
  - [执行摘要](#执行摘要)
  - [1. C ABI互操作基础](#1-c-abi互操作基础)
    - [1.1 ABI兼容性表](#11-abi兼容性表)
    - [1.2 基本数据类型映射](#12-基本数据类型映射)
    - [1.3 结构体布局](#13-结构体布局)
  - [2. bindgen使用指南](#2-bindgen使用指南)
    - [2.1 安装与配置](#21-安装与配置)
    - [2.2 基本用法](#22-基本用法)
    - [2.3 高级配置](#23-高级配置)
  - [3. cbindgen生成C头文件](#3-cbindgen生成c头文件)
    - [3.1 配置与使用](#31-配置与使用)
    - [3.2 导出Rust类型](#32-导出rust类型)
  - [4. 跨语言内存管理](#4-跨语言内存管理)
    - [4.1 所有权规则](#41-所有权规则)
    - [4.2 内存分配策略](#42-内存分配策略)
    - [4.3 智能指针封装](#43-智能指针封装)
  - [5. 错误处理映射](#5-错误处理映射)
    - [5.1 错误码转换](#51-错误码转换)
    - [5.2 异常与panic](#52-异常与panic)
  - [6. 完整示例项目](#6-完整示例项目)
    - [6.1 项目结构](#61-项目结构)
    - [6.2 构建系统](#62-构建系统)
    - [6.3 测试策略](#63-测试策略)
  - [7. 性能优化技巧](#7-性能优化技巧)
  - [8. 调试与故障排除](#8-调试与故障排除)
  - [9. 安全检查清单](#9-安全检查清单)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 执行摘要

```
┌─────────────────────────────────────────────────────────────────┐
│              C/Rust FFI：安全互操作的核心                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  FFI (Foreign Function Interface) 是C和Rust协同工作的桥梁       │
│                                                                  │
│  核心原则                                                        │
│  • C ABI是通用语言：两种语言都通过C ABI进行互操作               │
│  • unsafe是边界：FFI代码必须用unsafe块包裹                      │
│  • 所有权必须明确：谁分配、谁释放、谁拥有                        │
│  • 验证输入：永远不要信任外部数据                               │
│                                                                  │
│  工具链                                                          │
│  • bindgen: C头文件 → Rust FFI声明                              │
│  • cbindgen: Rust代码 → C头文件                                 │
│  • cargo-c: 构建C兼容库                                         │
│                                                                  │
│  风险警告                                                        │
│  ⚠️ FFI边界是安全漏洞高发区，需要特别小心                       │
│  ⚠️ C可以轻易违反Rust的安全保证                                 │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1. C ABI互操作基础

### 1.1 ABI兼容性表

```
┌─────────────────────────────────────────────────────────────────┐
│                    C ↔ Rust ABI兼容性                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  调用约定 (Calling Conventions)                                  │
│  ┌──────────────────┬───────────────┬────────────────────────┐  │
│  │ 约定             │ Rust语法      │ 适用平台               │  │
│  ├──────────────────┼───────────────┼────────────────────────┤  │
│  │ C (cdecl)        │ extern "C"    │ 跨平台默认             │  │
│  │ System V AMD64   │ extern "C"    │ Linux, macOS, BSD x64 │  │
│  │ Windows x64      │ extern "C"    │ Windows x64            │  │
│  │ stdcall          │ extern "system"│ Windows 32-bit API    │  │
│  │ fastcall         │ 需特定属性    │ 特定优化场景           │  │
│  │ thiscall         │ 不直接支持    │ C++成员函数            │  │
│  └──────────────────┴───────────────┴────────────────────────┘  │
│                                                                  │
│  推荐使用 extern "C" 以获得最大兼容性                           │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 基本数据类型映射

| C类型 | Rust类型 | 说明 | 注意 |
|:------|:---------|:-----|:-----|
| `char` | `c_char` | 有符号/无符号取决于平台 | 使用`c_char`而非`i8`/`u8` |
| `short` | `c_short` | 16位 | 通常是`i16` |
| `int` | `c_int` | 32位 | 通常是`i32` |
| `long` | `c_long` | 32/64位取决于平台 | LP64/LLP64差异 |
| `long long` | `c_longlong` | 64位 | 通常是`i64` |
| `float` | `c_float` | 32位IEEE 754 | `f32` |
| `double` | `c_double` | 64位IEEE 754 | `f64` |
| `void*` | `*mut c_void` | 通用指针 | 类型擦除 |
| `size_t` | `usize` | 平台相关大小 | 通常相同 |
| `ptrdiff_t` | `isize` | 有符号指针差值 | 通常相同 |

```rust
// 标准类型映射示例
use std::os::raw::{c_char, c_int, c_void, c_double};

// Rust调用C函数
extern "C" {
    // C: int process_data(const char* input, double* output);
    fn process_data(input: *const c_char, output: *mut c_double) -> c_int;
}

// Rust暴露给C的函数
#[no_mangle]
pub extern "C" fn rust_compute(value: c_int) -> c_double {
    // 安全的Rust实现
    (value as f64).sqrt()
}
```

### 1.3 结构体布局

```rust
// C结构体
/*
typedef struct {
    int id;
    double value;
    char name[32];
} DataRecord;
*/

// 对应的Rust结构体
#[repr(C)]  // 关键！确保C兼容布局
pub struct DataRecord {
    pub id: c_int,
    pub value: c_double,
    pub name: [c_char; 32],
}

// 可变长度结构体 (C柔性数组)
/*
typedef struct {
    size_t count;
    double items[];  // 柔性数组成员 (C99)
} FlexArray;
*/

#[repr(C)]
pub struct FlexArray {
    pub count: usize,
    // 柔性数组在Rust中需要特殊处理
    pub items: [c_double; 0],  // 零长度数组作为占位
}

impl FlexArray {
    /// 安全访问柔性数组元素
    pub fn get_item(&self, index: usize) -> Option<&f64> {
        if index < self.count {
            // 计算偏移并安全访问
            unsafe {
                let ptr = self.items.as_ptr().add(index);
                Some(&*(ptr as *const f64))
            }
        } else {
            None
        }
    }
}
```

---

## 2. bindgen使用指南

### 2.1 安装与配置

```toml
# Cargo.toml

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../README.md](../../README.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/README.md](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/README.md](../../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```

学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]

```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/README.md](../../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/README.md](../../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/README.md](../../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/README.md](../../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```

┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘

```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/README.md) |

---

[build-dependencies]
bindgen = "0.69"
```

```rust
// build.rs
use std::env;
use std::path::PathBuf;

fn main() {
    // 告诉cargo在C文件变化时重新构建
    println!("cargo:rerun-if-changed=wrapper.h");

    // 配置bindgen
    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        // 只包含特定前缀的函数
        .allowlist_function("mylib_.*")
        .allowlist_type("MyLib.*")
        // 为常量生成正确类型
        .constified_enum("mylib_status")
        // C解析选项
        .clang_arg("-I/usr/local/include")
        .clang_arg("-DMYLIB_BUILD")
        // 生成选项
        .derive_default(true)
        .derive_debug(true)
        .impl_debug(true)
        // 完成生成
        .generate()
        .expect("Unable to generate bindings");

    // 写入输出文件
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");
}
```

### 2.2 基本用法

```c
// mylib.h - C库头文件
#ifndef MYLIB_H
#define MYLIB_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 错误码枚举
typedef enum {
    MYLIB_OK = 0,
    MYLIB_ERROR_INVALID_PARAM = -1,
    MYLIB_ERROR_OUT_OF_MEMORY = -2,
    MYLIB_ERROR_IO = -3,
} mylib_status;

// 不透明句柄
typedef struct mylib_context* mylib_handle_t;

// 配置结构体
typedef struct {
    uint32_t version;
    size_t buffer_size;
    const char* config_path;
} mylib_config;

// API函数
mylib_status mylib_init(const mylib_config* config, mylib_handle_t* handle);
mylib_status mylib_process(mylib_handle_t handle, const uint8_t* input, size_t len);
void mylib_cleanup(mylib_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif // MYLIB_H
```

```rust
// wrapper.h - 用于bindgen的包装器
#include "mylib.h"

// src/ffi.rs - 使用生成的绑定
pub mod bindings {
    #![allow(non_upper_case_globals)]
    #![allow(non_camel_case_types)]
    #![allow(non_snake_case)]
    #![allow(dead_code)]

    include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
}

// src/lib.rs - 安全封装层
use std::ffi::CString;
use std::ptr;

pub struct MyLibContext {
    handle: bindings::mylib_handle_t,
}

#[derive(Debug)]
pub enum MyLibError {
    InvalidParam,
    OutOfMemory,
    Io,
    Unknown(i32),
}

impl MyLibContext {
    pub fn new(buffer_size: usize, config_path: &str) -> Result<Self, MyLibError> {
        let path_cstring = CString::new(config_path)
            .map_err(|_| MyLibError::InvalidParam)?;

        let config = bindings::mylib_config {
            version: 1,
            buffer_size,
            config_path: path_cstring.as_ptr(),
        };

        let mut handle = ptr::null_mut();

        // SAFETY: 配置结构有效，handle指针有效
        let status = unsafe {
            bindings::mylib_init(&config, &mut handle)
        };

        match status {
            bindings::mylib_status_MYLIB_OK => {
                Ok(MyLibContext { handle })
            }
            bindings::mylib_status_MYLIB_ERROR_INVALID_PARAM => {
                Err(MyLibError::InvalidParam)
            }
            bindings::mylib_status_MYLIB_ERROR_OUT_OF_MEMORY => {
                Err(MyLibError::OutOfMemory)
            }
            bindings::mylib_status_MYLIB_ERROR_IO => {
                Err(MyLibError::Io)
            }
            other => Err(MyLibError::Unknown(other)),
        }
    }

    pub fn process(&mut self, data: &[u8]) -> Result<(), MyLibError> {
        // SAFETY: handle在构造时验证有效，数据切片有效
        let status = unsafe {
            bindings::mylib_process(
                self.handle,
                data.as_ptr(),
                data.len()
            )
        };

        if status == bindings::mylib_status_MYLIB_OK {
            Ok(())
        } else {
            Err(MyLibError::Unknown(status))
        }
    }
}

impl Drop for MyLibContext {
    fn drop(&mut self) {
        // SAFETY: handle有效，cleanup可重入安全
        unsafe {
            bindings::mylib_cleanup(self.handle);
        }
    }
}
```

### 2.3 高级配置

```rust
// build.rs - 高级bindgen配置
fn main() {
    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        // 函数过滤
        .allowlist_function("mylib_.*")
        .blocklist_function("mylib_internal_.*")
        // 类型过滤
        .allowlist_type("mylib_.*")
        .blocklist_type("pthread.*")
        // 变量过滤
        .allowlist_var("MYLIB_.*")
        // 枚举处理
        .rustified_enum("mylib_mode")  // 生成Rust枚举
        .constified_enum("mylib_flags") // 生成常量
        // 类型替换
        .size_t_is_usize(true)
        // 注释处理
        .parse_comments(true)
        .clang_arg("-fretain-comments-from-system-headers")
        // 布局测试
        .layout_tests(true)
        // 自定义派生
        .derive_copy(true)
        .derive_debug(true)
        .derive_default(true)
        .derive_hash(true)
        .derive_partialord(true)
        .derive_ord(true)
        .derive_partialeq(true)
        .derive_eq(true)
        // 选项
        .default_enum_style(bindgen::EnumVariation::Rust {
            non_exhaustive: true
        })
        .generate()
        .unwrap();

    bindings
        .write_to_file(PathBuf::from(env::var("OUT_DIR").unwrap()).join("bindings.rs"))
        .unwrap();
}
```

---

## 3. cbindgen生成C头文件

### 3.1 配置与使用

```toml
# Cargo.toml
[package]
name = "my-rust-lib"
version = "0.1.0"
edition = "2021"

[dependencies]

[build-dependencies]
cbindgen = "0.26"
```

```rust
// build.rs
use std::env;
use std::path::PathBuf;

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    let output_file = PathBuf::from(&crate_dir)
        .join("include")
        .join("my_rust_lib.h");

    cbindgen::Builder::new()
        .with_crate(crate_dir)
        .with_config(cbindgen::Config {
            language: cbindgen::Language::C,
            header: Some("/* Auto-generated C header */".to_string()),
            include_guard: Some("MY_RUST_LIB_H".to_string()),
            autogen_warning: Some("/* Warning: this file is auto-generated */".to_string()),
            ..Default::default()
        })
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(output_file);
}
```

```toml
# cbindgen.toml 配置文件
language = "C"
header = "/* My Rust Library C API */"
include_guard = "MY_RUST_LIB_H"
sys_includes = ["stdint.h", "stdbool.h"]

[export]
include = ["MyStruct", "my_function"]
exclude = ["internal_*"]

[fn]
rename_types = "SnakeCase"

[struct]
rename_types = "PascalCase"
```

### 3.2 导出Rust类型

```rust
// src/lib.rs

/// 处理器配置
#[repr(C)]
#[derive(Debug, Clone)]
pub struct ProcessorConfig {
    pub threads: u32,
    pub buffer_size: usize,
    pub enable_compression: bool,
}

/// 错误码
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub enum ErrorCode {
    Success = 0,
    InvalidArgument = 1,
    OutOfMemory = 2,
    ProcessingError = 3,
}

/// 不透明上下文类型
pub struct ProcessorContext {
    inner: crate::processor::Processor,
}

/// 创建处理器
///
/// # Safety
/// config必须是有效的非空指针
/// out_context必须是有效的可写指针
#[no_mangle]
pub unsafe extern "C" fn processor_create(
    config: *const ProcessorConfig,
    out_context: *mut *mut ProcessorContext,
) -> ErrorCode {
    if config.is_null() || out_context.is_null() {
        return ErrorCode::InvalidArgument;
    }

    let config = &*config;

    match crate::processor::Processor::new(config) {
        Ok(processor) => {
            let context = Box::new(ProcessorContext { inner: processor });
            *out_context = Box::into_raw(context);
            ErrorCode::Success
        }
        Err(_) => ErrorCode::OutOfMemory,
    }
}

/// 处理数据
///
/// # Safety
/// context必须是有效的处理器上下文
/// data可以是空（当len为0时）
#[no_mangle]
pub unsafe extern "C" fn processor_process(
    context: *mut ProcessorContext,
    data: *const u8,
    len: usize,
) -> ErrorCode {
    if context.is_null() {
        return ErrorCode::InvalidArgument;
    }

    let context = &mut *context;
    let data = if data.is_null() || len == 0 {
        &[]
    } else {
        std::slice::from_raw_parts(data, len)
    };

    match context.inner.process(data) {
        Ok(_) => ErrorCode::Success,
        Err(_) => ErrorCode::ProcessingError,
    }
}

/// 销毁处理器
///
/// # Safety
/// context必须是有效的由processor_create返回的指针，或NULL
#[no_mangle]
pub unsafe extern "C" fn processor_destroy(context: *mut ProcessorContext) {
    if !context.is_null() {
        let _ = Box::from_raw(context);  // 自动drop
    }
}
```

生成的C头文件：

```c
/* Auto-generated C header */
#ifndef MY_RUST_LIB_H
#define MY_RUST_LIB_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Warning: this file is auto-generated */

/**
 * 处理器配置
 */
typedef struct {
    uint32_t threads;
    size_t buffer_size;
    bool enable_compression;
} ProcessorConfig;

/**
 * 错误码
 */
typedef enum {
    Success = 0,
    InvalidArgument = 1,
    OutOfMemory = 2,
    ProcessingError = 3,
} ErrorCode;

/**
 * 不透明上下文类型
 */
typedef struct ProcessorContext ProcessorContext;

/**
 * 创建处理器
 *
 * # Safety
 * config必须是有效的非空指针
 * out_context必须是有效的可写指针
 */
ErrorCode processor_create(const ProcessorConfig *config,
                           ProcessorContext **out_context);

/**
 * 处理数据
 *
 * # Safety
 * context必须是有效的处理器上下文
 * data可以是空（当len为0时）
 */
ErrorCode processor_process(ProcessorContext *context,
                            const uint8_t *data,
                            size_t len);

/**
 * 销毁处理器
 *
 * # Safety
 * context必须是有效的由processor_create返回的指针，或NULL
 */
void processor_destroy(ProcessorContext *context);

#endif /* MY_RUST_LIB_H */
```

---

## 4. 跨语言内存管理

### 4.1 所有权规则

```
┌─────────────────────────────────────────────────────────────────┐
│                  FFI内存所有权约定                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  规则1: 谁分配，谁释放                                           │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ C分配 → C释放                                            │   │
│  │   C: malloc() → Rust使用 → C: free()                    │   │
│  │                                                         │   │
│  │ Rust分配 → Rust释放                                     │   │
│  │   Rust: Box::new() → C使用 → Rust: Box::from_raw()      │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                  │
│  规则2: 借用必须明确生命周期                                     │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ C借Rust数据                                             │   │
│  │   - 禁止在回调之外访问                                  │   │
│  │   - Rust必须保证回调期间数据有效                        │   │
│  │                                                         │   │
│  │ Rust借C数据                                             │   │
│  │   - 使用引用而非指针                                    │   │
│  │   - 显式标注生命周期                                    │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                  │
│  规则3: 不透明指针模式                                           │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ C使用Rust对象                                           │   │
│  │   Rust: Box::into_raw() → C: void* → Rust从回调取回    │   │
│  │                                                         │   │
│  │ Rust使用C对象                                           │   │
│  │   C返回句柄 → Rust保存 → 传回C使用                      │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 内存分配策略

```rust
// 策略1: Rust拥有，C借用
pub struct RustOwned {
    data: Vec<u8>,
}

impl RustOwned {
    /// C可以读取数据，但不拥有
    /// 返回的指针只在self有效期间有效
    pub fn as_ptr(&self) -> *const u8 {
        self.data.as_ptr()
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }
}

// 策略2: C拥有，Rust借用
pub struct CBorrowed<'a> {
    ptr: *const u8,
    len: usize,
    _marker: std::marker::PhantomData<&'a ()>,
}

impl<'a> CBorrowed<'a> {
    /// SAFETY: ptr必须有效且至少有len字节，在'a期间保持有效
    pub unsafe fn from_raw(ptr: *const u8, len: usize) -> Self {
        Self {
            ptr,
            len,
            _marker: std::marker::PhantomData,
        }
    }

    pub fn as_slice(&self) -> &[u8] {
        // SAFETY: 由构造函数保证
        unsafe { std::slice::from_raw_parts(self.ptr, self.len) }
    }
}

// 策略3: 共享所有权 (谨慎使用)
use std::sync::Arc;

pub struct SharedContext {
    inner: Arc<InnerContext>,
}

#[no_mangle]
pub extern "C" fn context_clone(ctx: *const SharedContext) -> *mut SharedContext {
    if ctx.is_null() {
        return std::ptr::null_mut();
    }

    let ctx = unsafe { &*ctx };
    let new_ctx = SharedContext {
        inner: Arc::clone(&ctx.inner),
    };

    Box::into_raw(Box::new(new_ctx))
}
```

### 4.3 智能指针封装

```rust
/// 安全的C字符串包装
pub struct CStringWrapper {
    ptr: *mut c_char,
    owned: bool,
}

impl CStringWrapper {
    /// 从Rust字符串创建 (Rust拥有)
    pub fn new(s: &str) -> Option<Self> {
        let cstring = CString::new(s).ok()?;
        let ptr = cstring.into_raw();
        Some(Self { ptr, owned: true })
    }

    /// 从C指针借用 (C拥有)
    ///
    /// # Safety
    /// ptr必须是有效的以null结尾的C字符串
    pub unsafe fn borrow(ptr: *const c_char) -> Self {
        Self {
            ptr: ptr as *mut c_char,
            owned: false,
        }
    }

    pub fn as_ptr(&self) -> *const c_char {
        self.ptr
    }

    pub fn to_str(&self) -> Option<&str> {
        if self.ptr.is_null() {
            return None;
        }
        // SAFETY: 假设是有效的C字符串
        let cstr = unsafe { CStr::from_ptr(self.ptr) };
        cstr.to_str().ok()
    }
}

impl Drop for CStringWrapper {
    fn drop(&mut self) {
        if self.owned && !self.ptr.is_null() {
            // SAFETY: 我们拥有这个字符串，由new()创建
            unsafe {
                let _ = CString::from_raw(self.ptr);
            }
        }
    }
}

/// 安全的缓冲区包装
pub struct Buffer {
    ptr: *mut u8,
    len: usize,
    capacity: usize,
}

impl Buffer {
    pub fn with_capacity(capacity: usize) -> Option<Self> {
        let layout = Layout::array::<u8>(capacity).ok()?;
        // SAFETY: layout大小非零
        let ptr = unsafe { alloc(layout) };
        if ptr.is_null() {
            return None;
        }
        Some(Self {
            ptr,
            len: 0,
            capacity,
        })
    }

    /// 导出给C使用
    pub fn into_raw(self) -> (*mut u8, usize) {
        let ptr = self.ptr;
        let len = self.len;
        std::mem::forget(self);  // 防止Drop
        (ptr, len)
    }

    /// 从C取回
    ///
    /// # Safety
    /// ptr必须由into_raw产生，且未被修改
    pub unsafe fn from_raw(ptr: *mut u8, len: usize, capacity: usize) -> Self {
        Self { ptr, len, capacity }
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        if !self.ptr.is_null() {
            unsafe {
                let layout = Layout::array::<u8>(self.capacity).unwrap_unchecked();
                dealloc(self.ptr, layout);
            }
        }
    }
}
```

---

## 5. 错误处理映射

### 5.1 错误码转换

```rust
/// C风格错误码
#[repr(C)]
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum CErrorCode {
    Success = 0,
    InvalidArg = -1,
    NullPointer = -2,
    OutOfMemory = -3,
    IoError = -4,
    Unknown = -99,
}

/// Rust错误类型
#[derive(Debug)]
pub enum RustError {
    InvalidArgument(String),
    NullPointer(&'static str),
    AllocationFailed,
    Io(std::io::Error),
    Other(Box<dyn std::error::Error>),
}

impl From<RustError> for CErrorCode {
    fn from(err: RustError) -> Self {
        match err {
            RustError::InvalidArgument(_) => CErrorCode::InvalidArg,
            RustError::NullPointer(_) => CErrorCode::NullPointer,
            RustError::AllocationFailed => CErrorCode::OutOfMemory,
            RustError::Io(_) => CErrorCode::IoError,
            RustError::Other(_) => CErrorCode::Unknown,
        }
    }
}

/// 带详细信息的错误处理
#[repr(C)]
pub struct CError {
    pub code: CErrorCode,
    pub message: *mut c_char,  // C需负责释放
}

impl CError {
    pub fn new(code: CErrorCode, message: &str) -> Self {
        let cstring = CString::new(message).unwrap_or_default();
        Self {
            code,
            message: cstring.into_raw(),
        }
    }

    pub fn success() -> Self {
        Self {
            code: CErrorCode::Success,
            message: std::ptr::null_mut(),
        }
    }
}

/// C释放错误信息
#[no_mangle]
pub extern "C" fn error_free(err: CError) {
    if !err.message.is_null() {
        unsafe {
            let _ = CString::from_raw(err.message);
        }
    }
}

/// 使用示例
#[no_mangle]
pub extern "C" fn do_something(input: *const c_char, out_err: *mut CError) -> c_int {
    let result = std::panic::catch_unwind(|| {
        if input.is_null() {
            return Err(RustError::NullPointer("input"));
        }

        // ... 实际工作

        Ok(0)
    });

    match result {
        Ok(Ok(val)) => {
            if !out_err.is_null() {
                unsafe { *out_err = CError::success(); }
            }
            val
        }
        Ok(Err(e)) => {
            if !out_err.is_null() {
                let error_code: CErrorCode = e.into();
                let msg = format!("{:?}", e);
                unsafe { *out_err = CError::new(error_code, &msg); }
            }
            -1
        }
        Err(_) => {
            if !out_err.is_null() {
                unsafe { *out_err = CError::new(CErrorCode::Unknown, "panic occurred"); }
            }
            -1
        }
    }
}
```

### 5.2 异常与panic

```rust
/// C友好的panic边界
///
/// 在FFI边界捕获panic，防止跨语言UB
pub fn ffi_boundary<F, R>(f: F) -> Option<R>
where
    F: FnOnce() -> R + std::panic::UnwindSafe,
{
    match std::panic::catch_unwind(f) {
        Ok(result) => Some(result),
        Err(_) => {
            // 记录panic，返回None给C
            eprintln!("Rust panic caught at FFI boundary");
            None
        }
    }
}

/// 设置panic钩子，在FFI上下文中安全处理
pub fn setup_ffi_panic_handler() {
    std::panic::set_hook(Box::new(|info| {
        // 记录到stderr，而不是默认的unwind
        eprintln!("FFI panic: {}", info);
    }));
}

/// 使用示例
#[no_mangle]
pub extern "C" fn safe_function() -> c_int {
    ffi_boundary(|| {
        // 可能panic的Rust代码
        let value = some_operation();
        value as c_int
    }).unwrap_or(-1)  // panic时返回默认值
}
```

---

## 6. 完整示例项目

### 6.1 项目结构

```
crypto_ffi/                     # Rust项目根目录
├── Cargo.toml
├── build.rs
├── cbindgen.toml
├── include/                    # 生成的C头文件
│   └── crypto_ffi.h
├── src/
│   ├── lib.rs                  # 库入口
│   ├── ffi/                    # FFI层
│   │   ├── mod.rs
│   │   ├── bindings.rs         # bindgen生成
│   │   └── wrappers.rs         # 安全包装
│   └── crypto/                 # 纯Rust实现
│       ├── mod.rs
│       └── hasher.rs
├── c-example/                  # C示例程序
│   ├── Makefile
│   └── main.c
└── tests/                      # 集成测试
    └── integration_tests.rs
```

### 6.2 构建系统

```toml
# Cargo.toml
[package]
name = "crypto_ffi"
version = "0.1.0"
edition = "2021"

[dependencies]
sha2 = "0.10"
hex = "0.4"

[build-dependencies]
bindgen = "0.69"
cbindgen = "0.26"

[lib]
name = "crypto_ffi"
crate-type = ["cdylib", "staticlib"]
```

```rust
// src/lib.rs
pub mod crypto;
pub mod ffi;

pub use crypto::Hasher;
```

```rust
// src/crypto/mod.rs
use sha2::{Digest, Sha256};

pub struct Hasher {
    inner: Sha256,
}

impl Hasher {
    pub fn new() -> Self {
        Self { inner: Sha256::new() }
    }

    pub fn update(&mut self, data: &[u8]) {
        self.inner.update(data);
    }

    pub fn finalize(self) -> [u8; 32] {
        self.inner.finalize().into()
    }
}

impl Default for Hasher {
    fn default() -> Self {
        Self::new()
    }
}
```

```rust
// src/ffi/mod.rs
use std::ffi::{c_char, c_int, CStr, CString};
use std::slice;

/// 不透明句柄
pub struct HasherHandle {
    inner: crate::crypto::Hasher,
}

/// 创建新hasher
///
/// # Safety
/// out_handle必须是有效的可写指针
#[no_mangle]
pub unsafe extern "C" fn crypto_hasher_create(out_handle: *mut *mut HasherHandle) -> c_int {
    if out_handle.is_null() {
        return -1;
    }

    let hasher = Box::new(HasherHandle {
        inner: crate::crypto::Hasher::new(),
    });

    *out_handle = Box::into_raw(hasher);
    0
}

/// 更新hasher
///
/// # Safety
/// handle必须是有效的hasher句柄
/// data可以是空（当len为0时）
#[no_mangle]
pub unsafe extern "C" fn crypto_hasher_update(
    handle: *mut HasherHandle,
    data: *const u8,
    len: usize,
) -> c_int {
    if handle.is_null() {
        return -1;
    }

    let hasher = &mut (*handle).inner;
    let data = if data.is_null() || len == 0 {
        &[]
    } else {
        slice::from_raw_parts(data, len)
    };

    hasher.update(data);
    0
}

/// 完成hash并返回hex字符串
///
/// # Safety
/// handle必须是有效的hasher句柄
/// 返回的字符串必须由调用者使用crypto_free_string释放
#[no_mangle]
pub unsafe extern "C" fn crypto_hasher_finalize(
    handle: *mut HasherHandle,
    out_hex: *mut *mut c_char,
) -> c_int {
    if handle.is_null() || out_hex.is_null() {
        return -1;
    }

    let handle = Box::from_raw(handle);
    let result = handle.inner.finalize();
    let hex_string = hex::encode(result);

    match CString::new(hex_string) {
        Ok(cstring) => {
            *out_hex = cstring.into_raw();
            0
        }
        Err(_) => -1,
    }
}

/// 释放字符串
///
/// # Safety
/// s必须是由本库分配的字符串指针，或NULL
#[no_mangle]
pub unsafe extern "C" fn crypto_free_string(s: *mut c_char) {
    if !s.is_null() {
        let _ = CString::from_raw(s);
    }
}

/// 计算数据的SHA256哈希 (便捷函数)
///
/// # Safety
/// data可以是空（当len为0时）
/// out_hex必须是有效的可写指针
/// 返回的字符串必须由调用者使用crypto_free_string释放
#[no_mangle]
pub unsafe extern "C" fn crypto_sha256_hex(
    data: *const u8,
    len: usize,
    out_hex: *mut *mut c_char,
) -> c_int {
    if out_hex.is_null() {
        return -1;
    }

    let data = if data.is_null() || len == 0 {
        &[]
    } else {
        slice::from_raw_parts(data, len)
    };

    let mut hasher = crate::crypto::Hasher::new();
    hasher.update(data);
    let result = hasher.finalize();
    let hex_string = hex::encode(result);

    match CString::new(hex_string) {
        Ok(cstring) => {
            *out_hex = cstring.into_raw();
            0
        }
        Err(_) => -1,
    }
}
```

```c
// c-example/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto_ffi.h"

int main() {
    // 方法1: 使用便捷函数
    const char* message = "Hello, Rust FFI!";
    char* hash1 = NULL;

    int result = crypto_sha256_hex(
        (const uint8_t*)message,
        strlen(message),
        &hash1
    );

    if (result == 0) {
        printf("SHA256: %s\n", hash1);
        crypto_free_string(hash1);
    }

    // 方法2: 使用流式API
    HasherHandle* hasher = NULL;
    result = crypto_hasher_create(&hasher);
    if (result != 0) {
        fprintf(stderr, "Failed to create hasher\n");
        return 1;
    }

    // 分块更新
    crypto_hasher_update(hasher, (const uint8_t*)"Hello, ", 7);
    crypto_hasher_update(hasher, (const uint8_t*)"World!", 6);

    char* hash2 = NULL;
    result = crypto_hasher_finalize(hasher, &hash2);  // hasher被消耗

    if (result == 0) {
        printf("SHA256 (stream): %s\n", hash2);
        crypto_free_string(hash2);
    }

    return 0;
}
```

```makefile
# c-example/Makefile
CC = gcc
CFLAGS = -Wall -Wextra -I../include
LDFLAGS = -L../target/release -lcrypto_ffi -lpthread -ldl

.PHONY: all clean

all: example

example: main.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
 rm -f example

# 运行前需要构建Rust库:
# cd .. && cargo build --release
```

### 6.3 测试策略

```rust
// tests/integration_tests.rs
use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int};

// 引入FFI函数
extern "C" {
    fn crypto_sha256_hex(
        data: *const u8,
        len: usize,
        out_hex: *mut *mut c_char,
    ) -> c_int;
    fn crypto_free_string(s: *mut c_char);
}

#[test]
fn test_sha256_empty() {
    unsafe {
        let mut hex: *mut c_char = std::ptr::null_mut();
        let result = crypto_sha256_hex(std::ptr::null(), 0, &mut hex);

        assert_eq!(result, 0);
        assert!(!hex.is_null());

        let hex_str = CStr::from_ptr(hex).to_str().unwrap();
        assert_eq!(hex_str, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

        crypto_free_string(hex);
    }
}

#[test]
fn test_sha256_known_vector() {
    unsafe {
        let data = b"abc";
        let mut hex: *mut c_char = std::ptr::null_mut();
        let result = crypto_sha256_hex(data.as_ptr(), data.len(), &mut hex);

        assert_eq!(result, 0);

        let hex_str = CStr::from_ptr(hex).to_str().unwrap();
        assert_eq!(hex_str, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

        crypto_free_string(hex);
    }
}

#[test]
fn test_null_output() {
    unsafe {
        let data = b"test";
        let result = crypto_sha256_hex(data.as_ptr(), data.len(), std::ptr::null_mut());

        assert_eq!(result, -1);  // 应该返回错误
    }
}
```

---

## 7. 性能优化技巧

```
┌─────────────────────────────────────────────────────────────────┐
│                  FFI性能优化指南                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  1. 最小化边界跨越                                               │
│     差: 循环中每次迭代调用C函数                                 │
│     好: 批量处理，减少调用次数                                  │
│                                                                  │
│  2. 避免不必要的拷贝                                             │
│     差: Vec → 切片 → C数组 → 拷贝回Rust                        │
│     好: 直接传递指针，C借用Rust数据                             │
│                                                                  │
│  3. 批量分配                                                     │
│     差: 每次返回都分配新字符串                                  │
│     好: 预分配缓冲区，传递可写缓冲区                            │
│                                                                  │
│  4. 内联小函数                                                   │
│     • 使用#[inline]跨边界                                       │
│     • LTO (Link Time Optimization)                              │
│                                                                  │
│  5. 零拷贝序列化                                                 │
│     • 使用repr(C)结构体                                         │
│     • 避免序列化/反序列化开销                                   │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 8. 调试与故障排除

```
常见FFI问题诊断
═══════════════════════════════════════════════════════════════════

问题1: 段错误 (Segmentation Fault)
原因:
├── 空指针解引用
├── 野指针 (Use-After-Free)
├── 缓冲区溢出
└── 类型不匹配

诊断:
• 使用AddressSanitizer (ASan)
  RUSTFLAGS="-Z sanitizer=address" cargo build

• 使用Valgrind
  valgrind --tool=memcheck ./program

问题2: 内存泄漏
原因:
├── Rust分配，C未释放
├── C分配，Rust未释放
└── 循环引用

诊断:
• Valgrind memcheck
• 自定义分配跟踪

问题3: ABI不匹配
症状:
├── 奇怪的数值结果
├── 栈损坏
└── 随机崩溃

检查:
• 确保使用extern "C"
• 验证结构体repr(C)
• 检查调用约定
```

---

## 9. 安全检查清单

```
FFI代码审查检查清单
═══════════════════════════════════════════════════════════════════

安全性:
☐ 所有FFI函数都有文档化Safety要求
☐ unsafe块尽可能小
☐ 所有指针参数都检查null
☐ 所有长度参数都验证
☐ 字符串正确null终止
☐ 没有transmute
☐ panic边界已处理

正确性:
☐ 类型大小匹配 (sizeof检查)
☐ 对齐要求满足
☐ 生命周期标注正确
☐ Drop实现正确
☐ 错误码正确转换

性能:
☐ 最小化跨边界调用
☐ 避免不必要的拷贝
☐ 批量处理数据

可维护性:
☐ 有充分的文档注释
☐ 有C使用示例
☐ 测试覆盖所有路径
☐ CI包含C编译测试
```

---

**相关文档：**

- [08_Incremental_Migration_Guide.md](./08_Incremental_Migration_Guide.md) - 渐进式迁移指南
- [06_Migration_Case_Studies.md](./06_Migration_Case_Studies.md) - 迁移案例研究

---

**参考资源：**

- [Rust FFI Omnibus](https://jakegoulding.com/rust-ffi-omnibus/)
- [The Rustonomicon - FFI](https://doc.rust-lang.org/nomicon/ffi.html)
- [bindgen User Guide](https://rust-lang.github.io/rust-bindgen/)
- [cbindgen Documentation](https://docs.rs/cbindgen/latest/cbindgen/)

---

**最后更新**: 2026-03-18


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
