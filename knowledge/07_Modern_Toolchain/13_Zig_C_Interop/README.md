# Zig C 互操作

> **定位**: 现代工具链 / C语言生态系统扩展 | **难度**: ⭐⭐⭐⭐

---

## 📚 目录

| 文档 | 说明 | 难度 |
|:-----|:-----|:----:|
| [01_Zig_C_Interoperability.md](01_Zig_C_Interoperability.md) | Zig与C互操作完全指南 | ⭐⭐⭐⭐ |

---

## 🎯 概述

Zig是一门现代系统编程语言，与C语言具有卓越的互操作性。本模块介绍如何在Zig中使用C库，以及如何将Zig代码导出给C使用。

### 为什么学习Zig C互操作？

| 优势 | 说明 |
|:-----|:-----|
| **无缝集成** | Zig可以直导入C头文件，无需手动绑定 |
| **性能** | 零成本抽象，与C同等性能 |
| **安全性** | 在保持C库性能的同时获得内存安全 |
| **交叉编译** | Zig内置交叉编译支持，简化多平台开发 |
| **构建系统** | Zig可以作为C/C++项目的构建系统替代 |

---

## 📖 核心内容

### 导入C头文件

Zig最强大的特性之一是能够通过`@cImport`直接导入C头文件：

```zig
const c = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

pub fn main() !void {
    _ = c.printf("Hello from Zig!\n");

    const ptr = c.malloc(1024);
    defer c.free(ptr);

    _ = c.memset(ptr, 0, 1024);
}
```

### 导出给C使用

Zig函数可以通过`export`关键字导出，供C代码调用：

```zig
// Zig代码
export fn add(a: i32, b: i32) i32 {
    return a + b;
}

export fn fibonacci(n: u32) u32 {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}
```

```c
// C代码使用Zig编译的库
#include <stdio.h>

extern int add(int a, int b);
extern unsigned int fibonacci(unsigned int n);

int main() {
    printf("2 + 3 = %d\n", add(2, 3));
    printf("fib(10) = %u\n", fibonacci(10));
    return 0;
}
```

### 类型映射

| C类型 | Zig类型 | 说明 |
|:------|:--------|:-----|
| `int` | `c_int` | 平台相关整数 |
| `unsigned long` | `c_ulong` | 平台相关无符号整数 |
| `size_t` | `usize` | 无符号指针大小整数 |
| `void*` | `*anyopaque` | 不透明指针 |
| `char*` | `[*:0]u8` 或 `[]u8` | C字符串 |
| `struct Foo` | `extern struct` | C兼容结构体 |
| `union` | `extern union` | C兼容联合体 |

### 内存管理桥接

```zig
const std = @import("std");

// 使用Zig分配器分配C内存
fn zig_malloc(size: usize) callconv(.C) ?*anyopaque {
    const ptr = std.heap.c_allocator.alloc(u8, size) catch return null;
    return ptr.ptr;
}

// 释放C内存
fn zig_free(ptr: ?*anyopaque) callconv(.C) void {
    if (ptr) |p| {
        std.heap.c_allocator.free(@as([*]u8, @ptrCast(p))[0..1]);
    }
}
```

---

## 🛠️ 实际应用场景

### 场景1：逐步迁移C项目到Zig

```text
阶段1: 使用Zig编译现有C代码
阶段2: 将独立模块重写为Zig
阶段3: 逐步替换核心组件
阶段4: 完全迁移或保持混合架构
```

### 场景2：为C库创建安全绑定

```zig
// 为不安全的C库创建类型安全包装
const raw = @cImport(@cInclude("legacy_lib.h"));

pub const SafeHandle = struct {
    raw_handle: *raw.LegacyHandle,

    pub fn open(path: []const u8) !SafeHandle {
        const c_path = try std.heap.c_allocator.dupeZ(u8, path);
        defer std.heap.c_allocator.free(c_path);

        const h = raw.legacy_open(c_path.ptr) orelse return error.OpenFailed;
        return SafeHandle{ .raw_handle = h };
    }

    pub fn close(self: *SafeHandle) void {
        raw.legacy_close(self.raw_handle);
        self.raw_handle = undefined;
    }
};
```

### 场景3：跨平台构建C项目

```zig
// build.zig - 替代CMake/Makefile
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 编译C库
    const lib = b.addStaticLibrary(.{
        .name = "mylib",
        .target = target,
        .optimize = optimize,
    });

    lib.addCSourceFiles(&.{
        "src/core.c",
        "src/utils.c",
        "src/api.c",
    }, &.{
        "-Wall",
        "-Wextra",
        "-O3",
    });

    lib.linkLibC();
    b.installArtifact(lib);
}
```

---

## 🔍 最佳实践

1. **始终检查空指针**: C代码可能返回null，Zig的`?*T`可选类型可帮助处理
2. **使用defer释放资源**: 利用Zig的defer确保C资源被正确释放
3. **包装不安全操作**: 将C库的不安全操作封装在安全的Zig API中
4. **测试边界情况**: C代码的内存安全问题应在Zig层被捕获

---

## 🔗 相关资源

- [Zig 官网](https://ziglang.org/)
- [Zig 文档](https://ziglang.org/documentation/master/)
- [Zig 标准库](https://ziglang.org/documentation/master/std/)
- [Ziglearn](https://ziglearn.org/)

---

## 📝 总结

Zig与C的互操作性为C开发者提供了一条渐进式现代化的路径：

- ✅ 无需重写即可使用Zig的构建系统
- ✅ 逐步将C模块替换为Zig实现
- ✅ 为遗留C代码创建内存安全包装
- ✅ 利用Zig的交叉编译简化部署

---

> **说明**: 本文档作为C语言知识库的扩展参考，展示现代语言如何与C生态系统集成。
>
> **最后更新**: 2026-03-12


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
