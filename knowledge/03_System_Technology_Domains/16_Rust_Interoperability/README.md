# Rust 与 C 互操作


---

## 📑 目录

- [Rust 与 C 互操作](#rust-与-c-互操作)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [FFI 基础](#ffi-基础)
    - [调用约定与链接](#调用约定与链接)
    - [Rust 调用 C 代码](#rust-调用-c-代码)
    - [C 代码（被 Rust 调用）](#c-代码被-rust-调用)
  - [C 调用 Rust 代码](#c-调用-rust-代码)
    - [Rust 库暴露 C 接口](#rust-库暴露-c-接口)
    - [Cargo.toml 配置](#cargotoml-配置)
  - [cbindgen 自动生成头文件](#cbindgen-自动生成头文件)
    - [cbindgen 配置](#cbindgen-配置)
    - [Rust 类型定义示例](#rust-类型定义示例)
    - [生成的 C 头文件](#生成的-c-头文件)
    - [使用 cbindgen 的构建脚本](#使用-cbindgen-的构建脚本)
  - [类型映射](#类型映射)
    - [基础类型对照表](#基础类型对照表)
    - [复杂类型映射](#复杂类型映射)
    - [C 头文件中使用 Rust 类型](#c-头文件中使用-rust-类型)
  - [unsafe Rust 模式](#unsafe-rust-模式)
    - [Unsafe 代码块规范](#unsafe-代码块规范)
    - [可变性与别名规则](#可变性与别名规则)
  - [内存管理](#内存管理)
    - [所有权转移规则](#所有权转移规则)
    - [自定义 Drop 处理](#自定义-drop-处理)
  - [完整示例项目](#完整示例项目)
    - [构建配置](#构建配置)
  - [延伸阅读](#延伸阅读)


---

## 概述

Rust 与 C 的互操作是现代系统编程的重要课题。Rust 提供了强大的 FFI（Foreign Function Interface）能力，使其能够与 C 代码无缝集成。本章节深入探讨 FFI 基础、cbindgen 工具、unsafe Rust 模式、类型映射策略和内存管理最佳实践。

---

## FFI 基础

### 调用约定与链接

```
Rust 与 C 互操作模型：

┌──────────────────────────────────────────────────────────────┐
│                     Rust Application                         │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                    Safe Rust Code                       │ │
│  │  fn main() { unsafe { call_c_function(); } }            │ │
│  └────────────────────────────────────────────────────────┘ │
│                          │                                    │
│                          ▼                                    │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                   FFI Boundary (extern "C")            │ │
│  │  extern "C" {                                          │ │
│  │      fn call_c_function();                             │ │
│  │  }                                                     │ │
│  └────────────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────┘
                              │
                              │ 系统 ABI (C calling convention)
                              ▼
┌──────────────────────────────────────────────────────────────┐
│                        C Library                             │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                    C Header                             │ │
│  │  void call_c_function(void);                           │ │
│  └────────────────────────────────────────────────────────┘ │
│                          │                                    │
│                          ▼                                    │
│  ┌────────────────────────────────────────────────────────┐ │
│  │                    C Implementation                     │ │
│  │  void call_c_function(void) { /* ... */ }              │ │
│  └────────────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────┘
```

### Rust 调用 C 代码

```rust
// Rust 代码：调用 C 库

// 声明 C 函数接口
#[link(name = "myclib")]  // 链接 libmyclib.so 或 myclib.dll
extern "C" {
    // 基本类型函数
    fn c_add(a: i32, b: i32) -> i32;
    fn c_strlen(s: *const c_char) -> usize;
    fn c_malloc(size: usize) -> *mut c_void;
    fn c_free(ptr: *mut c_void);

    // 回调函数类型
    type CallbackFn = extern "C" fn(data: *mut c_void, len: usize);
    fn c_register_callback(cb: CallbackFn, user_data: *mut c_void);
}

// 安全包装函数
pub fn safe_add(a: i32, b: i32) -> i32 {
    unsafe { c_add(a, b) }
}

use std::ffi::{CStr, CString, c_char, c_void};

pub fn safe_strlen(s: &str) -> Result<usize, std::ffi::NulError> {
    let c_str = CString::new(s)?;
    Ok(unsafe { c_strlen(c_str.as_ptr()) })
}

// 管理 C 分配的内存
pub struct CMemory {
    ptr: *mut c_void,
    size: usize,
}

impl CMemory {
    pub fn new(size: usize) -> Option<Self> {
        let ptr = unsafe { c_malloc(size) };
        if ptr.is_null() {
            None
        } else {
            Some(CMemory { ptr, size })
        }
    }

    pub fn as_ptr(&self) -> *mut c_void {
        self.ptr
    }

    pub fn size(&self) -> usize {
        self.size
    }
}

impl Drop for CMemory {
    fn drop(&mut self) {
        if !self.ptr.is_null() {
            unsafe { c_free(self.ptr) }
        }
    }
}
```

### C 代码（被 Rust 调用）

```c
/* myclib.c - C 库实现 */
#include <stdlib.h>
#include <string.h>

int c_add(int a, int b) {
    return a + b;
}

size_t c_strlen(const char *s) {
    return strlen(s);
}

// 回调机制
static void (*g_callback)(void *data, size_t len) = NULL;
static void *g_user_data = NULL;

void c_register_callback(void (*cb)(void *, size_t), void *user_data) {
    g_callback = cb;
    g_user_data = user_data;
}

void c_trigger_callback(const void *data, size_t len) {
    if (g_callback) {
        g_callback(g_user_data, len);
    }
}
```

---

## C 调用 Rust 代码

### Rust 库暴露 C 接口

```rust
// Rust 库：lib.rs - 编译为动态库

use std::ffi::{CStr, CString, c_char, c_int, c_void};
use std::slice;

// 公开 C 可调用的函数
#[no_mangle]  // 禁止 Rust 名称修饰
pub extern "C" fn rust_add(a: c_int, b: c_int) -> c_int {
    a + b
}

#[no_mangle]
pub extern "C" fn rust_process_data(
    data: *const u8,
    len: usize,
    out_len: *mut usize
) -> *mut u8 {
    if data.is_null() || out_len.is_null() {
        return std::ptr::null_mut();
    }

    // 安全地将 C 指针转为 Rust slice
    let input = unsafe { slice::from_raw_parts(data, len) };

    // Rust 处理逻辑
    let result: Vec<u8> = input.iter().map(|&x| x.wrapping_mul(2)).collect();

    let result_len = result.len();
    let result_ptr = result.as_ptr() as *mut u8;

    // 忘记 Vec，防止 Rust 释放内存（C 负责释放）
    std::mem::forget(result);

    unsafe { *out_len = result_len; }
    result_ptr
}

#[no_mangle]
pub extern "C" fn rust_free_data(ptr: *mut u8, len: usize) {
    if !ptr.is_null() {
        unsafe {
            let _ = Vec::from_raw_parts(ptr, len, len);
            // Vec 在这里被 drop，内存被释放
        }
    }
}

// 字符串处理
#[no_mangle]
pub extern "C" fn rust_reverse_string(s: *const c_char) -> *mut c_char {
    if s.is_null() {
        return std::ptr::null_mut();
    }

    let c_str = unsafe { CStr::from_ptr(s) };
    let mut rust_str = c_str.to_string_lossy().into_owned();
    rust_str = rust_str.chars().rev().collect();

    match CString::new(rust_str) {
        Ok(cstring) => cstring.into_raw(),  // 转移所有权到 C
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn rust_free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe {
            let _ = CString::from_raw(s);
            // CString 在这里被 drop
        }
    }
}

// 错误处理 - 返回结果结构体
#[repr(C)]
pub struct RustResult {
    pub success: bool,
    pub data: *mut c_void,
    pub error_msg: *mut c_char,
}

#[no_mangle]
pub extern "C" fn rust_compute(value: c_int) -> RustResult {
    if value < 0 {
        let error = CString::new("Value must be non-negative").unwrap();
        return RustResult {
            success: false,
            data: std::ptr::null_mut(),
            error_msg: error.into_raw(),
        };
    }

    let result = Box::into_raw(Box::new(value * value));

    RustResult {
        success: true,
        data: result as *mut c_void,
        error_msg: std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn rust_free_result(result: RustResult) {
    if !result.data.is_null() {
        unsafe { let _ = Box::from_raw(result.data as *mut c_int); }
    }
    if !result.error_msg.is_null() {
        unsafe { let _ = CString::from_raw(result.error_msg); }
    }
}
```

### Cargo.toml 配置

```toml
[package]
name = "rust_ffi_lib"
version = "0.1.0"
edition = "2021"

[lib]
name = "rustffi"
crate-type = ["cdylib", "staticlib"]  # 生成动态库和静态库

[dependencies]

[profile.release]
opt-level = 3
lto = true  # 链接时优化，减小库体积
```

---

## cbindgen 自动生成头文件

### cbindgen 配置

```toml
# cbindgen.toml - cbindgen 配置文件

language = "C"
cpp_compat = true
header = "/* Auto-generated by cbindgen */"

# 命名规则
[export]
prefix = "rust_"
item_types = ["globals", "enums", "structs", "unions", "typedefs", "opaque", "functions"]

[fn]
rename_types = "SnakeCase"

[enum]
rename_variants = "ScreamingSnakeCase"

[parse]
parse_deps = false

[struct]
rename_fields = "SnakeCase"
```

### Rust 类型定义示例

```rust
// types.rs - cbindgen 会为此生成 C 头文件

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Point {
    pub x: f64,
    pub y: f64,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Rectangle {
    pub top_left: Point,
    pub bottom_right: Point,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub enum StatusCode {
    Ok = 0,
    InvalidInput = 1,
    OutOfMemory = 2,
    IoError = 3,
}

#[repr(C)]
pub struct Buffer {
    pub data: *mut u8,
    pub len: usize,
    pub capacity: usize,
}

// Opaque 类型（隐藏实现细节）
pub struct DatabaseConnection {
    inner: rusqlite::Connection,
}

#[no_mangle]
pub extern "C" fn db_connect(path: *const c_char) -> *mut DatabaseConnection {
    // 实现...
    std::ptr::null_mut()
}

#[no_mangle]
pub extern "C" fn db_disconnect(conn: *mut DatabaseConnection) {
    // 实现...
}
```

### 生成的 C 头文件

```c
/* Auto-generated by cbindgen */

#ifndef RUSTFFI_H
#define RUSTFFI_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point top_left;
    Point bottom_right;
} Rectangle;

typedef enum {
    STATUS_CODE_OK = 0,
    STATUS_CODE_INVALID_INPUT = 1,
    STATUS_CODE_OUT_OF_MEMORY = 2,
    STATUS_CODE_IO_ERROR = 3,
} StatusCode;

typedef struct {
    uint8_t *data;
    size_t len;
    size_t capacity;
} Buffer;

// Opaque 类型
typedef struct DatabaseConnection DatabaseConnection;

// 函数声明
int32_t rust_add(int32_t a, int32_t b);
StatusCode rust_process_rectangle(const Rectangle *rect, Rectangle *out);
DatabaseConnection *db_connect(const char *path);
void db_disconnect(DatabaseConnection *conn);

#endif /* RUSTFFI_H */
```

### 使用 cbindgen 的构建脚本

```bash
#!/bin/bash
# generate_headers.sh

# 安装 cbindgen
cargo install cbindgen

# 生成头文件
cbindgen --lang c --crate rust_ffi_lib --output include/rustffi.h

# 或者使用 toml 配置
cbindgen -c cbindgen.toml --crate rust_ffi_lib --output include/rustffi.h
```

---

## 类型映射

### 基础类型对照表

```
Rust 类型            C 类型                  说明
─────────────────────────────────────────────────────────────
i8/i16/i32/i64      int8_t/int16_t/...     有符号整数
u8/u16/u32/u64      uint8_t/uint16_t/...   无符号整数
f32/f64             float/double           浮点数
bool                bool (_Bool in C99)    布尔值
char                N/A                    Unicode 标量
i8 (特别用法)        char                   C 字符
()                  void                   空类型
*const T            const T*               常量指针
*mut T              T*                     可变指针
&'a T               const T*               引用（生存期）
&'a mut T           T*                     可变引用
```

### 复杂类型映射

```rust
// Rust 类型映射示例
use std::ffi::*;

// 字符串类型
// C: const char*  <-> Rust: *const c_char 或 &CStr
// C: char*        <-> Rust: *mut c_char 或 CString

// 数组和切片
// C: T* + size_t  <-> Rust: &[T] 或 &mut [T]

// 回调函数
// C: typedef void (*callback_t)(int, void*);
// Rust: extern "C" fn(i32, *mut c_void)

// 结构体
#[repr(C)]
pub struct Config {
    pub name: *const c_char,     // C: const char*
    pub value: c_int,             // C: int
    pub flags: c_uint,            // C: unsigned int
    pub data: *mut c_void,        // C: void*
    pub callback: Option<extern "C" fn(*mut c_void)>,  // C: void (*)(void*)
}

// 变长数组
#[no_mangle]
pub extern "C" fn process_items(
    items: *const c_int,
    count: usize
) -> c_int {
    if items.is_null() {
        return -1;
    }

    let slice = unsafe { std::slice::from_raw_parts(items, count) };
    slice.iter().sum::<i32>() as c_int
}

// 可选指针（可空）
#[no_mangle]
pub extern "C" fn optional_process(
    data: *const c_char  // 可以为 null
) -> c_int {
    if data.is_null() {
        println!("No data provided");
        return 0;
    }

    let c_str = unsafe { CStr::from_ptr(data) };
    println!("Received: {:?}", c_str);
    1
}
```

### C 头文件中使用 Rust 类型

```c
/* rust_types.h - C 端使用的类型定义 */

#ifndef RUST_TYPES_H
#define RUST_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 对应 Rust 的 Option<*mut T>
typedef struct {
    void *ptr;
    bool is_some;
} RustOptionPtr;

// 对应 Rust 的 Result<T, E>（简化）
typedef struct {
    int32_t value;
    uint32_t error_code;
    bool is_ok;
} RustResultI32;

// 字符串视图（零拷贝）
typedef struct {
    const char *ptr;
    size_t len;
} RustStr;

// 切片视图
typedef struct {
    const uint8_t *ptr;
    size_t len;
} RustSliceU8;

#endif
```

---

## unsafe Rust 模式

### Unsafe 代码块规范

```rust
// unsafe 使用原则与模式

pub struct SafeWrapper {
    // 内部包含 unsafe 指针，但对外提供安全接口
    raw_ptr: *mut c_void,
    size: usize,
}

impl SafeWrapper {
    pub fn new(size: usize) -> Option<Self> {
        let ptr = unsafe { libc::malloc(size) };
        if ptr.is_null() {
            None
        } else {
            Some(SafeWrapper { raw_ptr: ptr, size })
        }
    }

    pub fn get(&self, index: usize) -> Option<u8> {
        if index >= self.size {
            return None;
        }

        // SAFETY: 我们已经检查了边界，且指针有效
        unsafe {
            Some(*(self.raw_ptr as *const u8).add(index))
        }
    }

    pub fn set(&mut self, index: usize, value: u8) -> bool {
        if index >= self.size {
            return false;
        }

        // SAFETY: 同上
        unsafe {
            *(self.raw_ptr as *mut u8).add(index) = value;
        }
        true
    }
}

impl Drop for SafeWrapper {
    fn drop(&mut self) {
        // SAFETY: raw_ptr 由我们分配，且只在此释放
        unsafe {
            libc::free(self.raw_ptr);
        }
    }
}

// 线程安全的 unsafe 封装
use std::sync::Mutex;

pub struct ThreadSafeContext {
    inner: Mutex<*mut c_void>,
}

impl ThreadSafeContext {
    pub fn call_c_function(&self) {
        let ptr = self.inner.lock().unwrap();

        // SAFETY: 我们持有锁，且指针在使用期间不会被修改
        unsafe {
            c_function(*ptr);
        }
    }
}
```

### 可变性与别名规则

```rust
// 严格遵守 Rust 的别名规则

// 错误：同时创建可变和不可变引用
pub unsafe fn broken_alias(data: *mut i32) {
    let ref1 = &*data;      // 不可变借用
    let ref2 = &mut *data;  // 可变借用 - 未定义行为！

    println!("{} {}", ref1, ref2);
}

// 正确：确保借用不重叠
pub unsafe fn correct_alias(data: *mut i32) {
    {
        let ref1 = &*data;
        println!("{}", ref1);
    }  // ref1 在这里结束

    {
        let ref2 = &mut *data;
        *ref2 += 1;
        println!("{}", ref2);
    }  // ref2 在这里结束
}
```

---

## 内存管理

### 所有权转移规则

```rust
// Rust 与 C 之间的所有权转移

// 情况 1：Rust 分配，C 释放
#[no_mangle]
pub extern "C" fn rust_alloc_buffer(size: usize) -> *mut u8 {
    let mut vec = vec![0u8; size];
    let ptr = vec.as_mut_ptr();
    std::mem::forget(vec);  // 防止 Rust 释放
    ptr
}

#[no_mangle]
pub extern "C" fn rust_free_buffer(ptr: *mut u8, size: usize) {
    if !ptr.is_null() {
        unsafe {
            let _ = Vec::from_raw_parts(ptr, size, size);
            // Vec 被 drop，内存释放
        }
    }
}

// 情况 2：C 分配，Rust 使用，C 释放
pub fn use_c_buffer(ptr: *const u8, len: usize) {
    // 仅借用，不获取所有权
    let slice = unsafe { std::slice::from_raw_parts(ptr, len) };
    // 处理 slice...
    // 函数返回后，C 仍然拥有内存
}

// 情况 3：引用计数共享
use std::sync::Arc;

pub struct SharedResource {
    data: Arc<Vec<u8>>,
}

#[no_mangle]
pub extern "C" fn shared_resource_new(data: *const u8, len: usize) -> *mut SharedResource {
    let slice = unsafe { std::slice::from_raw_parts(data, len) };
    let resource = SharedResource {
        data: Arc::new(slice.to_vec()),
    };
    Box::into_raw(Box::new(resource))
}

#[no_mangle]
pub extern "C" fn shared_resource_clone(res: *mut SharedResource) -> *mut SharedResource {
    if res.is_null() { return std::ptr::null_mut(); }

    unsafe {
        let new_res = SharedResource {
            data: Arc::clone(&(*res).data),
        };
        Box::into_raw(Box::new(new_res))
    }
}

#[no_mangle]
pub extern "C" fn shared_resource_free(res: *mut SharedResource) {
    if !res.is_null() {
        unsafe { let _ = Box::from_raw(res); }
    }
}
```

### 自定义 Drop 处理

```rust
// 为 C 资源实现 Drop trait

pub struct CFileHandle {
    handle: *mut libc::FILE,
}

impl CFileHandle {
    pub fn open(path: &str) -> Option<Self> {
        let c_path = std::ffi::CString::new(path).ok()?;
        let handle = unsafe { libc::fopen(c_path.as_ptr(), b"r\0".as_ptr() as *const i8) };

        if handle.is_null() {
            None
        } else {
            Some(CFileHandle { handle })
        }
    }

    pub fn read(&mut self, buf: &mut [u8]) -> usize {
        unsafe {
            libc::fread(
                buf.as_mut_ptr() as *mut c_void,
                1,
                buf.len(),
                self.handle
            )
        }
    }
}

impl Drop for CFileHandle {
    fn drop(&mut self) {
        if !self.handle.is_null() {
            unsafe {
                libc::fclose(self.handle);
            }
        }
    }
}
```

---

## 完整示例项目

### 构建配置

```toml
# Cargo.toml
[package]
name = "rust_c_bridge"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["cdylib", "staticlib"]

[dependencies]
libc = "0.2"

[build-dependencies]
cc = "1.0"
```

```rust
// build.rs
fn main() {
    // 编译 C 代码
    cc::Build::new()
        .file("src/native/mylib.c")
        .include("src/native")
        .compile("mylib");
}
```

---

## 延伸阅读

- [Rust FFI 指南](https://doc.rust-lang.org/nomicon/ffi.html)
- [cbindgen 文档](https://github.com/eqrion/cbindgen)
- [Rust 安全抽象](https://doc.rust-lang.org/nomicon/)
- [The Rust FFI Omnibus](http://jakegoulding.com/rust-ffi-omnibus/)


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
