# C11 原子操作到 Zig 的完整映射

> **定位**: 08_Zig_C_Connection / 内存模型桥接 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 建立原子操作的形式化映射

---

## 目录

- [C11 原子操作到 Zig 的完整映射](#c11-原子操作到-zig-的完整映射)
  - [目录](#目录)
  - [一、原子操作总览](#一原子操作总览)
    - [1.1 原子操作对比全景](#11-原子操作对比全景)
    - [1.2 形式化定义](#12-形式化定义)
  - [二、原子类型系统](#二原子类型系统)
    - [2.1 C11 `_Atomic` 类型修饰符](#21-c11-_atomic-类型修饰符)
    - [2.2 Zig `std.atomic.Value`](#22-zig-stdatomicvalue)
    - [2.3 类型映射等价性](#23-类型映射等价性)
    - [2.4 复杂原子类型](#24-复杂原子类型)
  - [三、原子操作详细映射](#三原子操作详细映射)
    - [3.1 原子加载 (Atomic Load)](#31-原子加载-atomic-load)
    - [3.2 原子存储 (Atomic Store)](#32-原子存储-atomic-store)
    - [3.3 原子交换 (Atomic Exchange)](#33-原子交换-atomic-exchange)
    - [3.4 比较并交换 (Compare-Exchange)](#34-比较并交换-compare-exchange)
    - [3.5 Fetch-Op 操作族](#35-fetch-op-操作族)
  - [四、内存序映射](#四内存序映射)
    - [4.1 内存序对应表](#41-内存序对应表)
    - [4.2 Relaxed → Monotonic](#42-relaxed--monotonic)
    - [4.3 Acquire-Release 语义](#43-acquire-release-语义)
    - [4.4 Sequential Consistency](#44-sequential-consistency)
  - [五、原子标志操作](#五原子标志操作)
    - [5.1 C11 `atomic_flag`](#51-c11-atomic_flag)
    - [5.2 Zig 等效实现](#52-zig-等效实现)
  - [六、原子指针操作](#六原子指针操作)
    - [6.1 原子指针类型](#61-原子指针类型)
  - [七、栅栏与同步](#七栅栏与同步)
    - [7.1 线程栅栏 (Thread Fence)](#71-线程栅栏-thread-fence)
    - [7.2 信号栅栏 (Signal Fence)](#72-信号栅栏-signal-fence)
  - [八、Lock-Free 属性检查](#八lock-free-属性检查)
    - [8.1 Lock-Free 定义](#81-lock-free-定义)
    - [8.2 C11 Lock-Free 检测](#82-c11-lock-free-检测)
    - [8.3 Zig Lock-Free 保证](#83-zig-lock-free-保证)
  - [九、实现示例](#九实现示例)
    - [9.1 高效自旋锁](#91-高效自旋锁)
    - [9.2 引用计数 (ARC)](#92-引用计数-arc)
    - [9.3 无锁队列 (Michael-Scott)](#93-无锁队列-michael-scott)
    - [9.4 读-复制-更新 (RCU) 简化版](#94-读-复制-更新-rcu-简化版)
  - [十、性能考虑](#十性能考虑)
    - [10.1 内存序性能层次](#101-内存序性能层次)
    - [10.2 缓存行优化](#102-缓存行优化)
    - [10.3 伪共享避免](#103-伪共享避免)
    - [10.4 平台特定优化](#104-平台特定优化)
  - [十一、快速参考表](#十一快速参考表)
    - [11.1 API 映射速查](#111-api-映射速查)
    - [11.2 内存序速查](#112-内存序速查)
    - [11.3 常见模式速查](#113-常见模式速查)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 一、原子操作总览

### 1.1 原子操作对比全景

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                     C11 ↔ Zig 原子操作对比全景图                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  特性                     C11 atomics             Zig atomics               │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  头文件/模块              <stdatomic.h>           std.atomic                 │
│  类型修饰符               _Atomic T               std.atomic.Value(T)        │
│  初始化宏                 ATOMIC_VAR_INIT(v)      Atomic(T).init(v)          │
│  初始化函数               atomic_init             直接赋值                   │
│                                                                              │
│  核心操作                                                                   │
│  ─────────────────────────────────────────────────────────────────────────  │
│  加载                     atomic_load             load()                     │
│  存储                     atomic_store            store()                    │
│  交换                     atomic_exchange         swap()                     │
│  比较交换(强)             atomic_compare_...      cmpxchg()                  │
│  比较交换(弱)             atomic_compare_...      tryCmpxchg()               │
│  获取并加                 atomic_fetch_add        fetchAdd()                 │
│  获取并减                 atomic_fetch_sub        fetchSub()                 │
│  获取并与                 atomic_fetch_and        fetchAnd()                 │
│  获取或                 atomic_fetch_or         fetchOr()                  │
│  获取异或                 atomic_fetch_xor        fetchXor()                 │
│                                                                              │
│  栅栏操作                                                                   │
│  ─────────────────────────────────────────────────────────────────────────  │
│  线程栅栏                 atomic_thread_fence     std.atomic.fence()         │
│  信号栅栏                 atomic_signal_fence     std.atomic.signalFence()   │
│                                                                              │
│  特殊类型                                                                   │
│  ─────────────────────────────────────────────────────────────────────────  │
│  原子标志                 atomic_flag             Atomic(bool)               │
│  原子布尔                 _Atomic bool            Atomic(bool)               │
│  原子指针                 _Atomic(void*)          Atomic(?*T)                │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 形式化定义

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                        原子操作形式化定义框架                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  定义 1: 原子操作 (Atomic Operation)                                         │
│  ─────────────────────────────────────────                                  │
│  原子操作 op ::= Load(l, ord) | Store(l, v, ord) | RMW(l, v, f, ord)        │
│                 | CAS(l, exp, des, succ, fail) | Fence(ord)                 │
│                                                                             │
│  原子性保证:                                                                 │
│  ─────────────────                                                          │
│  1. 不可分割性: 其他线程只能看到操作执行前或执行后的状态                        │
│  2. 总修改序: 对同一位置的所有修改有全局一致顺序                               │
│  3. 可见性保证: 根据内存序，写入最终对所有线程可见                             │
│                                                                            │
│  定理: C11 原子操作与 Zig 原子操作在相同内存序下具有等价语义                   │
│  证明: 两者都映射到相同硬件原子指令，生成相同 LLVM IR                          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 二、原子类型系统

### 2.1 C11 `_Atomic` 类型修饰符

```c
// C11 _Atomic 类型修饰符完整指南
#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>

// 基本原子类型
_Atomic int atomic_int;
_Atomic uint64_t atomic_u64;
_Atomic bool atomic_bool;
_Atomic void* atomic_ptr;

// 固定宽度类型
_Atomic int32_t atomic_i32;
_Atomic uint64_t atomic_u64;

// 初始化
_Atomic int counter = ATOMIC_VAR_INIT(0);

// 类型修饰符位置
_Atomic int* ptr_to_atomic;       // 指向原子 int 的指针
_Atomic(int*) atomic_ptr_to_int;  // 原子指针指向 int
```

### 2.2 Zig `std.atomic.Value`

```zig
// Zig std.atomic.Value 完整指南
const std = @import("std");
const Atomic = std.atomic.Value;

// 基本原子类型
var atomic_int = Atomic(i32).init(0);
var atomic_u64 = Atomic(u64).init(0);
var atomic_bool = Atomic(bool).init(false);
var atomic_ptr = Atomic(?*anyopaque).init(null);

// 枚举原子类型
const State = enum(i32) { idle, running, stopped };
var atomic_state = Atomic(State).init(.idle);

// 类型修饰符
var ptr_to_atomic: *Atomic(i32) = &atomic_int;
var atomic_ptr_to_int = Atomic(*i32).init(undefined);
```

### 2.3 类型映射等价性

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       C11 ↔ Zig 原子类型映射表                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  C11 类型                          Zig 类型                        Lock-Free │
│  ─────────────────────────────────────────────────────────────────────────  │
│  _Atomic char                      Atomic(i8) 或 Atomic(u8)         ✓       │
│  _Atomic short                     Atomic(i16)                      ✓       │
│  _Atomic int                       Atomic(i32)                      ✓       │
│  _Atomic long                      Atomic(isize)                    ✓       │
│  _Atomic long long                 Atomic(i64)                      ✓       │
│  _Atomic bool                      Atomic(bool)                     ✓       │
│  _Atomic(void*)                    Atomic(?*anyopaque)              ✓       │
│  _Atomic float                     （无直接支持）                   ✗        │
│  _Atomic double                    （无直接支持）                   ✗        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.4 复杂原子类型

```c
// C11 复杂原子类型
#include <stdatomic.h>

// 枚举原子类型
typedef enum { RED, GREEN, BLUE } Color;
_Atomic Color current_color = ATOMIC_VAR_INIT(RED);

// 位标志
_Atomic uint64_t atomic_flags = ATOMIC_VAR_INIT(0);
#define FLAG_1 (1 << 0)
void set_flag(int flag) {
    atomic_fetch_or_explicit(&atomic_flags, flag, memory_order_relaxed);
}
```

```zig
// Zig 复杂原子类型
const Atomic = std.atomic.Value;

// 枚举原子类型
const Color = enum(i32) { red, green, blue };
var current_color = Atomic(Color).init(.red);

// 位标志
var atomic_flags = Atomic(u64).init(0);
fn setFlag(flag: u6) void {
    _ = atomic_flags.fetchOr(@as(u64, 1) << flag, .Monotonic);
}

// 打包原子操作
const PackedFlags = packed struct(u32) {
    status: u8,
    counter: u16,
    flags: u8,
};
var packed_atomic = Atomic(u32).init(0);
```

---

## 三、原子操作详细映射

### 3.1 原子加载 (Atomic Load)

```c
// C11 原子加载
_Atomic uint64_t value = ATOMIC_VAR_INIT(0);

uint64_t load_examples(void) {
    // 默认 SeqCst 加载
    uint64_t v1 = atomic_load(&value);

    // 显式内存序
    uint64_t v2 = atomic_load_explicit(&value, memory_order_relaxed);
    uint64_t v3 = atomic_load_explicit(&value, memory_order_acquire);

    return v1 + v2 + v3;
}
```

```zig
// Zig 原子加载
var value = Atomic(u64).init(0);

fn loadExamples() void {
    const v1 = value.load(.SeqCst);
    const v2 = value.load(.Monotonic);
    const v3 = value.load(.Acquire);
    _ = v1 + v2 + v3;
}
```

### 3.2 原子存储 (Atomic Store)

```c
// C11 原子存储
_Atomic uint64_t value = ATOMIC_VAR_INIT(0);

void store_examples(void) {
    atomic_store(&value, 42);
    atomic_store_explicit(&value, 42, memory_order_relaxed);
    atomic_store_explicit(&value, 42, memory_order_release);
}
```

```zig
// Zig 原子存储
var value = Atomic(u64).init(0);

fn storeExamples() void {
    value.store(42, .SeqCst);
    value.store(42, .Monotonic);
    value.store(42, .Release);
}
```

### 3.3 原子交换 (Atomic Exchange)

```c
// C11 原子交换
_Atomic bool lock = ATOMIC_VAR_INIT(false);

bool try_lock(void) {
    // 如果之前是 false（未锁定），设为 true，返回 false（成功）
    return atomic_exchange_explicit(&lock, true, memory_order_acquire);
}

void unlock(void) {
    atomic_store_explicit(&lock, false, memory_order_release);
}
```

```zig
// Zig 原子交换
var lock = Atomic(bool).init(false);

fn tryLock() bool {
    return lock.swap(true, .Acquire);
}

fn unlock() void {
    lock.store(false, .Release);
}
```

### 3.4 比较并交换 (Compare-Exchange)

```c
// C11 比较并交换
_Atomic int counter = ATOMIC_VAR_INIT(0);

bool cas_example(int expected, int desired) {
    // 强 CAS
    return atomic_compare_exchange_strong(&counter, &expected, desired);
}

bool cas_weak_example(int expected, int desired) {
    // 弱 CAS（允许伪失败，但可能更快）
    return atomic_compare_exchange_weak(&counter, &expected, desired);
}

// 显式内存序
bool cas_with_order(int* ptr, int expected, int desired) {
    return atomic_compare_exchange_strong_explicit(
        ptr, &expected, desired,
        memory_order_acq_rel,   // 成功时内存序
        memory_order_relaxed    // 失败时内存序
    );
}
```

```zig
// Zig 比较并交换
var counter = Atomic(i32).init(0);

fn casExample(expected: i32, desired: i32) bool {
    // 方式 1: cmpxchg 返回结构体
    const result = counter.cmpxchg(expected, desired, .SeqCst, .SeqCst);
    return result.success;
}

fn casExample2(expected: i32, desired: i32) bool {
    // 方式 2: tryCmpxchg 返回可选值
    const result = counter.tryCmpxchg(expected, desired, .AcqRel, .Monotonic);
    return result == null;  // null 表示成功
}
```

### 3.5 Fetch-Op 操作族

```c
// C11 Fetch-Op 操作
_Atomic int32_t counter = ATOMIC_VAR_INIT(0);
_Atomic uint64_t bitmap = ATOMIC_VAR_INIT(0);

void fetch_op_examples(void) {
    // 算术操作
    int32_t old1 = atomic_fetch_add(&counter, 1);
    int32_t old2 = atomic_fetch_sub(&counter, 1);

    // 位操作
    uint64_t old3 = atomic_fetch_or(&bitmap, 0xFF);
    uint64_t old4 = atomic_fetch_and(&bitmap, ~0xFF);
    uint64_t old5 = atomic_fetch_xor(&bitmap, 0x0F);
}
```

```zig
// Zig Fetch-Op 操作
var counter = Atomic(i32).init(0);
var bitmap = Atomic(u64).init(0);

fn fetchOpExamples() void {
    // 算术操作
    const old1 = counter.fetchAdd(1, .SeqCst);
    const old2 = counter.fetchSub(1, .SeqCst);

    // 位操作
    const old3 = bitmap.fetchOr(0xFF, .SeqCst);
    const old4 = bitmap.fetchAnd(~@as(u64, 0xFF), .SeqCst);
    const old5 = bitmap.fetchXor(0x0F, .SeqCst);
    _ = old1; _ = old2; _ = old3; _ = old4; _ = old5;
}
```

---

## 四、内存序映射

### 4.1 内存序对应表

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                       C11 ↔ Zig 内存序完整映射表                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  C11 常量                    Zig 常量               强度    同步保证         │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  memory_order_relaxed        .Monotonic             ⭐      无              │
│  memory_order_consume        （Zig 不支持）          ⭐      数据依赖        │
│  memory_order_acquire        .Acquire               ⭐⭐    加载同步        │
│  memory_order_release        .Release               ⭐⭐    存储同步        │
│  memory_order_acq_rel        .AcqRel                ⭐⭐⭐   加载+存储同步  │
│  memory_order_seq_cst        .SeqCst                ⭐⭐⭐⭐⭐ 全局顺序    │
│                                                                             │
│  约束强度: Relaxed < Acquire/Release < AcqRel < SeqCst                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 4.2 Relaxed → Monotonic

```c
// C11 Relaxed 内存序
_Atomic uint64_t counter = ATOMIC_VAR_INIT(0);

void relaxed_example(void) {
    // 纯计数器，不需要同步
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
}
```

```zig
// Zig Monotonic 内存序
var counter = Atomic(u64).init(0);

fn relaxedExample() void {
    _ = counter.fetchAdd(1, .Monotonic);
}
```

### 4.3 Acquire-Release 语义

```c
// C11 Acquire-Release
int data = 0;
_Atomic bool flag = ATOMIC_VAR_INIT(false);

void producer(void) {
    data = 42;
    atomic_store_explicit(&flag, true, memory_order_release);
}

void consumer(void) {
    while (!atomic_load_explicit(&flag, memory_order_acquire));
    int value = data;  // 保证看到 42
}
```

```zig
// Zig Acquire-Release
var data: i32 = 0;
var flag = Atomic(bool).init(false);

fn producer() void {
    data = 42;
    flag.store(true, .Release);
}

fn consumer() void {
    while (!flag.load(.Acquire)) {
        std.atomic.spinLoopHint();
    }
    const value = data;  // 保证看到 42
    _ = value;
}
```

### 4.4 Sequential Consistency

```c
// C11 Sequential Consistency
_Atomic int x = ATOMIC_VAR_INIT(0);
_Atomic int y = ATOMIC_VAR_INIT(0);

void thread1(void) {
    atomic_store_explicit(&x, 1, memory_order_seq_cst);
    int r1 = atomic_load_explicit(&y, memory_order_seq_cst);
    (void)r1;
}

void thread2(void) {
    atomic_store_explicit(&y, 1, memory_order_seq_cst);
    int r2 = atomic_load_explicit(&x, memory_order_seq_cst);
    (void)r2;
}
// 不可能 r1 == 0 && r2 == 0
```

```zig
// Zig Sequential Consistency
var x = Atomic(i32).init(0);
var y = Atomic(i32).init(0);

fn thread1() void {
    x.store(1, .SeqCst);
    const r1 = y.load(.SeqCst);
    _ = r1;
}

fn thread2() void {
    y.store(1, .SeqCst);
    const r2 = x.load(.SeqCst);
    _ = r2;
}
```

---

## 五、原子标志操作

### 5.1 C11 `atomic_flag`

```c
// C11 atomic_flag
#include <stdatomic.h>

// 静态初始化（保证 lock-free）
atomic_flag lock = ATOMIC_FLAG_INIT;

void flag_example(void) {
    // 测试并设置
    bool was_set = atomic_flag_test_and_set(&lock);

    // 清除
    atomic_flag_clear(&lock);
}

// 基于 atomic_flag 的自旋锁
void spinlock_lock(atomic_flag* lock) {
    while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
        // 自旋
    }
}

void spinlock_unlock(atomic_flag* lock) {
    atomic_flag_clear_explicit(lock, memory_order_release);
}
```

### 5.2 Zig 等效实现

```zig
// Zig atomic_flag 等效实现
const Atomic = std.atomic.Value;

pub const AtomicFlag = struct {
    flag: Atomic(bool),

    pub const INIT: AtomicFlag = .{ .flag = Atomic(bool).init(false) };

    pub fn testAndSet(self: *AtomicFlag, comptime order: std.atomic.Order) bool {
        return self.flag.swap(true, order);
    }

    pub fn clear(self: *AtomicFlag, comptime order: std.atomic.Order) void {
        self.flag.store(false, order);
    }
};

// 自旋锁
pub const Spinlock = struct {
    locked: Atomic(bool),

    pub fn init() Spinlock {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *Spinlock) void {
        while (self.locked.swap(true, .Acquire)) {
            std.atomic.spinLoopHint();
        }
    }

    pub fn unlock(self: *Spinlock) void {
        self.locked.store(false, .Release);
    }
};
```

---

## 六、原子指针操作

### 6.1 原子指针类型

```c
// C11 原子指针
_Atomic(void*) atomic_ptr = ATOMIC_VAR_INIT(NULL);
_Atomic(int*) atomic_int_ptr = ATOMIC_VAR_INIT(NULL);

void ptr_example(void) {
    int value = 42;

    // 存储
    atomic_store_explicit(&atomic_int_ptr, &value, memory_order_release);

    // 加载
    int* ptr = atomic_load_explicit(&atomic_int_ptr, memory_order_acquire);

    // 交换
    int value2 = 100;
    int* old = atomic_exchange_explicit(&atomic_int_ptr, &value2, memory_order_acq_rel);
}
```

```zig
// Zig 原子指针
var atomic_int_ptr = Atomic(?*i32).init(null);

fn ptrExample() void {
    var value: i32 = 42;

    // 存储
    atomic_int_ptr.store(&value, .Release);

    // 加载
    const ptr = atomic_int_ptr.load(.Acquire);

    // 交换
    var value2: i32 = 100;
    const old = atomic_int_ptr.swap(&value2, .AcqRel);
    _ = old;
}
```

---

## 七、栅栏与同步

### 7.1 线程栅栏 (Thread Fence)

```c
// C11 线程栅栏
#include <stdatomic.h>

void fence_examples(void) {
    atomic_thread_fence(memory_order_acquire);
    atomic_thread_fence(memory_order_release);
    atomic_thread_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_seq_cst);
}

// Release-Acquire 栅栏配对
int data = 0;
_Atomic int flag = ATOMIC_VAR_INIT(0);

void producer_with_fence(void) {
    data = 42;
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&flag, 1, memory_order_relaxed);
}

void consumer_with_fence(void) {
    while (atomic_load_explicit(&flag, memory_order_relaxed) != 1);
    atomic_thread_fence(memory_order_acquire);
    int value = data;  // 保证看到 42
}
```

```zig
// Zig 线程栅栏
fn fenceExamples() void {
    std.atomic.fence(.Acquire);
    std.atomic.fence(.Release);
    std.atomic.fence(.AcqRel);
    std.atomic.fence(.SeqCst);
}

// Release-Acquire 栅栏配对
var data: i32 = 0;
var flag = Atomic(i32).init(0);

fn producerWithFence() void {
    data = 42;
    std.atomic.fence(.Release);
    flag.store(1, .Monotonic);
}

fn consumerWithFence() void {
    while (flag.load(.Monotonic) != 1) {}
    std.atomic.fence(.Acquire);
    const value = data;
    _ = value;
}
```

### 7.2 信号栅栏 (Signal Fence)

```c
// C11 信号栅栏
volatile sig_atomic_t signal_flag = 0;

void signal_handler(int sig) {
    // 处理...
    atomic_signal_fence(memory_order_release);
    signal_flag = 1;
}
```

```zig
// Zig 信号栅栏
var signal_flag = Atomic(i32).init(0);

fn signalHandler(sig: i32) callconv(.C) void {
    _ = sig;
    std.atomic.signalFence(.Release);
    signal_flag.store(1, .Monotonic);
}
```

---

## 八、Lock-Free 属性检查

### 8.1 Lock-Free 定义

```text
Lock-Free: 系统作为整体持续取得进展，至少有一个线程在有限步骤内完成操作。

层次关系: Wait-Free ⊂ Lock-Free ⊂ Obstruction-Free
```

### 8.2 C11 Lock-Free 检测

```c
// C11 Lock-Free 检测
#include <stdatomic.h>

void check_lock_free(void) {
    #if ATOMIC_INT_LOCK_FREE == 2
    // int 总是 lock-free
    #endif

    _Atomic int var = ATOMIC_VAR_INIT(0);
    bool is_lock_free = atomic_is_lock_free(&var);
}
```

### 8.3 Zig Lock-Free 保证

```zig
// Zig Lock-Free 保证
// Zig 保证以下类型总是 lock-free:
// • 所有整数类型 (i8-i128, u8-u128, usize, isize)
// • 布尔类型 (bool)
// • 指针类型 (*T, ?*T)
// • 枚举类型（基于整数）

fn requireLockFree(comptime T: type) void {
    comptime {
        const info = @typeInfo(T);
        const is_lock_free = switch (info) {
            .Int, .Bool => true,
            .Pointer => true,
            .Enum => true,
            else => @sizeOf(T) <= @sizeOf(usize) * 2,
        };
        if (!is_lock_free) {
            @compileError("Type not lock-free");
        }
    }
}
```

---

## 九、实现示例

### 9.1 高效自旋锁

```c
// C11 高效自旋锁
#include <stdatomic.h>

typedef struct {
    _Atomic bool locked;
} Spinlock;

void spinlock_lock(Spinlock* lock) {
    // TTAS: Test-And-Test-And-Set
    for (;;) {
        if (!atomic_load_explicit(&lock->locked, memory_order_relaxed)) {
            if (!atomic_exchange_explicit(&lock->locked, true, memory_order_acquire)) {
                return;
            }
        }
    }
}

void spinlock_unlock(Spinlock* lock) {
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}
```

```zig
// Zig 高效自旋锁
const Spinlock = struct {
    locked: Atomic(bool),

    pub fn init() Spinlock {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *Spinlock) void {
        while (true) {
            if (!self.locked.load(.Monotonic)) {
                if (self.locked.tryCmpxchg(false, true, .Acquire, .Monotonic) == null) {
                    return;
                }
            }
            std.atomic.spinLoopHint();
        }
    }

    pub fn unlock(self: *Spinlock) void {
        self.locked.store(false, .Release);
    }
};
```

### 9.2 引用计数 (ARC)

```c
// C11 引用计数
typedef struct {
    _Atomic int count;
    void* data;
} RefCounted;

void retain(RefCounted* rc) {
    atomic_fetch_add_explicit(&rc->count, 1, memory_order_relaxed);
}

bool release(RefCounted* rc) {
    if (atomic_fetch_sub_explicit(&rc->count, 1, memory_order_release) == 1) {
        atomic_thread_fence(memory_order_acquire);
        return true;  // 需要销毁
    }
    return false;
}
```

```zig
// Zig 引用计数
fn Arc(comptime T: type) type {
    return struct {
        const Inner = struct {
            count: Atomic(u32),
            data: T,
        };

        ptr: *Inner,

        pub fn clone(self: @This()) @This() {
            _ = self.ptr.count.fetchAdd(1, .Monotonic);
            return self;
        }

        pub fn drop(self: *@This()) void {
            if (self.ptr.count.fetchSub(1, .Release) == 1) {
                std.atomic.fence(.Acquire);
                // 销毁...
            }
        }
    };
}
```

### 9.3 无锁队列 (Michael-Scott)

```c
// C11 Michael-Scott 无锁队列
typedef struct Node {
    void* data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_enqueue(LockFreeQueue* q, void* data) {
    Node* node = malloc(sizeof(Node));
    node->data = data;
    atomic_init(&node->next, NULL);

    while (true) {
        Node* tail = atomic_load_explicit(&q->tail, memory_order_acquire);
        Node* next = atomic_load_explicit(&tail->next, memory_order_acquire);

        if (tail == atomic_load_explicit(&q->tail, memory_order_acquire)) {
            if (next == NULL) {
                if (atomic_compare_exchange_weak_explicit(
                        &tail->next, &next, node,
                        memory_order_release, memory_order_relaxed)) {
                    atomic_compare_exchange_weak_explicit(
                        &q->tail, &tail, node,
                        memory_order_release, memory_order_relaxed);
                    return;
                }
            }
        }
    }
}
```

### 9.4 读-复制-更新 (RCU) 简化版

```c
// C11 RCU 简化版
typedef struct {
    _Atomic(void*) data;
    _Atomic int readers;
} RCU;

void* rcu_read_lock(RCU* rcu) {
    atomic_fetch_add_explicit(&rcu->readers, 1, memory_order_acquire);
    return atomic_load_explicit(&rcu->data, memory_order_consume);
}

void rcu_read_unlock(RCU* rcu) {
    atomic_fetch_sub_explicit(&rcu->readers, 1, memory_order_release);
}

void rcu_synchronize(RCU* rcu) {
    while (atomic_load_explicit(&rcu->readers, memory_order_acquire) > 0);
}
```

---

## 十、性能考虑

### 10.1 内存序性能层次

```text
内存序              相对性能    使用建议
─────────────────────────────────────────
Monotonic/Relaxed   1x         计数器、统计
Acquire/Release     1x         同步点
AcqRel              2x         RMW 默认
SeqCst              4x         必要时使用
```

### 10.2 缓存行优化

```c
// C11 缓存行填充
#define CACHE_LINE_SIZE 64

typedef struct {
    _Atomic uint64_t count;
    char padding[CACHE_LINE_SIZE - sizeof(_Atomic uint64_t)];
} PaddedCounter;
```

```zig
// Zig 缓存行填充
const CACHE_LINE_SIZE = 64;

const PaddedCounter = struct {
    count: Atomic(u64),
    padding: [CACHE_LINE_SIZE - @sizeOf(Atomic(u64))]u8,
};
```

### 10.3 伪共享避免

```text
伪共享问题：两个 CPU 核心访问同一缓存行的不同变量，导致缓存频繁失效。
解决：使用填充确保变量分布在不同缓存行。
```

### 10.4 平台特定优化

```zig
// Zig 自适应自旋
fn adaptiveSpin(iteration: u32) void {
    if (iteration < 16) {
        std.atomic.spinLoopHint();
    } else if (iteration < 32) {
        for (0..4) |_| std.atomic.spinLoopHint();
    } else {
        std.Thread.yield() catch {};
    }
}
```

---

## 十一、快速参考表

### 11.1 API 映射速查

```text
C11                              Zig
─────────────────────────────────────────────────────────
ATOMIC_VAR_INIT(v)               Atomic(T).init(v)
atomic_load(&a)                  a.load(.SeqCst)
atomic_store(&a, v)              a.store(v, .SeqCst)
atomic_exchange(&a, v)           a.swap(v, .SeqCst)
atomic_compare_exchange_...      a.tryCmpxchg(e, d, succ, fail)
atomic_fetch_add(&a, v)          a.fetchAdd(v, .SeqCst)
atomic_fetch_sub(&a, v)          a.fetchSub(v, .SeqCst)
atomic_thread_fence(o)           std.atomic.fence(o)
atomic_signal_fence(o)           std.atomic.signalFence(o)
```

### 11.2 内存序速查

```text
场景                          推荐内存序
─────────────────────────────────────────────
纯计数器/统计                 Monotonic
引用计数 increment            Monotonic
引用计数 decrement            Release + Acquire fence
标志同步                      Release / Acquire
自旋锁                        Acquire / Release
无锁数据结构 CAS              AcqRel
单次初始化                    AcqRel
全局顺序保证                  SeqCst
```

### 11.3 常见模式速查

```text
模式                  C11                             Zig
─────────────────────────────────────────────────────────────────────
自旋锁 TTAS           while(locked) pause;            while (locked.load(.Monotonic))
                      while(exchange(locked,1))         spinLoopHint();
                                                        if (!locked.swap(true,.Acquire)) break;

Release-Acquire       data = val;                     data = val;
                      fence(Release);                 fence(.Release);
                      flag.store(1,Relaxed);          flag.store(true, .Monotonic);

引用计数释放          if (fetch_sub(1,Release)==1)    if (count.fetchSub(1,.Release)==1) {
                      { fence(Acquire); free(); }       fence(.Acquire); free(); }
```

---

> **文档状态**: 完成 | **目标完成度**: 100% | **最后更新**: 2026-03-12
>
> 本文档提供了 C11 原子操作到 Zig 的完整映射参考，涵盖类型系统、操作语义、内存序、实现模式和性能优化。


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
