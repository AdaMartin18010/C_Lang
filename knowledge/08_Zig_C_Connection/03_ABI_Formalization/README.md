---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# ABI 形式化与调用约定

本目录深入探讨应用程序二进制接口（ABI）的形式化定义，以及 Zig 和 C 在不同平台上的调用约定兼容性。

## 目录结构

| 文件 | 主题 | 覆盖平台 |
|------|------|----------|
| `01_System_V_ABI_Zig_C.md` | System-V AMD64 ABI | Linux, macOS, *BSD |
| `02_Windows_ABI_Compatibility.md` | Windows x64 ABI | Windows |
| `03_Calling_Convention_Proofs.md` | 调用约定形式化证明 | 全平台 |

## ABI 概念与重要性

### 什么是 ABI？

应用程序二进制接口（Application Binary Interface）定义了：

1. **数据类型大小和对齐** - `sizeof(int)`, `alignof(struct)`
2. **寄存器使用约定** - 参数传递、返回值、保留寄存器
3. **栈帧布局** - 局部变量、参数、返回地址
4. **调用约定** - 谁清理栈、参数传递顺序
5. **名称修饰（Mangling）** - 符号表命名规则

```
┌─────────────────────────────────────────────────────────┐
│                    ABI 层次架构                          │
├─────────────────────────────────────────────────────────┤
│  应用层: 类型布局、虚表、异常处理                         │
├─────────────────────────────────────────────────────────┤
│  调用层: 参数传递、寄存器分配、栈清理                     │
├─────────────────────────────────────────────────────────┤
│  二进制层: 对象格式、链接语义、重定位类型                 │
├─────────────────────────────────────────────────────────┤
│  系统层: 系统调用约定、信号处理、线程本地存储             │
└─────────────────────────────────────────────────────────┘
```

### 为什么 ABI 兼容性至关重要

```zig
// 如果 ABI 不匹配，这段代码将导致未定义行为
const c_lib = @cImport({
    @cInclude("legacy_c_lib.h");
});

// 假设 C 库期望 System-V AMD64 ABI
// 但编译器使用了不同的调用约定
pub export fn call_c_function(x: f64, y: f64) f64 {
    // 如果浮点参数没有正确放入 XMM 寄存器
    // C 库将读取到错误的值
    return c_lib.compute_distance(x, y);
}
```

## C ABI 标准详解

### System-V AMD64 ABI

Linux、macOS 和大多数 Unix 系统的标准：

```
寄存器用途 (System-V AMD64):
┌─────────────────────────────────────────────────────┐
│ %rax    返回值, 第6个整数参数                        │
│ %rbx    被调用者保存                                  │
│ %rcx    第4个整数参数                                 │
│ %rdx    第3个整数参数, 第2个返回值                   │
│ %rsi    第2个整数参数                                 │
│ %rdi    第1个整数参数                                 │
│ %rbp    帧指针 (被调用者保存)                         │
│ %rsp    栈指针                                        │
│ %r8-%r9 第5-6个整数参数                               │
│ %r10-%r11 调用者保存 (临时)                           │
│ %r12-%r15 被调用者保存                                │
├─────────────────────────────────────────────────────┤
│ %xmm0-%xmm7  浮点/向量参数 (前8个)                    │
│ %xmm8-%xmm15 调用者保存                               │
│ %ymm*/%zmm*  扩展向量寄存器                           │
└─────────────────────────────────────────────────────┘
```

### Windows x64 ABI

Microsoft 的调用约定：

```
寄存器用途 (Windows x64):
┌─────────────────────────────────────────────────────┐
│ RCX/XMM0   第1个参数                                  │
│ RDX/XMM1   第2个参数                                  │
│ R8/XMM2    第3个参数                                  │
│ R9/XMM3    第4个参数                                  │
│ RAX        返回值                                     │
│ R10-R11    调用者保存/内核陷阱                        │
│ R12-R15    被调用者保存                               │
├─────────────────────────────────────────────────────┤
│ 栈分配: 32字节 "shadow space" 必须预留                │
│ 栈对齐: 16字节对齐                                    │
│ 浮点:   所有 XMM 寄存器都是易失的                    │
└─────────────────────────────────────────────────────┘
```

## Zig 的 C 兼容 ABI

### 显式调用约定指定

