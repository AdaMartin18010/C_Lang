# C ↔ Rust FFI：跨语言互操作完全指南

> **Rust 版本**: 1.78+
> **C 标准**: C11/C17/C23
> **核心目标**: 安全、零开销的 C/Rust 边界设计

---

## 1. 为什么需要 C ↔ Rust FFI

### 1.1 迁移驱动力

| 场景 | 策略 | 示例 |
|:-----|:-----|:-----|
| **渐进式迁移** | 新模块用 Rust，遗留系统保持 C | Linux 内核驱动 |
| **性能热点** | C 框架 + Rust 算法库 | 游戏引擎 |
| **安全加固** | C 输入层 → Rust 处理核心 | 网络协议栈 |
| **生态复用** | Rust 调用 C 库 (OpenSSL, zlib) | 几乎所有 Rust 项目 |

### 1.2 FFI 边界风险

```
┌─────────────┐      FFI 边界      ┌─────────────┐
│    Rust     │ ←── 不安全地带 ──→ │      C      │
│  (内存安全)  │                    │ (手动管理)   │
└─────────────┘                    └─────────────┘
         ↑
    所有 FFI 调用本质上都是 unsafe
    Rust 编译器无法验证 C 代码的正确性
```

---

## 2. 基础：C 调用 Rust

### 2.1 Rust 库暴露 C 接口

```rust
// rustlib/src/lib.rs
use std::ffi::{c_char, c_int, CStr, CString};
use std::ptr;

/// 计算两个整数的和
/// 
/// # Safety
/// 无特殊要求，所有输入都有效
#[no_mangle]
pub extern "C" fn rust_add(a: c_int, b: c_int) -> c_int {
    a + b
}

/// 处理字符串，返回新分配的字符串
/// 
/// # Safety
/// - `input` 必须是有效的 null-terminated C 字符串
/// - 调用方必须使用 `rust_free_string` 释放返回值
#[no_mangle]
pub unsafe extern "C" fn rust_process_string(input: *const c_char) -> *mut c_char {
    if input.is_null() {
        return ptr::null_mut();
    }
    
    let c_str = match CStr::from_ptr(input).to_str() {
        Ok(s) => s,
        Err(_) => return ptr::null_mut(),  // 无效 UTF-8
    };
    
    let processed = format!("Rust processed: {}", c_str.to_uppercase());
    match CString::new(processed) {
        Ok(c_string) => c_string.into_raw(),  // 转移所有权到 C
        Err(_) => ptr::null_mut(),
    }
}

/// 释放 Rust 分配的字符串
/// 
/// # Safety
/// - `s` 必须是由 `rust_process_string` 返回的指针
/// - 只能释放一次
#[no_mangle]
pub unsafe extern "C" fn rust_free_string(s: *mut c_char) {
    if !s.is_null() {
        let _ = CString::from_raw(s);  // 回收所有权并丢弃
    }
}
```

### 2.2 C 代码调用 Rust

```c
// main.c
#include <stdio.h>
#include <stdlib.h>

// Rust 函数声明
extern int rust_add(int a, int b);
extern char *rust_process_string(const char *input);
extern void rust_free_string(char *s);

int main(void) {
    // 调用简单函数
    int result = rust_add(40, 2);
    printf("rust_add(40, 2) = %d\n", result);
    
    // 调用字符串处理函数
    char *processed = rust_process_string("hello ffi");
    if (processed != NULL) {
        printf("%s\n", processed);
        rust_free_string(processed);  // 必须释放！
    }
    
    return 0;
}
```

### 2.3 构建配置 (Cargo + 静态库)

```toml
# rustlib/Cargo.toml
[package]
name = "rustlib"
version = "0.1.0"
edition = "2021"

[lib]
name = "rustlib"
crate-type = ["staticlib", "cdylib"]  # 同时生成静态库和动态库

[profile.release]
opt-level = 3
lto = true          # 链接时优化，减少 FFI 边界开销
panic = "abort"     # 避免 Rust panic 展开干扰 C 栈
```

