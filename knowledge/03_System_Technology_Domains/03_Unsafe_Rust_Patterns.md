# Unsafe Rust模式与C互操作

## 目录

- [Unsafe Rust模式与C互操作](#unsafe-rust模式与c互操作)
  - [目录](#目录)
  - [Unsafe Rust概述](#unsafe-rust概述)
    - [什么是Unsafe Rust](#什么是unsafe-rust)
    - [Unsafe提供的五大能力](#unsafe提供的五大能力)
    - [Unsafe的三大使用场景](#unsafe的三大使用场景)
  - [原始指针](#原始指针)
    - [原始指针类型](#原始指针类型)
    - [原始指针 vs 引用](#原始指针-vs-引用)
    - [常见原始指针操作](#常见原始指针操作)
    - [切片与原始指针](#切片与原始指针)
  - [FFI边界](#ffi边界)
    - [FFI基础概念](#ffi基础概念)
    - [调用约定](#调用约定)
    - [C与Rust类型映射](#c与rust类型映射)
    - [内存布局控制](#内存布局控制)
    - [不透明类型](#不透明类型)
  - [Unsafe块最佳实践](#unsafe块最佳实践)
    - [1. 最小化unsafe范围](#1-最小化unsafe范围)
    - [2. 封装安全抽象](#2-封装安全抽象)
    - [3. 前置条件文档化](#3-前置条件文档化)
    - [4. 使用assert!进行运行时检查](#4-使用assert进行运行时检查)
    - [5. 利用类型系统](#5-利用类型系统)
  - [与C代码集成模式](#与c代码集成模式)
    - [模式1：单向调用C库](#模式1单向调用c库)
    - [模式2：C调用Rust回调](#模式2c调用rust回调)
    - [模式3：共享内存结构](#模式3共享内存结构)
    - [模式4：错误处理转换](#模式4错误处理转换)
    - [模式5：对象封装模式](#模式5对象封装模式)
    - [模式6：资源管理RAII](#模式6资源管理raii)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## Unsafe Rust概述

### 什么是Unsafe Rust

Safe Rust通过所有权系统和借用检查器在编译时保证内存安全，但某些底层操作无法被编译器验证，这时就需要使用`unsafe`关键字。

```rust
// 基本的unsafe块
unsafe {
    // 执行不安全操作
    let raw_ptr = &mut 5 as *mut i32;
    *raw_ptr = 10;
}
```

### Unsafe提供的五大能力

| 能力 | 说明 | 风险 |
|------|------|------|
| 解引用原始指针 | `*const T` 和 `*mut T` | 悬空指针、use-after-free |
| 调用unsafe函数 | `unsafe fn` | 违反函数前置条件 |
| 实现unsafe trait | `unsafe trait` | 破坏trait的不变式 |
| 访问或修改可变静态变量 | `static mut` | 数据竞争 |
| 访问union字段 | `union` | 类型混淆 |

### Unsafe的三大使用场景

1. **与底层系统交互**：操作系统API、硬件访问
2. **性能优化**：绕过某些安全检查的零成本抽象
3. **实现安全抽象**：在内部使用unsafe，对外暴露安全接口

---

## 原始指针

### 原始指针类型

```rust
// 不可变原始指针
let x = 5;
let r = &x as *const i32;

// 可变原始指针
let mut y = 10;
let m = &mut y as *mut i32;

// 解引用需要unsafe块
unsafe {
    println!("r points to: {}", *r);
    *m = 20;
}
```

### 原始指针 vs 引用

| 特性 | 原始指针 | 引用(&T/&mut T) |
|------|----------|-----------------|
| 可以为null | ✅ 允许 | ❌ 不允许 |
| 允许多个可变指针 | ✅ 允许 | ❌ 独占访问 |
| 自动解引用 | ❌ 需要unsafe | ✅ 自动 |
| 生命周期检查 | ❌ 无 | ✅ 编译器检查 |
| 野指针风险 | ⚠️ 高风险 | ✅ 编译器保护 |

### 常见原始指针操作

```rust
// 指针算术
let arr = [1, 2, 3, 4, 5];
let ptr = arr.as_ptr();

unsafe {
    // 偏移操作
    let second = ptr.offset(1);
    println!("Second element: {}", *second);

    // 计算偏移量
    let offset = second.offset_from(ptr);
    assert_eq!(offset, 1);
}

// 指针转换
let u8_ptr: *const u8 = &0u8;
let u32_ptr: *const u32 = u8_ptr as *const u32;

// 转换为整数地址
let addr = u8_ptr as usize;
```

### 切片与原始指针

```rust
fn split_at_mut(slice: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) {
    let len = slice.len();
    assert!(mid <= len);

    let ptr = slice.as_mut_ptr();

    unsafe {
        (
            std::slice::from_raw_parts_mut(ptr, mid),
            std::slice::from_raw_parts_mut(ptr.add(mid), len - mid),
        )
    }
}
```

---

## FFI边界

### FFI基础概念

FFI（Foreign Function Interface）允许Rust代码与其他语言（主要是C）进行互操作。

```rust
// 声明C标准库函数
extern "C" {
    fn printf(format: *const c_char, ...) -> c_int;
    fn malloc(size: usize) -> *mut c_void;
    fn free(ptr: *mut c_void);
}

// 使用C函数
unsafe {
    let msg = CString::new("Hello from Rust\n").unwrap();
    printf(msg.as_ptr());
}
```

### 调用约定

| ABI字符串 | 描述 |
|-----------|------|
| `"C"` | C调用约定（默认） |
| `"system"` | 平台系统调用约定 |
| `"cdecl"` | C声明调用约定 |
| `"stdcall"` | Windows API标准调用 |
| `"fastcall"` | 快速寄存器调用 |
| `"win64"` | Windows 64位调用 |

```rust
// 为函数指定ABI
#[no_mangle]
pub extern "C" fn rust_add(a: i32, b: i32) -> i32 {
    a + b
}
```

### C与Rust类型映射

| C类型 | Rust类型 | 说明 |
|-------|----------|------|
| `void*` | `*mut c_void` | 通用指针 |
| `const char*` | `*const c_char` | C字符串 |
| `int` | `c_int` | 平台相关整数 |
| `size_t` | `usize` | 无符号大小类型 |
| `struct T` | `#[repr(C)] struct T` | C兼容结构体 |
| `union U` | `#[repr(C)] union U` | C兼容联合体 |

### 内存布局控制

```rust
// 确保C兼容的内存布局
#[repr(C)]
pub struct Point {
    pub x: f64,
    pub y: f64,
}

// 对齐控制
#[repr(C, align(16))]
pub struct AlignedData {
    pub value: u64,
}

// _packed_结构体
#[repr(C, packed)]
pub struct PackedData {
    pub a: u8,
    pub b: u32, // 通常无填充
}
```

### 不透明类型

```rust
// C头文件中的不透明类型
typedef struct opaque_handle* handle_t;

// Rust中的对应声明
pub enum OpaqueHandle {}

extern "C" {
    pub fn create_handle() -> *mut OpaqueHandle;
    pub fn destroy_handle(handle: *mut OpaqueHandle);
    pub fn use_handle(handle: *const OpaqueHandle);
}
```

---

## Unsafe块最佳实践

### 1. 最小化unsafe范围

```rust
// ❌ 不好的做法：整个函数都是unsafe
pub unsafe fn bad_process(data: *mut u8, len: usize) {
    // ... 大量代码 ...
}

// ✅ 好的做法：unsafe块尽可能小
pub fn good_process(data: &mut [u8]) {
    let ptr = data.as_mut_ptr();
    let len = data.len();

    unsafe {
        // 只有真正需要的部分用unsafe
        libc::memset(ptr as *mut c_void, 0, len);
    }
}
```

### 2. 封装安全抽象

```rust
// 内部使用unsafe，对外安全
pub struct SafeBuffer {
    ptr: *mut u8,
    len: usize,
    capacity: usize,
}

impl SafeBuffer {
    pub fn new(capacity: usize) -> Self {
        let layout = Layout::array::<u8>(capacity).unwrap();
        let ptr = unsafe { std::alloc::alloc(layout) };
        if ptr.is_null() {
            panic!("内存分配失败");
        }
        Self { ptr, len: 0, capacity }
    }

    pub fn push(&mut self, val: u8) {
        assert!(self.len < self.capacity);
        unsafe {
            *self.ptr.add(self.len) = val;
        }
        self.len += 1;
    }
}

impl Drop for SafeBuffer {
    fn drop(&mut self) {
        unsafe {
            let layout = Layout::array::<u8>(self.capacity).unwrap();
            std::alloc::dealloc(self.ptr, layout);
        }
    }
}
```

### 3. 前置条件文档化

```rust
/// # Safety
///
/// 调用者必须确保：
/// - `ptr` 是有效且对齐的指针
/// - `ptr` 指向至少 `len` 个连续的 `T` 元素
/// - `ptr` 在返回的切片生命周期内保持有效
/// - 返回的切片不会被同时可变借用
unsafe fn from_raw_parts<'a, T>(ptr: *const T, len: usize) -> &'a [T] {
    std::slice::from_raw_parts(ptr, len)
}
```

### 4. 使用assert!进行运行时检查

```rust
pub unsafe fn read_offset<T>(ptr: *const T, offset: usize) -> T {
    // 运行时安全检查
    assert!(!ptr.is_null(), "指针不能为空");
    assert!(offset < isize::MAX as usize, "偏移量过大");

    ptr.add(offset).read()
}
```

### 5. 利用类型系统

```rust
// 使用NewType模式确保类型安全
pub struct ValidatedPointer<T>(*mut T);

impl<T> ValidatedPointer<T> {
    pub fn new(ptr: *mut T) -> Option<Self> {
        if ptr.is_null() {
            None
        } else {
            Some(Self(ptr))
        }
    }

    pub unsafe fn as_mut(&mut self) -> &mut T {
        &mut *self.0
    }
}
```

---

## 与C代码集成模式

### 模式1：单向调用C库

```rust
// build.rs
fn main() {
    println!("cargo:rustc-link-lib=z"); // 链接zlib
}

// src/lib.rs
use std::os::raw::{c_ulong, c_int};

extern "C" {
    fn compress(
        dest: *mut u8,
        dest_len: *mut c_ulong,
        source: *const u8,
        source_len: c_ulong,
    ) -> c_int;
}

pub fn rust_compress(input: &[u8]) -> Result<Vec<u8>, &'static str> {
    let mut output = vec![0u8; input.len() * 2];
    let mut out_len = output.len() as c_ulong;

    let result = unsafe {
        compress(
            output.as_mut_ptr(),
            &mut out_len,
            input.as_ptr(),
            input.len() as c_ulong,
        )
    };

    if result == 0 {
        output.truncate(out_len as usize);
        Ok(output)
    } else {
        Err("压缩失败")
    }
}
```

### 模式2：C调用Rust回调

```rust
// 定义回调类型
pub type ProgressCallback = extern "C" fn(current: usize, total: usize);

static mut CALLBACK: Option<ProgressCallback> = None;

#[no_mangle]
pub extern "C" fn register_callback(cb: ProgressCallback) {
    unsafe {
        CALLBACK = Some(cb);
    }
}

#[no_mangle]
pub extern "C" fn process_data(data: *const u8, len: usize) {
    unsafe {
        let slice = std::slice::from_raw_parts(data, len);

        for (i, chunk) in slice.chunks(1024).enumerate() {
            // 处理数据块...

            if let Some(cb) = CALLBACK {
                cb(i * 1024, len);
            }
        }
    }
}
```

### 模式3：共享内存结构

```rust
// 定义C兼容的共享结构
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct SharedConfig {
    pub version: u32,
    pub timeout_ms: u32,
    pub buffer_size: usize,
    pub flags: u32,
}

impl SharedConfig {
    /// 从C指针安全读取
    /// # Safety
    /// ptr必须指向有效的SharedConfig
    pub unsafe fn from_ptr(ptr: *const SharedConfig) -> Option<Self> {
        if ptr.is_null() {
            None
        } else {
            Some(*ptr)
        }
    }

    /// 写入到C指针
    /// # Safety
    /// ptr必须指向有效的可写内存
    pub unsafe fn write_to(&self, ptr: *mut SharedConfig) {
        assert!(!ptr.is_null());
        *ptr = *self;
    }
}
```

### 模式4：错误处理转换

```rust
use std::ffi::CStr;
use std::os::raw::c_char;

#[repr(C)]
pub struct RustResult {
    pub success: bool,
    pub error_code: i32,
    pub message: *const c_char, // 由Rust分配，C释放
}

pub fn to_c_result<T, E: std::fmt::Display>(result: Result<T, E>) -> RustResult {
    match result {
        Ok(_) => RustResult {
            success: true,
            error_code: 0,
            message: std::ptr::null(),
        },
        Err(e) => {
            let msg = CString::new(e.to_string()).unwrap_or_default();
            let msg_ptr = msg.into_raw(); // 转移所有权
            RustResult {
                success: false,
                error_code: -1,
                message: msg_ptr,
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn free_result_message(msg: *mut c_char) {
    if !msg.is_null() {
        unsafe {
            let _ = CString::from_raw(msg); // 回收所有权
        }
    }
}
```

### 模式5：对象封装模式

```rust
// 将Rust对象封装为C可操作的句柄
pub struct DataProcessor {
    data: Vec<u8>,
    config: ProcessorConfig,
}

pub struct ProcessorConfig {
    threshold: f64,
    max_iterations: usize,
}

// 不透明指针类型
pub type ProcessorHandle = *mut DataProcessor;

#[no_mangle]
pub extern "C" fn processor_create() -> ProcessorHandle {
    let processor = Box::new(DataProcessor {
        data: Vec::new(),
        config: ProcessorConfig {
            threshold: 0.5,
            max_iterations: 100,
        },
    });
    Box::into_raw(processor)
}

#[no_mangle]
pub extern "C" fn processor_destroy(handle: ProcessorHandle) {
    if !handle.is_null() {
        unsafe {
            let _ = Box::from_raw(handle);
        }
    }
}

#[no_mangle]
pub extern "C" fn processor_process(
    handle: ProcessorHandle,
    input: *const u8,
    len: usize,
) -> i32 {
    if handle.is_null() || input.is_null() {
        return -1;
    }

    unsafe {
        let processor = &mut *handle;
        let slice = std::slice::from_raw_parts(input, len);

        processor.data.extend_from_slice(slice);

        // 执行处理...
        0 // 成功
    }
}
```

### 模式6：资源管理RAII

```rust
// 安全包装器管理C资源
pub struct CFile {
    handle: *mut FILE,
}

impl CFile {
    pub fn open(path: &str) -> Option<Self> {
        let c_path = CString::new(path).ok()?;
        let handle = unsafe { fopen(c_path.as_ptr(), c"r".as_ptr() as *const c_char) };

        if handle.is_null() {
            None
        } else {
            Some(Self { handle })
        }
    }

    pub fn read(&mut self, buf: &mut [u8]) -> usize {
        unsafe {
            fread(
                buf.as_mut_ptr() as *mut c_void,
                1,
                buf.len(),
                self.handle,
            )
        }
    }
}

impl Drop for CFile {
    fn drop(&mut self) {
        unsafe {
            fclose(self.handle);
        }
    }
}

// 使用安全的RAII包装
fn use_c_file() {
    let file = CFile::open("data.txt").expect("无法打开文件");
    let mut buffer = vec![0u8; 1024];
    // 自动关闭，无需手动管理
}
```

---

## 总结

| 主题 | 核心要点 |
|------|----------|
| Unsafe Rust | 提供底层操作能力，但需要开发者自行保证安全 |
| 原始指针 | 灵活但危险，避免悬空指针和类型混淆 |
| FFI边界 | 注意ABI兼容性和类型映射，使用`#[repr(C)]` |
| 最佳实践 | 最小化unsafe范围，封装安全抽象，充分文档化 |
| C集成 | 单向调用、回调、共享内存、错误转换、对象封装 |

> **关键原则**：`unsafe`块应该像小刀一样——锋利但精准，只在必要时使用，且始终保证内存安全。

> **状态**: ✅ 已完成


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