```zig
// 默认使用 C 调用约定 (.C)
extern fn c_function(x: i32) i32;

// 显式指定不同调用约定
extern "cdecl" fn cdecl_function(ptr: *anyopaque) void;
extern "stdcall" fn stdcall_function(hwnd: usize, msg: u32) i64;
extern "fastcall" fn fastcall_function(a: i32, b: i32) i32;
extern "vectorcall" fn vectorcall_function(v: @Vector(4, f32)) f32;

// Windows 特定
extern "system" fn system_function() void;  // stdcall on Win32

// 内联汇编级别的控制
pub export fn precise_abi_control(arg1: i64, arg2: f64) i64 {
    return asm volatile (
        \\ mov %rdi, %rax    # 从正确寄存器读取
        \\ add %xmm0, %xmm0  # 处理浮点参数
        : [ret] "={rax}" (-> i64)
        : [a1] "{rdi}" (arg1),
          [a2] "{xmm0}" (arg2)
        : "memory"
    );
}
```

### 跨平台函数导出

```zig
const std = @import("std");
const builtin = @import("builtin");

/// 根据平台自动选择正确的调用约定
pub const CallConv = switch (builtin.target.os.tag) {
    .windows => if (builtin.target.cpu.arch == .x86)
        .Stdcall
    else
        .C,
    else => .C,
};

/// 跨平台回调类型定义
pub const CallbackFn = *const fn (ctx: ?*anyopaque, data: []const u8) callconv(CallConv) void;

/// 导出函数使用平台特定调用约定
export fn register_callback(callback: CallbackFn, ctx: ?*anyopaque) callconv(CallConv) i32 {
    _ = callback;
    _ = ctx;
    return 0;
}
```

## 调用约定详解

### cdecl (C Declaration)

```zig
/// C 标准调用约定 - 调用者清理栈
/// 参数从右向左压栈
extern "cdecl" fn cdecl_example(
    arg1: i32,  // 压栈顺序: 最后
    arg2: i32,  // 压栈顺序: 其次
    arg3: i32,  // 压栈顺序: 首先
) i32;

// 等效的汇编理解:
// push arg3
// push arg2
// push arg1
// call cdecl_example
// add rsp, 12  ; 调用者清理栈
```

### stdcall

```zig
/// stdcall - 被调用者清理栈
/// Win32 API 标准调用约定
extern "stdcall" fn stdcall_example(
    arg1: i32,
    arg2: i32,
) i32;

// 汇编理解:
// push arg2
// push arg1
// call stdcall_example
// ; 无需清理栈，被调用函数使用 ret 8
```

### fastcall

```zig
/// fastcall - 使用寄存器传递前几个参数
/// x86: ECX, EDX 用于前两个整数参数
/// x64: 与标准 C 调用约定相同 (寄存器传参)
extern "fastcall" fn fastcall_example(
    a: i32,  // ECX (x86) / RDX (x64)
    b: i32,  // EDX (x86) / RCX (x64)
    c: i32,  // 栈
) i32;
```

### 调用约定对比表

| 特性 | cdecl | stdcall | fastcall | thiscall | vectorcall |
|------|-------|---------|----------|----------|------------|
| 参数传递 | 栈 (右→左) | 栈 (右→左) | 寄存器+栈 | 寄存器+栈 | 寄存器+栈 |
| 栈清理 | 调用者 | 被调用者 | 被调用者 | 被调用者 | 被调用者 |
| 可变参数 | ✓ | ✗ | ✗ | ✗ | ✗ |
| 寄存器使用 | 无 | 无 | ECX/EDX | ECX | XMM/YMM |
| 主要用途 | 通用 | Win32 API | 性能优化 | C++ 成员 | 向量运算 |

## 结构体传递方式

### 按值传递结构体

```c
// C 代码 - 结构体传递
typedef struct {
    double x, y, z;
} Point3D;

// 结构体大小 <= 16 字节，可能通过寄存器传递
// 结构体大小 > 16 字节，通过栈或指针传递
Point3D transform_point(Point3D p, double matrix[3][3]);
```

```zig
// Zig 中的等效声明
const Point3D = extern struct {
    x: f64,
    y: f64,
    z: f64,
};

// 确保与 C 相同的传递方式
extern fn transform_point(p: Point3D, matrix: *[3][3]f64) Point3D;

// 验证结构体大小和布局
comptime {
    @import("std").debug.assert(@sizeOf(Point3D) == 24);
    @import("std").debug.assert(@alignOf(Point3D) == 8);
}
```