```makefile
# Makefile
CC = gcc
RUST_DIR = rustlib

.PHONY: all clean

all: main

$(RUST_DIR)/target/release/librustlib.a:
	cd $(RUST_DIR) && cargo build --release

main: main.c $(RUST_DIR)/target/release/librustlib.a
	$(CC) -o $@ $< -L$(RUST_DIR)/target/release -lrustlib -lpthread -ldl

clean:
	rm -f main
	cd $(RUST_DIR) && cargo clean
```

---

## 3. 基础：Rust 调用 C

### 3.1 C 库头文件

```c
// clib/include/calc.h
#ifndef CALC_H
#define CALC_H

#ifdef __cplusplus
extern "C" {
#endif

// 简单计算
double c_sqrt_sum(double a, double b);

// 数据结构
typedef struct {
    double x;
    double y;
} Point;

double c_distance(const Point *p1, const Point *p2);

// 回调函数类型
typedef void (*ProgressCallback)(int percent, void *user_data);

// 异步处理（带回调）
int c_process_async(const char *input, 
                     ProgressCallback callback, 
                     void *user_data);

#ifdef __cplusplus
}
#endif

#endif
```

### 3.2 Rust FFI 声明

```rust
// src/ffi.rs
use std::ffi::{c_char, c_double, c_int, c_void, CStr};
use std::ptr;

// 直接映射 C 结构体
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Point {
    pub x: c_double,
    pub y: c_double,
}

// 回调函数类型（extern "C" 函数指针）
pub type ProgressCallback = Option<
    extern "C" fn(percent: c_int, user_data: *mut c_void)
>;

extern "C" {
    // 简单函数
    pub fn c_sqrt_sum(a: c_double, b: c_double) -> c_double;
    
    // 结构体操作
    pub fn c_distance(p1: *const Point, p2: *const Point) -> c_double;
    
    // 回调函数
    pub fn c_process_async(
        input: *const c_char,
        callback: ProgressCallback,
        user_data: *mut c_void,
    ) -> c_int;
}
```

### 3.3 安全封装层

```rust
// src/lib.rs
mod ffi;
pub use ffi::Point;

use std::ffi::{c_int, c_void, CString};
use std::ptr;

/// 安全的 sqrt_sum 包装
pub fn sqrt_sum(a: f64, b: f64) -> f64 {
    unsafe { ffi::c_sqrt_sum(a, b) }
}

/// 安全的 distance 包装
pub fn distance(p1: &Point, p2: &Point) -> f64 {
    unsafe { ffi::c_distance(p1, p2) }
}

/// 带回调的异步处理（安全封装）
pub fn process_async<F>(input: &str, mut callback: F) -> Result<(), String>
where
    F: FnMut(i32),
{
    let c_input = CString::new(input)
        .map_err(|e| format!("Invalid input string: {}", e))?;
    
    // 将 Rust 闭包转换为 C 可调用的函数指针
    // 使用 trampoline 模式
    extern "C" fn trampoline(percent: c_int, user_data: *mut c_void) {
        unsafe {
            let closure = &mut *(user_data as *mut &mut dyn FnMut(i32));
            closure(percent);
        }
    }
    
    let mut closure: &mut dyn FnMut(i32) = &mut callback;
    let user_data = &mut closure as *mut _ as *mut c_void;
    
    let result = unsafe {
        ffi::c_process_async(
            c_input.as_ptr(),
            Some(trampoline),
            user_data,
        )
    };
    
    if result == 0 {
        Ok(())
    } else {
        Err(format!("c_process_async failed with code {}", result))
    }
}
```

---

## 4. 所有权与生命周期管理

### 4.1 内存所有权转移规则

