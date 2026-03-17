# Zig 语言完全指南：从入门到系统编程精通

> **定位**: 系统编程语言 | **难度**: ⭐⭐⭐ - ⭐⭐⭐⭐⭐ | **预估学时**: 60-80小时
> **版本**: Zig 0.13+ (当前master分支特性)
> **目标**: 替代 C 语言的现代系统级编程

---

## 📋 目录

- [Zig 语言完全指南：从入门到系统编程精通](#zig-语言完全指南从入门到系统编程精通)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、Zig 语言哲学与设计目标](#一zig-语言哲学与设计目标)
    - [1.1 为什么需要 Zig？](#11-为什么需要-zig)
    - [1.2 Zig 设计原则](#12-zig-设计原则)
  - [二、环境搭建与工具链](#二环境搭建与工具链)
    - [2.1 安装与版本管理](#21-安装与版本管理)
    - [2.2 第一个 Zig 程序](#22-第一个-zig-程序)
    - [2.3 项目结构](#23-项目结构)
  - [三、基础语法深度解析](#三基础语法深度解析)
    - [3.1 变量与常量](#31-变量与常量)
    - [3.2 控制流](#32-控制流)
    - [3.3 函数](#33-函数)
  - [四、类型系统全解](#四类型系统全解)
    - [4.1 基本类型](#41-基本类型)
    - [4.2 复合类型](#42-复合类型)
    - [4.3 类型转换](#43-类型转换)
  - [五、内存管理与分配器](#五内存管理与分配器)
    - [5.1 分配器模式](#51-分配器模式)
    - [5.2 内存泄漏检测](#52-内存泄漏检测)
  - [六、comptime：编译时编程](#六comptime编译时编程)
    - [6.1 comptime 基础](#61-comptime-基础)
    - [6.2 编译时代码生成](#62-编译时代码生成)
  - [七、错误处理机制](#七错误处理机制)
    - [7.1 错误联合类型](#71-错误联合类型)
  - [八、指针与内存安全](#八指针与内存安全)
    - [8.1 指针类型](#81-指针类型)
    - [8.2 内存安全特性](#82-内存安全特性)
  - [九、C 互操作详解](#九c-互操作详解)
    - [9.1 导入 C 头文件](#91-导入-c-头文件)
    - [9.2 构建 C 项目](#92-构建-c-项目)
  - [十、构建系统 (build.zig)](#十构建系统-buildzig)
    - [10.1 build.zig 完整示例](#101-buildzig-完整示例)
  - [十一、标准库深度探索](#十一标准库深度探索)
    - [11.1 常用数据结构](#111-常用数据结构)
    - [11.2 I/O 操作](#112-io-操作)
  - [十二、泛型与元编程](#十二泛型与元编程)
  - [十三、异步与并发](#十三异步与并发)
  - [十四、测试与调试](#十四测试与调试)
  - [十五、实战项目：操作系统内核](#十五实战项目操作系统内核)

---


---

## 📑 目录

- [Zig 语言完全指南：从入门到系统编程精通](#zig-语言完全指南从入门到系统编程精通)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、Zig 语言哲学与设计目标](#一zig-语言哲学与设计目标)
    - [1.1 为什么需要 Zig？](#11-为什么需要-zig)
    - [1.2 Zig 设计原则](#12-zig-设计原则)
  - [二、环境搭建与工具链](#二环境搭建与工具链)
    - [2.1 安装与版本管理](#21-安装与版本管理)
    - [2.2 第一个 Zig 程序](#22-第一个-zig-程序)
    - [2.3 项目结构](#23-项目结构)
  - [三、基础语法深度解析](#三基础语法深度解析)
    - [3.1 变量与常量](#31-变量与常量)
    - [3.2 控制流](#32-控制流)
    - [3.3 函数](#33-函数)
  - [四、类型系统全解](#四类型系统全解)
    - [4.1 基本类型](#41-基本类型)
    - [4.2 复合类型](#42-复合类型)
    - [4.3 类型转换](#43-类型转换)
  - [五、内存管理与分配器](#五内存管理与分配器)
    - [5.1 分配器模式](#51-分配器模式)
    - [5.2 内存泄漏检测](#52-内存泄漏检测)
  - [六、comptime：编译时编程](#六comptime编译时编程)
    - [6.1 comptime 基础](#61-comptime-基础)
    - [6.2 编译时代码生成](#62-编译时代码生成)
  - [七、错误处理机制](#七错误处理机制)
    - [7.1 错误联合类型](#71-错误联合类型)
  - [八、指针与内存安全](#八指针与内存安全)
    - [8.1 指针类型](#81-指针类型)
    - [8.2 内存安全特性](#82-内存安全特性)
  - [九、C 互操作详解](#九c-互操作详解)
    - [9.1 导入 C 头文件](#91-导入-c-头文件)
    - [9.2 构建 C 项目](#92-构建-c-项目)
  - [十、构建系统 (build.zig)](#十构建系统-buildzig)
    - [10.1 build.zig 完整示例](#101-buildzig-完整示例)
  - [十一、标准库深度探索](#十一标准库深度探索)
    - [11.1 常用数据结构](#111-常用数据结构)
    - [11.2 I/O 操作](#112-io-操作)
  - [十二、泛型与元编程](#十二泛型与元编程)
  - [十三、异步与并发](#十三异步与并发)
  - [十四、测试与调试](#十四测试与调试)
  - [十五、实战项目：操作系统内核](#十五实战项目操作系统内核)


---

## 一、Zig 语言哲学与设计目标

### 1.1 为什么需要 Zig？

```text
┌─────────────────────────────────────────────────────────────────────┐
│                      C 语言的问题 vs Zig 的解决方案                   │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  C 的问题                              Zig 的解决方案                 │
│  ─────────                             ───────────────               │
│                                                                      │
│  1. 宏定义不安全                        comptime：类型安全的编译时代码 │
│     #define MAX(a,b) ((a)>(b)?(a):(b))  fn max(comptime T: type,     │
│     无类型检查，副作用难控                a: T, b: T) T { ... }      │
│                                                                      │
│  2. 手动内存管理易出错                  分配器模式 + 泄漏检测          │
│     malloc/free 无统一策略              每个分配必须指定分配器         │
│     内存泄漏难以追踪                     defer/errdefer 确保释放      │
│                                                                      │
│  3. 构建系统复杂 (Makefile/CMake)       内置构建系统 build.zig       │
│     依赖外部工具链                       零依赖，交叉编译内置          │
│                                                                      │
│  4. 错误处理不一致                       错误联合类型 !              │
│     返回码/errno/异常混用                强制错误处理，无隐藏控制流   │
│                                                                      │
│  5. 无标准项目结构                       约定优于配置                 │
│     每个项目结构不同                     src/main.zig, build.zig     │
│                                                                      │
│  6. 交叉编译困难                         内置交叉编译支持            │
│     需要配置交叉工具链                   zig build -Dtarget=...      │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.2 Zig 设计原则

```zig
// 原则1: 无隐藏控制流
// 不像 C++ 的构造函数/析构函数，Zig 的一切都是显式的

// 原则2: 无隐藏内存分配
// 每个可能分配内存的操作都必须传递分配器
const std = @import("std");

fn good_example(allocator: std.mem.Allocator) !void {
    const buf = try allocator.alloc(u8, 100);  // 显式分配
    defer allocator.free(buf);                  // 显式释放
    // ...
}

// 原则3: 编译时代码执行
// 利用 comptime 在编译期完成计算
const array_len = blk: {
    var sum: usize = 0;
    for (0..10) |i| {
        sum += i;
    }
    break :blk sum;  // 编译时常量 45
};

// 原则4: 与 C 无缝互操作
const c = @cImport({
    @cInclude("stdio.h");
});

pub export fn zig_hello() void {
    c.printf("Hello from Zig!\n");
}
```

---

## 二、环境搭建与工具链

### 2.1 安装与版本管理

```bash
# 安装 Zig (macOS/Linux)
brew install zig                    # macOS
apt install zig                     # Ubuntu/Debian
pacman -S zig                       # Arch Linux

# 或者使用 zigup 管理多版本
curl -L https://github.com/marler8997/zigup/releases/download/v2023_07_27/zigup-x86_64-linux.tar.gz | tar xz
./zigup 0.13.0                      # 安装特定版本

# 验证安装
zig version                         # 0.13.0-dev.xxx
zig zen                             # Zig 哲学
```

### 2.2 第一个 Zig 程序

```zig
// hello.zig
const std = @import("std");

pub fn main() !void {
    // 获取标准输出流
    const stdout = std.io.getStdOut().writer();

    // 打印到 stdout
    try stdout.print("Hello, {s}!\n", .{"World"});

    // 或者使用控制台输出
    std.debug.print("Debug output: {}\n", .{42});
}

// 编译和运行
// zig run hello.zig          # 直接运行
// zig build-exe hello.zig    # 编译可执行文件
// ./hello                    # 运行
```

### 2.3 项目结构

```
my_project/
├── build.zig              # 构建脚本 (类似 Makefile/CMake)
├── build.zig.zon          # 包管理清单 (类似 package.json/Cargo.toml)
├── src/
│   ├── main.zig           # 程序入口
│   ├── lib.zig            # 库代码
│   └── utils/
│       └── helpers.zig    # 模块
└── tests/
    └── integration.zig    # 集成测试
```

---

## 三、基础语法深度解析

### 3.1 变量与常量

```zig
const std = @import("std");

pub fn main() void {
    // const: 编译时常量，不可修改
    const pi: f64 = 3.14159;
    const greeting = "Hello";  // 类型自动推断为 []const u8

    // var: 可变变量
    var count: u32 = 0;
    count += 1;

    // 块级作用域和标签
    const result = blk: {
        var sum: u32 = 0;
        var i: u32 = 0;
        while (i < 10) : (i += 1) {
            sum += i;
        }
        break :blk sum;  // 块的返回值
    };
    std.debug.print("Sum: {}\n", .{result});  // 45

    // 可选类型 (?T)
    var maybe_value: ?i32 = null;
    maybe_value = 42;

    // if 捕获可选值
    if (maybe_value) |value| {
        std.debug.print("Value: {}\n", .{value});
    } else {
        std.debug.print("No value\n", .{});
    }

    // 错误联合类型 (T!E)
    var maybe_error: !i32 = 42;  // 可以是 i32 或错误
    maybe_error = error.OutOfMemory;
}
```

### 3.2 控制流

```zig
const std = @import("std");

pub fn main() void {
    // if-else (表达式，不是语句！)
    const x: i32 = 10;
    const message = if (x > 0) "positive" else if (x < 0) "negative" else "zero";

    // while 循环
    var i: u32 = 0;
    while (i < 10) : (i += 1) {
        std.debug.print("{} ", .{i});
    }
    std.debug.print("\n", .{});

    // while 带条件
    var optional_num: ?u32 = 5;
    while (optional_num) |num| : (optional_num = if (num > 0) num - 1 else null) {
        std.debug.print("{} ", .{num});
    }
    std.debug.print("\n", .{});

    // for 循环 (用于数组/切片)
    const arr = [_]u32{ 1, 2, 3, 4, 5 };
    for (arr) |item, index| {
        std.debug.print("[{}] = {}\n", .{ index, item });
    }

    // for 带指针 (可修改)
    var mutable_arr = [_]u32{ 1, 2, 3, 4, 5 };
    for (&mutable_arr) |*item| {
        item.* *= 2;  // 解引用指针并修改
    }

    // switch (穷尽匹配)
    const n: u8 = 5;
    const description = switch (n) {
        0 => "zero",
        1...9 => "single digit",  // 范围
        10 => "ten",
        else => "other",
    };

    // switch 带枚举
    const Color = enum { red, green, blue };
    const color = Color.green;
    const color_name = switch (color) {
        .red => "Red",
        .green => "Green",
        .blue => "Blue",
    };

    // defer (函数退出时执行)
    defer std.debug.print("Function ending\n", .{});
    std.debug.print("Function running\n", .{});

    // errdefer (只有错误时才执行)
    may_fail() catch |err| {
        std.debug.print("Error: {}\n", .{err});
    };
}

fn may_fail() !void {
    errdefer std.debug.print("Cleaning up after error\n", .{});
    return error.SomeError;
}
```

### 3.3 函数

```zig
const std = @import("std");

// 基本函数
fn add(a: i32, b: i32) i32 {
    return a + b;
}

// 递归函数
fn factorial(n: u32) u32 {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 多返回值 (通过结构体或元组)
fn divmod(a: u32, b: u32) struct { quot: u32, rem: u32 } {
    return .{
        .quot = a / b,
        .rem = a % b,
    };
}

// 错误返回
const MyError = error{
    OutOfMemory,
    InvalidInput,
    DivisionByZero,
};

fn safe_divide(a: f64, b: f64) MyError!f64 {
    if (b == 0) return MyError.DivisionByZero;
    return a / b;
}

// 任意类型参数 (comptime)
fn print_anything(comptime T: type, value: T) void {
    std.debug.print("Value: {}\n", .{value});
}

// 可变参数 (comptime 切片)
fn sum_all(items: []const i32) i32 {
    var sum: i32 = 0;
    for (items) |item| {
        sum += item;
    }
    return sum;
}

// 内联汇编 (类似 C)
fn read_timestamp() u64 {
    var low: u32 = undefined;
    var high: u32 = undefined;
    asm volatile ("rdtsc"
        : [low] "=a" (low),
          [high] "=d" (high),
    );
    return (@as(u64, high) << 32) | low;
}

pub fn main() void {
    const result = add(5, 3);
    std.debug.print("5 + 3 = {}\n", .{result});

    const dm = divmod(17, 5);
    std.debug.print("17 / 5 = {} rem {}\n", .{ dm.quot, dm.rem });

    const div_result = safe_divide(10, 0) catch |err| {
        std.debug.print("Division failed: {}\n", .{err});
        return;
    };
    _ = div_result;
}
```

---

## 四、类型系统全解

### 4.1 基本类型

```zig
// 整数类型
const a: i8 = -128;           // 有符号 8 位
const b: u8 = 255;            // 无符号 8 位
const c: i32 = -1000000;      // 32 位
const d: u64 = 1 << 63;       // 64 位
const e: isize = -1;          // 目标指针大小的有符号整数
const f: usize = 100;         // 目标指针大小的无符号整数

// 浮点类型
const f32_val: f32 = 3.14;
const f64_val: f64 = 3.14159265358979;

// 布尔类型
const flag: bool = true;

// 字符类型
const letter: u8 = 'A';       // 实际上是 u8
const unicode: u21 = '🔥';    // Unicode 码点

// void 类型
fn do_nothing() void {
    return;
}

// noreturn 类型 (无限循环或崩溃)
fn infinite_loop() noreturn {
    while (true) {}
}
```

### 4.2 复合类型

```zig
const std = @import("std");

pub fn main() void {
    // 数组 (定长)
    const arr1 = [5]u32{ 1, 2, 3, 4, 5 };
    const arr2 = [_]u32{ 1, 2, 3, 4, 5 };  // 推断长度

    // 哨兵终止数组
    const sentinel_arr = [5:0]u8{ 'h', 'e', 'l', 'l', 'o' };  // 末尾隐含 0

    // 切片 (动态长度，指向数组)
    var buf: [100]u8 = undefined;
    const slice = buf[0..50];  // 长度为 50 的切片
    const len_slice = buf[10..20];  // buf[10] 到 buf[19]

    // 结构体
    const Point = struct {
        x: f32,
        y: f32,

        // 方法
        fn distance(self: Point, other: Point) f32 {
            const dx = self.x - other.x;
            const dy = self.y - other.y;
            return std.math.sqrt(dx * dx + dy * dy);
        }
    };

    const p1 = Point{ .x = 0, .y = 0 };
    const p2 = Point{ .x = 3, .y = 4 };
    std.debug.print("Distance: {}\n", .{p1.distance(p2)});  // 5

    // 元组 (匿名结构体)
    const tuple = .{ 1, "hello", true, 3.14 };
    std.debug.print("First: {}, Second: {s}\n", .{ tuple[0], tuple[1] });

    // 枚举
    const Status = enum {
        ok,
        error,
        warning,

        // 带方法的枚举
        fn is_error(self: Status) bool {
            return self == .error;
        }
    };

    const s = Status.ok;
    if (s == .ok) std.debug.print("All good\n", .{});

    // 联合 (Tagged Union)
    const Value = union(enum) {
        int: i64,
        float: f64,
        string: []const u8,

        fn print(self: Value) void {
            switch (self) {
                .int => |i| std.debug.print("int: {}\n", .{i}),
                .float => |f| std.debug.print("float: {}\n", .{f}),
                .string => |s| std.debug.print("string: {s}\n", .{s}),
            }
        }
    };

    var v = Value{ .int = 42 };
    v.print();
    v = .{ .string = "hello" };
    v.print();

    // 可选类型
    const maybe: ?i32 = 10;
    const definitely: i32 = maybe.?;  // 解包 (运行时检查)
    const safe: i32 = maybe orelse 0;  // 提供默认值

    // 错误联合类型
    const MyError = error{ NotFound, Invalid };
    const result: MyError!i32 = 42;
    const value = result catch 0;  // 捕获错误并提供默认值

    // 向量 (SIMD)
    const v4f = @Vector(4, f32);
    const a_vec: v4f = .{ 1, 2, 3, 4 };
    const b_vec: v4f = .{ 5, 6, 7, 8 };
    const sum_vec = a_vec + b_vec;  // 向量加法
    std.debug.print("Vector sum: {}\n", .{sum_vec});
}
```

### 4.3 类型转换

```zig
const std = @import("std");

pub fn main() void {
    // 显式转换 (@intCast, @floatCast, etc.)
    const a: i32 = 1000;
    const b: i64 = @intCast(a);  // i32 -> i64

    const f: f64 = 3.14;
    const i: i32 = @intFromFloat(f);  // f64 -> i32 (截断)

    // 溢出检查
    const small: u8 = 255;
    const overflow_add = @addWithOverflow(small, 1);  // 返回元组 {result, overflow_bit}
    std.debug.print("Result: {}, Overflow: {}\n", .{ overflow_add[0], overflow_add[1] });

    // 饱和运算
    const sat_add = @addWithSaturation(small, 10);  // 结果为 255

    // 指针转换
    var arr = [_]u8{ 1, 2, 3, 4 };
    const ptr: [*]u8 = &arr;  // 多指针
    const slice: []u8 = ptr[0..4];  // 转切片

    // 类型擦除 (@ptrCast)
    const any_ptr: *anyopaque = @ptrCast(&arr);
    const back_to_slice: *[]u8 = @ptrCast(@alignCast(any_ptr));
}
```

---

## 五、内存管理与分配器

### 5.1 分配器模式

```zig
const std = @import("std");

// Zig 的核心理念：显式分配器
// 不像 C 的 malloc/free，Zig 要求传递分配器

pub fn main() !void {
    // 获取通用分配器
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 动态分配内存
    const buf = try allocator.alloc(u8, 100);
    defer allocator.free(buf);  // 确保释放

    // 使用 buf
    @memset(buf, 0);

    // 重新分配
    const larger_buf = try allocator.realloc(buf, 200);
    // 注意：如果 realloc 失败，原来的 buf 仍然有效
    // 所以通常需要重新赋值

    // 分配单个对象
    const ptr = try allocator.create(MyStruct);
    defer allocator.destroy(ptr);
    ptr.* = MyStruct{ .x = 1, .y = 2 };

    // 使用 ArrayList (动态数组)
    var list = std.ArrayList(u32).init(allocator);
    defer list.deinit();

    try list.append(1);
    try list.append(2);
    try list.append(3);

    std.debug.print("ArrayList: ", .{});
    for (list.items) |item| {
        std.debug.print("{} ", .{item});
    }
    std.debug.print("\n", .{});

    // 使用 HashMap
    var map = std.StringHashMap(i32).init(allocator);
    defer map.deinit();

    try map.put("one", 1);
    try map.put("two", 2);

    if (map.get("one")) |value| {
        std.debug.print("one = {}\n", .{value});
    }
}

const MyStruct = struct {
    x: i32,
    y: i32,
};

// 自定义分配器： arena 分配器
fn use_arena_allocator() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();  // 一次性释放所有内存
    const allocator = arena.allocator();

    // 分配大量临时内存
    _ = try allocator.alloc(u8, 100);
    _ = try allocator.alloc(u8, 200);
    _ = try allocator.alloc(u8, 300);
    // 不需要单独 free，arena.deinit() 会释放所有
}

// 固定缓冲区分配器 (无堆分配)
fn use_fixed_buffer_allocator() void {
    var buffer: [1024]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const allocator = fba.allocator();

    const data = allocator.alloc(u8, 100) catch {
        std.debug.print("Out of memory!\n", .{});
        return;
    };
    _ = data;
}
```

### 5.2 内存泄漏检测

```zig
const std = @import("std");

pub fn main() !void {
    // GPA 自带泄漏检测
    var gpa = std.heap.GeneralPurposeAllocator(.{
        .safety = true,      // 安全检查
        .verbose_log = true, // 详细日志
    }){};
    defer {
        const leaked = gpa.deinit();
        if (leaked) {
            std.debug.print("Memory leak detected!\n", .{});
        }
    }
    const allocator = gpa.allocator();

    // 分配但不释放 (故意泄漏)
    const leaked_ptr = try allocator.alloc(u8, 100);
    _ = leaked_ptr;  // 忘记 free！
}
```

---

## 六、comptime：编译时编程

### 6.1 comptime 基础

```zig
const std = @import("std");

// comptime 是 Zig 最强大的特性
// 在编译期执行代码

// 编译时常量
const array_len = comptime blk: {
    var sum: usize = 0;
    for (0..100) |i| {
        sum += i;
    }
    break :blk sum;  // 4950
};

// 编译时函数
fn fibonacci(comptime n: u32) u32 {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

const fib_10 = comptime fibonacci(10);  // 55，编译期计算

// 类型作为参数
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

// 生成类型
fn Vector(comptime n: usize, comptime T: type) type {
    return struct {
        data: [n]T,

        fn add(self: @This(), other: @This()) @This() {
            var result: @This() = undefined;
            for (0..n) |i| {
                result.data[i] = self.data[i] + other.data[i];
            }
            return result;
        }

        fn dot(self: @This(), other: @This()) T {
            var sum: T = 0;
            for (0..n) |i| {
                sum += self.data[i] * other.data[i];
            }
            return sum;
        }
    };
}

pub fn main() void {
    // 使用 comptime 生成的类型
    const Vec3f = Vector(3, f32);

    const v1 = Vec3f{ .data = .{ 1, 2, 3 } };
    const v2 = Vec3f{ .data = .{ 4, 5, 6 } };

    const sum = v1.add(v2);
    std.debug.print("Sum: {}\n", .{sum.data});

    const dot_product = v1.dot(v2);  // 1*4 + 2*5 + 3*6 = 32
    std.debug.print("Dot: {}\n", .{dot_product});

    // 编译时类型检查
    const is_int = comptime std.meta.trait.isIntegral(i32);
    std.debug.print("i32 is integral: {}\n", .{is_int});
}
```

### 6.2 编译时代码生成

```zig
const std = @import("std");

// 生成查找表
const sin_table = comptime blk: {
    var table: [360]f32 = undefined;
    for (0..360) |i| {
        const rad = @as(f32, @floatFromInt(i)) * std.math.pi / 180.0;
        table[i] = @sin(rad);
    }
    break :blk table;
};

// 自动生成枚举和字符串映射
fn makeEnumAndStringMap(comptime names: []const []const u8) type {
    var enum_fields: [names.len]std.builtin.Type.EnumField = undefined;
    var map_entries: [names.len]struct { []const u8, usize } = undefined;

    for (names, 0..) |name, i| {
        enum_fields[i] = .{
            .name = name,
            .value = i,
        };
        map_entries[i] = .{ name, i };
    }

    return struct {
        pub const Enum = @Type(.{
            .Enum = .{
                .tag_type = u32,
                .fields = &enum_fields,
                .decls = &.{},
                .is_exhaustive = true,
            },
        });

        pub const map = std.StaticStringMap(usize).initComptime(&map_entries);
    };
}

const ColorSet = makeEnumAndStringMap(&.{ "red", "green", "blue", "yellow" });
const Color = ColorSet.Enum;

pub fn main() void {
    std.debug.print("Sin(30°) = {}\n", .{sin_table[30]});

    const c = Color.green;
    std.debug.print("Color: {}\n", .{@intFromEnum(c)});

    if (ColorSet.map.get("blue")) |idx| {
        std.debug.print("Blue index: {}\n", .{idx});
    }
}
```

---

## 七、错误处理机制

### 7.1 错误联合类型

```zig
const std = @import("std");

// 定义错误集合
const FileError = error{
    NotFound,
    NoAccess,
    OutOfMemory,
    SystemResources,
};

const ParseError = error{
    InvalidSyntax,
    UnexpectedEnd,
};

// 组合错误
const MyError = FileError || ParseError;

// 函数返回错误
fn read_file(path: []const u8) FileError![]u8 {
    if (path.len == 0) return FileError.InvalidArgument;

    // 模拟文件读取
    const allocator = std.heap.page_allocator;
    const data = allocator.alloc(u8, 100) catch return FileError.OutOfMemory;

    @memcpy(data, "Hello from file");
    return data;
}

// try: 传播错误
fn process_file(path: []const u8) !void {
    const data = try read_file(path);  // 如果出错，立即返回
    defer std.heap.page_allocator.free(data);

    std.debug.print("File content: {s}\n", .{data});
}

// catch: 处理错误
fn safe_process_file(path: []const u8) void {
    const data = read_file(path) catch |err| {
        std.debug.print("Error reading file: {}\n", .{err});
        return;
    };
    defer std.heap.page_allocator.free(data);

    std.debug.print("File content: {s}\n", .{data});
}

// errdefer: 仅在错误时执行
fn may_fail_with_cleanup() !void {
    const allocator = std.heap.page_allocator;

    const resource = try allocator.create(Resource);
    errdefer allocator.destroy(resource);  // 只有出错时才销毁

    const data = try allocator.alloc(u8, 100);
    errdefer allocator.free(data);  // 只有出错时才释放

    // 正常流程
    resource.init(data);

    // 成功时手动清理
    resource.deinit();
    allocator.destroy(resource);
    allocator.free(data);
}

const Resource = struct {
    data: []u8,

    fn init(self: *Resource, d: []u8) void {
        self.data = d;
    }

    fn deinit(self: *Resource) void {
        _ = self;
    }
};

// switch 捕获错误
fn handle_error_detailed() void {
    const result = risky_operation();

    switch (result) {
        error.NotFound => std.debug.print("File not found\n", .{}),
        error.NoAccess => std.debug.print("Permission denied\n", .{}),
        error.OutOfMemory => std.debug.print("Out of memory\n", .{}),
        else => |e| std.debug.print("Other error: {}\n", .{e}),
    }
}

fn risky_operation() !void {
    return error.NotFound;
}

// if 错误捕获
pub fn main() void {
    if (read_file("test.txt")) |data| {
        std.debug.print("Success: {s}\n", .{data});
        std.heap.page_allocator.free(data);
    } else |err| {
        std.debug.print("Failed: {}\n", .{err});
    }

    // while 错误捕获
    while (retryable_operation()) |result| : (std.time.sleep(1_000_000)) {
        std.debug.print("Result: {}\n", .{result});
        if (result == 42) break;
    } else |err| {
        std.debug.print("Final error: {}\n", .{err});
    }
}

fn retryable_operation() !i32 {
    return 42;
}
```

---

## 八、指针与内存安全

### 8.1 指针类型

```zig
const std = @import("std");

pub fn main() void {
    var x: i32 = 42;

    // 单指针 (*T)
    const ptr: *i32 = &x;
    ptr.* = 100;  // 解引用
    std.debug.print("x = {}\n", .{x});  // 100

    // 切片 ([]T) - 胖指针：指针+长度
    var arr = [_]u32{ 1, 2, 3, 4, 5 };
    const slice = arr[1..4];  // { 2, 3, 4 }
    std.debug.print("Slice len: {}, first: {}\n", .{ slice.len, slice[0] });

    // 多指针 ([*]T) - 类似 C 指针
    const many_ptr: [*]u8 = @ptrCast(&arr);
    std.debug.print("First via many ptr: {}\n", .{many_ptr[0]});

    // 哨兵指针 ([*:0]T) - 以哨兵值终止
    const c_string: [*:0]const u8 = "hello";
    _ = c_string;

    // 可选指针 (?*T)
    const maybe_ptr: ?*i32 = &x;
    if (maybe_ptr) |p| {
        std.debug.print("Pointer value: {}\n", .{p.*});
    }

    // 任意指针 (*anyopaque, ?*anyopaque)
    const any_ptr: *anyopaque = @ptrCast(&x);
    const back_to_i32: *i32 = @ptrCast(@alignCast(any_ptr));
    std.debug.print("Via opaque: {}\n", .{back_to_i32.*});
}

// 指针运算
fn pointer_arithmetic() void {
    var arr = [_]u32{ 10, 20, 30, 40, 50 };
    var ptr: [*]u32 = &arr;

    ptr += 2;  // 指向 arr[2]
    std.debug.print("Value at ptr: {}\n", .{ptr[0]});  // 30
}

// 对齐
fn alignment() void {
    // 检查对齐
    const ptr: *u32 = @ptrFromInt(0x1004);
    const is_aligned = @intFromPtr(ptr) % @alignOf(u32) == 0;
    std.debug.print("Is aligned: {}\n", .{is_aligned});

    // 对齐指针
    var unaligned_ptr: usize = 0x1001;
    const aligned = std.mem.alignForward(usize, unaligned_ptr, @alignOf(u32));
    std.debug.print("Aligned: 0x{x}\n", .{aligned});  // 0x1004
}
```

### 8.2 内存安全特性

```zig
// Zig 的安全模式 (Debug 和 ReleaseSafe 模式)
// 运行时边界检查

fn safety_checks() void {
    var arr = [_]u8{ 1, 2, 3 };

    // 这将触发运行时 panic (索引越界)
    // const x = arr[10];

    // 安全的访问方式
    if (arr.len > 10) {
        const x = arr[10];
        _ = x;
    }

    // 使用 @setRuntimeSafety 控制检查
    @setRuntimeSafety(false);  // 关闭安全检查 (危险！)
    // 现在 arr[10] 不会 panic，但可能导致未定义行为
    @setRuntimeSafety(true);
}

// 释放模式选择
// Debug: 完整安全检查 + 优化关闭
// ReleaseSafe: 完整安全检查 + 优化开启
// ReleaseFast: 最小检查 + 最大优化
// ReleaseSmall: 最小检查 + 最小代码体积
```

---

## 九、C 互操作详解

### 9.1 导入 C 头文件

```zig
const std = @import("std");

// 导入 C 标准库
const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

// 导入自定义 C 库
const mylib = @cImport({
    @cInclude("mylib.h");
});

pub fn main() void {
    // 使用 C 函数
    _ = c.printf("Hello from C printf!\n");

    // C 内存分配
    const ptr = c.malloc(100);
    defer c.free(ptr);

    _ = c.strcpy(@as([*c]u8, @ptrCast(ptr)), "Zig + C");
    _ = c.printf("String: %s\n", @as([*c]const u8, @ptrCast(ptr)));

    // 调用自定义 C 函数
    const result = mylib.add(5, 3);
    _ = c.printf("Result: %d\n", result);
}

// 导出 Zig 函数给 C 使用
pub export fn zig_add(a: i32, b: i32) i32 {
    return a + b;
}

pub export fn zig_process(data: [*c]u8, len: usize) void {
    const slice = data[0..len];
    for (slice) |*byte| {
        byte.* = std.ascii.toUpper(byte.*);
    }
}

// 处理 C 结构体
const CStruct = extern struct {
    x: i32,
    y: i32,
    name: [32]u8,
};

pub export fn zig_process_struct(s: *CStruct) void {
    s.x += 10;
    s.y += 20;
    _ = std.fmt.bufPrint(&s.name, "Zig_{s}", .{s.name}) catch {};
}
```

### 9.2 构建 C 项目

```zig
// build.zig - 编译 C 代码
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 编译 C 库
    const lib = b.addStaticLibrary(.{
        .name = "myclib",
        .target = target,
        .optimize = optimize,
    });

    lib.addCSourceFiles(.{
        .files = &.{
            "src/c_lib/file1.c",
            "src/c_lib/file2.c",
        },
        .flags = &.{
            "-Wall",
            "-Wextra",
            "-O2",
        },
    });

    lib.addIncludePath(.{ .path = "src/c_lib/include" });
    lib.linkLibC();

    // 主程序
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    exe.addIncludePath(.{ .path = "src/c_lib/include" });
    exe.linkLibrary(lib);
    exe.linkLibC();

    b.installArtifact(exe);
}
```

---

## 十、构建系统 (build.zig)

### 10.1 build.zig 完整示例

```zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    // 标准目标选项
    const target = b.standardTargetOptions(.{});

    // 标准优化选项
    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .ReleaseFast,
    });

    // 模块定义
    const mylib_module = b.addModule("mylib", .{
        .root_source_file = .{ .path = "src/mylib.zig" },
    });

    // 可执行文件
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 添加模块依赖
    exe.root_module.addImport("mylib", mylib_module);

    // 链接选项
    exe.linkLibC();
    exe.linkSystemLibrary("m");  // 数学库

    // 添加 C 源码
    exe.addCSourceFile(.{
        .file = .{ .path = "src/helper.c" },
        .flags = &.{"-Wall"},
    });

    // 包含路径
    exe.addIncludePath(.{ .path = "include" });

    // 链接库
    exe.addLibraryPath(.{ .path = "lib" });
    exe.linkSystemLibrary("mylib");

    // 定义宏
    exe.defineCMacro("DEBUG", "1");

    // 安装
    b.installArtifact(exe);

    // 运行命令
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // 单元测试
    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_unit_tests.step);

    // 集成测试
    const integration_tests = b.addTest(.{
        .root_source_file = .{ .path = "tests/integration.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_integration_tests = b.addRunArtifact(integration_tests);

    const integration_test_step = b.step("integration-test", "Run integration tests");
    integration_test_step.dependOn(&run_integration_tests.step);

    // 格式化检查
    const fmt_step = b.addFmt(.{
        .paths = &.{
            "src",
            "tests",
            "build.zig",
        },
        .check = true,
    });

    const check_step = b.step("check-fmt", "Check formatting");
    check_step.dependOn(&fmt_step.step);
}
```

---

## 十一、标准库深度探索

### 11.1 常用数据结构

```zig
const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // ArrayList - 动态数组
    var list = std.ArrayList(i32).init(allocator);
    defer list.deinit();

    try list.append(1);
    try list.append(2);
    try list.insert(1, 99);

    // 遍历
    for (list.items, 0..) |item, i| {
        std.debug.print("[{}] = {}\n", .{ i, item });
    }

    // HashMap
    var map = std.StringHashMap(f64).init(allocator);
    defer map.deinit();

    try map.put("pi", 3.14159);
    try map.put("e", 2.71828);

    var it = map.iterator();
    while (it.next()) |entry| {
        std.debug.print("{s} = {}\n", .{ entry.key_ptr.*, entry.value_ptr.* });
    }

    // 自动哈希 map (BufMap)
    var buf_map = std.BufMap.init(allocator);
    defer buf_map.deinit();

    try buf_map.put("key1", "value1");
    if (buf_map.get("key1")) |val| {
        std.debug.print("Found: {s}\n", .{val});
    }

    // 优先队列 (PriorityQueue)
    const PQ = std.PriorityQueue(i32, void, struct {
        fn lessThan(_: void, a: i32, b: i32) std.math.Order {
            return std.math.order(a, b);
        }
    }.lessThan);

    var pq = PQ.init(allocator, {});
    defer pq.deinit();

    try pq.add(5);
    try pq.add(1);
    try pq.add(3);

    while (pq.removeOrNull()) |min| {
        std.debug.print("Min: {}\n", .{min});
    }

    // 位集合
    var bit_set = std.DynamicBitSet.initEmpty(allocator, 100);
    defer bit_set.deinit();

    bit_set.set(10);
    bit_set.set(20);
    bit_set.toggle(10);
    std.debug.print("Bit 10: {}, Bit 20: {}\n", .{ bit_set.isSet(10), bit_set.isSet(20) });
}
```

### 11.2 I/O 操作

```zig
const std = @import("std");

pub fn main() !void {
    const allocator = std.heap.page_allocator;

    // 文件读取
    const cwd = std.fs.cwd();
    const data = try cwd.readFileAlloc(allocator, "input.txt", 1024 * 1024);
    defer allocator.free(data);
    std.debug.print("Read {} bytes\n", .{data.len});

    // 文件写入
    try cwd.writeFile(.{
        .sub_path = "output.txt",
        .data = "Hello, Zig!\n",
    });

    // 流式读取
    var file = try cwd.openFile("input.txt", .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var reader = buf_reader.reader();

    var line_buf: [1024]u8 = undefined;
    while (try reader.readUntilDelimiterOrEof(&line_buf, '\n')) |line| {
        std.debug.print("Line: {s}\n", .{line});
    }

    // 标准输入/输出
    const stdout = std.io.getStdOut().writer();
    try stdout.print("Enter name: ", .{});

    const stdin = std.io.getStdIn().reader();
    const name = try stdin.readUntilDelimiterAlloc(allocator, '\n', 100);
    defer allocator.free(name);

    try stdout.print("Hello, {s}!\n", .{name});

    // JSON 解析
    const json_str =
        \\{"name": "Zig", "version": "0.13"}
    ;

    const Package = struct {
        name: []const u8,
        version: []const u8,
    };

    const parsed = try std.json.parseFromSlice(Package, allocator, json_str, .{});
    defer parsed.deinit();

    std.debug.print("Package: {s} v{s}\n", .{ parsed.value.name, parsed.value.version });

    // 序列化
    var json_out = std.ArrayList(u8).init(allocator);
    defer json_out.deinit();

    try std.json.stringify(parsed.value, .{}, json_out.writer());
    std.debug.print("JSON: {s}\n", .{json_out.items});
}
```

---

## 十二、泛型与元编程

```zig
const std = @import("std");

// 泛型栈
fn Stack(comptime T: type) type {
    return struct {
        const Self = @This();

        items: []T,
        capacity: usize,
        len: usize,
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) !Self {
            const initial_capacity = 16;
            const items = try allocator.alloc(T, initial_capacity);
            return Self{
                .items = items,
                .capacity = initial_capacity,
                .len = 0,
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            self.allocator.free(self.items);
        }

        pub fn push(self: *Self, item: T) !void {
            if (self.len >= self.capacity) {
                const new_capacity = self.capacity * 2;
                self.items = try self.allocator.realloc(self.items, new_capacity);
                self.capacity = new_capacity;
            }
            self.items[self.len] = item;
            self.len += 1;
        }

        pub fn pop(self: *Self) ?T {
            if (self.len == 0) return null;
            self.len -= 1;
            return self.items[self.len];
        }

        pub fn peek(self: Self) ?T {
            if (self.len == 0) return null;
            return self.items[self.len - 1];
        }
    };
}

// 泛型排序
fn quickSort(comptime T: type, items: []T, comptime cmp: fn (T, T) bool) void {
    if (items.len <= 1) return;

    const pivot = items[items.len / 2];
    var i: usize = 0;
    var j: usize = items.len - 1;

    while (i <= j) {
        while (cmp(items[i], pivot)) i += 1;
        while (cmp(pivot, items[j])) j -= 1;
        if (i <= j) {
            const tmp = items[i];
            items[i] = items[j];
            items[j] = tmp;
            i += 1;
            if (j > 0) j -= 1;
        }
    }

    if (j > 0) quickSort(T, items[0..j + 1], cmp);
    quickSort(T, items[i..], cmp);
}

fn lessThan(a: i32, b: i32) bool {
    return a < b;
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 使用泛型栈
    var int_stack = try Stack(i32).init(allocator);
    defer int_stack.deinit();

    try int_stack.push(10);
    try int_stack.push(20);
    try int_stack.push(30);

    while (int_stack.pop()) |val| {
        std.debug.print("Popped: {}\n", .{val});
    }

    // 泛型排序
    var arr = [_]i32{ 5, 2, 8, 1, 9, 3 };
    quickSort(i32, &arr, lessThan);
    std.debug.print("Sorted: {any}\n", .{arr});

    // 使用标准库的 sort
    std.mem.sort(i32, &arr, {}, std.sort.asc(i32));
    std.debug.print("Sorted (std): {any}\n", .{arr});
}
```

---

## 十三、异步与并发

```zig
const std = @import("std");

// Zig 的异步 I/O (需要事件循环)
const EventLoop = struct {
    // 简化示例
};

// 基本线程
fn basic_threads() !void {
    const allocator = std.heap.page_allocator;

    var handles: [10]std.Thread = undefined;

    for (&handles, 0..) |*handle, i| {
        handle.* = try std.Thread.spawn(.{}, worker, .{i});
    }

    for (handles) |handle| {
        handle.join();
    }
}

fn worker(id: usize) void {
    std.time.sleep(100 * std.time.ns_per_ms);
    std.debug.print("Worker {} done\n", .{id});
}

// Mutex 和 Condition Variable
fn thread_synchronization() !void {
    const Context = struct {
        mutex: std.Thread.Mutex = .{},
        cond: std.Thread.Condition = .{},
        ready: bool = false,
        data: i32 = 0,
    };

    var ctx: Context = .{};

    const producer = struct {
        fn run(c: *Context) void {
            c.mutex.lock();
            defer c.mutex.unlock();

            c.data = 42;
            c.ready = true;
            c.cond.signal();
        }
    }.run;

    const consumer = struct {
        fn run(c: *Context) void {
            c.mutex.lock();
            defer c.mutex.unlock();

            while (!c.ready) {
                c.cond.wait(&c.mutex);
            }
            std.debug.print("Got data: {}\n", .{c.data});
        }
    }.run;

    const t1 = try std.Thread.spawn(.{}, producer, .{&ctx});
    const t2 = try std.Thread.spawn(.{}, consumer, .{&ctx});

    t1.join();
    t2.join();
}

// 原子操作
fn atomic_operations() void {
    var counter: std.atomic.Value(u32) = std.atomic.Value(u32).init(0);

    // fetch_add 返回旧值
    const old = counter.fetchAdd(1, .seq_cst);
    std.debug.print("Old: {}, New: {}\n", .{ old, counter.load(.seq_cst) });

    // CAS (Compare and Swap)
    while (true) {
        const current = counter.load(.seq_cst);
        if (counter.cmpxchgStrong(current, current + 1, .seq_cst, .seq_cst)) |_| {
            break;
        }
    }
}

pub fn main() !void {
    try basic_threads();
    try thread_synchronization();
    atomic_operations();
}
```

---

## 十四、测试与调试

```zig
const std = @import("std");

// 普通单元测试
test "basic arithmetic" {
    try std.testing.expectEqual(2 + 2, 4);
    try std.testing.expect(2 + 2 == 4);
}

test "array operations" {
    var arr = [_]i32{ 1, 2, 3 };
    try std.testing.expectEqual(arr.len, 3);
    try std.testing.expectEqual(arr[0], 1);
}

// 使用分配的测试
test "allocator test" {
    const allocator = std.testing.allocator;

    const buf = try allocator.alloc(u8, 100);
    defer allocator.free(buf);

    @memset(buf, 0xAA);
    try std.testing.expectEqual(buf[50], 0xAA);
}

// 失败测试
test "expected failure" {
    const result = failing_function();
    try std.testing.expectError(error.SomeError, result);
}

fn failing_function() !void {
    return error.SomeError;
}

// 模糊测试 (Zig 内置支持)
test "fuzz example" {
    const input_bytes = std.testing.fuzzInput(.{});

    // 使用输入进行模糊测试
    if (input_bytes.len > 10) return;

    // 测试解析器是否会崩溃
    _ = parseSomething(input_bytes) catch return;
}

fn parseSomething(data: []const u8) !i32 {
    if (data.len < 4) return error.TooShort;
    if (data[0] != 0xAA) return error.InvalidHeader;
    return 42;
}

// 基准测试
var bench_result: i64 = 0;

test "benchmark" {
    const iterations: usize = 1_000_000;

    var timer = try std.time.Timer.start();

    var sum: i64 = 0;
    for (0..iterations) |i| {
        sum += @intCast(i);
    }
    bench_result = sum;

    const elapsed_ns = timer.read();
    const elapsed_ms = elapsed_ns / std.time.ns_per_ms;

    std.debug.print("Benchmark: {} iterations in {} ms\n", .{ iterations, elapsed_ms });
}
```

---

## 十五、实战项目：操作系统内核

```zig
// kernel.zig - 简化版操作系统内核
// 目标: x86_64, bare metal

const std = @import("std");

// 内核入口点
export fn _start() callconv(.Naked) noreturn {
    asm volatile (
        \\mov $0x200000, %rsp  // 设置栈指针
        \\call kernel_main
        \\cli
        \\hlt
    );
    unreachable;
}

// VGA 文本模式输出
const VGA_BUFFER: [*]volatile u16 = @ptrFromInt(0xB8000);
const VGA_WIDTH = 80;
const VGA_HEIGHT = 25;

var cursor_x: usize = 0;
var cursor_y: usize = 0;

fn vga_putchar(c: u8) void {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 1;
    } else {
        const index = cursor_y * VGA_WIDTH + cursor_x;
        VGA_BUFFER[index] = vga_entry(c, 0x0F);  // 白字黑底
        cursor_x += 1;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y += 1;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll();
    }
}

fn vga_entry(c: u8, color: u8) u16 {
    return @as(u16, c) | (@as(u16, color) << 8);
}

fn scroll() void {
    // 简化实现
    cursor_y = VGA_HEIGHT - 1;
}

fn print(s: []const u8) void {
    for (s) |c| {
        vga_putchar(c);
    }
}

// 中断处理
const IDTEntry = packed struct {
    offset_low: u16,
    selector: u16,
    ist: u8,
    type_attr: u8,
    offset_mid: u16,
    offset_high: u32,
    reserved: u32,
};

var idt: [256]IDTEntry = undefined;

fn init_idt() void {
    const idt_descriptor = packed struct {
        size: u16,
        offset: u64,
    }{
        .size = @sizeOf(@TypeOf(idt)) - 1,
        .offset = @intFromPtr(&idt),
    };

    // 加载 IDT
    asm volatile ("lidt (%[desc])"
        :
        : [desc] "r" (&idt_descriptor),
    );
}

// 内存管理
const PAGE_SIZE = 4096;

fn init_memory() void {
    // 简单的页表设置
    // ...
}

// 调度器
const Task = struct {
    id: u32,
    stack: [1024]u8,
    context: Context,
    state: TaskState,
};

const TaskState = enum {
    ready,
    running,
    blocked,
};

const Context = struct {
    rsp: u64,
    rbp: u64,
    rip: u64,
};

var tasks: [10]Task = undefined;
var current_task: *Task = undefined;
var task_count: u32 = 0;

fn scheduler_init() void {
    // 初始化第一个任务 (当前)
    tasks[0] = Task{
        .id = 0,
        .stack = undefined,
        .context = undefined,
        .state = .running,
    };
    current_task = &tasks[0];
    task_count = 1;
}

fn schedule() void {
    // 轮询调度
    const next_id = (current_task.id + 1) % task_count;
    const next = &tasks[next_id];

    if (next.state == .ready) {
        const prev = current_task;
        current_task = next;
        switch_context(&prev.context, &next.context);
    }
}

fn switch_context(old: *Context, new: *const Context) void {
    // 汇编实现上下文切换
    asm volatile (
        \\push %rbp
        \\push %rbx
        \\push %r12
        \\push %r13
        \\push %r14
        \\push %r15
        \\mov %rsp, (%[old_rsp])
        \\mov (%[new_rsp]), %rsp
        \\pop %r15
        \\pop %r14
        \\pop %r13
        \\pop %r12
        \\pop %rbx
        \\pop %rbp
        \\ret
        :
        : [old_rsp] "r" (&old.rsp),
          [new_rsp] "r" (&new.rsp),
    );
}

// 系统调用
export fn syscall_handler() callconv(.Naked) void {
    asm volatile (
        \\push %rcx
        \\push %r11
        \\call handle_syscall
        \\pop %r11
        \\pop %rcx
        \\sysretq
    );
}

export fn handle_syscall(number: u64, arg1: u64, arg2: u64) u64 {
    return switch (number) {
        0 => 0,  // sys_exit
        1 => blk: {  // sys_write
            const buf: [*]const u8 = @ptrFromInt(arg1);
            const len: usize = @intCast(arg2);
            for (0..len) |i| {
                vga_putchar(buf[i]);
            }
            break :blk len;
        },
        else => 0,
    };
}

// 内核主函数
export fn kernel_main() void {
    print("Zig Kernel Booting...\n");

    init_idt();
    print("IDT initialized\n");

    init_memory();
    print("Memory initialized\n");

    scheduler_init();
    print("Scheduler initialized\n");

    print("Welcome to Zig OS!\n");

    // 进入空闲循环
    while (true) {
        asm volatile ("hlt");
    }
}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - Zig 版本: 0.13.0 (master)
> - 目标: 系统编程、嵌入式、游戏开发、WebAssembly
> - 特点: comptime、零成本抽象、C 互操作、内存安全