### 大型结构体处理

```zig
/// 当结构体过大无法通过寄存器传递时
const LargeStruct = extern struct {
    data: [256]u64,
    count: usize,
};

// C 编译器可能隐式转换为指针传递
// Zig 需要显式处理
export fn process_large_struct(ptr: *const LargeStruct) void {
    // 通过指针访问，避免不必要的拷贝
    for (ptr.data[0..ptr.count]) |item| {
        _ = item;
    }
}

// 如果需要 C 兼容的按值语义
export fn copy_large_struct(s: LargeStruct) LargeStruct {
    return s;  // 编译器将生成适当的拷贝代码
}
```

## 浮点数和向量寄存器

### SIMD 类型 ABI

```zig
const std = @import("std");

/// SSE 128-bit 向量
pub const Vec4f = @Vector(4, f32);

/// AVX 256-bit 向量
pub const Vec8f = @Vector(8, f32);

/// 确保正确的寄存器传递
export fn simd_operation(a: Vec4f, b: Vec4f) Vec4f {
    // x86_64 System-V: a 在 %xmm0, b 在 %xmm1
    // 返回通过 %xmm0
    return a + b;
}

/// 混合标量和向量参数
export fn mixed_simd(scalar: f64, v1: Vec4f, v2: Vec4f) f64 {
    // System-V AMD64:
    // scalar -> %xmm0
    // v1 -> %xmm1 (需要检查实际分配)
    // v2 -> %xmm2
    const sum = @reduce(.Add, v1 * v2);
    return scalar + sum;
}
```

### 浮点寄存器状态

```zig
/// 保存和恢复浮点寄存器状态
export fn save_fp_state() void {
    var state: [64]u8 align(16) = undefined;  // FXSAVE 缓冲区
    asm volatile (
        \\ fxsave (%[buf])
        :
        : [buf] "r" (&state)
        : "memory"
    );

    // 执行可能修改 FP 寄存器的 C 代码...

    asm volatile (
        \\ fxrstor (%[buf])
        :
        : [buf] "r" (&state)
        : "memory"
    );
}
```

## 验证 ABI 兼容性

### 运行时测试

```zig
test "ABI compatibility verification" {
    const c = @cImport(@cInclude("abi_test.h"));

    // 测试整数寄存器传递
    const int_result = c.test_int_regs(1, 2, 3, 4, 5, 6);
    try std.testing.expectEqual(int_result, 21);

    // 测试浮点寄存器传递
    const float_result = c.test_float_regs(1.0, 2.0, 3.0, 4.0);
    try std.testing.expectApproxEqAbs(float_result, 10.0, 0.001);

    // 测试混合参数
    const mixed_result = c.test_mixed(1, 2.0, 3, 4.0);
    try std.testing.expectEqual(mixed_result, 10.0);
}
```

### 汇编级验证

```zig
/// 检查生成的汇编代码
export fn inspect_call_convention(a: i64, b: f64, c: i32) i64 {
    // 编译并检查:
    // zig build-obj -O ReleaseFast -target x86_64-linux-gnu
    //
    // 期望的寄存器分配 (System-V):
    // a -> %rdi
    // b -> %xmm0
    // c -> %esi
    return a + @as(i64, @intFromFloat(b)) + c;
}
```

## 平台特定注意事项

| 平台 | 调用约定 | 特殊要求 |
|------|----------|----------|
| Linux x64 | System-V | 红区（128字节低于 %rsp） |
| macOS x64 | System-V | 栈对齐要求严格 |
| Windows x64 | Microsoft | 32字节 Shadow Space |
| ARM64 | AAPCS | X0-X7 参数寄存器 |
| RISC-V | RISCV-CC | A0-A7 参数寄存器 |

## 延伸阅读

1. [System V ABI 文档](https://gitlab.com/x86-psABIs/x86-64-ABI) - 官方规范
2. [Microsoft x64 调用约定](https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention) - MSDN 文档
3. [Zig 调用约定文档](https://ziglang.org/documentation/master/#toc-Calling-Convention)

---

*本文档是 Zig-C 连接系列的一部分，专注于 ABI 形式化与调用约定兼容性。*