```
┌─────────────────────────────────────────────────────────────┐
│                    FFI 内存所有权约定                          │
├─────────────────────────────────────────────────────────────┤
│  C 分配 → C 释放:  malloc() / free()                          │
│  Rust 分配 → Rust 释放:  Box::into_raw() / Box::from_raw()    │
│  跨边界转移: 必须通过文档明确约定                               │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Rust → C：释放 Rust 分配的内存

```rust
// ❌ 错误：C 调用 free() 释放 Rust 分配的内存在某些分配器上不兼容

// ✅ 正确：提供 Rust 释放函数
#[no_mangle]
pub unsafe extern "C" fn rust_free_buffer(ptr: *mut u8, len: usize) {
    if !ptr.is_null() {
        let _ = Vec::from_raw_parts(ptr, 0, len);  // 回收 Vec
    }
}

// 更安全的模式：使用不透明句柄
type BufferHandle = *mut c_void;

#[no_mangle]
pub extern "C" fn rust_buffer_create(size: usize) -> BufferHandle {
    let vec = vec![0u8; size];
    let boxed = vec.into_boxed_slice();
    Box::into_raw(boxed) as BufferHandle
}

#[no_mangle]
pub unsafe extern "C" fn rust_buffer_free(handle: BufferHandle) {
    if !handle.is_null() {
        let _ = Box::from_raw(handle as *mut [u8]);
    }
}

#[no_mangle]
pub unsafe extern "C" fn rust_buffer_data(handle: BufferHandle) -> *mut u8 {
    if handle.is_null() {
        return ptr::null_mut();
    }
    let slice = &mut *(handle as *mut [u8]);
    slice.as_mut_ptr()
}
```

### 4.3 C → Rust：借用 vs 拥有

```rust
/// 借用 C 的内存（不释放）
/// 
/// # Safety
/// `data` 必须在返回的切片使用期间保持有效
pub unsafe fn borrow_c_slice<'a>(data: *const u8, len: usize) -> &'a [u8] {
    std::slice::from_raw_parts(data, len)
}

/// 复制 C 的内存到 Rust（Rust 拥有副本）
pub fn copy_c_string(ptr: *const c_char) -> Option<String> {
    if ptr.is_null() {
        return None;
    }
    unsafe {
        CStr::from_ptr(ptr)
            .to_str()
            .ok()
            .map(|s| s.to_owned())
    }
}
```

---

## 5. 错误处理与 Panic 安全

### 5.1 Rust Panic 穿越 FFI 边界 = UB

```rust
// ❌ 极度危险：panic 会展开 C 栈，导致未定义行为
#[no_mangle]
pub extern "C" fn dangerous_function(input: *const c_char) -> c_int {
    let s = unsafe { CStr::from_ptr(input).to_str().unwrap() };  // 可能 panic!
    // ...
}

// ✅ 正确：捕获 panic，转换为错误码
use std::panic::catch_unwind;

#[no_mangle]
pub extern "C" fn safe_function(input: *const c_char) -> c_int {
    let result = catch_unwind(|| {
        if input.is_null() {
            return -1;
        }
        let s = match unsafe { CStr::from_ptr(input).to_str() } {
            Ok(s) => s,
            Err(_) => return -2,
        };
        // 正常处理...
        0
    });
    
    match result {
        Ok(code) => code,
        Err(_) => -999,  // panic 发生
    }
}
```

### 5.2 Cargo.toml Panic 配置

```toml
[profile.release]
panic = "abort"  # 禁止展开，直接 abort（推荐用于库）

# 或捕获所有 panic
[dependencies]
# 使用 panic_hook 记录 panic 信息后再 abort
```

### 5.3 C 侧的错误码约定

```c
// 统一的错误码定义（C 和 Rust 共享）
#ifndef FFI_ERROR_CODES_H
#define FFI_ERROR_CODES_H

#define FFI_OK              0
#define FFI_ERR_NULL_PTR   -1
#define FFI_ERR_INVALID_UTF8 -2
#define FFI_ERR_OOM        -3
#define FFI_ERR_INVALID_ARG -4
#define FFI_ERR_RUST_PANIC -999

