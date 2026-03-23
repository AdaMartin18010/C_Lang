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
﻿# C11 与 Zig 内存模型对比与映射

> **定位**: 08_Zig_C_Connection / 内存模型 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 建立形式化内存模型对应关系

---

## 目录

- [C11 与 Zig 内存模型对比与映射](#c11-与-zig-内存模型对比与映射)
  - [目录](#目录)
  - [一、内存模型总览](#一内存模型总览)
    - [1.1 内存模型对比全景](#11-内存模型对比全景)
    - [1.2 形式化定义框架](#12-形式化定义框架)
  - [二、内存序类型](#二内存序类型)
    - [2.1 内存序对比表](#21-内存序对比表)
    - [2.2 Relaxed 内存序](#22-relaxed-内存序)
    - [2.3 Acquire-Release 内存序](#23-acquire-release-内存序)
    - [2.4 Sequential Consistency](#24-sequential-consistency)
  - [三、原子操作映射](#三原子操作映射)
    - [3.1 C11 \_Atomic 与 Zig std.atomic.Value](#31-c11-_atomic-与-zig-stdatomicvalue)
    - [3.2 原子操作 API 映射](#32-原子操作-api-映射)
    - [3.3 原子内存序显式指定](#33-原子内存序显式指定)
  - [四、Happens-Before 关系](#四happens-before-关系)
    - [4.1 C11 Happens-Before 定义](#41-c11-happens-before-定义)
    - [4.2 Zig Happens-Before 定义](#42-zig-happens-before-定义)
    - [4.3 同步规则对比](#43-同步规则对比)
  - [五、数据竞争与未定义行为](#五数据竞争与未定义行为)
    - [5.1 数据竞争定义](#51-数据竞争定义)
    - [5.2 C11 数据竞争语义](#52-c11-数据竞争语义)
    - [5.3 Zig 数据竞争语义](#53-zig-数据竞争语义)
    - [5.4 未定义行为对比](#54-未定义行为对比)
  - [六、代码示例](#六代码示例)
    - [6.1 基本原子操作](#61-基本原子操作)
    - [6.2 Spinlock 实现对比](#62-spinlock-实现对比)
    - [6.3 无锁队列](#63-无锁队列)
    - [6.4 生产者-消费者模式](#64-生产者-消费者模式)
    - [6.5 引用计数](#65-引用计数)
  - [七、形式化语义](#七形式化语义)
    - [7.1 操作语义表示](#71-操作语义表示)
    - [7.2 执行轨迹形式化](#72-执行轨迹形式化)
    - [7.3 正确性条件](#73-正确性条件)
  - [八、映射参考表](#八映射参考表)
    - [8.1 操作符映射表](#81-操作符映射表)
    - [8.2 内存序映射表](#82-内存序映射表)
    - [8.3 快速参考](#83-快速参考)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 一、内存模型总览

### 1.1 内存模型对比全景

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     C11 ↔ Zig 内存模型对比全景图                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  特性                     C11 Memory Model        Zig Memory Model          │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  核心标准                 ISO/IEC 9899:2011       语言内置 + std.atomic      │
│  原子类型                 _Atomic T               std.atomic.Value(T)        │
│  原子头文件               <stdatomic.h>           std.atomic                 │
│  线程支持                 <threads.h> (可选)       std.Thread                │
│                                                                              │
│  内存序枚举                                                             │
│  ─────────────────────────────────────────────────────────────────────────  │
│  宽松序                   memory_order_relaxed    .Monotonic                 │
│  获取序                   memory_order_acquire    .Acquire                   │
│  释放序                   memory_order_release    .Release                   │
│  获取-释放                memory_order_acq_rel    .AcqRel                    │
│  顺序一致                 memory_order_seq_cst    .SeqCst                    │
│                                                                              │
│  原子操作类型                                                           │
│  ─────────────────────────────────────────────────────────────────────────  │
│  加载                     atomic_load           load()                       │
│  存储                     atomic_store          store()                      │
│  交换                     atomic_exchange       swap()                       │
│  比较交换                 atomic_compare_...    cmpxchg() / tryCmpxchg()    │
│  获取并添加               atomic_fetch_add      fetchAdd()                   │
│  获取并子               atomic_fetch_sub      fetchSub()                   │
│  获取并与               atomic_fetch_and      fetchAnd()                   │
│  获取或               atomic_fetch_or       fetchOr()                    │
│  获取异或               atomic_fetch_xor      fetchXor()                   │
│                                                                              │
│  栅栏操作                                                               │
│  ─────────────────────────────────────────────────────────────────────────  │
│  线程栅栏                 atomic_thread_fence     std.atomic.fence()         │
│  信号栅栏                 atomic_signal_fence     std.atomic.signalFence()   │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 形式化定义框架

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                         内存模型形式化定义框架                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 1: 内存操作 (Memory Operation)                                         │
│  ─────────────────────────────────────────                                   │
│  内存操作 op ::= Load(l, ord) | Store(l, v, ord) | RMW(l, v, f, ord)        │
│                                                                              │
│  其中:                                                                       │
│    l  ∈ Location       - 内存位置                                            │
│    v  ∈ Value          - 值                                                  │
│    ord ∈ MemoryOrder   - 内存序                                              │
│    f  : Value → Value  - 更新函数                                            │
│                                                                              │
│  定义 2: 内存序 (Memory Order)                                               │
│  ─────────────────────────────────────────                                   │
│  MemoryOrder ::= Relaxed | Acquire | Release | AcqRel | SeqCst              │
│                                                                              │
│  C11 映射:                                                                   │
│    Relaxed  ↔ memory_order_relaxed                                          │
│    Acquire  ↔ memory_order_acquire                                          │
│    Release  ↔ memory_order_release                                          │
│    AcqRel   ↔ memory_order_acq_rel                                          │
│    SeqCst   ↔ memory_order_seq_cst                                          │
│                                                                              │
│  定义 3: 执行轨迹 (Execution Trace)                                          │
│  ─────────────────────────────────────────                                   │
│  执行轨迹 E = (O, <po, <sw, <hb)                                            │
│                                                                              │
│  其中:                                                                       │
│    O  = 操作集合                                                             │
│    <po = 程序序 (Program Order)                                              │
│    <sw = 同步序 (Synchronizes-With)                                          │
│    <hb = Happens-Before 关系                                                 │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 二、内存序类型

### 2.1 内存序对比表

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        内存序详细对比表                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  内存序           C11 常量              Zig 常量              复杂度        │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  Relaxed          memory_order_         .Monotonic            ⭐            │
│                   relaxed                                                    │
│                                                                              │
│  Acquire          memory_order_         .Acquire              ⭐⭐          │
│                   acquire                                                    │
│                                                                              │
│  Release          memory_order_         .Release              ⭐⭐          │
│                   release                                                    │
│                                                                              │
│  AcqRel           memory_order_         .AcqRel               ⭐⭐⭐        │
│                   acq_rel                                                    │
│                                                                              │
│  SeqCst           memory_order_         .SeqCst               ⭐⭐⭐⭐⭐     │
│                   seq_cst                                                    │
│                                                                              │
│  约束强度: Relaxed < Acquire/Release < AcqRel < SeqCst                      │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 Relaxed 内存序

```c
// C11 Relaxed 内存序示例
#include <stdatomic.h>
#include <stdint.h>

_Atomic uint64_t counter = ATOMIC_VAR_INIT(0);

void increment_relaxed(void) {
    // Relaxed 递增 - 无同步保证
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
}

uint64_t get_relaxed(void) {
    // Relaxed 读取
    return atomic_load_explicit(&counter, memory_order_relaxed);
}
```

```zig
// Zig Relaxed 内存序示例
const std = @import("std");
const Atomic = std.atomic.Value;

var counter = Atomic(u64).init(0);

fn incrementRelaxed() void {
    // Relaxed 递增 - 无同步保证
    _ = counter.fetchAdd(1, .Monotonic);
}

fn getRelaxed() u64 {
    // Relaxed 读取
    return counter.load(.Monotonic);
}
```

**形式化语义**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Relaxed 内存序形式化语义                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  语义:                                                                       │
│  ─────────────────                                                           │
│  Load_relaxed(l):                                                           │
│    返回 l 的当前值，不建立任何同步关系                                        │
│                                                                              │
│  Store_relaxed(l, v):                                                       │
│    写入 v 到 l，不建立任何同步关系                                            │
│                                                                              │
│  RMW_relaxed(l, f):                                                         │
│    原子地读取 l 的旧值，计算 f(old)，写入结果                                 │
│    保证 RMW 本身原子，但不与其他操作同步                                      │
│                                                                              │
│  适用场景:                                                                   │
│  ─────────────────                                                           │
│  • 纯计数器（非用于同步）                                                    │
│  • 统计信息收集                                                              │
│  • 标志位检查（当不需要顺序保证时）                                           │
│                                                                              │
│  警告:                                                                       │
│  ─────────────────                                                           │
│  Relaxed 操作不保证可见性顺序！以下代码可能有数据竞争:                        │
│                                                                              │
│    Thread 1:                 Thread 2:                                       │
│    data = 42;                if (flag.load(Relaxed))                         │
│    flag.store(1, Relaxed);       use(data);  // 可能读取旧值!                │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.3 Acquire-Release 内存序

```c
// C11 Acquire-Release 语义示例
#include <stdatomic.h>
#include <stdbool.h>

_Atomic bool flag = ATOMIC_VAR_INIT(false);
int data = 0;

// 生产者线程
void producer(void) {
    data = 42;
    // Release: 确保此操作之前的写入对其他线程可见
    atomic_store_explicit(&flag, true, memory_order_release);
}

// 消费者线程
void consumer(void) {
    // Acquire: 确保看到 flag=true 后，能看到 producer 的写入
    bool expected = true;
    while (!atomic_compare_exchange_weak_explicit(
        &flag, &expected, false,
        memory_order_acquire, memory_order_relaxed));

    // 现在可以安全读取 data
    int result = data;  // 保证看到 42
}
```

```zig
// Zig Acquire-Release 语义示例
const std = @import("std");
const Atomic = std.atomic.Value;

var flag = Atomic(bool).init(false);
var data: i32 = 0;

// 生产者线程
fn producer() void {
    data = 42;
    // Release: 确保此操作之前的写入对其他线程可见
    flag.store(true, .Release);
}

// 消费者线程
fn consumer() void {
    // Acquire: 确保看到 flag=true 后，能看到 producer 的写入
    while (flag.load(.Acquire) != true) {
        std.atomic.spinLoopHint();
    }
    flag.store(false, .Release);

    // 现在可以安全读取 data
    const result = data;  // 保证看到 42
    _ = result;
}
```

**形式化语义**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                   Acquire-Release 形式化语义                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 4: Release 语义                                                        │
│  ─────────────────────────────────────────                                   │
│  Store_release(l, v):                                                       │
│    1. 执行所有之前的内存操作（在程序序中）                                    │
│    2. 写入 v 到 l                                                            │
│    3. 与匹配的 Acquire 操作建立同步关系                                       │
│                                                                              │
│  形式化: ∀ op ∈ O: op <po Store_release(l, v) ⇒ op <sw Load_acquire(l, v)   │
│                                                                              │
│  定义 5: Acquire 语义                                                        │
│  ─────────────────────────────────────────                                   │
│  Load_acquire(l):                                                           │
│    1. 从 l 读取值 v                                                          │
│    2. 确保看到 Release 写入的值后，能看到 Release 之前的所有操作             │
│                                                                              │
│  配对规则 (Synchronizes-With):                                              │
│  ─────────────────────────────────────────                                   │
│  如果:                                                                       │
│    - Thread A 执行 Store_release(l, v)                                      │
│    - Thread B 执行 Load_acquire(l) 且读取到 v                               │
│  则:                                                                         │
│    - Store_release(l, v) <sw Load_acquire(l)                                │
│    - 且所有 Thread A 在 Store 之前的操作 Happens-Before Thread B           │
│                                                                              │
│  AcqRel 语义:                                                               │
│  ─────────────────                                                           │
│  RMW_acq_rel(l, f):                                                         │
│    - 加载部分使用 Acquire 语义                                                │
│    - 存储部分使用 Release 语义                                                │
│    - 用于需要双向同步的场景（如解锁-加锁）                                    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.4 Sequential Consistency

```c
// C11 Sequential Consistency 示例
#include <stdatomic.h>

_Atomic int x = ATOMIC_VAR_INIT(0);
_Atomic int y = ATOMIC_VAR_INIT(0);

void thread1(void) {
    // SeqCst: 所有线程以相同顺序看到此操作
    atomic_store_explicit(&x, 1, memory_order_seq_cst);
    int r1 = atomic_load_explicit(&y, memory_order_seq_cst);
    // r1 == 0 表示 Thread2 的 x=1 还没看到
}

void thread2(void) {
    atomic_store_explicit(&y, 1, memory_order_seq_cst);
    int r2 = atomic_load_explicit(&x, memory_order_seq_cst);
    // r2 == 0 表示 Thread1 的 y=1 还没看到
}

// 不可能同时 r1 == 0 && r2 == 0（使用 SeqCst）
// 但可能使用 Relaxed!
```

```zig
// Zig Sequential Consistency 示例
const std = @import("std");
const Atomic = std.atomic.Value;

var x = Atomic(i32).init(0);
var y = Atomic(i32).init(0);

fn thread1() void {
    // SeqCst: 所有线程以相同顺序看到此操作
    x.store(1, .SeqCst);
    const r1 = y.load(.SeqCst);
    _ = r1;
    // r1 == 0 表示 Thread2 的 x=1 还没看到
}

fn thread2() void {
    y.store(1, .SeqCst);
    const r2 = x.load(.SeqCst);
    _ = r2;
    // r2 == 0 表示 Thread1 的 y=1 还没看到
}

// 不可能同时 r1 == 0 && r2 == 0（使用 SeqCst）
```

**形式化语义**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Sequential Consistency 形式化语义                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 6: Sequential Consistency                                              │
│  ─────────────────────────────────────────                                   │
│  SeqCst 提供:                                                                │
│  1. 所有 SeqCst 操作在全局上有一个一致的顺序 (total order)                   │
│  2. 此顺序与每个线程的程序序一致                                              │
│  3. 每个 SeqCst 加载都看到最后 SeqCst 存储的值                               │
│                                                                              │
│  形式化:                                                                     │
│  ─────────────────                                                           │
│  存在全序 <sc 使得:                                                          │
│    1. <po ⊆ <sc （程序序是全序的子集）                                        │
│    2. 如果 Store_seq_cst(l, v) <sc Load_seq_cst(l)，则加载看到 v            │
│    3. <sc 对所有线程一致                                                      │
│                                                                              │
│  与 AcqRel 的区别:                                                           │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  场景: 独立变量的同步                                                        │
│                                                                              │
│    Thread 1:              Thread 2:                                          │
│    x.store(1, SeqCst);    y.store(1, SeqCst);                                │
│    r1 = y.load(SeqCst);   r2 = x.load(SeqCst);                               │
│                                                                              │
│  SeqCst 保证: 不可能 r1 == 0 && r2 == 0                                     │
│  AcqRel 不保证此性质！                                                       │
│                                                                              │
│  性能考虑:                                                                   │
│  ─────────────────                                                           │
│  SeqCst 通常比 AcqRel 更昂贵（需要更多内存栅栏）                              │
│  仅在需要全局顺序保证时使用                                                   │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 三、原子操作映射

### 3.1 C11 _Atomic 与 Zig std.atomic.Value

```c
// C11 原子类型声明
#include <stdatomic.h>
#include <stdint.h>

// 基本原子类型
_Atomic int atomic_int;
_Atomic uint64_t atomic_u64;
_Atomic double atomic_double;  // C11 支持（通常需要锁）
_Atomic void* atomic_ptr;

// 原子结构体（C11 支持）
typedef struct { int x, y; } Point;
_Atomic Point atomic_point;  // 可能需要锁

// 初始化
_Atomic int counter = ATOMIC_VAR_INIT(0);

// 类型修饰符等价
_Atomic int* ptr_to_atomic;      // 指向原子 int 的指针
_Atomic(int*) atomic_ptr_to_int; // 原子指针指向 int
```

```zig
// Zig 原子类型声明
const std = @import("std");
const Atomic = std.atomic.Value;

// 基本原子类型
var atomic_int = Atomic(i32).init(0);
var atomic_u64 = Atomic(u64).init(0);
// Zig 不支持原子浮点（需手动实现）
var atomic_ptr = Atomic(?*i32).init(null);

// 原子结构体（需确保可原子操作）
const Point = extern struct { x: i32, y: i32 };
var atomic_point: Atomic(Point) = undefined;

// 初始化
var counter = Atomic(i32).init(0);

// 类型修饰符等价
var ptr_to_atomic: *Atomic(i32) = &counter;  // 指向原子 int 的指针
var atomic_ptr_to_int = Atomic(*i32).init(undefined); // 原子指针
```

**类型等价性证明**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     原子类型等价性形式化证明                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定理: C _Atomic T ≅ Zig Atomic(T) 当 sizeof(T) ≤ platform_word_size        │
│                                                                              │
│  证明:                                                                       │
│  ─────────────────                                                           │
│  1. 大小相等:                                                                │
│     sizeof(C _Atomic T) = sizeof(T) 或 sizeof(T) + padding                   │
│     sizeof(Zig Atomic(T)) = sizeof(T) + 对齐填充（编译时确定）                │
│     对于原生支持的原子类型，sizeof 相同                                        │
│                                                                              │
│  2. 对齐相等:                                                                │
│     alignof(C _Atomic T) = max(alignof(T), cache_line_size / 2)             │
│     alignof(Zig Atomic(T)) = @alignOf(T)                                     │
│     通常 alignof(_Atomic T) ≥ alignof(T)                                     │
│                                                                              │
│  3. 操作语义等价:                                                            │
│     C atomic_load_explicit(&a, order)                                        │
│       ≅ a.load(order) in Zig                                                │
│     两者都生成相同的底层指令序列                                               │
│                                                                              │
│  限制条件:                                                                   │
│  ─────────────────                                                           │
│  • C11 允许 _Atomic 应用于任意大小类型（可能使用锁）                          │
│  • Zig Atomic 仅原生支持大小不超过指针的类型                                  │
│  • 大结构体需要手动实现锁机制                                                 │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 原子操作 API 映射

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      原子操作 API 详细映射表                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  操作           C11 API                              Zig API                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  初始化         ATOMIC_VAR_INIT(v)                   Atomic(T).init(v)       │
│  加载           atomic_load(&a)                      a.load(.SeqCst)         │
│  加载(显式)     atomic_load_explicit(&a, ord)        a.load(order)           │
│  存储           atomic_store(&a, v)                  a.store(v, .SeqCst)     │
│  存储(显式)     atomic_store_explicit(&a, v, ord)    a.store(v, order)       │
│  交换           atomic_exchange(&a, v)               a.swap(v, .SeqCst)      │
│  交换(显式)     atomic_exchange_explicit(&a, v, o)   a.swap(v, order)        │
│  比较交换       atomic_compare_exchange_strong       a.cmpxchg(...)          │
│                 (&a, &exp, des)                                               │
│  比较交换(弱)   atomic_compare_exchange_weak         a.tryCmpxchg(...)       │
│                 (&a, &exp, des)                                               │
│                                                                              │
│  算术操作                                                                    │
│  ─────────────────────────────────────────────────────────────────────────  │
│  获取并加       atomic_fetch_add(&a, v)              a.fetchAdd(v, .SeqCst)  │
│  获取并加(显式) atomic_fetch_add_explicit            a.fetchAdd(v, order)    │
│  获取并减       atomic_fetch_sub(&a, v)              a.fetchSub(v, .SeqCst)  │
│  获取并与       atomic_fetch_and(&a, v)              a.fetchAnd(v, .SeqCst)  │
│  获取或       atomic_fetch_or(&a, v)               a.fetchOr(v, .SeqCst)   │
│  获取异或       atomic_fetch_xor(&a, v)              a.fetchXor(v, .SeqCst)  │
│                                                                              │
│  特殊操作                                                                    │
│  ─────────────────────────────────────────────────────────────────────────  │
│  加后获取       (a += v) 后用 SeqCst 加载           a.fetchAdd + load        │
│  获取并最大     atomic_fetch_max (C23)              需手动实现               │
│  获取并最小     atomic_fetch_min (C23)              需手动实现               │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.3 原子内存序显式指定

```c
// C11 显式内存序完整示例
#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    _Atomic int ref_count;
    void* data;
} SharedObject;

SharedObject* create_object(void) {
    SharedObject* obj = malloc(sizeof(SharedObject));
    atomic_init(&obj->ref_count, 1);  // 初始化不需要序
    obj->data = NULL;
    return obj;
}

void retain(SharedObject* obj) {
    // Relaxed: 仅计数，不需要同步
    atomic_fetch_add_explicit(&obj->ref_count, 1, memory_order_relaxed);
}

void release_object(SharedObject* obj) {
    // Release: 确保之前的写入可见
    if (atomic_fetch_sub_explicit(&obj->ref_count, 1,
                                   memory_order_release) == 1) {
        // Acquire: 看到 count==0 后，确保看到所有 retain 的操作
        atomic_thread_fence(memory_order_acquire);
        free(obj->data);
        free(obj);
    }
}

// 更现代的 C11 方式 (C17)
bool compare_and_set(_Atomic int* ptr, int expected, int desired) {
    return atomic_compare_exchange_strong_explicit(
        ptr, &expected, desired,
        memory_order_acq_rel,   // 成功时的内存序
        memory_order_relaxed    // 失败时的内存序
    );
}
```

```zig
// Zig 显式内存序完整示例
const std = @import("std");
const Atomic = std.atomic.Value;

const SharedObject = struct {
    ref_count: Atomic(i32),
    data: ?*anyopaque,
};

fn createObject() *SharedObject {
    const obj = std.heap.page_allocator.create(SharedObject) catch unreachable;
    obj.ref_count = Atomic(i32).init(1);  // 初始化
    obj.data = null;
    return obj;
}

fn retain(obj: *SharedObject) void {
    // Relaxed: 仅计数，不需要同步
    _ = obj.ref_count.fetchAdd(1, .Monotonic);
}

fn releaseObject(obj: *SharedObject) void {
    // Release: 确保之前的写入可见
    if (obj.ref_count.fetchSub(1, .Release) == 1) {
        // Acquire: 看到 count==0 后，确保看到所有 retain 的操作
        std.atomic.fence(.Acquire);
        if (obj.data) |data| {
            std.heap.page_allocator.destroy(@as(*SharedObject, @ptrCast(data)));
        }
        std.heap.page_allocator.destroy(obj);
    }
}

// Zig 比较交换
fn compareAndSet(ptr: *Atomic(i32), expected: i32, desired: i32) bool {
    // tryCmpxchg: 返回 ?i32（旧值），成功时返回 null
    const result = ptr.tryCmpxchg(
        expected,           // 期望值
        desired,            // 新值
        .AcqRel,            // 成功内存序
        .Monotonic          // 失败内存序
    );
    return result == null;  // null 表示成功
}

// Zig cmpxchg 返回结构体版本
fn compareAndSetStrong(ptr: *Atomic(i32), expected: i32, desired: i32) bool {
    const result = ptr.cmpxchg(
        expected,
        desired,
        .AcqRel,
        .Monotonic
    );
    return result == null;
}
```

---

## 四、Happens-Before 关系

### 4.1 C11 Happens-Before 定义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      C11 Happens-Before 形式化定义                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 7: C11 Happens-Before 关系                                             │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  Happens-Before (<hb) 是程序中最基本的顺序关系，定义为:                       │
│                                                                              │
│  1. 程序序 (Program Order, <po):                                            │
│     同一线程内，操作 A 在源代码中出现在 B 之前，则 A <po B                   │
│                                                                              │
│  2. 同步序 (Synchronizes-With, <sw):                                        │
│     如果:                                                                    │
│       - Thread 1 执行 Release/AcqRel/SeqCst 存储到位置 L                    │
│       - Thread 2 执行 Acquire/AcqRel/SeqCst 从 L 加载且看到该值              │
│     则: 存储 <sw 加载                                                        │
│                                                                              │
│  3. Happens-Before 传递闭包:                                                 │
│     A <hb B 当且仅当:                                                        │
│       - A <po B，或                                                          │
│       - A <sw B，或                                                          │
│       - ∃C: A <hb C 且 C <hb B                                               │
│                                                                              │
│  示意图:                                                                     │
│  ─────────────────                                                           │
│                                                                              │
│  Thread 1:              Thread 2:                                            │
│  ─────────────────      ─────────────────                                    │
│  A: data = 42;                                         [程序序]              │
│       ↓                                                                    │
│  B: flag.store(1, Release) ────<sw────> C: while(!flag.load(Acquire));      │
│                                              ↓                               │
│                                          D: use(data);                       │
│                                                                              │
│  Happens-Before 关系: A <po B <sw C <po D                                   │
│  因此: A <hb D，Thread 2 的 D 操作保证看到 data=42                           │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 Zig Happens-Before 定义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Zig Happens-Before 形式化定义                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 8: Zig Happens-Before 关系                                             │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  Zig 的 Happens-Before 与 C11 概念上相同，但表述略有不同:                    │
│                                                                              │
│  1. 程序序:                                                                  │
│     Zig 语句按源代码顺序执行（单个线程内）                                    │
│                                                                              │
│  2. 同步边 (Synchronization Edge):                                          │
│     • a.store(v, .Release) 与 b.load(.Acquire) 配对                         │
│     • a.store(v, .SeqCst) 与 b.load(.SeqCst) 配对                           │
│     • atomic.fence(.Release) / atomic.fence(.Acquire) 配对                  │
│                                                                              │
│  3. 传递性:                                                                  │
│     与 C11 相同，Happens-Before 是传递的                                     │
│                                                                              │
│  Zig 特有的同步原语:                                                         │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  • std.Thread.Mutex: 解锁操作 Release，加锁操作 Acquire                     │
│  • std.Thread.Condition: wait/signal 有各自的同步语义                         │
│  • std.atomic.fence: 显式内存栅栏                                            │
│                                                                              │
│  内存序对应:                                                                 │
│  ─────────────────                                                           │
│  C11 memory_order_release  ↔ Zig .Release                                   │
│  C11 memory_order_acquire  ↔ Zig .Acquire                                   │
│  C11 memory_order_acq_rel  ↔ Zig .AcqRel                                    │
│  C11 memory_order_seq_cst  ↔ Zig .SeqCst                                    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.3 同步规则对比

```c
// C11 同步规则演示
#include <stdatomic.h>
#include <stdbool.h>

_Atomic int flag = ATOMIC_VAR_INIT(0);
int data1 = 0, data2 = 0;

// 规则 1: Release-Acquire 配对
void thread1_rule1(void) {
    data1 = 1;
    data2 = 2;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

void thread2_rule1(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1);
    // 保证看到 data1=1, data2=2
    assert(data1 == 1);
    assert(data2 == 2);
}

// 规则 2: 传递性
_Atomic int flag2 = ATOMIC_VAR_INIT(0);

void thread1_rule2(void) {
    data1 = 1;
    atomic_store_explicit(&flag, 1, memory_order_release);
}

void thread2_rule2(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1);
    data2 = 2;
    atomic_store_explicit(&flag2, 1, memory_order_release);
}

void thread3_rule2(void) {
    while (atomic_load_explicit(&flag2, memory_order_acquire) != 1);
    // 通过传递性，保证看到 data1=1, data2=2
    assert(data1 == 1);
    assert(data2 == 2);
}

// 规则 3: SeqCst 全序
_Atomic int x = ATOMIC_VAR_INIT(0);
_Atomic int y = ATOMIC_VAR_INIT(0);

void thread1_rule3(void) {
    atomic_store_explicit(&x, 1, memory_order_seq_cst);
    int r1 = atomic_load_explicit(&y, memory_order_seq_cst);
}

void thread2_rule3(void) {
    atomic_store_explicit(&y, 1, memory_order_seq_cst);
    int r2 = atomic_load_explicit(&x, memory_order_seq_cst);
}
// 不可能 r1=0 && r2=0（使用 SeqCst）
```

```zig
// Zig 同步规则演示
const std = @import("std");
const Atomic = std.atomic.Value;

var flag = Atomic(i32).init(0);
var data1: i32 = 0;
var data2: i32 = 0;

// 规则 1: Release-Acquire 配对
fn thread1Rule1() void {
    data1 = 1;
    data2 = 2;
    flag.store(1, .Release);
}

fn thread2Rule1() void {
    while (flag.load(.Acquire) != 1) {
        std.atomic.spinLoopHint();
    }
    // 保证看到 data1=1, data2=2
    std.debug.assert(data1 == 1);
    std.debug.assert(data2 == 2);
}

// 规则 2: 传递性
var flag2 = Atomic(i32).init(0);

fn thread1Rule2() void {
    data1 = 1;
    flag.store(1, .Release);
}

fn thread2Rule2() void {
    while (flag.load(.Acquire) != 1) {
        std.atomic.spinLoopHint();
    }
    data2 = 2;
    flag2.store(1, .Release);
}

fn thread3Rule2() void {
    while (flag2.load(.Acquire) != 1) {
        std.atomic.spinLoopHint();
    }
    // 通过传递性，保证看到 data1=1, data2=2
    std.debug.assert(data1 == 1);
    std.debug.assert(data2 == 2);
}

// 规则 3: SeqCst 全序
var x = Atomic(i32).init(0);
var y = Atomic(i32).init(0);

fn thread1Rule3() void {
    x.store(1, .SeqCst);
    const r1 = y.load(.SeqCst);
    _ = r1;
}

fn thread2Rule3() void {
    y.store(1, .SeqCst);
    const r2 = x.load(.SeqCst);
    _ = r2;
}
// 不可能 r1=0 && r2=0（使用 SeqCst）
```

---

## 五、数据竞争与未定义行为

### 5.1 数据竞争定义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       数据竞争形式化定义                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 9: 数据竞争 (Data Race)                                                │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  两个内存访问 A 和 B 构成数据竞争，当且仅当:                                   │
│                                                                              │
│  1. A 和 B 访问同一内存位置                                                  │
│  2. A 和 B 来自不同线程                                                      │
│  3. 至少一个是写操作                                                         │
│  4. A 和 B 之间没有 Happens-Before 关系                                       │
│                                                                              │
│  形式化:                                                                     │
│  ─────────────────                                                           │
│  DataRace(A, B) ≡                                                          │
│    location(A) = location(B) ∧                                              │
│    thread(A) ≠ thread(B) ∧                                                  │
│    (isWrite(A) ∨ isWrite(B)) ∧                                              │
│    ¬(A <hb B ∨ B <hb A)                                                     │
│                                                                              │
│  数据竞争类型:                                                               │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  1. 读-写竞争 (Read-Write Race):                                            │
│     线程 A 读取，线程 B 同时写入，无同步                                      │
│                                                                              │
│  2. 写-写竞争 (Write-Write Race):                                           │
│     线程 A 和 B 同时写入，无同步                                              │
│                                                                              │
│  3. 写-读竞争 (Write-Read Race):                                            │
│     线程 A 写入，线程 B 同时读取，无同步                                      │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 5.2 C11 数据竞争语义

```c
// C11 数据竞争示例
#include <threads.h>
#include <stdio.h>

int shared = 0;  // 非原子变量

int thread_func(void* arg) {
    // 与 main 线程的读取构成数据竞争
    shared = 42;  // 写操作
    return 0;
}

int main(void) {
    thrd_t t;
    thrd_create(&t, thread_func, NULL);

    // 与 thread_func 的写入构成数据竞争
    int value = shared;  // 读操作 - 未定义行为！

    thrd_join(t, NULL);
    printf("value: %d\n", value);  // 可能输出任意值，或崩溃
    return 0;
}

// 正确版本 - 使用原子变量
#include <stdatomic.h>

_Atomic int shared_atomic = ATOMIC_VAR_INIT(0);

int thread_func_safe(void* arg) {
    atomic_store_explicit(&shared_atomic, 42, memory_order_release);
    return 0;
}

int main_safe(void) {
    thrd_t t;
    thrd_create(&t, thread_func_safe, NULL);

    // 使用 acquire 读取，与 release 写入配对
    int value = atomic_load_explicit(&shared_atomic, memory_order_acquire);

    thrd_join(t, NULL);
    printf("value: %d\n", value);  // 保证输出 42
    return 0;
}
```

### 5.3 Zig 数据竞争语义

```zig
// Zig 数据竞争示例
const std = @import("std");

var shared: i32 = 0;  // 非原子变量

fn threadFunc() void {
    // 与 main 的读取构成数据竞争
    shared = 42;  // 写操作
}

pub fn main() !void {
    const t = try std.Thread.spawn(.{}, threadFunc, .{});

    // 与 threadFunc 的写入构成数据竞争
    const value = shared;  // 读操作 - 未定义行为！

    t.join();
    std.debug.print("value: {d}\n", .{value});  // 可能输出任意值
}

// 正确版本 - 使用原子变量
const Atomic = std.atomic.Value;

var shared_atomic = Atomic(i32).init(0);

fn threadFuncSafe() void {
    shared_atomic.store(42, .Release);
}

pub fn mainSafe() !void {
    const t = try std.Thread.spawn(.{}, threadFuncSafe, .{});

    // 使用 acquire 读取，与 release 写入配对
    const value = shared_atomic.load(.Acquire);

    t.join();
    std.debug.print("value: {d}\n", .{value});  // 保证输出 42
}
```

### 5.4 未定义行为对比

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     并发未定义行为对比表                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  场景                     C11 行为              Zig 行为          解决方案   │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  非原子数据竞争           未定义行为            未定义行为          使用原子   │
│  (rw/wr/ww)                                                                 │
│                                                                              │
│  原子类型未对齐           未定义行为            编译错误            确保对齐   │
│                                                                              │
│  信号处理器中使用         未定义行为            未定义行为          使用       │
│  非信号安全函数                                                signalFence   │
│                                                                              │
│  死锁                     未定义行为            运行时可能          锁顺序     │
│                                              挂起/崩溃            或超时     │
│                                                                              │
│  内存序不匹配             允许，可能            允许，可能          正确配对   │
│  (Relaxed 读 Release)     不可预期              不可预期            内存序     │
│                                                                              │
│  双重释放                 未定义行为            运行时检查          RAII/      │
│                                              (安全模式)            智能指针   │
│                                                                              │
│  使用已释放内存           未定义行为            运行时检查          引用计数   │
│                                              (安全模式)            或 Arena   │
│                                                                              │
│  关键区别:                                                                   │
│  ─────────────────                                                           │
│                                                                              │
│  C11: 数据竞争 = 未定义行为（即使使用原子）                                  │
│  Zig: 数据竞争 = 未定义行为，但提供更多安全模式检查                          │
│                                                                              │
│  Zig 的安全模式:                                                             │
│  - 运行时检查内存访问                                                        │
│  - 检测到竞争条件时可 panic                                                  │
│  - ReleaseFast/ReleaseSmall 模式关闭这些检查                                 │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 六、代码示例

### 6.1 基本原子操作

```c
// C11 基本原子操作完整示例
#include <stdatomic.h>
#include <stdio.h>
#include <stdint.h>

// 声明原子变量
_Atomic int32_t counter = ATOMIC_VAR_INIT(0);
_Atomic uint64_t big_counter = ATOMIC_VAR_INIT(0);
_Atomic bool flag = ATOMIC_VAR_INIT(false);
_Atomic(void*) ptr = ATOMIC_VAR_INIT(NULL);

void basic_operations(void) {
    // 加载和存储
    int32_t val = atomic_load(&counter);
    atomic_store(&counter, 100);

    // 显式内存序
    atomic_store_explicit(&counter, 200, memory_order_release);
    val = atomic_load_explicit(&counter, memory_order_acquire);

    // 获取并修改
    int32_t old = atomic_fetch_add(&counter, 1);  // ++counter
    old = atomic_fetch_sub(&counter, 1);           // --counter
    old = atomic_fetch_and(&counter, 0xFF);        // counter &= 0xFF
    old = atomic_fetch_or(&counter, 0x10);         // counter |= 0x10
    old = atomic_fetch_xor(&counter, 0x01);        // counter ^= 0x01

    // 交换
    int32_t prev = atomic_exchange(&counter, 999);

    // 比较交换 (CAS)
    int32_t expected = 999;
    bool success = atomic_compare_exchange_strong(
        &counter, &expected, 1000
    );
    // 成功: counter = 1000, success = true
    // 失败: expected = 当前值, success = false

    // 弱 CAS（允许伪失败，但更快）
    expected = 1000;
    success = atomic_compare_exchange_weak(
        &counter, &expected, 2000
    );
}

// 不同内存序的 CAS
void cas_with_order(void) {
    int32_t exp = 0;
    atomic_compare_exchange_strong_explicit(
        &counter,
        &exp,
        1,
        memory_order_acq_rel,   // 成功时内存序
        memory_order_relaxed    // 失败时内存序
    );
}
```

```zig
// Zig 基本原子操作完整示例
const std = @import("std");
const Atomic = std.atomic.Value;

// 声明原子变量
var counter = Atomic(i32).init(0);
var big_counter = Atomic(u64).init(0);
var flag = Atomic(bool).init(false);
var ptr = Atomic(?*anyopaque).init(null);

fn basicOperations() void {
    // 加载和存储
    const val = counter.load(.SeqCst);
    counter.store(100, .SeqCst);

    // 显式内存序
    counter.store(200, .Release);
    const val2 = counter.load(.Acquire);
    _ = val2;

    // 获取并修改
    const old1 = counter.fetchAdd(1, .SeqCst);   // ++counter
    const old2 = counter.fetchSub(1, .SeqCst);   // --counter
    const old3 = counter.fetchAnd(0xFF, .SeqCst); // counter &= 0xFF
    const old4 = counter.fetchOr(0x10, .SeqCst);  // counter |= 0x10
    const old5 = counter.fetchXor(0x01, .SeqCst); // counter ^= 0x01
    _ = old1; _ = old2; _ = old3; _ = old4; _ = old5;

    // 交换
    const prev = counter.swap(999, .SeqCst);
    _ = prev;

    // 比较交换 - Zig 两种 API
    // 1. cmpxchg: 返回包含旧值和成功标志的结构体
    const result1 = counter.cmpxchg(999, 1000, .SeqCst, .SeqCst);
    // result1.old_value, result1.success

    // 2. tryCmpxchg: 返回 ?T，成功时返回 null，失败返回当前值
    const result2 = counter.tryCmpxchg(1000, 2000, .SeqCst, .SeqCst);
    if (result2) |old_value| {
        // 失败，old_value 是当前值
        _ = old_value;
    } else {
        // 成功
    }
}

// 不同内存序的 CAS
fn casWithOrder() void {
    const exp: i32 = 0;
    const result = counter.tryCmpxchg(
        exp,
        1,
        .AcqRel,    // 成功时内存序
        .Monotonic  // 失败时内存序
    );
    _ = result;
}
```

### 6.2 Spinlock 实现对比

```c
// C11 Spinlock 实现
#include <stdatomic.h>
#include <stdbool.h>
#include <threads.h>

typedef struct {
    _Atomic bool locked;
} Spinlock;

#define SPINLOCK_INIT { ATOMIC_VAR_INIT(false) }

void spinlock_init(Spinlock* lock) {
    atomic_init(&lock->locked, false);
}

void spinlock_lock(Spinlock* lock) {
    // 测试并测试-设置 (test-and-test-and-set)
    for (;;) {
        // 首先宽松检查，避免在锁忙时产生内存屏障
        if (!atomic_load_explicit(&lock->locked, memory_order_relaxed)) {
            // 尝试获取锁
            if (!atomic_exchange_explicit(&lock->locked, true,
                                          memory_order_acquire)) {
                return;  // 成功获取锁
            }
        }
        // 自旋等待
        thrd_yield();
    }
}

bool spinlock_trylock(Spinlock* lock) {
    return !atomic_exchange_explicit(&lock->locked, true,
                                     memory_order_acquire);
}

void spinlock_unlock(Spinlock* lock) {
    // Release: 确保锁内所有写入在解锁后可见
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}

// 使用示例
Spinlock g_lock = SPINLOCK_INIT;
int shared_data = 0;

int worker(void* arg) {
    (void)arg;
    for (int i = 0; i < 1000; i++) {
        spinlock_lock(&g_lock);
        shared_data++;  // 临界区
        spinlock_unlock(&g_lock);
    }
    return 0;
}
```

```zig
// Zig Spinlock 实现
const std = @import("std");
const Atomic = std.atomic.Value;

pub const Spinlock = struct {
    locked: Atomic(bool),

    pub const Self = @This();

    pub fn init() Self {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *Self) void {
        // 测试并测试-设置
        while (true) {
            // 首先宽松检查
            if (!self.locked.load(.Monotonic)) {
                // 尝试获取锁
                const acquired = self.locked.tryCmpxchg(
                    false,
                    true,
                    .Acquire,    // 成功时获取
                    .Monotonic   // 失败时宽松
                );
                if (acquired == null) {
                    return;  // 成功获取锁
                }
            }
            // 自旋等待 - x86 上使用 pause 指令
            std.atomic.spinLoopHint();
        }
    }

    pub fn tryLock(self: *Self) bool {
        const result = self.locked.tryCmpxchg(
            false,
            true,
            .Acquire,
            .Monotonic
        );
        return result == null;
    }

    pub fn unlock(self: *Self) void {
        // Release: 确保锁内所有写入在解锁后可见
        self.locked.store(false, .Release);
    }

    // Zig 惯用法: 提供 LockGuard 模式
    pub fn acquire(self: *Self) LockGuard {
        self.lock();
        return LockGuard{ .lock = self };
    }
};

pub const LockGuard = struct {
    lock: *Spinlock,

    pub fn release(self: LockGuard) void {
        self.lock.unlock();
    }
};

// 使用示例
var g_lock = Spinlock.init();
var shared_data: i32 = 0;

fn worker() void {
    var i: i32 = 0;
    while (i < 1000) : (i += 1) {
        const guard = g_lock.acquire();
        shared_data += 1;  // 临界区
        guard.release();
    }
}

// 或使用 defer 模式
fn workerWithDefer() void {
    var i: i32 = 0;
    while (i < 1000) : (i += 1) {
        g_lock.lock();
        defer g_lock.unlock();
        shared_data += 1;  // 临界区
    }
}
```

### 6.3 无锁队列

```c
// C11 无锁队列 (Michael-Scott 算法简化版)
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node {
    void* data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_init(LockFreeQueue* q) {
    Node* dummy = malloc(sizeof(Node));
    dummy->data = NULL;
    atomic_init(&dummy->next, NULL);
    atomic_init(&q->head, dummy);
    atomic_init(&q->tail, dummy);
}

void queue_enqueue(LockFreeQueue* q, void* data) {
    Node* node = malloc(sizeof(Node));
    node->data = data;
    atomic_init(&node->next, NULL);

    Node* tail;
    Node* next;

    for (;;) {
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        // 检查 tail 是否仍然有效
        if (tail == atomic_load_explicit(&q->tail, memory_order_acquire)) {
            if (next == NULL) {
                // 尝试链接新节点
                if (atomic_compare_exchange_weak_explicit(
                    &tail->next,
                    &next,
                    node,
                    memory_order_release,
                    memory_order_relaxed)) {
                    break;  // 成功
                }
            } else {
                // 帮助移动 tail
                atomic_compare_exchange_weak_explicit(
                    &q->tail,
                    &tail,
                    next,
                    memory_order_release,
                    memory_order_relaxed
                );
            }
        }
    }

    // 尝试移动 tail
    atomic_compare_exchange_weak_explicit(
        &q->tail,
        &tail,
        node,
        memory_order_release,
        memory_order_relaxed
    );
}

bool queue_dequeue(LockFreeQueue* q, void** data) {
    Node* head;
    Node* tail;
    Node* next;

    for (;;) {
        head = atomic_load_explicit(&q->head, memory_order_acquire);
        tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        if (head == atomic_load_explicit(&q->head, memory_order_acquire)) {
            if (head == tail) {
                if (next == NULL) {
                    return false;  // 队列为空
                }
                // 帮助移动 tail
                atomic_compare_exchange_weak_explicit(
                    &q->tail,
                    &tail,
                    next,
                    memory_order_release,
                    memory_order_relaxed
                );
            } else {
                *data = next->data;
                if (atomic_compare_exchange_weak_explicit(
                    &q->head,
                    &head,
                    next,
                    memory_order_release,
                    memory_order_relaxed)) {
                    break;  // 成功
                }
            }
        }
    }

    free(head);  // 释放旧 dummy 节点
    return true;
}
```

```zig
// Zig 无锁队列 (Michael-Scott 算法简化版)
const std = @import("std");
const Atomic = std.atomic.Value;

pub fn LockFreeQueue(comptime T: type) type {
    return struct {
        const Self = @This();

        const Node = struct {
            data: ?T,
            next: Atomic(?*Node),
        };

        head: Atomic(*Node),
        tail: Atomic(*Node),
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) !Self {
            const dummy = try allocator.create(Node);
            dummy.* = .{
                .data = null,
                .next = Atomic(?*Node).init(null),
            };
            return .{
                .head = Atomic(*Node).init(dummy),
                .tail = Atomic(*Node).init(dummy),
                .allocator = allocator,
            };
        }

        pub fn enqueue(self: *Self, data: T) !void {
            const node = try self.allocator.create(Node);
            node.* = .{
                .data = data,
                .next = Atomic(?*Node).init(null),
            };

            while (true) {
                const tail = self.tail.load(.Acquire);
                const next = tail.next.load(.Acquire);

                // 检查 tail 是否仍然有效
                if (tail == self.tail.load(.Acquire)) {
                    if (next == null) {
                        // 尝试链接新节点
                        const result = tail.next.tryCmpxchg(
                            null,
                            node,
                            .Release,
                            .Monotonic
                        );
                        if (result == null) {
                            // 成功，尝试移动 tail
                            _ = self.tail.tryCmpxchg(tail, node, .Release, .Monotonic);
                            return;
                        }
                    } else {
                        // 帮助移动 tail
                        _ = self.tail.tryCmpxchg(tail, next.?, .Release, .Monotonic);
                    }
                }
            }
        }

        pub fn dequeue(self: *Self) ?T {
            while (true) {
                const head = self.head.load(.Acquire);
                const tail = self.tail.load(.Acquire);
                const next = head.next.load(.Acquire);

                if (head == self.head.load(.Acquire)) {
                    if (head == tail) {
                        if (next == null) {
                            return null;  // 队列为空
                        }
                        // 帮助移动 tail
                        _ = self.tail.tryCmpxchg(tail, next.?, .Release, .Monotonic);
                    } else {
                        const data = next.?.data.?;
                        const result = self.head.tryCmpxchg(
                            head,
                            next.?,
                            .Release,
                            .Monotonic
                        );
                        if (result == null) {
                            // 成功
                            self.allocator.destroy(head);
                            return data;
                        }
                    }
                }
            }
        }
    };
}
```

### 6.4 生产者-消费者模式

```c
// C11 生产者-消费者 (有界缓冲区)
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 1024

typedef struct {
    char buffer[BUFFER_SIZE];
    _Atomic size_t write_idx;
    _Atomic size_t read_idx;
} RingBuffer;

void ringbuffer_init(RingBuffer* rb) {
    atomic_init(&rb->write_idx, 0);
    atomic_init(&rb->read_idx, 0);
}

// 返回可写入的字节数
size_t ringbuffer_write_avail(const RingBuffer* rb) {
    size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_relaxed);
    size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_acquire);
    return (BUFFER_SIZE - 1 + read_idx - write_idx) % BUFFER_SIZE;
}

// 返回可读取的字节数
size_t ringbuffer_read_avail(const RingBuffer* rb) {
    size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_acquire);
    size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_relaxed);
    return (BUFFER_SIZE + write_idx - read_idx) % BUFFER_SIZE;
}

bool ringbuffer_write(RingBuffer* rb, const char* data, size_t len) {
    if (ringbuffer_write_avail(rb) < len) {
        return false;  // 空间不足
    }

    size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_relaxed);

    for (size_t i = 0; i < len; i++) {
        rb->buffer[(write_idx + i) % BUFFER_SIZE] = data[i];
    }

    // Release: 确保数据写入在索引更新前可见
    atomic_store_explicit(&rb->write_idx,
                         (write_idx + len) % BUFFER_SIZE,
                         memory_order_release);
    return true;
}

bool ringbuffer_read(RingBuffer* rb, char* data, size_t len) {
    if (ringbuffer_read_avail(rb) < len) {
        return false;  // 数据不足
    }

    size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_relaxed);

    for (size_t i = 0; i < len; i++) {
        data[i] = rb->buffer[(read_idx + i) % BUFFER_SIZE];
    }

    // Release: 确保读取完成在索引更新前可见
    atomic_store_explicit(&rb->read_idx,
                         (read_idx + len) % BUFFER_SIZE,
                         memory_order_release);
    return true;
}
```

```zig
// Zig 生产者-消费者 (有界缓冲区)
const std = @import("std");
const Atomic = std.atomic.Value;

pub fn RingBuffer(comptime size: usize) type {
    return struct {
        const Self = @This();

        buffer: [size]u8,
        write_idx: Atomic(usize),
        read_idx: Atomic(usize),

        pub fn init() Self {
            return .{
                .buffer = undefined,
                .write_idx = Atomic(usize).init(0),
                .read_idx = Atomic(usize).init(0),
            };
        }

        // 返回可写入的字节数
        pub fn writeAvail(self: *const Self) usize {
            const write_idx = self.write_idx.load(.Monotonic);
            const read_idx = self.read_idx.load(.Acquire);
            return (size - 1 + read_idx - write_idx) % size;
        }

        // 返回可读取的字节数
        pub fn readAvail(self: *const Self) usize {
            const write_idx = self.write_idx.load(.Acquire);
            const read_idx = self.read_idx.load(.Monotonic);
            return (size + write_idx - read_idx) % size;
        }

        pub fn write(self: *Self, data: []const u8) bool {
            if (self.writeAvail() < data.len) {
                return false;  // 空间不足
            }

            const write_idx = self.write_idx.load(.Monotonic);

            for (data, 0..) |byte, i| {
                self.buffer[(write_idx + i) % size] = byte;
            }

            // Release: 确保数据写入在索引更新前可见
            self.write_idx.store((write_idx + data.len) % size, .Release);
            return true;
        }

        pub fn read(self: *Self, data: []u8) bool {
            if (self.readAvail() < data.len) {
                return false;  // 数据不足
            }

            const read_idx = self.read_idx.load(.Monotonic);

            for (data, 0..) |*byte, i| {
                byte.* = self.buffer[(read_idx + i) % size];
            }

            // Release: 确保读取完成在索引更新前可见
            self.read_idx.store((read_idx + data.len) % size, .Release);
            return true;
        }

        // 使用切片 API 的惯用版本
        pub fn writeSlice(self: *Self, data: []const u8) ?[]const u8 {
            const avail = self.writeAvail();
            const to_write = @min(avail, data.len);
            if (to_write == 0) return null;

            const write_idx = self.write_idx.load(.Monotonic);

            for (0..to_write) |i| {
                self.buffer[(write_idx + i) % size] = data[i];
            }

            self.write_idx.store((write_idx + to_write) % size, .Release);
            return data[0..to_write];
        }
    };
}
```

### 6.5 引用计数

```c
// C11 引用计数智能指针
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    _Atomic int count;
    void (*destroy)(void*);
    char data[];
} RcHeader;

typedef struct {
    RcHeader* ptr;
} Rc;

Rc rc_new(size_t data_size, void (*destroy)(void*)) {
    RcHeader* header = malloc(sizeof(RcHeader) + data_size);
    atomic_init(&header->count, 1);
    header->destroy = destroy;
    return (Rc){.ptr = header};
}

void* rc_data(Rc rc) {
    return rc.ptr->data;
}

Rc rc_clone(Rc rc) {
    // Relaxed: 仅计数，不需要顺序保证
    atomic_fetch_add_explicit(&rc.ptr->count, 1, memory_order_relaxed);
    return rc;
}

void rc_drop(Rc* rc) {
    if (rc->ptr == NULL) return;

    // Release: 确保之前的写入在可能销毁前可见
    int old_count = atomic_fetch_sub_explicit(&rc->ptr->count, 1,
                                               memory_order_release);

    if (old_count == 1) {
        // 最后一个引用
        // Acquire: 确保看到 count==0 后，能看到所有 clone 的操作
        atomic_thread_fence(memory_order_acquire);

        if (rc->ptr->destroy) {
            rc->ptr->destroy(rc->ptr->data);
        }
        free(rc->ptr);
    }

    rc->ptr = NULL;
}

// 使用示例
typedef struct {
    int value;
} MyData;

void my_data_destroy(void* data) {
    MyData* d = data;
    printf("Destroying data with value %d\n", d->value);
}

void example(void) {
    Rc rc1 = rc_new(sizeof(MyData), my_data_destroy);
    MyData* data = rc_data(rc1);
    data->value = 42;

    Rc rc2 = rc_clone(rc1);  // 引用计数 = 2

    rc_drop(&rc1);  // 引用计数 = 1，不销毁
    rc_drop(&rc2);  // 引用计数 = 0，销毁
}
```

```zig
// Zig 引用计数智能指针
const std = @import("std");
const Atomic = std.atomic.Value;

pub fn Rc(comptime T: type) type {
    return struct {
        const Self = @This();

        const Inner = struct {
            count: Atomic(usize),
            value: T,
        };

        ptr: *Inner,

        pub fn create(allocator: std.mem.Allocator, value: T) !Self {
            const ptr = try allocator.create(Inner);
            ptr.* = .{
                .count = Atomic(usize).init(1),
                .value = value,
            };
            return .{ .ptr = ptr };
        }

        pub fn clone(self: Self) Self {
            // Relaxed: 仅计数，不需要顺序保证
            _ = self.ptr.count.fetchAdd(1, .Monotonic);
            return .{ .ptr = self.ptr };
        }

        pub fn drop(self: *Self, allocator: std.mem.Allocator) void {
            // Release: 确保之前的写入在可能销毁前可见
            const old_count = self.ptr.count.fetchSub(1, .Release);

            if (old_count == 1) {
                // 最后一个引用
                // Acquire: 确保看到 count==0 后，能看到所有 clone 的操作
                std.atomic.fence(.Acquire);

                // 销毁值
                self.ptr.value.deinit();

                // 释放内存
                allocator.destroy(self.ptr);
            }

            self.ptr = undefined;
        }

        pub fn get(self: Self) *T {
            return &self.ptr.value;
        }

        pub fn getConst(self: Self) *const T {
            return &self.ptr.value;
        }
    };
}

// 使用示例
const MyData = struct {
    value: i32,

    pub fn deinit(self: *MyData) void {
        std.debug.print("Destroying data with value {d}\n", .{self.value});
    }
};

fn example() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var rc1 = try Rc(MyData).create(allocator, .{ .value = 42 });
    rc1.get().value = 100;

    var rc2 = rc1.clone();  // 引用计数 = 2

    rc1.drop(allocator);  // 引用计数 = 1，不销毁
    rc2.drop(allocator);  // 引用计数 = 0，销毁
}
```

---

## 七、形式化语义

### 7.1 操作语义表示

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     内存模型操作语义形式化表示                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  配置: Σ = (M, T, <po)                                                       │
│    M: 内存状态 → Location → Value                                            │
│    T: 线程集合                                                               │
│    <po: 程序序关系                                                           │
│                                                                              │
│  基本操作语义:                                                               │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  1. Relaxed 加载:                                                            │
│                                                                              │
│     ────────────────────────────────────────                                 │
│     M(l) = v                                                                 │
│     ──────────────────────────────────────── (Load-Relaxed)                  │
│     <Σ, load_relaxed(l)> → <Σ, v>                                            │
│                                                                              │
│  2. Relaxed 存储:                                                            │
│                                                                              │
│     ──────────────────────────────────────── (Store-Relaxed)                 │
│     <Σ, store_relaxed(l, v)> → <Σ[l↦v], ()>                                  │
│                                                                              │
│  3. Acquire 加载:                                                            │
│                                                                              │
│     M(l) = v                                                                 │
│     ∀s: store_release(l, v') <sw load_acquire(l) ⇒ v = v'                  │
│     ──────────────────────────────────────── (Load-Acquire)                  │
│     <Σ, load_acquire(l)> → <Σ, v>                                            │
│                                                                              │
│  4. Release 存储:                                                            │
│                                                                              │
│     ∀op ∈ T: op <po store_release(l, v) ⇒ op <sw load_acquire(l)           │
│     ──────────────────────────────────────── (Store-Release)                 │
│     <Σ, store_release(l, v)> → <Σ[l↦v], ()>                                  │
│                                                                              │
│  5. RMW (Compare-Exchange):                                                  │
│                                                                              │
│     M(l) = expected                                                          │
│     ──────────────────────────────────────── (CAS-Success)                   │
│     <Σ, cas(l, expected, desired)> → <Σ[l↦desired], true>                    │
│                                                                              │
│     M(l) = actual ≠ expected                                                 │
│     ──────────────────────────────────────── (CAS-Fail)                      │
│     <Σ, cas(l, expected, desired)> → <Σ, false>                              │
│                                                                              │
│  内存序传播规则:                                                             │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  1. 程序序内传播:                                                            │
│     A <po B ∧ A <hb C ⇒ B <hb C                                             │
│                                                                              │
│  2. 同步关系传递:                                                            │
│     A <sw B ∧ B <po C ⇒ A <hb C                                             │
│                                                                              │
│  3. Happens-Before 传递:                                                     │
│     A <hb B ∧ B <hb C ⇒ A <hb C                                             │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7.2 执行轨迹形式化

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        执行轨迹形式化定义                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 10: 执行轨迹 (Execution Trace)                                         │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  执行轨迹 E = (O, <po, <sw, <hb, rf, mo)                                    │
│                                                                              │
│  其中:                                                                       │
│    O  = 操作集合                                                             │
│    <po = 程序序 (Program Order)                                              │
│    <sw = 同步序 (Synchronizes-With)                                          │
│    <hb = Happens-Before 关系 (<po ∪ <sw)+                                    │
│    rf  = 读取来源 (Read-From) 关系                                          │
│    mo  = 修改序 (Modification Order) - 对同一位置写入的全序                 │
│                                                                              │
│  一致性公理:                                                                 │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  1.  happens-before 一致性:                                                  │
│     ∀r ∈ Read: rf(r) <hb r                                                  │
│     （读取必须看到 Happens-Before 它的写入）                                  │
│                                                                              │
│  2. 修改序一致性:                                                            │
│     ∀w1, w2 ∈ Write: w1 <mo w2 ∧ r 从 w2 读取 ⇒ ¬(r <hb w1)                │
│     （读取不能看到 Happens-Before 它的写入之前的写入）                        │
│                                                                              │
│  3. 顺序一致性 (SeqCst):                                                     │
│     所有 SeqCst 操作在全局上有一个一致的顺序                                  │
│                                                                              │
│  C11 与 Zig 的等价性:                                                        │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  定理: 对于相同的原子操作序列，C11 和 Zig 产生相同的执行轨迹类别              │
│                                                                              │
│  证明概要:                                                                   │
│    1. 两者都基于 C++11 内存模型                                              │
│    2. 内存序语义完全对应                                                     │
│    3. 原子操作 API 映射到相同的底层指令                                       │
│    ∴ 执行轨迹等价                                                            │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 7.3 正确性条件

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        内存模型正确性条件                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定义 11: 无数据竞争程序 (Data-Race-Free Program)                            │
│  ─────────────────────────────────────────────────────────                   │
│                                                                              │
│  程序 P 是无数据竞争的，当且仅当:                                             │
│  对于 P 的所有可能执行轨迹 E，都不存在数据竞争                                │
│                                                                              │
│  形式化: DRF(P) ≡ ∀E ∈ executions(P): ¬∃A,B ∈ E: DataRace(A,B)              │
│                                                                              │
│  定义 12: 顺序一致性 (Sequential Consistency)                                │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  程序 P 是顺序一致的，当且仅当:                                               │
│  存在一个全局操作顺序，与每个线程的程序序一致                                  │
│                                                                              │
│  正确性条件层次:                                                             │
│  ─────────────────────────────────────────                                   │
│                                                                              │
│  1. 数据竞争自由 (DRF):                                                      │
│     • 所有共享数据访问使用原子操作                                            │
│     • 原子操作正确配对（Release-Acquire）                                     │
│     • 是最弱的安全保证                                                        │
│                                                                              │
│  2. 顺序一致执行 (SC):                                                       │
│     • 所有同步使用 SeqCst                                                     │
│     • 或证明 AcqRel 足够且正确使用                                            │
│     • 提供最强的直观保证                                                      │
│                                                                              │
│  验证策略:                                                                   │
│  ─────────────────                                                           │
│                                                                              │
│  1. 静态分析:                                                                │
│     • 检查所有共享内存访问                                                    │
│     • 验证内存序配对                                                          │
│     • 工具: ThreadSanitizer, Helgrind                                        │
│                                                                              │
│  2. 动态验证:                                                                │
│     • 压力测试                                                                │
│     • 使用 Relaxed 测试弱内存序下的正确性                                     │
│     • 在不同架构上测试 (x86, ARM, POWER)                                      │
│                                                                              │
│  3. 形式化证明:                                                              │
│     • 使用模型检查器 (CDSChecker, RCU-Checker)                               │
│     • 交互式定理证明 (Coq, Isabelle)                                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 八、映射参考表

### 8.1 操作符映射表

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      原子操作符完整映射表                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C11 操作符                    Zig 操作符               返回值差异          │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  atomic_load(&a)               a.load(.SeqCst)          相同                │
│  atomic_load_explicit          a.load(order)            相同                │
│  (&a, order)                                                                 │
│                                                                              │
│  atomic_store(&a, v)           a.store(v, .SeqCst)      无返回值            │
│  atomic_store_explicit         a.store(v, order)        无返回值            │
│  (&a, v, order)                                                              │
│                                                                              │
│  atomic_exchange(&a, v)        a.swap(v, .SeqCst)       相同: 旧值          │
│  atomic_exchange_explicit      a.swap(v, order)         相同: 旧值          │
│  (&a, v, order)                                                              │
│                                                                              │
│  atomic_fetch_add(&a, v)       a.fetchAdd(v, .SeqCst)   相同: 旧值          │
│  atomic_fetch_add_explicit     a.fetchAdd(v, order)     相同: 旧值          │
│  (&a, v, order)                                                              │
│                                                                              │
│  atomic_fetch_sub(&a, v)       a.fetchSub(v, .SeqCst)   相同: 旧值          │
│  atomic_fetch_and(&a, v)       a.fetchAnd(v, .SeqCst)   相同: 旧值          │
│  atomic_fetch_or(&a, v)        a.fetchOr(v, .SeqCst)    相同: 旧值          │
│  atomic_fetch_xor(&a, v)       a.fetchXor(v, .SeqCst)   相同: 旧值          │
│                                                                              │
│  atomic_compare_exchange_      a.cmpxchg(exp, des,      Zig 返回结构体      │
│    strong(&a, &exp, des,       succ, fail)               包含 old_value     │
│    succ, fail)                                        C 修改 exp 参数       │
│                                                                              │
│  atomic_compare_exchange_      a.tryCmpxchg(exp, des,   Zig 返回 ?T         │
│    weak(&a, &exp, des,         succ, fail)              null = 成功         │
│    succ, fail)                                          非 null = 失败      │
│                                                                              │
│  atomic_thread_fence(order)    std.atomic.fence(order)  相同                │
│  atomic_signal_fence(order)    std.atomic.signalFence() 相同                │
│                                                                              │
│  atomic_is_lock_free(&a)       @atomicRmwSupported(T)   Zig 编译时函数       │
│                                                                              │
│  ATOMIC_VAR_INIT(v)            Atomic(T).init(v)        相同                │
│  atomic_init(&a, v)            a = Atomic(T).init(v)    Zig 直接赋值        │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 8.2 内存序映射表

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       内存序枚举映射表                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C11 memory_order_*          Zig AtomicOrder      说明                       │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  memory_order_relaxed        .Monotonic           宽松序，无同步             │
│  memory_order_consume        (无直接对应)         C11 特有，较少使用         │
│                                                    Zig 使用 .Acquire 替代    │
│  memory_order_acquire        .Acquire             获取序，建立同步           │
│  memory_order_release        .Release             释放序，建立同步           │
│  memory_order_acq_rel        .AcqRel              获取-释放，用于 RMW        │
│  memory_order_seq_cst        .SeqCst              顺序一致，全局序           │
│                                                                              │
│  内存序强度排序:                                                             │
│  ─────────────────                                                           │
│                                                                              │
│  Monotonic < Acquire < AcqRel < SeqCst                                     │
│            < Release <                                                     │
│                                                                              │
│  注意:                                                                       │
│  ─────────────────                                                           │
│  • C11 memory_order_consume 在 Zig 中使用 .Acquire 替代                     │
│  • 这是因为 consume 序在 C11 中实现复杂且容易出错                             │
│  • 现代编译器通常将 consume 提升为 acquire                                  │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 8.3 快速参考

```c
// C11 快速参考
#include <stdatomic.h>

// 声明
_Atomic int a = ATOMIC_VAR_INIT(0);

// 基本操作
int v = atomic_load(&a);
atomic_store(&a, 42);
int old = atomic_exchange(&a, 100);
int prev = atomic_fetch_add(&a, 1);

// 显式内存序
atomic_store_explicit(&a, 1, memory_order_release);
int x = atomic_load_explicit(&a, memory_order_acquire);

// CAS
int expected = 0;
bool ok = atomic_compare_exchange_strong(&a, &expected, 1);

// 栅栏
atomic_thread_fence(memory_order_seq_cst);
```

```zig
// Zig 快速参考
const Atomic = std.atomic.Value;

// 声明
var a = Atomic(i32).init(0);

// 基本操作
const v = a.load(.SeqCst);
a.store(42, .SeqCst);
const old = a.swap(100, .SeqCst);
const prev = a.fetchAdd(1, .SeqCst);

// 显式内存序
a.store(1, .Release);
const x = a.load(.Acquire);

// CAS
const result = a.tryCmpxchg(0, 1, .AcqRel, .Monotonic);
const success = (result == null);

// 栅栏
std.atomic.fence(.SeqCst);

// 提示
std.atomic.spinLoopHint();
```

**完整转换速查表**:

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                    C11 → Zig 内存模型快速转换表                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  模式                        C11                          Zig                │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  头文件                      <stdatomic.h>                std.atomic         │
│  类型                        _Atomic int                  Atomic(i32)        │
│  初始化                      ATOMIC_VAR_INIT(0)           Atomic(i32).init(0)│
│                                                                              │
│  宽松加载                    atomic_load_explicit         .load(.Monotonic)  │
│  (&a, memory_order_relaxed)                                                 │
│                                                                              │
│  获取加载                    atomic_load_explicit         .load(.Acquire)    │
│  (&a, memory_order_acquire)                                                 │
│                                                                              │
│  宽松存储                    atomic_store_explicit        .store(v,          │
│  (&a, v, memory_order_                                  .Monotonic)          │
│  relaxed)                                                                    │
│                                                                              │
│  释放存储                    atomic_store_explicit        .store(v,          │
│  (&a, v, memory_order_                                  .Release)            │
│  release)                                                                    │
│                                                                              │
│  获取-释放 RMW            atomic_fetch_add_explicit     .fetchAdd(v,        │
│  (&a, v, memory_order_                                  .AcqRel)             │
│  acq_rel)                                                                    │
│                                                                              │
│  CAS (强)                 atomic_compare_exchange_      .tryCmpxchg(         │
│  strong(&a, &e, d, s, f)  e, d, s, f) == null                               │
│                                                                              │
│  CAS (弱)                 atomic_compare_exchange_      .tryCmpxchgWeak(     │
│  weak(&a, &e, d, s, f)    e, d, s, f) == null  (Zig 无弱版本)               │
│                                                                              │
│  线程栅栏                 atomic_thread_fence           std.atomic.fence     │
│  (memory_order_release)   (.Release)                                         │
│                                                                              │
│  常见惯用法:                                                                 │
│  ─────────────────                                                           │
│                                                                              │
│  C: atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed)            │
│  Zig: _ = counter.fetchAdd(1, .Monotonic);                                   │
│                                                                              │
│  C: while (!atomic_compare_exchange_weak(&lock, &e, 1,                      │
│           memory_order_acquire, memory_order_relaxed));                      │
│  Zig: while (lock.tryCmpxchg(false, true, .Acquire, .Monotonic)             │
│         != null) std.atomic.spinLoopHint();                                  │
│                                                                              │
│  C: atomic_store_explicit(&flag, 1, memory_order_release);                  │
│  Zig: flag.store(1, .Release);                                               │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

> **文档状态**: 核心内容完成 | **验证状态**: 理论框架 | **最后更新**: 2026-03-12


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
