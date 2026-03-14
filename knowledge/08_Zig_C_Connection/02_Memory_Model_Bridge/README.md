# Zig-C 内存模型桥接

本目录深入探讨 Zig 与 C 语言在内存管理方面的差异与桥接技术，涵盖从基础内存模型对比到高级互操作技术的完整知识体系。

## 目录结构

| 文件 | 主题 | 难度 |
|------|------|------|
| `01_C11_vs_Zig_Memory_Model.md` | C11 与 Zig 内存模型对比 | ⭐⭐⭐ |
| `02_Atomic_Operations_Mapping.md` | 原子操作映射 | ⭐⭐⭐⭐ |
| `03_Pointer_Provenance_Analysis.md` | 指针来源分析 | ⭐⭐⭐⭐⭐ |

## 核心概念

### C 内存模型回顾

C 语言的内存模型基于三个主要区域：

```c
// 全局/静态存储区 - 程序生命周期内持续存在
static int global_counter = 0;
const char* message = "Hello";

// 栈区 - 自动分配和释放
void stack_example(void) {
    int local_array[1024];  // 栈分配
    char buffer[256];       // 栈帧内分配
} // 函数返回时自动释放

// 堆区 - 手动管理
void heap_example(void) {
    int* dynamic = malloc(sizeof(int) * 100);
    if (dynamic) {
        // 使用内存...
        free(dynamic);  // 必须手动释放
    }
}
```

### Zig 显式分配器模型

Zig 采用完全不同的哲学——所有内存分配必须通过显式分配器：

```zig
const std = @import("std");

// 使用 GPA (General Purpose Allocator) 检测内存问题
test "zig allocator pattern" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer {
        const check = gpa.deinit();
        if (check == .leak) @panic("内存泄漏检测到！");
    }
    const allocator = gpa.allocator();

    // 动态分配数组
    const slice = try allocator.alloc(u32, 100);
    defer allocator.free(slice);  // 确保释放

    // 使用 Arena 分配器进行批量分配
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();
    const arena_alloc = arena.allocator();

    // 多次分配，只需一次 deinit
    _ = try arena_alloc.alloc(u8, 100);
    _ = try arena_alloc.alloc(u8, 200);
    _ = try arena_alloc.alloc(u8, 300);
}
```

## 桥接技术详解

### 在 Zig 中使用 C 内存函数

当与 C 库交互时，可能需要使用 C 的内存管理：

```zig
const c = @cImport({
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

/// 包装 C 的 malloc/free，确保与 Zig 错误处理兼容
pub const CAllocator = struct {
    pub fn alloc(len: usize) ?[*]u8 {
        return @ptrCast(c.malloc(len));
    }

    pub fn free(ptr: ?[*]u8) void {
        c.free(@ptrCast(ptr));
    }

    pub fn realloc(ptr: ?[*]u8, new_len: usize) ?[*]u8 {
        return @ptrCast(c.realloc(@ptrCast(ptr), new_len));
    }
};

// 使用示例：与返回 malloc 内存的 C 库交互
pub fn processCData() !void {
    // 假设 C 函数: char* get_data(size_t* len);
    var len: usize = 0;
    const c_data = c.get_data(&len);
    if (c_data == null) return error.CAllocationFailed;
    defer c.free(c_data);  // 确保释放 C 分配的内存

    // 安全地转换为 Zig slice
    const zig_slice: []const u8 = @as([*]const u8, @ptrCast(c_data))[0..len];

    // 处理数据...
    std.log.info("收到 {d} 字节数据", .{zig_slice.len});
}
```

### 在 C 中使用 Zig 分配器

反过来，将 Zig 的内存安全特性暴露给 C：

```zig
const ExportAlloc = struct {
    var internal_gpa: ?std.heap.GeneralPurposeAllocator(.{}) = null;

    pub export fn zig_alloc_init() c_int {
        internal_gpa = std.heap.GeneralPurposeAllocator(.{}){};
        return 0;
    }

    pub export fn zig_alloc_deinit() void {
        if (internal_gpa) |*gpa| {
            _ = gpa.deinit();
            internal_gpa = null;
        }
    }

    pub export fn zig_malloc(size: usize) ?*anyopaque {
        const gpa = &(internal_gpa orelse return null);
        const ptr = gpa.allocator().alloc(u8, size) catch return null;
        return ptr.ptr;
    }

    pub export fn zig_free(ptr: ?*anyopaque, size: usize) void {
        const gpa = &(internal_gpa orelse return);
        if (ptr) |p| {
            const slice: []u8 = @as([*]u8, @ptrCast(p))[0..size];
            gpa.allocator().free(slice);
        }
    }
};
```

