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
﻿# C Struct 与 Zig extern struct 等价性深度分析

> **定位**: 08_Zig_C_Connection / 类型系统 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 建立结构体布局等价的形式化证明

---

## 目录

- [C Struct 与 Zig extern struct 等价性深度分析](#c-struct-与-zig-extern-struct-等价性深度分析)
  - [目录](#目录)
  - [一、结构体等价性概述](#一结构体等价性概述)
    - [1.1 核心概念](#11-核心概念)
    - [1.2 等价性层次](#12-等价性层次)
  - [二、内存布局保证](#二内存布局保证)
    - [2.1 C 结构体内存模型](#21-c-结构体内存模型)
    - [2.2 Zig extern struct 内存模型](#22-zig-extern-struct-内存模型)
    - [2.3 布局等价性定理](#23-布局等价性定理)
  - [三、对齐要求与填充](#三对齐要求与填充)
    - [3.1 对齐原则](#31-对齐原则)
    - [3.2 填充计算](#32-填充计算)
    - [3.3 显式对齐控制](#33-显式对齐控制)
  - [四、位域处理差异](#四位域处理差异)
    - [4.1 C 位域语义](#41-c-位域语义)
    - [4.2 Zig packed struct 位域](#42-zig-packed-struct-位域)
    - [4.3 位域布局兼容性分析](#43-位域布局兼容性分析)
  - [五、Packed Struct 语义](#五packed-struct-语义)
    - [5.1 C Packed 结构体](#51-c-packed-结构体)
    - [5.2 Zig packed struct](#52-zig-packed-struct)
    - [5.3 语义差异对比](#53-语义差异对比)
  - [六、柔性数组成员](#六柔性数组成员)
    - [6.1 C 柔性数组](#61-c-柔性数组)
    - [6.2 Zig 等效实现](#62-zig-等效实现)
    - [6.3 内存管理对比](#63-内存管理对比)
  - [七、匿名结构体与联合体](#七匿名结构体与联合体)
    - [7.1 C 匿名成员](#71-c-匿名成员)
    - [7.2 Zig 模拟方案](#72-zig-模拟方案)
  - [八、类型双关与严格别名](#八类型双关与严格别名)
    - [8.1 C 严格别名规则](#81-c-严格别名规则)
    - [8.2 Zig 类型双关机制](#82-zig-类型双关机制)
    - [8.3 安全转换模式](#83-安全转换模式)
  - [九、跨平台考量](#九跨平台考量)
    - [9.1 平台差异分析](#91-平台差异分析)
    - [9.2 可移植性策略](#92-可移植性策略)
  - [十、形式化布局等价证明](#十形式化布局等价证明)
    - [10.1 形式化语义框架](#101-形式化语义框架)
    - [10.2 等价性证明](#102-等价性证明)
  - [十一、完整代码示例](#十一完整代码示例)
    - [11.1 基础结构体](#111-基础结构体)
    - [11.2 复杂嵌套结构体](#112-复杂嵌套结构体)
    - [11.3 实际 FFI 用例](#113-实际-ffi-用例)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 一、结构体等价性概述

### 1.1 核心概念

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       结构体等价性核心概念                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C struct                     Zig extern struct                              │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  struct Point {               const Point = extern struct {                  │
│      int x;                       x: c_int,                                  │
│      int y;                       y: c_int,                                  │
│  };                           };                                             │
│                                                                              │
│  关键区别:                                                                   │
│  ─────────────────                                                           │
│  • C struct: 编译器定义布局，依赖实现                                         │
│  • Zig extern struct: 显式 C 兼容布局保证                                     │
│  • Zig struct: 优化布局，不保证 C 兼容                                        │
│                                                                              │
│  extern 关键字含义:                                                          │
│  ─────────────────────────────────────────                                   │
│  1. 字段顺序与声明一致 (ordered)                                              │
│  2. 对齐规则遵循 C ABI                                                        │
│  3. 支持在 @import("C") 边界使用                                              │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 等价性层次

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                          结构体等价性层次模型                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  Level 4: ABI 等价 (ABI Equivalence)                                         │
│  ─────────────────────────────────────                                       │
│  C struct 和 Zig extern struct 在函数调用约定中表现一致                        │
│  • 相同的寄存器/栈传递规则                                                    │
│  • 相同的返回值处理                                                           │
│  • 相同的边界对齐                                                             │
│                                                                              │
│  Level 3: 语义等价 (Semantic Equivalence)                                    │
│  ────────────────────────────────────────                                    │
│  所有对 C struct 的有效操作对 Zig extern struct 同样有效                       │
│  • 字段访问语义一致                                                           │
│  • 赋值语义一致                                                               │
│  • 地址运算语义一致                                                           │
│                                                                              │
│  Level 2: 布局等价 (Layout Equivalence)                                      │
│  ─────────────────────────────────────                                       │
│  sizeof 和 alignof 相等，字段偏移一致                                          │
│  • offsetof 每个字段相等                                                      │
│  • 总大小相等                                                                 │
│  • 对齐要求相等                                                               │
│                                                                              │
│  Level 1: 表示等价 (Representation Equivalence)                              │
│  ─────────────────────────────────────────────                               │
│  位模式完全相同的内存表示                                                     │
│  • 可以安全地进行类型双关                                                      │
│  • memcpy 可互换                                                              │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 二、内存布局保证

### 2.1 C 结构体内存模型

```c
// C 代码 - 标准结构体布局规则
#include <stddef.h>
#include <stdio.h>

struct Example {
    char c;      // 1 byte
    int i;       // 4 bytes (通常)
    short s;     // 2 bytes
};

// 内存布局分析 (典型 32/64 位平台):
// offset 0:  char c    [1 byte]
// offset 1:  padding   [3 bytes]  <-- 为 int 对齐到 4 字节边界
// offset 4:  int i     [4 bytes]
// offset 8:  short s   [2 bytes]
// offset 10: padding   [2 bytes]  <-- 结构体总大小对齐到最大成员
// Total: 12 bytes

int main() {
    printf("sizeof(struct Example) = %zu\n", sizeof(struct Example));
    printf("offsetof(c) = %zu\n", offsetof(struct Example, c));
    printf("offsetof(i) = %zu\n", offsetof(struct Example, i));
    printf("offsetof(s) = %zu\n", offsetof(struct Example, s));
    return 0;
}
```

**C 结构体布局规则**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                         C 结构体内存布局规则                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  规则 1: 顺序分配 (Sequential Allocation)                                    │
│  ─────────────────────────────────────────                                   │
│  成员按声明顺序在内存中依次排列，地址递增                                      │
│                                                                              │
│  规则 2: 对齐要求 (Alignment Requirement)                                    │
│  ─────────────────────────────────────────                                   │
│  每个成员的偏移必须是其 alignof 的整数倍                                       │
│  offset(field_i) = k * alignof(field_i), k ∈ ℕ                               │
│                                                                              │
│  规则 3: 结构体对齐 (Struct Alignment)                                       │
│  ─────────────────────────────────────────                                   │
│  alignof(struct) = max(alignof(member_i))                                    │
│                                                                              │
│  规则 4: 结构体大小 (Struct Size)                                            │
│  ───────────────────────────────                                             │
│  sizeof(struct) 必须是 alignof(struct) 的整数倍                               │
│  必要时在末尾添加填充                                                         │
│                                                                              │
│  规则 5: 实现定义行为 (Implementation-defined)                               │
│  ────────────────────────────────────────────                                │
│  • 位域布局                                                                  │
│  • 是否支持零大小数组                                                          │
│  • 对齐超过 _Alignof(max_align_t) 的类型                                      │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Zig extern struct 内存模型

```zig
// Zig 代码 - extern struct 布局保证
const std = @import("std");

const Example = extern struct {
    c: u8,       // 1 byte
    i: c_int,    // 4 bytes (平台相关)
    s: c_short,  // 2 bytes
};

// 内存布局分析与 C 完全一致:
// offset 0:  u8 c       [1 byte]
// offset 1:  padding    [3 bytes]
// offset 4:  c_int i    [4 bytes]
// offset 8:  c_short s  [2 bytes]
// offset 10: padding    [2 bytes]
// Total: 12 bytes

pub fn main() void {
    std.debug.print("sizeof(Example) = {d}\n", .{@sizeOf(Example)});
    std.debug.print("offsetof(c) = {d}\n", .{@offsetOf(Example, "c")});
    std.debug.print("offsetof(i) = {d}\n", .{@offsetOf(Example, "i")});
    std.debug.print("offsetof(s) = {d}\n", .{@offsetOf(Example, "s")});
}
```

**Zig extern struct 保证**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Zig extern struct 布局保证                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  保证 1: C ABI 兼容                                                          │
│  ───────────────────                                                         │
│  extern struct 保证与目标平台的 C 编译器使用相同的布局算法                      │
│  编译目标: x86_64-linux-gnu → GCC/Clang 布局                                 │
│  编译目标: x86_64-windows-msvc → MSVC 布局                                   │
│                                                                              │
│  保证 2: 字段顺序保留                                                         │
│  ────────────────────                                                        │
│  @offsetOf(Struct, "field_n") 按声明顺序递增                                  │
│  ∀i < j, @offsetOf(S, field_i) < @offsetOf(S, field_j)                       │
│                                                                              │
│  保证 3: 可预测的对齐                                                         │
│  ───────────────────                                                         │
│  @alignOf(extern struct) = max(@alignOf(field_i))                            │
│  与 C 的 _Alignof 语义一致                                                    │
│                                                                              │
│  保证 4: 运行时验证                                                           │
│  ───────────────────                                                         │
│  @sizeOf, @alignOf, @offsetOf 是编译时常量                                     │
│  可以在编译时验证 C/Zig 布局一致性                                             │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.3 布局等价性定理

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        布局等价性形式化定理                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定理 2.1: 基础布局等价性                                                    │
│  ────────────────────────────                                                │
│                                                                              │
│  给定:                                                                       │
│    C_struct = struct { T1 f1; T2 f2; ...; Tn fn; }                          │
│    Zig_struct = extern struct { f1: Z1, f2: Z2, ..., fn: Zn }               │
│                                                                              │
│  条件:                                                                       │
│    ∀i ∈ [1, n], Ti ≅ Zi (类型等价)                                          │
│                                                                              │
│  结论:                                                                       │
│    sizeof(C_struct) = @sizeOf(Zig_struct)                                    │
│    alignof(C_struct) = @alignOf(Zig_struct)                                  │
│    offsetof(C_struct, fi) = @offsetOf(Zig_struct, "fi")                      │
│                                                                              │
│  证明:                                                                       │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  基础步骤 (n = 1):                                                           │
│    单成员结构体大小等于成员大小（向上取整到对齐边界）                           │
│    sizeof(struct { T f; }) = roundup(sizeof(T), alignof(T))                 │
│    @sizeOf(extern struct { f: Z }) = roundup(@sizeOf(Z), @alignOf(Z))       │
│    由于 T ≅ Z, sizeof(T) = @sizeOf(Z), alignof(T) = @alignOf(Z)              │
│    ∴ 基础步骤成立                                                            │
│                                                                              │
│  归纳步骤:                                                                   │
│    假设对于 n-1 个成员定理成立                                                │
│    对于 n 个成员的结构体:                                                     │
│    offset(fn) = roundup(offset(fn-1) + sizeof(Tn-1), alignof(Tn))           │
│    由于归纳假设和类型等价条件，C 和 Zig 计算相同                              │
│    ∴ 归纳步骤成立                                                            │
│                                                                              │
│  由数学归纳法，定理得证 ∎                                                    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 三、对齐要求与填充

### 3.1 对齐原则

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                          对齐原则详解                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  对齐的数学定义:                                                             │
│  ─────────────────                                                           │
│  地址 a 对对齐值 A 对齐 ⟺ a mod A = 0                                       │
│  等价表述: a = k × A, k ∈ ℕ                                                  │
│                                                                              │
│  平台标准对齐值 (System V AMD64 ABI):                                       │
│  ─────────────────────────────────────                                       │
│  类型              对齐值    说明                                            │
│  ─────────────────────────────────                                         │
│  char              1         字节寻址                                        │
│  short             2         半字对齐                                        │
│  int               4         字对齐                                          │
│  long              8 (LP64)  长字对齐                                        │
│  long long         8         长字对齐                                        │
│  float             4         单精度浮点                                      │
│  double            8         双精度浮点                                      │
│  pointer           8         64位地址                                        │
│  __int128          16        扩展整数                                        │
│  long double       16        扩展精度(x87)                                   │
│                                                                              │
│  对齐的必要性:                                                               │
│  ─────────────                                                               │
│  1. 硬件要求: 某些架构 (ARM, MIPS, SPARC) 非对齐访问导致总线错误              │
│  2. 性能优化: x86_64 非对齐访问有性能惩罚                                     │
│  3. 原子操作: 大多数平台要求自然对齐以支持原子操作                            │
│  4. SIMD 指令: SSE/AVX 通常需要 16/32 字节对齐                                │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 填充计算

```c
// C 代码 - 填充计算示例
#include <stdio.h>

// 示例 1: 自然填充
struct Padded {
    char  a;  // 1 byte + 3 bytes padding
    int   b;  // 4 bytes
    char  c;  // 1 byte + 3 bytes padding
};
// 预期: sizeof = 12 (非最优)

// 示例 2: 重排优化
struct Optimized {
    int   b;  // 4 bytes
    char  a;  // 1 byte
    char  c;  // 1 byte + 2 bytes padding
};
// 预期: sizeof = 8 (更优)

// 示例 3: 混合类型
struct Mixed {
    char   a;  // 1 + 7 padding
    double d;  // 8
    char   b;  // 1 + 3 padding
    int    i;  // 4
};
// 预期: sizeof = 24

int main() {
    printf("sizeof(Padded)    = %zu\n", sizeof(struct Padded));
    printf("sizeof(Optimized) = %zu\n", sizeof(struct Optimized));
    printf("sizeof(Mixed)     = %zu\n", sizeof(struct Mixed));
    return 0;
}
```

```zig
// Zig 代码 - 相同结构的 extern struct
const std = @import("std");

const Padded = extern struct {
    a: u8,
    b: c_int,
    c: u8,
};

const Optimized = extern struct {
    b: c_int,
    a: u8,
    c: u8,
};

const Mixed = extern struct {
    a: u8,
    d: f64,
    b: u8,
    i: c_int,
};

pub fn main() void {
    std.debug.print("@sizeOf(Padded)    = {d}\n", .{@sizeOf(Padded)});
    std.debug.print("@sizeOf(Optimized) = {d}\n", .{@sizeOf(Optimized)});
    std.debug.print("@sizeOf(Mixed)     = {d}\n", .{@sizeOf(Mixed)});

    // 详细的填充分析
    std.debug.print("\nPadded layout:\n", .{});
    std.debug.print("  a @ {d} (size {d})\n", .{@offsetOf(Padded, "a"), @sizeOf(u8)});
    std.debug.print("  b @ {d} (size {d})\n", .{@offsetOf(Padded, "b"), @sizeOf(c_int)});
    std.debug.print("  c @ {d} (size {d})\n", .{@offsetOf(Padded, "c"), @sizeOf(u8)});
}
```

**填充计算算法**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       结构体填充计算算法                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  算法: 计算结构体大小和字段偏移                                               │
│  ─────────────────────────────────────                                       │
│                                                                              │
│  function ComputeStructLayout(fields[]):                                     │
│      current_offset ← 0                                                      │
│      max_alignment ← 1                                                       │
│                                                                              │
│      for each (name, type) in fields:                                        │
│          alignment ← AlignOf(type)                                           │
│          size ← SizeOf(type)                                                 │
│                                                                              │
│          // 对齐当前偏移                                                     │
│          padding ← (alignment - (current_offset % alignment)) % alignment   │
│          offset[name] ← current_offset + padding                             │
│                                                                              │
│          // 更新状态                                                         │
│          current_offset ← offset[name] + size                                │
│          max_alignment ← max(max_alignment, alignment)                       │
│                                                                              │
│      // 结构体末尾填充                                                        │
│      tail_padding ← (max_alignment - (current_offset % max_alignment))      │
│                      % max_alignment                                         │
│      total_size ← current_offset + tail_padding                              │
│                                                                              │
│      return {size: total_size, alignment: max_alignment, offsets: offset}   │
│                                                                              │
│  示例验证:                                                                   │
│  ───────────                                                                 │
│  Padded {u8, c_int, u8}:                                                     │
│    f1: offset = 0, size = 1, next = 1                                        │
│    f2: padding = 3, offset = 4, size = 4, next = 8                           │
│    f3: offset = 8, size = 1, next = 9                                        │
│    tail_padding = 3, total_size = 12                                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.3 显式对齐控制

```c
// C 代码 - 显式对齐控制
#include <stdalign.h>

// 标准 C11 对齐方式
struct alignas(16) AlignedStruct {
    int x;
};
// alignof(AlignedStruct) = 16

// GCC/Clang 扩展
struct __attribute__((aligned(32))) GCCAligned {
    double data[4];
};

// 字段级对齐
struct FieldAlign {
    char c;
    alignas(8) int i;  // i 从 8 字节边界开始
};

//  packed 结构体 (无填充)
struct __attribute__((packed)) PackedStruct {
    char c;
    int i;  // 可能非对齐访问
};
```

```zig
// Zig 代码 - 对齐控制
const std = @import("std");

// 结构体级对齐
const AlignedStruct = extern struct {
    x: c_int,
};
// Zig 中 extern struct 不能直接用 align() 修饰整个结构体
// 但可以通过其他方式控制

// 字段级对齐 - Zig 使用 align 修饰符
const FieldAlign = extern struct {
    c: u8,
    i: c_int align(8),  // i 从 8 字节边界开始
};

// packed struct (1 字节对齐)
const PackedStruct = packed struct {
    c: u8,
    i: c_int,  // 在 offset 1，可能非对齐
};

// 显式指定结构体对齐
const ForcedAlign = extern struct {
    data: [4]f64,
    pub const alignment = 32;  // 自定义对齐
};

pub fn main() void {
    std.debug.print("@alignOf(FieldAlign) = {d}\n", .{@alignOf(FieldAlign)});
    std.debug.print("@alignOf(PackedStruct) = {d}\n", .{@alignOf(PackedStruct)});
    std.debug.print("@offsetOf(FieldAlign, \"i\") = {d}\n", .{@offsetOf(FieldAlign, "i")});
}
```

---

## 四、位域处理差异

### 4.1 C 位域语义

```c
// C 代码 - 位域声明与语义
#include <stdio.h>

struct BitField {
    unsigned int a : 4;   // 4 bits
    unsigned int b : 8;   // 8 bits
    unsigned int c : 12;  // 12 bits
    unsigned int d : 8;   // 8 bits (跨 int 边界?)
};

// 可能的内存布局 (实现定义!):
// 方案 A (紧凑): [aaaa bbbb bbbb cccc cccc cccc dddd dddd] = 4 bytes
// 方案 B (对齐): [aaaa bbbb bbbb cccc cccc cccc .... ....]
//                [dddd dddd .... .... .... .... .... ....] = 8 bytes

// 有符号位域
struct SignedBitField {
    int x : 3;  // -4 到 3
    int y : 3;  // -4 到 3
};

// 匿名位域 (用于对齐)
struct WithGap {
    unsigned int a : 4;
    unsigned int   : 4;  // 4 bits padding
    unsigned int b : 8;
};

// 零宽度位域 (强制对齐到下一个单元)
struct ZeroWidth {
    unsigned int a : 4;
    unsigned int   : 0;  // 强制填充到 int 边界
    unsigned int b : 4;  // 从新 int 开始
};

int main() {
    printf("sizeof(BitField) = %zu\n", sizeof(struct BitField));

    struct BitField bf = {10, 200, 3000, 128};
    printf("bf.a = %u, bf.b = %u, bf.c = %u, bf.d = %u\n",
           bf.a, bf.b, bf.c, bf.d);
    return 0;
}
```

**C 位域实现定义行为**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        C 位域实现定义行为                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  未指定的行为:                                                               │
│  ───────────────                                                             │
│  1. 位域内存顺序: 从高到低 (MSB→LSB) 还是从低到高 (LSB→MSB)                   │
│  2. 位域跨越存储单元边界的处理方式                                             │
│  3. 有符号位域的符号表示 (原码/反码/补码)                                      │
│                                                                              │
│  平台差异示例:                                                               │
│  ───────────────                                                             │
│                                                                              │
│  struct { unsigned a:4, b:4; }                                               │
│                                                                              │
│  Little Endian (x86):        Big Endian (某些 MIPS)                         │
│  ┌─────────────────┐         ┌─────────────────┐                             │
│  │ bbbb bbbb aaaa  │         │ aaaa bbbb ....  │                             │
│  │ (b 在高 4 位)    │         │ (a 在低 4 位)    │                             │
│  └─────────────────┘         └─────────────────┘                             │
│                                                                              │
│  GCC x86_64: 位域从 LSB 开始分配                                              │
│  MSVC x86_64: 位域从 LSB 开始分配                                             │
│  ARM GCC: 通常与 x86 相同                                                     │
│                                                                              │
│  建议: 位域不应用于可移植的 C/Zig 接口                                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 Zig packed struct 位域

```zig
// Zig 代码 - packed struct 位域
const std = @import("std");

// packed struct 提供确定性的位布局
const BitField = packed struct {
    a: u4,   // 4 bits
    b: u8,   // 8 bits
    c: u12,  // 12 bits
    d: u8,   // 8 bits
};
// 总大小: 4 + 8 + 12 + 8 = 32 bits = 4 bytes

// 确定性的位布局: 字段从 LSB 开始
// Bit layout: [cccc cccc dddd dddd cccc bbbb bbbb aaaa]
//              31..24   23..16   15..12 11..4   3..0

// 有符号位域
const SignedBitField = packed struct {
    x: i3,  // -4 到 3
    y: i3,  // -4 到 3
};

// 结合整数类型的 packed struct
const MixedPacked = packed struct {
    flag1: u1,
    flag2: u1,
    reserved: u6,
    data: u8,
};

// 位操作辅助函数
fn setBitA(bf: *BitField, value: u4) void {
    bf.a = value;
}

fn getBitD(bf: BitField) u8 {
    return bf.d;
}

pub fn main() void {
    std.debug.print("@sizeOf(BitField) = {d}\n", .{@sizeOf(BitField)});

    var bf: BitField = .{
        .a = 10,
        .b = 200,
        .c = 3000,
        .d = 128,
    };

    // 查看底层字节表示
    const bytes = std.mem.asBytes(&bf);
    std.debug.print("Raw bytes: ", .{});
    for (bytes) |b| {
        std.debug.print("{x:0>2} ", .{b});
    }
    std.debug.print("\n", .{});

    // 验证值
    std.debug.print("a={d}, b={d}, c={d}, d={d}\n",
        .{bf.a, bf.b, bf.c, bf.d});
}
```

### 4.3 位域布局兼容性分析

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        位域兼容性分析                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  兼容性矩阵:                                                                 │
│  ─────────────                                                               │
│                                                                              │
│                     │ C (GCC) │ C (MSVC) │ Zig packed                       │
│  ───────────────────┼─────────┼──────────┼─────────────────                  │
│  C (GCC)            │    ✓    │    ✗     │    ?                              │
│  C (MSVC)           │    ✗    │    ✓     │    ✗                              │
│  Zig packed         │    ?    │    ✗     │    ✓                              │
│                                                                              │
│  ✓ : 完全兼容    ✗ : 不兼容    ? : 可能兼容（取决于具体布局）                  │
│                                                                              │
│  推荐策略:                                                                   │
│  ───────────                                                                 │
│                                                                              │
│  策略 1: 避免在 C/Zig 边界使用位域                                            │
│  ─────────────────────────────────────────                                   │
│  使用明确的位掩码操作替代位域                                                  │
│                                                                              │
│  C 代码:                                                                     │
│  struct Flags {                                                              │
│      uint32_t value;                                                         │
│  };                                                                          │
│  #define FLAG_A_MASK 0xF                                                     │
│  #define GET_FLAG_A(f) ((f).value & FLAG_A_MASK)                             │
│                                                                              │
│  Zig 代码:                                                                   │
│  const Flags = extern struct {                                               │
│      value: u32,                                                             │
│  };                                                                          │
│  const FLAG_A_MASK: u32 = 0xF;                                               │
│  fn getFlagA(f: Flags) u32 { return f.value & FLAG_A_MASK; }                 │
│                                                                              │
│  策略 2: 使用 packed struct 并验证布局                                        │
│  ─────────────────────────────────────────                                   │
│  如果必须使用位域，在 Zig 端使用 packed struct，                               │
│  并通过测试验证与目标 C 编译器的布局一致性                                     │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 五、Packed Struct 语义

### 5.1 C Packed 结构体

```c
// C 代码 - packed 结构体
#include <stdio.h>
#include <stdint.h>

// GCC/Clang 扩展
struct __attribute__((packed)) PackedGCC {
    char c;
    int i;    // 可能位于非对齐地址
    short s;
};
// sizeof = 7 (1 + 4 + 2)，无填充

// MSVC 扩展
#pragma pack(push, 1)
struct PackedMSVC {
    char c;
    int i;
    short s;
};
#pragma pack(pop)

// 标准 C11 _Static_assert 验证大小
_Static_assert(sizeof(struct PackedGCC) == 7, "Packed size mismatch");

// packed 结构体的陷阱
struct __attribute__((packed)) NetworkHeader {
    uint16_t src_port;   // 可能非对齐
    uint16_t dst_port;
    uint32_t seq_num;    // offset 4，可能非对齐
};

// 非对齐访问可能导致性能问题或崩溃
void process_header(volatile struct NetworkHeader *hdr) {
    // 在 ARM 上可能产生对齐异常
    uint32_t seq = hdr->seq_num;  // 潜在的非对齐访问

    // 更安全的做法: 使用 memcpy
    uint32_t safe_seq;
    memcpy(&safe_seq, &hdr->seq_num, sizeof(safe_seq));
}
```

### 5.2 Zig packed struct

```zig
// Zig 代码 - packed struct
const std = @import("std");

// packed struct: 1 字节对齐，字段紧凑排列
const PackedStruct = packed struct {
    c: u8,
    i: u32,
    s: u16,
};
// @sizeOf = 7
// @alignOf = 1

// packed struct 的字段访问
fn accessPacked() void {
    var p: PackedStruct = .{
        .c = 1,
        .i = 0x12345678,
        .s = 0xABCD,
    };

    // 非对齐字段访问由 Zig 编译器处理
    // 可能生成多字节加载指令或memcpy
    const val = p.i;
    std.debug.print("i = 0x{x}\n", .{val});
}

// 网络协议头 (packed struct 的常见用例)
const TcpHeader = packed struct {
    src_port: u16,
    dst_port: u16,
    seq_num: u32,
    ack_num: u32,
    // 位域模拟
    data_offset: u4,
    reserved: u6,
    flags: u6,
    window_size: u16,
    checksum: u16,
    urgent_ptr: u16,
};

// packed struct 指针的特殊性
fn packedPointerDemo() void {
    var p: PackedStruct = undefined;

    // packed struct 字段的指针是对齐的
    const ptr_to_c: *u8 = &p.c;      // 1-byte aligned

    // 对于非对齐字段，Zig 生成特殊处理
    // 注意: &p.i 会产生警告或错误，因为 i 未对齐

    // 通过指针访问需要使用 @ptrCast 谨慎处理
    const bytes = std.mem.asBytes(&p);
    const i_ptr: *const u32 = @ptrCast(@alignCast(bytes.ptr + 1));
    std.debug.print("i via pointer = 0x{x}\n", .{i_ptr.*});
}

// packed struct 的内存布局
fn printLayout() void {
    std.debug.print("PackedStruct layout:\n", .{});
    std.debug.print("  @sizeOf = {d}\n", .{@sizeOf(PackedStruct)});
    std.debug.print("  @alignOf = {d}\n", .{@alignOf(PackedStruct)});
}
```

### 5.3 语义差异对比

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                    packed struct 语义差异对比                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  特性                     C packed           Zig packed struct               │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  语法                     __attribute__      packed struct 关键字            │
│                           #pragma pack                                      │
│                                                                              │
│  对齐                     1 (或指定值)       总是 1                          │
│                                                                              │
│  字段对齐                 强制 1             强制 1                          │
│                                                                              │
│  非对齐访问处理           未定义行为         编译器生成安全代码               │
│  (unaligned access)                                                          │
│                                                                              │
│  指针取址 (&field)        总是允许           对齐字段允许                     │
│                                              非对齐字段受限                   │
│                                                                              │
│  平台可移植性             编译器相关         Zig 保证行为一致                  │
│                                                                              │
│  性能                     可能更慢           可能更慢，但更安全                │
│  (由于非对齐访问)                                                            │
│                                                                              │
│  常见用例                                                                  │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  C packed struct:                                                            │
│  • 网络协议解析                                                              │
│  • 二进制文件格式                                                            │
│  • 硬件寄存器映射                                                            │
│  • 与遗留系统兼容                                                            │
│                                                                              │
│  Zig packed struct:                                                          │
│  • 与 C packed 结构交互（需验证布局）                                         │
│  • 位域的确定性替代                                                          │
│  • 协议实现                                                                  │
│  • 内存受限环境                                                              │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 六、柔性数组成员

### 6.1 C 柔性数组

```c
// C 代码 - 柔性数组成员 (C99 起标准)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 柔性数组声明: 必须是最后一个成员，大小为 []
struct FlexibleArray {
    size_t len;
    int data[];  // 或 int data[0] (GCC 扩展)
};

// 内存布局:
// [ len (8 bytes) ][ data (0 bytes, 占位) ]
// 实际分配时:
// [ len ][ data[0] ][ data[1] ] ... [ data[n-1] ]

struct FlexibleArray* create_flexible(size_t n) {
    // 关键: 分配 sizeof(struct) + n * sizeof(element)
    struct FlexibleArray *fa = malloc(
        sizeof(struct FlexibleArray) + n * sizeof(int)
    );
    if (!fa) return NULL;

    fa->len = n;
    for (size_t i = 0; i < n; i++) {
        fa->data[i] = (int)(i * i);
    }
    return fa;
}

// 包含柔性数组的结构体数组注意事项
void demo_array_of_flexible() {
    // 危险: 柔性数组结构体不能作为数组元素
    // struct FlexibleArray arr[10];  // 未定义行为！

    // 安全: 使用指针数组
    struct FlexibleArray *arr[10];
    for (int i = 0; i < 10; i++) {
        arr[i] = create_flexible(i + 1);
    }

    // 清理
    for (int i = 0; i < 10; i++) {
        free(arr[i]);
    }
}

int main() {
    struct FlexibleArray *fa = create_flexible(5);
    if (fa) {
        printf("len = %zu\n", fa->len);
        for (size_t i = 0; i < fa->len; i++) {
            printf("data[%zu] = %d\n", i, fa->data[i]);
        }
        free(fa);
    }
    return 0;
}
```

### 6.2 Zig 等效实现

```zig
// Zig 代码 - 柔性数组等效模式
const std = @import("std");
const Allocator = std.mem.Allocator;

// 方案 1: 使用切片模拟柔性数组
const FlexibleSlice = extern struct {
    len: usize,
    // 实际数据存储在单独的分配中
};

const FlexibleArraySlice = struct {
    header: FlexibleSlice,
    data: []i32,  // 切片 = [ptr, len]

    fn create(allocator: Allocator, n: usize) !*FlexibleArraySlice {
        // 分配包含元数据和一个切片的结构体
        const self = try allocator.create(FlexibleArraySlice);
        errdefer allocator.destroy(self);

        self.header.len = n;
        self.data = try allocator.alloc(i32, n);
        errdefer allocator.free(self.data);

        for (0..n) |i| {
            self.data[i] = @intCast(i * i);
        }

        return self;
    }

    fn destroy(self: *FlexibleArraySlice, allocator: Allocator) void {
        allocator.free(self.data);
        allocator.destroy(self);
    }
};

// 方案 2: 单块分配 (更接近 C 语义)
const FlexibleArrayBlock = struct {
    const Header = extern struct {
        len: usize,
        // 数据紧随其后
    };

    // 分配一块内存: [Header][i32, i32, ...]
    fn create(allocator: Allocator, n: usize) !struct { ptr: [*]align(@alignOf(Header)) u8, header: *Header } {
        const header_size = @sizeOf(Header);
        const data_size = n * @sizeOf(i32);
        const total_size = header_size + data_size;

        // 按 Header 对齐要求分配
        const ptr = try allocator.alignedAlloc(u8, @alignOf(Header), total_size);

        const header: *Header = @ptrCast(ptr.ptr);
        header.len = n;

        // 获取数据指针
        const data_ptr: [*]i32 = @ptrCast(@alignCast(ptr.ptr + header_size));

        for (0..n) |i| {
            data_ptr[i] = @intCast(i * i);
        }

        return .{ .ptr = ptr.ptr, .header = header };
    }

    fn getDataPtr(header: *Header) [*]i32 {
        const ptr: [*]u8 = @ptrCast(header);
        return @ptrCast(@alignCast(ptr + @sizeOf(Header)));
    }

    fn destroy(ptr: [*]align(@alignOf(Header)) u8, allocator: Allocator, n: usize) void {
        const header_size = @sizeOf(Header);
        const data_size = n * @sizeOf(i32);
        const total_size = header_size + data_size;
        allocator.free(ptr[0..total_size]);
    }
};

// 方案 3: C 兼容的 extern struct (使用指针)
const CFlexibleArray = extern struct {
    len: usize,
    data: [*]i32,  // 指向可变数量元素的指针
};

pub fn main() void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 使用方案 2
    const result = FlexibleArrayBlock.create(allocator, 5) catch unreachable;
    defer FlexibleArrayBlock.destroy(result.ptr, allocator, 5);

    std.debug.print("len = {d}\n", .{result.header.len});
    const data = FlexibleArrayBlock.getDataPtr(result.header);
    for (0..result.header.len) |i| {
        std.debug.print("data[{d}] = {d}\n", .{i, data[i]});
    }
}
```

### 6.3 内存管理对比

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        柔性数组内存管理对比                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C 柔性数组:                                                                 │
│  ───────────                                                                 │
│                                                                              │
│  分配:                                                                       │
│  struct FA *p = malloc(sizeof(struct FA) + n * sizeof(element));            │
│  ┌─────────────────────────────────────────────┐                             │
│  │  len  │  data[0]  │  data[1]  │  ...       │                             │
│  │ 固定   │  动态数量元素                         │                             │
│  └─────────────────────────────────────────────┘                             │
│                                                                              │
│  释放:                                                                       │
│  free(p);  // 单次释放                                                       │
│                                                                              │
│  Zig 等效方案:                                                              │
│  ────────────────                                                            │
│                                                                              │
│  方案 1 - 切片分离 (安全)                                                     │
│  ┌──────────────┐    ┌─────────────────────────┐                             │
│  │ len │ data_ptr │ -> │ data[0] │ data[1] │ ... │                           │
│  └──────────────┘    └─────────────────────────┘                             │
│  分配: 两次分配 (Header + 数据数组)                                           │
│  释放: 两次释放                                                               │
│                                                                              │
│  方案 2 - 单块分配 (C 兼容)                                                   │
│  ┌─────────────────────────────────────────────┐                             │
│  │  len  │  data[0]  │  data[1]  │  ...       │                             │
│  └─────────────────────────────────────────────┘                             │
│  分配: alignedAlloc(u8, align_of_header, total_size)                        │
│  释放: free 整个块                                                            │
│                                                                              │
│  方案 3 - 使用 std.io.FixedBufferStream (高级)                               │
│  适合序列化/反序列化场景                                                       │
│                                                                              │
│  推荐:                                                                       │
│  ────────                                                                    │
│  • 新代码使用方案 1 (切片分离)，更清晰、更安全                                 │
│  • C 互操作使用方案 2 (单块分配)，保持内存布局兼容                              │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 七、匿名结构体与联合体

### 7.1 C 匿名成员

```c
// C 代码 - 匿名结构体和联合体 (C11 标准)
#include <stdio.h>
#include <stdint.h>

// 匿名结构体成员
struct WithAnonymousStruct {
    int type;
    union {
        struct {
            int x;
            int y;
        };  // 匿名结构体 - 成员直接访问
        struct {
            float r;
            float theta;
        } polar;  // 命名结构体 - 通过 polar.r 访问
    };
};

void demo_anonymous() {
    struct WithAnonymousStruct p = {
        .type = 0,
        .x = 10,    // 直接访问匿名结构体成员
        .y = 20,
    };

    printf("x = %d, y = %d\n", p.x, p.y);  // 直接访问

    // 切换到极坐标视图
    p.polar.r = 22.36f;
    p.polar.theta = 1.107f;
}

// 匿名联合体 - 常用于类型双关
struct Variant {
    uint32_t type;
    union {
        int i;
        float f;
        void *p;
    };  // 匿名联合体
};

void demo_variant() {
    struct Variant v;
    v.type = 1;  // 假设 1 表示 float
    v.f = 3.14159f;  // 直接访问匿名联合体成员

    // 读取整数值 (类型双关)
    printf("float %f has int representation 0x%x\n", v.f, v.i);
}

// 嵌套匿名结构体
struct ComplexNested {
    int header;
    struct {
        struct {
            int deep_field;
        };
        int sibling;
    };
    int footer;
};

void demo_nested() {
    struct ComplexNested n = {
        .header = 1,
        .deep_field = 42,  // 直接访问深层字段
        .sibling = 100,
        .footer = 2,
    };
    printf("deep_field = %d\n", n.deep_field);
}
```

### 7.2 Zig 模拟方案

```zig
// Zig 代码 - 模拟 C 匿名成员
const std = @import("std");

// Zig 不支持匿名成员，需显式命名

// 方案 1: 扁平化 - 所有字段提升到父结构体
const WithFlattenedStruct = extern struct {
    type: c_int,
    // 匿名结构体成员展开
    x: c_int,
    y: c_int,
    // 极坐标视图通过方法提供

    fn setPolar(self: *WithFlattenedStruct, r: f32, theta: f32) void {
        self.x = @intFromFloat(r * @cos(theta));
        self.y = @intFromFloat(r * @sin(theta));
    }

    fn getR(self: WithFlattenedStruct) f32 {
        return @sqrt(@as(f32, @floatFromInt(self.x * self.x + self.y * self.y)));
    }
};

// 方案 2: 显式命名联合体成员
const WithNamedUnion = extern struct {
    type: c_int,
    data: extern union {
        cartesian: extern struct {
            x: c_int,
            y: c_int,
        },
        polar: extern struct {
            r: f32,
            theta: f32,
        },
    },
};

// 方案 3: 使用 packed struct 模拟 Variant
const Variant = extern struct {
    type: u32,
    value: extern union {
        i: c_int,
        f: f32,
        p: ?*anyopaque,
    },
};

// 方案 4: 使用 @ptrCast 实现灵活的类型双关
fn typePunDemo() void {
    var f: f32 = 3.14159;
    const i_ptr: *const i32 = @ptrCast(&f);
    std.debug.print("float {d} as int: 0x{x}\n", .{f, i_ptr.*});
}

// 方案 5: 使用 @bitCast (更安全的类型双关)
fn bitCastDemo() void {
    const f: f32 = 3.14159;
    const i: i32 = @bitCast(f);
    std.debug.print("float {d} as int: 0x{x}\n", .{f, i});

    // 双向转换
    const f_back: f32 = @bitCast(i);
    std.debug.print("back to float: {d}\n", .{f_back});
}

pub fn main() void {
    // 演示方案 1
    var p: WithFlattenedStruct = .{
        .type = 0,
        .x = 10,
        .y = 20,
    };
    std.debug.print("x = {d}, y = {d}\n", .{p.x, p.y});
    std.debug.print("r = {d}\n", .{p.getR()});

    // 演示方案 2
    var p2: WithNamedUnion = .{
        .type = 0,
        .data = .{ .cartesian = .{ .x = 10, .y = 20 } },
    };
    std.debug.print("via union: x = {d}\n", .{p2.data.cartesian.x});

    // 类型双关演示
    typePunDemo();
    bitCastDemo();
}
```

---

## 八、类型双关与严格别名

### 8.1 C 严格别名规则

```c
// C 代码 - 严格别名规则详解
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 严格别名规则: 两种类型必须兼容才能别名访问
// 允许的类型: char*, unsigned char*, void* (C 中 void* 有特殊规则)

// 未定义行为: 通过不兼容类型指针访问
void undefined_alias(int *ip, float *fp) {
    *ip = 42;
    // 假设编译器知道 ip 和 fp 不指向同一对象
    // 可能优化掉对 fp 的重新加载
    float f = *fp;  // 未定义行为如果 ip 和 fp 别名
    printf("%f\n", f);
}

// 安全: 使用 char* 访问
void safe_char_alias(int *ip) {
    char *cp = (char*)ip;
    for (size_t i = 0; i < sizeof(int); i++) {
        printf("%02x ", (unsigned char)cp[i]);
    }
    printf("\n");
}

// 安全: 使用 union (C99 起允许的类型双关)
union IntFloat {
    int i;
    float f;
};

void safe_union_pun() {
    union IntFloat u;
    u.f = 3.14159f;
    printf("Float %f has int representation: 0x%x\n", u.f, u.i);
}

// 安全: 使用 memcpy
void safe_memcpy_pun() {
    float f = 3.14159f;
    int i;
    memcpy(&i, &f, sizeof(i));
    printf("Via memcpy: 0x%x\n", i);
}

// 陷阱: 通过指针转换的类型双关
void dangerous_pun() {
    float f = 3.14159f;
    int *ip = (int*)&f;  // 违反严格别名规则
    printf("Via pointer cast: 0x%x\n", *ip);  // 未定义行为
}

// 常见模式: 网络字节序转换
uint32_t safe_ntohl(uint8_t *data) {
    // 通过 char* 访问是安全的
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8)  |
           ((uint32_t)data[3]);
}

// C23 新增: _BitInt 和 #embed 等特性
```

**严格别名规则详解**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       C 严格别名规则详解                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  规则定义 (C99 §6.5/7):                                                     │
│  ───────────────────────                                                     │
│  如果通过类型 T 的左值访问类型 T2 的对象，且 T 和 T2 不兼容，                  │
│  则行为未定义，除非:                                                         │
│                                                                              │
│  例外情况:                                                                   │
│  ─────────                                                                   │
│  1. T 是与 T2 兼容的类型                                                     │
│  2. T 是与 T2 兼容的类型的限定版本                                            │
│  3. T 是有符号/无符号版本对应类型                                            │
│  4. T 是聚合/联合类型，包含 T2 作为成员                                       │
│  5. T 是 char/unsigned char 类型                                             │
│                                                                              │
│  兼容类型示例:                                                               │
│  ───────────────                                                             │
│  • int 和 int (相同)                                                         │
│  • int 和 const int (cv 限定不影响兼容性)                                     │
│  • int 和 unsigned int (不兼容!)                                             │
│  • struct A 和 struct A (相同标签)                                           │
│                                                                              │
│  安全类型双关方法:                                                           │
│  ───────────────────                                                         │
│                                                                              │
│  方法 1: union (C99 起类型双关合法)                                          │
│  union { int i; float f; } u;                                                │
│  u.f = 3.14f; int i = u.i;  // ✓ 安全                                       │
│                                                                              │
│  方法 2: memcpy                                                              │
│  float f; int i; memcpy(&i, &f, sizeof(i));  // ✓ 安全                      │
│                                                                              │
│  方法 3: char* 逐字节访问                                                    │
│  char *cp = (char*)&f; for (...) cp[i];  // ✓ 安全                          │
│                                                                              │
│  危险方法 (未定义行为):                                                      │
│  ───────────────────────                                                     │
│  int *ip = (int*)&f; int i = *ip;  // ✗ 违反严格别名                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 8.2 Zig 类型双关机制

```zig
// Zig 代码 - 类型双关机制
const std = @import("std");

// Zig 没有严格别名规则，但提供了安全的类型双关原语

// 方法 1: @bitCast - 大小相同类型的位模式转换
fn bitCastExample() void {
    const f: f32 = 3.14159;
    const i: u32 = @bitCast(f);
    std.debug.print("f={d} -> bits=0x{x:0>8}\n", .{f, i});

    // 双向转换
    const f_back: f32 = @bitCast(i);
    std.debug.print("bits=0x{x:0>8} -> f={d}\n", .{i, f_back});

    // 适用于任何大小相同的类型
    const bytes: [4]u8 = @bitCast(f);
    std.debug.print("As bytes: ", .{});
    for (bytes) |b| std.debug.print("{x:0>2} ", .{b});
    std.debug.print("\n", .{});
}

// 方法 2: @ptrCast - 指针类型转换
fn ptrCastExample() void {
    var arr = [4]u8{ 0x12, 0x34, 0x56, 0x78 };

    // 字节数组转整数
    const int_ptr: *align(1) u32 = @ptrCast(&arr);
    std.debug.print("As u32: 0x{x}\n", .{int_ptr.*});

    // 注意: @ptrCast 可能需要 @alignCast
    const aligned_ptr: *u32 = @alignCast(@ptrCast(&arr));
    _ = aligned_ptr;
}

// 方法 3: std.mem.asBytes / std.mem.bytesAsValue
fn stdMemExample() void {
    const value: u32 = 0x12345678;

    // 转换为字节切片
    const bytes = std.mem.asBytes(&value);
    std.debug.print("Bytes: ", .{});
    for (bytes) |b| std.debug.print("{x:0>2} ", .{b});
    std.debug.print("\n", .{});

    // 字节切片转值
    const back: *const u32 = std.mem.bytesAsValue(u32, bytes);
    std.debug.print("Back to u32: 0x{x}\n", .{back.*});
}

// 方法 4: 使用 extern union (C 兼容)
const IntFloatUnion = extern union {
    i: i32,
    f: f32,
};

fn externUnionExample() void {
    var u: IntFloatUnion = .{ .f = 2.71828 };
    std.debug.print("f={d}, i=0x{x}\n", .{u.f, u.i});
}

// 方法 5: @ptrFromInt / @intFromPtr - 整数与指针互转
fn pointerIntExample() void {
    var value: u32 = 42;
    const ptr: *u32 = &value;

    // 指针转整数
    const addr: usize = @intFromPtr(ptr);
    std.debug.print("Pointer as int: 0x{x}\n", .{addr});

    // 整数转指针
    const ptr_back: *u32 = @ptrFromInt(addr);
    std.debug.print("Value via reconstructed pointer: {d}\n", .{ptr_back.*});
}

// 网络字节序转换 (实际应用)
fn ntohl(data: *const [4]u8) u32 {
    return @bitCast([4]u8{
        data[3], data[2], data[1], data[0],
    });
}

pub fn main() void {
    std.debug.print("=== @bitCast ===\n", .{});
    bitCastExample();

    std.debug.print("\n=== @ptrCast ===\n", .{});
    ptrCastExample();

    std.debug.print("\n=== std.mem ===\n", .{});
    stdMemExample();

    std.debug.print("\n=== extern union ===\n", .{});
    externUnionExample();

    std.debug.print("\n=== pointer/int ===\n", .{});
    pointerIntExample();

    // 网络字节序
    const network_bytes = [4]u8{ 0x12, 0x34, 0x56, 0x78 };
    std.debug.print("\n=== ntohl ===\n", .{});
    std.debug.print("Network bytes [12 34 56 78] -> 0x{x}\n", .{ntohl(&network_bytes)});
}
```

### 8.3 安全转换模式

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        C/Zig 安全转换模式总结                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  转换场景                     C 方法                    Zig 方法             │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  相同大小类型双关             union                     @bitCast             │
│  (int <-> float)              memcpy                                           │
│                                                                              │
│  指针类型转换                 显式转换                  @ptrCast               │
│                               (T*)ptr                   @alignCast(需要时)    │
│                                                                              │
│  指针 <-> 整数                (intptr_t)ptr             @intFromPtr            │
│                               (void*)addr               @ptrFromInt            │
│                                                                              │
│  值 <-> 字节                  memcpy                    std.mem.asBytes        │
│                                                       std.mem.bytesAsValue   │
│                                                                              │
│  类型擦除                     void*                     ?*anyopaque           │
│                                                                              │
│  推荐的安全模式:                                                             │
│  ─────────────────                                                           │
│                                                                              │
│  C 代码:                                                                     │
│  // 使用 union 进行类型双关                                                  │
│  union Converter { int i; float f; } c;                                      │
│  c.f = value; return c.i;                                                    │
│                                                                              │
│  // 或使用 memcpy                                                            │
│  int i; memcpy(&i, &f, sizeof(i));                                           │
│                                                                              │
│  Zig 代码:                                                                   │
│  // 首选 @bitCast                                                            │
│  const bits = @bitCast(value);                                               │
│                                                                              │
│  // 或使用标准库                                                             │
│  const bytes = std.mem.asBytes(&value);                                      │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 九、跨平台考量

### 9.1 平台差异分析

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        跨平台结构体布局差异                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  数据模型差异:                                                               │
│  ───────────────                                                             │
│                                                                              │
│  特性           LP64 (Linux/macOS)    LLP64 (Windows)    ILP32 (32位)       │
│  ────────────────────────────────────────────────────────────────────────   │
│  sizeof(long)   8                     4                  4                   │
│  sizeof(void*)  8                     8                  4                   │
│  sizeof(int)    4                     4                  4                   │
│  sizeof(size_t) 8                     8                  4                   │
│                                                                              │
│  结构体对齐差异:                                                             │
│  ────────────────                                                            │
│                                                                              │
│  struct { long a; int b; } 在不同平台:                                       │
│                                                                              │
│  Linux x86_64:  sizeof = 16, alignment = 8                                   │
│  Windows x64:   sizeof = 16, alignment = 8                                   │
│  Linux x86:     sizeof = 8,  alignment = 4                                   │
│                                                                              │
│  位域布局差异:                                                               │
│  ───────────────                                                             │
│  • 位域分配方向 (LSB→MSB vs MSB→LSB)                                         │
│  • 位域跨越边界的处理                                                         │
│  • 有符号位域的扩展方式                                                       │
│                                                                              │
│  编译器差异:                                                                 │
│  ─────────────                                                               │
│                                                                              │
│  GCC/Clang vs MSVC:                                                          │
│  • 某些复杂类型的对齐可能不同                                                  │
│  • 位域处理有差异                                                            │
│  • empty base class optimization (C++，但也影响 C 的某些扩展)                │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 9.2 可移植性策略

```c
// C 代码 - 可移植结构体定义策略
#ifndef PORTABLE_STRUCT_H
#define PORTABLE_STRUCT_H

#include <stdint.h>
#include <stdalign.h>

// 策略 1: 使用固定宽度类型
struct PortableHeader {
    uint32_t magic;      // 4 bytes, 固定大小
    uint32_t version;    // 4 bytes
    uint64_t timestamp;  // 8 bytes
    uint64_t data_size;  // 8 bytes
};
// 此结构体在 LP64/LLP64/ILP32 上大小和布局相同

// 策略 2: 显式填充确保布局一致
struct WithPadding {
    uint16_t id;       // 2 bytes
    uint16_t _pad1;    // 显式填充
    uint32_t flags;    // 4 bytes
    uint64_t offset;   // 8 bytes
};
// 总大小: 16 bytes，所有平台一致

// 策略 3: 使用 packed 结构体 (网络/文件格式)
#ifdef _MSC_VER
    #define PACKED __pragma(pack(push, 1))
    #define PACKED_END __pragma(pack(pop))
#else
    #define PACKED __attribute__((packed))
    #define PACKED_END
#endif

struct PACKED NetworkPacket {
    uint8_t type;
    uint16_t length;  // 2 bytes，可能在非对齐地址
    uint32_t seq;
} PACKED_END;

// 策略 4: 编译时断言验证布局
_Static_assert(sizeof(struct PortableHeader) == 24, "Unexpected size");
_Static_assert(alignof(struct PortableHeader) == 8, "Unexpected alignment");

// 策略 5: 序列化/反序列化函数 (最可移植)
struct SerializeBuf {
    uint8_t *data;
    size_t len;
};

static inline void write_u32(uint8_t **p, uint32_t v) {
    (*p)[0] = (v >> 24) & 0xFF;
    (*p)[1] = (v >> 16) & 0xFF;
    (*p)[2] = (v >> 8)  & 0xFF;
    (*p)[3] = v & 0xFF;
    *p += 4;
}

static inline uint32_t read_u32(const uint8_t **p) {
    uint32_t v = ((uint32_t)(*p)[0] << 24) |
                 ((uint32_t)(*p)[1] << 16) |
                 ((uint32_t)(*p)[2] << 8)  |
                 (uint32_t)(*p)[3];
    *p += 4;
    return v;
}

#endif
```

```zig
// Zig 代码 - 跨平台策略
const std = @import("std");

// 策略 1: 使用 Zig 的 fixed-width 类型
const PortableHeader = extern struct {
    magic: u32,      // 固定 4 字节
    version: u32,    // 固定 4 字节
    timestamp: u64,  // 固定 8 字节
    data_size: u64,  // 固定 8 字节
};

// 编译时验证
comptime {
    std.debug.assert(@sizeOf(PortableHeader) == 24);
    std.debug.assert(@alignOf(PortableHeader) == 8);
}

// 策略 2: 使用 packed struct 保证位级兼容
const NetworkPacket = packed struct {
    type: u8,
    length: u16,
    seq: u32,
};

comptime {
    std.debug.assert(@sizeOf(NetworkPacket) == 7);
    std.debug.assert(@alignOf(NetworkPacket) == 1);
}

// 策略 3: 显式字节序处理
fn writeU32Be(buf: *[4]u8, value: u32) void {
    buf[0] = @truncate(value >> 24);
    buf[1] = @truncate(value >> 16);
    buf[2] = @truncate(value >> 8);
    buf[3] = @truncate(value);
}

fn readU32Be(buf: *const [4]u32) u32 {
    return (@as(u32, buf[0]) << 24) |
           (@as(u32, buf[1]) << 16) |
           (@as(u32, buf[2]) << 8)  |
           @as(u32, buf[3]);
}

// 策略 4: 使用标准库序列化
fn serializeHeader(writer: anytype, h: PortableHeader) !void {
    try writer.writeInt(u32, h.magic, .big);
    try writer.writeInt(u32, h.version, .big);
    try writer.writeInt(u64, h.timestamp, .big);
    try writer.writeInt(u64, h.data_size, .big);
}

// 策略 5: 目标平台检测
const Target = struct {
    const is_little_endian = @import("builtin").target.cpu.arch.endian() == .little;
    const word_size = @sizeOf(usize);

    fn describe() void {
        std.debug.print("Target: {s}-endian, {d}-bit\n", .{
            if (is_little_endian) "little" else "big",
            word_size * 8,
        });
    }
};

pub fn main() void {
    Target.describe();

    std.debug.print("PortableHeader: size={d}, align={d}\n", .{
        @sizeOf(PortableHeader),
        @alignOf(PortableHeader),
    });

    std.debug.print("NetworkPacket: size={d}, align={d}\n", .{
        @sizeOf(NetworkPacket),
        @alignOf(NetworkPacket),
    });
}
```

---

## 十、形式化布局等价证明

### 10.1 形式化语义框架

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     结构体布局形式化语义框架                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 10.1: 类型布局 (Type Layout)                                           │
│  ─────────────────────────────────                                           │
│                                                                              │
│  类型 T 的布局 L(T) 是一个三元组:                                            │
│  L(T) = ⟨Size(T), Align(T), Fields(T)⟩                                       │
│                                                                              │
│  其中:                                                                       │
│  • Size(T) ∈ ℕ⁺ : 类型占用的字节数                                           │
│  • Align(T) ∈ ℕ⁺ : 类型的对齐要求                                            │
│  • Fields(T) : 字段名到 (偏移, 类型) 的映射                                   │
│                                                                              │
│  定义 10.2: 布局等价 (Layout Equivalence)                                    │
│  ────────────────────────────────────────                                    │
│                                                                              │
│  类型 T₁ 和 T₂ 布局等价 (记作 T₁ ≃ T₂) 当且仅当:                             │
│  1. Size(T₁) = Size(T₂)                                                      │
│  2. Align(T₁) = Align(T₂)                                                    │
│  3. dom(Fields(T₁)) = dom(Fields(T₂))  (字段名相同)                          │
│  4. ∀f ∈ dom(Fields(T₁)):                                                    │
│     Fields(T₁)[f].offset = Fields(T₂)[f].offset                              │
│     Fields(T₁)[f].type ≃ Fields(T₂)[f].type                                  │
│                                                                              │
│  定义 10.3: C 布局函数 (C Layout Function)                                   │
│  ───────────────────────────────────────                                     │
│                                                                              │
│  LC: CType → Layout                                                          │
│                                                                              │
│  LC(struct { T₁ f₁; ...; Tₙ fₙ; }) =                                        │
│    let align = max(Align(T₁), ..., Align(Tₙ))                                │
│    let (offsets, size) = LayoutFields([(f₁,T₁), ..., (fₙ,Tₙ)], 0, align)     │
│    in ⟨roundup(size, align), align, offsets⟩                                 │
│                                                                              │
│  LayoutFields([], pos, align) = ([], roundup(pos, align))                    │
│  LayoutFields((f,T)::rest, pos, align) =                                     │
│    let offset = roundup(pos, Align(T))                                       │
│    let (rest_offsets, rest_size) = LayoutFields(rest, offset + Size(T), align)│
│    in ((f, ⟨offset, T⟩)::rest_offsets, rest_size)                            │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 10.2 等价性证明

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     结构体布局等价性形式化证明                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定理 10.1: C struct / Zig extern struct 布局等价性                           │
│  ─────────────────────────────────────────────────                           │
│                                                                              │
│  给定:                                                                       │
│    CS = C struct { C₁ f₁; C₂ f₂; ...; Cₙ fₙ; }                              │
│    ZS = Zig extern struct { f₁: Z₁, f₂: Z₂, ..., fₙ: Zₙ }                   │
│                                                                              │
│  假设:                                                                       │
│    ∀i ∈ [1,n], Cᵢ ≃ Zᵢ (C 和 Zig 类型布局等价)                              │
│                                                                              │
│  证明: LC(CS) = LZ(ZS)                                                       │
│  ─────────────────────────────────────                                       │
│                                                                              │
│  步骤 1: 对齐相等                                                            │
│  ─────────────────────────────────────                                       │
│                                                                              │
│  LC(CS).Align = max(Align(C₁), ..., Align(Cₙ))                              │
│  LZ(ZS).Align = max(Align(Z₁), ..., Align(Zₙ))                              │
│                                                                              │
│  由于 ∀i, Cᵢ ≃ Zᵢ ⟹ Align(Cᵢ) = Align(Zᵢ)                                   │
│  ∴ LC(CS).Align = LZ(ZS).Align                                               │
│                                                                              │
│  步骤 2: 字段偏移相等 (归纳法)                                               │
│  ─────────────────────────────────────────────                               │
│                                                                              │
│  基础: offset(f₁)                                                            │
│    LC: roundup(0, Align(C₁)) = 0 (因为 Align ≥ 1)                           │
│    LZ: roundup(0, Align(Z₁)) = 0                                            │
│    相等 ✓                                                                    │
│                                                                              │
│  归纳: 假设 offset(fₖ) 相等                                                  │
│    LC: posₖ₊₁ = offset(fₖ) + Size(Cₖ)                                       │
│    LZ: posₖ₊₁ = offset(fₖ) + Size(Zₖ)                                       │
│    由于 Cₖ ≃ Zₖ ⟹ Size(Cₖ) = Size(Zₖ)                                       │
│    ∴ posₖ₊₁ 相等                                                             │
│                                                                              │
│    LC: offset(fₖ₊₁) = roundup(posₖ₊₁, Align(Cₖ₊₁))                          │
│    LZ: offset(fₖ₊₁) = roundup(posₖ₊₁, Align(Zₖ₊₁))                          │
│    由于 Align(Cₖ₊₁) = Align(Zₖ₊₁)                                           │
│    ∴ offset(fₖ₊₁) 相等 ✓                                                    │
│                                                                              │
│  步骤 3: 总大小相等                                                          │
│  ─────────────────────                                                       │
│                                                                              │
│  LC(CS).Size = roundup(offset(fₙ) + Size(Cₙ), LC(CS).Align)                 │
│  LZ(ZS).Size = roundup(offset(fₙ) + Size(Zₙ), LZ(ZS).Align)                 │
│                                                                              │
│  由步骤 1: 对齐相等                                                          │
│  由步骤 2: offset(fₙ) 相等                                                   │
│  由假设: Size(Cₙ) = Size(Zₙ)                                                 │
│  ∴ LC(CS).Size = LZ(ZS).Size                                                 │
│                                                                              │
│  结论:                                                                       │
│  ────────                                                                    │
│  LC(CS) = LZ(ZS) ∎                                                           │
│  即 C struct 和 Zig extern struct 布局等价                                   │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 十一、完整代码示例

### 11.1 基础结构体

```c
// C 代码 - basic_struct.c
#ifndef BASIC_STRUCT_H
#define BASIC_STRUCT_H

#include <stdint.h>

// 基础点结构体
struct Point {
    int32_t x;
    int32_t y;
};

// 3D 点
struct Point3D {
    struct Point base;
    int32_t z;
};

// 矩形
struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};

// 颜色 (RGBA)
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// 混合类型结构体
struct Mixed {
    uint8_t type;
    uint16_t flags;
    uint32_t data;
    double timestamp;
};

#endif
```

```zig
// Zig 代码 - basic_struct.zig
const std = @import("std");

// 基础点结构体 - 与 C 完全等价
const Point = extern struct {
    x: i32,
    y: i32,
};

// 3D 点
const Point3D = extern struct {
    base: Point,
    z: i32,
};

// 矩形
const Rect = extern struct {
    top_left: Point,
    bottom_right: Point,
};

// 颜色
const Color = extern struct {
    r: u8,
    g: u8,
    b: u8,
    a: u8,
};

// 混合类型
const Mixed = extern struct {
    type: u8,
    flags: u16,
    data: u32,
    timestamp: f64,
};

// 验证与 C 的等价性
comptime {
    // 大小验证
    std.debug.assert(@sizeOf(Point) == 8);
    std.debug.assert(@sizeOf(Point3D) == 12);
    std.debug.assert(@sizeOf(Rect) == 16);
    std.debug.assert(@sizeOf(Color) == 4);
    std.debug.assert(@sizeOf(Mixed) == 24);

    // 偏移验证
    std.debug.assert(@offsetOf(Point3D, "z") == 8);
    std.debug.assert(@offsetOf(Rect, "bottom_right") == 8);
    std.debug.assert(@offsetOf(Mixed, "timestamp") == 16);
}

pub fn main() void {
    std.debug.print("Point: size={d}, align={d}\n", .{@sizeOf(Point), @alignOf(Point)});
    std.debug.print("Point3D: size={d}, align={d}\n", .{@sizeOf(Point3D), @alignOf(Point3D)});
    std.debug.print("Rect: size={d}, align={d}\n", .{@sizeOf(Rect), @alignOf(Rect)});
    std.debug.print("Color: size={d}, align={d}\n", .{@sizeOf(Color), @alignOf(Color)});
    std.debug.print("Mixed: size={d}, align={d}\n", .{@sizeOf(Mixed), @alignOf(Mixed)});
}
```

### 11.2 复杂嵌套结构体

```c
// C 代码 - complex_struct.c
#ifndef COMPLEX_STRUCT_H
#define COMPLEX_STRUCT_H

#include <stdint.h>
#include <stddef.h>

// 嵌套结构体 - 模拟配置数据结构
struct ConfigValue {
    uint32_t type;
    union {
        int64_t i;
        double f;
        struct {
            const char* data;
            size_t len;
        } s;
        struct {
            void* ptr;
            void (*destructor)(void*);
        } p;
    } value;
};

// 网络包头
struct __attribute__((packed)) PacketHeader {
    uint8_t version;
    uint8_t type;
    uint16_t length;
    uint32_t seq;
    uint64_t timestamp;
};

// 内存分配器统计
struct AllocatorStats {
    struct {
        size_t current;
        size_t peak;
        size_t total;
    } used;
    struct {
        size_t current;
        size_t total;
    } blocks;
    uint32_t alloc_count;
    uint32_t free_count;
};

// 文件元数据
struct FileMetadata {
    uint64_t inode;
    uint64_t size;
    struct {
        int64_t sec;
        int32_t nsec;
    } atime, mtime, ctime;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t links;
};

#endif
```

```zig
// Zig 代码 - complex_struct.zig
const std = @import("std");

// 配置值 - 使用 extern union
const ConfigValue = extern struct {
    type: u32,
    value: extern union {
        i: i64,
        f: f64,
        s: extern struct {
            data: [*c]const u8,
            len: usize,
        },
        p: extern struct {
            ptr: ?*anyopaque,
            destructor: ?*const fn (?*anyopaque) callconv(.C) void,
        },
    },
};

// 网络包头 - packed struct
const PacketHeader = packed struct {
    version: u8,
    type: u8,
    length: u16,
    seq: u32,
    timestamp: u64,
};

// 分配器统计
const AllocatorStats = extern struct {
    used: extern struct {
        current: usize,
        peak: usize,
        total: usize,
    },
    blocks: extern struct {
        current: usize,
        total: usize,
    },
    alloc_count: u32,
    free_count: u32,
};

// 时间戳结构
const Timespec = extern struct {
    sec: i64,
    nsec: i32,
};

// 文件元数据
const FileMetadata = extern struct {
    inode: u64,
    size: u64,
    atime: Timespec,
    mtime: Timespec,
    ctime: Timespec,
    mode: u32,
    uid: u32,
    gid: u32,
    links: u32,
};

comptime {
    // 验证大小
    std.debug.assert(@sizeOf(ConfigValue) == 24);
    std.debug.assert(@sizeOf(PacketHeader) == 16);
    std.debug.assert(@sizeOf(AllocatorStats) == 48);
    std.debug.assert(@sizeOf(FileMetadata) == 72);
}

pub fn main() void {
    std.debug.print("=== Complex Struct Layouts ===\n\n", .{});

    std.debug.print("ConfigValue:\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(ConfigValue), @alignOf(ConfigValue)});
    std.debug.print("  offset(value)={d}\n\n", .{@offsetOf(ConfigValue, "value")});

    std.debug.print("PacketHeader (packed):\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(PacketHeader), @alignOf(PacketHeader)});

    std.debug.print("\nAllocatorStats:\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(AllocatorStats), @alignOf(AllocatorStats)});

    std.debug.print("\nFileMetadata:\n", .{});
    std.debug.print("  size={d}, align={d}\n", .{@sizeOf(FileMetadata), @alignOf(FileMetadata)});
}
```

### 11.3 实际 FFI 用例

```c
// C 代码 - library.h (FFI 接口)
#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 不透明句柄
typedef struct Context* ContextPtr;

// 选项结构体
struct Options {
    uint32_t version;           // 结构体版本
    uint32_t flags;             // 行为标志
    uint16_t timeout_ms;        // 超时毫秒
    uint16_t retry_count;       // 重试次数
    uint32_t buffer_size;       // 缓冲区大小
    const char* config_path;    // 配置文件路径
};

// 结果结构体
struct Result {
    int32_t code;               // 返回码
    uint32_t data_len;          // 数据长度
    const uint8_t* data;        // 数据指针 (由库管理)
};

// 回调函数类型
typedef void (*DataCallback)(const struct Result* result, void* user_data);
typedef void (*LogCallback)(int level, const char* message);

// API 函数
ContextPtr context_create(const struct Options* options);
void context_destroy(ContextPtr ctx);
int context_execute(ContextPtr ctx, const uint8_t* input, uint32_t len, DataCallback cb, void* user_data);
void set_log_callback(LogCallback cb);

// 版本信息
const char* get_version(void);
uint32_t get_api_version(void);

#ifdef __cplusplus
}
#endif

#endif
```

```zig
// Zig 代码 - library_binding.zig
const std = @import("std");

// 导入 C 库
const c = @cImport({
    @cInclude("library.h");
});

// 或使用直接 Zig 映射
pub const ContextPtr = *opaque {};

pub const Options = extern struct {
    version: u32,
    flags: u32,
    timeout_ms: u16,
    retry_count: u16,
    buffer_size: u32,
    config_path: ?[*:0]const u8,
};

pub const Result = extern struct {
    code: i32,
    data_len: u32,
    data: ?[*]const u8,
};

pub const DataCallback = *const fn (*const Result, ?*anyopaque) callconv(.C) void;
pub const LogCallback = *const fn (c_int, [*:0]const u8) callconv(.C) void;

// 外部函数声明
pub extern fn context_create(options: *const Options) ?ContextPtr;
pub extern fn context_destroy(ctx: ?ContextPtr) void;
pub extern fn context_execute(
    ctx: ?ContextPtr,
    input: ?[*]const u8,
    len: u32,
    cb: ?DataCallback,
    user_data: ?*anyopaque,
) c_int;
pub extern fn set_log_callback(cb: ?LogCallback) void;
pub extern fn get_version() [*:0]const u8;
pub extern fn get_api_version() u32;

// 安全的 Zig 包装器
pub const Context = struct {
    ptr: ?ContextPtr,

    pub const Error = error{
        NullContext,
        CreateFailed,
        ExecutionFailed,
    };

    pub fn init(opts: *const Options) Error!Context {
        const ptr = context_create(opts);
        if (ptr == null) return Error.CreateFailed;
        return .{ .ptr = ptr };
    }

    pub fn deinit(self: *Context) void {
        if (self.ptr) |p| {
            context_destroy(p);
            self.ptr = null;
        }
    }

    pub fn execute(
        self: Context,
        input: []const u8,
        comptime Callback: type,
        callback: Callback,
        user_data: ?*anyopaque,
    ) Error!void {
        if (self.ptr == null) return Error.NullContext;

        const cb = struct {
            fn wrapper(result: *const Result, ud: ?*anyopaque) callconv(.C) void {
                const cb_ptr: *const Callback = @ptrCast(@alignCast(ud));
                cb_ptr(result);
            }
        }.wrapper;

        const rc = context_execute(
            self.ptr,
            input.ptr,
            @intCast(input.len),
            cb,
            user_data,
        );

        if (rc != 0) return Error.ExecutionFailed;
    }
};

// 验证 FFI 兼容性
comptime {
    std.debug.assert(@sizeOf(Options) == @sizeOf(c.struct_Options));
    std.debug.assert(@offsetOf(Options, "flags") == @offsetOf(c.struct_Options, "flags"));
    std.debug.assert(@sizeOf(Result) == @sizeOf(c.struct_Result));
}

pub fn main() void {
    std.debug.print("FFI bindings loaded\n", .{});
    std.debug.print("API version: {d}\n", .{get_api_version()});
    std.debug.print("Version: {s}\n", .{get_version()});
}
```

---

> **文档状态**: 核心内容完成 | **验证状态**: 理论框架 + 代码示例 | **最后更新**: 2026-03-12


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