#endif
```

```rust
// Rust 侧使用相同错误码
#[repr(i32)]
pub enum FfiError {
    Ok = 0,
    NullPtr = -1,
    InvalidUtf8 = -2,
    Oom = -3,
    InvalidArg = -4,
    RustPanic = -999,
}
```

---

## 6. 自动化绑定生成

### 6.1 bindgen：C → Rust

```bash
# 安装
cargo install bindgen-cli

# 生成绑定
bindgen clib/include/calc.h \
    --ctypes-prefix "libc" \
    --no-layout-tests \
    --output src/bindings.rs \
    -- -Iclib/include
```

```rust
// build.rs - 自动构建时生成
use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-search=native=clib/lib");
    println!("cargo:rustc-link-lib=static=calc");
    
    let bindings = bindgen::Builder::default()
        .header("clib/include/calc.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings");
    
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings");
}
```

### 6.2 cbindgen：Rust → C

```toml
# cbindgen.toml
language = "C"
header = "/* Auto-generated C bindings */"
include_guard = "RUSTLIB_H"

[parse]
parse_deps = false

[export]
prefix = "rust_"
```

```bash
# 生成 C 头文件
cbindgen --config cbindgen.toml --crate rustlib --output rustlib.h
```

---

## 7. 常见陷阱与最佳实践

### 7.1 陷阱清单

| 陷阱 | 后果 | 解决方案 |
|:-----|:-----|:---------|
| `CString::from_raw` 后继续使用 | use-after-free | 立即将原指针设为 null |
| C 调用 `free()` 释放 Rust 内存 | 堆损坏 | 始终提供配套的 Rust 释放函数 |
| Panic 穿越 FFI | 未定义行为 | `catch_unwind` + `panic = "abort"` |
| `&T` 来自可能为 null 的指针 | 段错误 | 始终检查 null，返回 Option |
| `bool` 类型不匹配 | 未定义行为 | C 使用 `_Bool`/`bool`，Rust 使用 `bool` |
| `char` 符号差异 | 数据错误 | FFI 中始终使用 `c_char` |
| 可变全局状态 | 数据竞争 | 使用 `Mutex` 或线程局部存储 |

### 7.2 类型映射速查表

| C 类型 | Rust 类型 | 说明 |
|:-------|:----------|:-----|
| `char` | `c_char` | 注意可能是 signed/unsigned |
| `short` | `c_short` | |
| `int` | `c_int` | 至少 16 位 |
| `long` | `c_long` | 平台相关（LP64/LLP64） |
| `long long` | `c_longlong` | 至少 64 位 |
| `float` | `c_float` | IEEE 754 binary32 |
| `double` | `c_double` | IEEE 754 binary64 |
| `void*` | `*mut c_void` | 不透明指针 |
| `char*` | `*mut c_char` | 需配合 CStr/CString |
| `struct T` | `#[repr(C)] struct T` | 显式 repr(C) |
| `enum T` | `#[repr(C)] enum T` | 或 `#[repr(u32)]` |
| `bool` | `bool` | C23 `_Bool` / `<stdbool.h>` |
| `size_t` | `usize` | |
| `intptr_t` | `isize` | |

---

## 8. 完整项目模板

```
project/
├── Cargo.toml              # Rust 项目配置
├── build.rs                # 构建脚本（bindgen）
├── c_api.h                 # 对外暴露的 C API（cbindgen 生成）
├── src/
│   ├── lib.rs              # Rust 库入口
│   ├── ffi.rs              # C 函数声明（bindgen 生成）
│   └── safe_wrapper.rs     # 安全封装层
├── c_example/
│   ├── main.c              # C 调用示例
│   └── Makefile
└── tests/
    └── integration_test.rs # Rust 侧集成测试
```

---

> **最后更新**: 2026-05-11
> **参考**: The Rust FFI Omnibus, Rustnomicon (FFI 章节), bindgen 文档