对应的 C 头文件：

```c
#ifndef ZIG_ALLOC_H
#define ZIG_ALLOC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int zig_alloc_init(void);
void zig_alloc_deinit(void);
void* zig_malloc(size_t size);
void zig_free(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif // ZIG_ALLOC_H
```

## 内存对齐和布局兼容性

### 对齐要求对比

| 类型 | C (x86_64) | Zig | 说明 |
|------|-----------|-----|------|
| `char` | 1 | 1 | 字节对齐 |
| `short` | 2 | 2 | 2字节边界 |
| `int` | 4 | 4 | 4字节边界 |
| `long` | 8 | 8 | 64位系统 |
| `long long` | 8 | 8 | 8字节边界 |
| `float` | 4 | 4 | 4字节边界 |
| `double` | 8 | 8 | 8字节边界 |
| `void*` | 8 | 8 | 指针大小 |

### 结构体布局兼容性

```zig
const std = @import("std");

// 确保与 C 结构体完全兼容
const CCompatibleStruct = extern struct {
    x: i32,      // 4字节
    // 隐式填充 4字节
    y: i64,      // 8字节，需8字节对齐
    z: i16,      // 2字节
    // 隐式填充 6字节
}; // 总计 24字节

// 显式控制布局
const PackedStruct = packed struct {
    x: i32,  // offset 0
    y: i64,  // offset 4 (无填充！)
    z: i16,  // offset 12
}; // 总计 14字节

comptime {
    std.debug.assert(@sizeOf(CCompatibleStruct) == 24);
    std.debug.assert(@sizeOf(PackedStruct) == 14);
    std.debug.assert(@alignOf(CCompatibleStruct) == 8);
}
```

### 验证 C 和 Zig 结构体布局

```zig
/// 验证 extern struct 与 C 的兼容性
test "struct layout verification" {
    const c = @cImport({
        @cDefine("VERIFY_STRUCT", "");
        @cInclude("test_struct.h");
    });

    const ZigStruct = extern struct {
        a: u8,
        b: u16,
        c: u32,
    };

    // 验证大小和对齐
    try std.testing.expectEqual(@sizeOf(ZigStruct), @sizeOf(c.TestStruct));
    try std.testing.expectEqual(@alignOf(ZigStruct), @alignOf(c.TestStruct));

    // 验证字段偏移
    try std.testing.expectEqual(
        @offsetOf(ZigStruct, "a"),
        c.offsetof_a
    );
    try std.testing.expectEqual(
        @offsetOf(ZigStruct, "b"),
        c.offsetof_b
    );
    try std.testing.expectEqual(
        @offsetOf(ZigStruct, "c"),
        c.offsetof_c
    );
}
```

## 调试内存问题

### 使用 GPA 检测内存问题

```zig
test "GPA detects double free" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const ptr = try allocator.alloc(u8, 100);
    allocator.free(ptr);
    // allocator.free(ptr); // 取消注释将导致双重释放错误
}

test "GPA detects use after free" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const ptr = try allocator.alloc(u8, 100);
    allocator.free(ptr);
    // ptr[0] = 1; // 错误：使用已释放内存
}
```

### 与 Valgrind/ASan 集成

```zig
pub export fn run_with_valgrind() void {
    // 在调试模式下使用 page_allocator 使 Valgrind 更容易检测问题
    const allocator = if (@import("builtin").mode == .Debug)
        std.heap.page_allocator
    else
        std.heap.c_allocator;

    const data = allocator.alloc(u8, 1024) catch return;
    // 处理数据...
    allocator.free(data);
}
```

## 最佳实践清单

- [x] **始终使用 defer 释放资源** - Zig 的资源管理哲学
- [x] **优先使用 Arena 进行批量分配** - 减少分配开销
- [x] **验证 C 结构体布局** - 使用 comptime 断言
- [x] **使用 GPA 进行调试** - 检测内存错误
- [x] **明确文档化内存所有权** - 谁分配，谁释放
- [x] **测试边界条件** - 空指针、零长度分配

## 延伸阅读

1. [C11 Memory Model](./01_C11_vs_Zig_Memory_Model.md) - C11 内存序与 Zig 的对比
2. [Atomic Operations](./02_Atomic_Operations_Mapping.md) - 原子内存操作映射
3. [Pointer Provenance](./03_Pointer_Provenance_Analysis.md) - 指针来源分析

---

*本文档是 Zig-C 连接系列的一部分，专注于内存模型的桥接与兼容性。*
