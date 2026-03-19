# Zig C 互操作：无缝集成 C 生态系统

> **定位**: Zig 核心 / C 互操作 | **难度**: ⭐⭐⭐⭐ | **目标**: 调用 C 库、导出给 C、头文件翻译

---

## 📋 目录

- [Zig C 互操作：无缝集成 C 生态系统](#zig-c-互操作无缝集成-c-生态系统)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、互操作概述](#一互操作概述)
  - [二、导入 C 头文件](#二导入-c-头文件)
    - [2.1 基本用法](#21-基本用法)
    - [2.2 导入第三方库](#22-导入第三方库)
    - [2.3 处理复杂的头文件](#23-处理复杂的头文件)
  - [三、调用 C 库](#三调用-c-库)
    - [3.1 链接配置](#31-链接配置)
    - [3.2 类型转换](#32-类型转换)
    - [3.3 回调函数](#33-回调函数)
  - [四、导出 Zig 给 C](#四导出-zig-给-c)
    - [4.1 基本导出](#41-基本导出)
    - [4.2 生成 C 头文件](#42-生成-c-头文件)
    - [4.3 C 兼容 API 设计](#43-c-兼容-api-设计)
  - [五、头文件翻译](#五头文件翻译)
    - [5.1 使用 translate-c](#51-使用-translate-c)
    - [5.2 在构建中使用](#52-在构建中使用)
    - [5.3 手动优化翻译结果](#53-手动优化翻译结果)
  - [六、内存兼容性](#六内存兼容性)
    - [6.1 内存分配器桥接](#61-内存分配器桥接)
    - [6.2 生命周期管理](#62-生命周期管理)
  - [七、实战：SQLite 绑定](#七实战sqlite-绑定)
  - [八、实战：OpenGL 绑定](#八实战opengl-绑定)

---


---

## 📑 目录

- [Zig C 互操作：无缝集成 C 生态系统](#zig-c-互操作无缝集成-c-生态系统)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、互操作概述](#一互操作概述)
  - [二、导入 C 头文件](#二导入-c-头文件)
    - [2.1 基本用法](#21-基本用法)
    - [2.2 导入第三方库](#22-导入第三方库)
    - [2.3 处理复杂的头文件](#23-处理复杂的头文件)
  - [三、调用 C 库](#三调用-c-库)
    - [3.1 链接配置](#31-链接配置)
    - [3.2 类型转换](#32-类型转换)
    - [3.3 回调函数](#33-回调函数)
  - [四、导出 Zig 给 C](#四导出-zig-给-c)
    - [4.1 基本导出](#41-基本导出)
    - [4.2 生成 C 头文件](#42-生成-c-头文件)
    - [4.3 C 兼容 API 设计](#43-c-兼容-api-设计)
  - [五、头文件翻译](#五头文件翻译)
    - [5.1 使用 translate-c](#51-使用-translate-c)
    - [5.2 在构建中使用](#52-在构建中使用)
    - [5.3 手动优化翻译结果](#53-手动优化翻译结果)
  - [六、内存兼容性](#六内存兼容性)
    - [6.1 内存分配器桥接](#61-内存分配器桥接)
    - [6.2 生命周期管理](#62-生命周期管理)
  - [七、实战：SQLite 绑定](#七实战sqlite-绑定)
  - [八、实战：OpenGL 绑定](#八实战opengl-绑定)


---

## 一、互操作概述

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Zig-C 互操作全景图                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Zig 代码                                                           │
│      │                                                               │
│      ▼                                                               │
│   ┌─────────────────┐     ┌─────────────────┐     ┌─────────────┐   │
│   │ @cImport        │────▶│ C 头文件        │────▶│ C 库        │   │
│   │ @cInclude       │     │ .h              │     │ .a/.so/.dll │   │
│   └─────────────────┘     └─────────────────┘     └─────────────┘   │
│          │                                                           │
│          ▼                                                           │
│   ┌─────────────────┐     ┌─────────────────┐                        │
│   │ zig translate-c │────▶│ .zig 绑定文件   │                        │
│   │ (头文件翻译)     │     │ (类型安全的API)  │                        │
│   └─────────────────┘     └─────────────────┘                        │
│                                                                      │
│   C 代码                                                             │
│      │                                                               │
│      ▼                                                               │
│   ┌─────────────────┐     ┌─────────────────┐                        │
│   │ export          │────▶│ .h 头文件       │                        │
│   │ extern "C"      │     │ (C 兼容的ABI)    │                        │
│   └─────────────────┘     └─────────────────┘                        │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、导入 C 头文件

### 2.1 基本用法

```zig
// 导入标准 C 库
const c = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdlib.h");
    @cInclude("stdio.h");
    @cInclude("string.h");
});

pub fn main() !void {
    // 使用 C 标准库函数
    const ptr = c.malloc(1024);
    defer c.free(ptr);

    _ = c.printf("Hello from Zig!\n");

    var buf: [256]u8 = undefined;
    _ = c.snprintf(&buf, buf.len, "Number: %d", 42);
}
```

### 2.2 导入第三方库

```zig
// sqlite.zig
const c = @cImport({
    @cInclude("sqlite3.h");
});

const std = @import("std");

pub const Database = struct {
    db: ?*c.sqlite3,

    pub fn open(path: []const u8) !Database {
        var db: ?*c.sqlite3 = null;
        const result = c.sqlite3_open(path.ptr, &db);

        if (result != c.SQLITE_OK) {
            return error.OpenFailed;
        }

        return .{ .db = db };
    }

    pub fn close(self: *Database) void {
        if (self.db) |db| {
            _ = c.sqlite3_close(db);
            self.db = null;
        }
    }

    pub fn execute(self: Database, sql: []const u8) !void {
        var err_msg: [*c]u8 = null;
        const result = c.sqlite3_exec(
            self.db,
            sql.ptr,
            null,
            null,
            &err_msg
        );

        if (result != c.SQLITE_OK) {
            if (err_msg) |msg| {
                std.log.err("SQL error: {s}", .{msg});
                c.sqlite3_free(msg);
            }
            return error.ExecutionFailed;
        }
    }
};
```

### 2.3 处理复杂的头文件

```zig
// 处理条件编译和宏
const c = @cImport({
    // 定义宏
    @cDefine("SDL_MAIN_HANDLED", "1");
    @cDefine("GL_GLEXT_PROTOTYPES", "1");

    // 包含路径
    @cInclude("GL/gl.h");
    @cInclude("GL/glext.h");
    @cInclude("SDL2/SDL.h");
});

// 处理系统特定头文件
const system_c = if (@import("builtin").os.tag == .windows)
    @cImport({
        @cInclude("windows.h");
        @cInclude("winsock2.h");
    })
else
    @cImport({
        @cInclude("unistd.h");
        @cInclude("sys/socket.h");
        @cInclude("netinet/in.h");
    });
```

---

## 三、调用 C 库

### 3.1 链接配置

```zig
// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 链接 C 标准库
    exe.linkLibC();

    // 链接系统库
    exe.linkSystemLibrary("sqlite3");
    exe.linkSystemLibrary("ssl");
    exe.linkSystemLibrary("crypto");

    // 链接静态库
    exe.addObjectFile(.{ .path = "libs/libcustom.a" });

    // 链接动态库
    exe.addObjectFile(.{ .path = "libs/libcustom.so" });

    // 库搜索路径
    exe.addLibraryPath(.{ .path = "/usr/local/lib" });
    exe.addLibraryPath(.{ .path = "libs" });

    // 包含路径
    exe.addIncludePath(.{ .path = "/usr/local/include" });
    exe.addIncludePath(.{ .path = "include" });

    // 链接选项
    exe.linkSystemLibrary("pthread");
    exe.linkSystemLibrary("m");  // 数学库

    b.installArtifact(exe);
}
```

### 3.2 类型转换

```zig
// C 和 Zig 类型转换

const c = @cImport({
    @cInclude("example.h");
});

// 指针转换
fn zig_to_c_ptr() void {
    var zig_array = [_]u8{ 1, 2, 3, 4 };

    // Zig 指针转 C 指针
    const c_ptr: [*c]u8 = &zig_array;

    // C 指针转 Zig 指针
    const zig_ptr: [*]u8 = c_ptr;

    // 可选指针
    const maybe_c_ptr: ?[*c]u8 = if (zig_array.len > 0) &zig_array else null;
}

// 字符串转换
fn string_conversions() void {
    // Zig 字符串转 C 字符串
    const zig_str: []const u8 = "hello";
    const c_str: [*c]const u8 = zig_str.ptr;

    // C 字符串转 Zig 字符串
    const c_string: [*c]const u8 = c.get_string();
    const zig_string = std.mem.span(c_string);  // 以 null 结尾的 C 字符串

    // 创建 C 兼容字符串
    var buffer: [256:0]u8 = undefined;
    const c_compatible = std.fmt.bufPrintZ(&buffer, "Hello {}", .{42}) catch unreachable;
}

// 数组和切片
fn array_conversions() void {
    // C 数组
    var c_array: [10]c_int = undefined;

    // 转 Zig 切片
    const zig_slice: []c_int = &c_array;

    // 从不定长 C 数组创建切片
    const c_ptr: [*c]c_int = c.get_array(&c_array.len);
    const zig_slice2: []c_int = c_ptr[0..c_array.len];
}

// 结构体转换
const CPoint = extern struct {
    x: c_int,
    y: c_int,
};

fn struct_conversions() void {
    // 创建 C 兼容结构体
    const point = CPoint{ .x = 10, .y = 20 };

    // 传递给 C 函数
    c.process_point(&point);

    // 从 C 获取结构体
    var received: CPoint = undefined;
    c.get_point(&received);
}
```

### 3.3 回调函数

```zig
// C 回调与 Zig

const c = @cImport({
    @cInclude("callback_api.h");
});

// C 回调类型: typedef void (*callback_t)(int data, void* user_data);

// Zig 回调函数
fn my_callback(data: c_int, user_data: ?*anyopaque) callconv(.C) void {
    const context = @as(*Context, @alignCast(@ptrCast(user_data.?)));
    context.handle(@intCast(data));
}

const Context = struct {
    count: u32,

    fn handle(self: *Context, data: u32) void {
        self.count += data;
    }
};

pub fn register_callback() void {
    var context = Context{ .count = 0 };

    // 注册回调
    c.register_callback(my_callback, &context);
}

// 使用闭包模式
fn CallbackClosure(comptime T: type) type {
    return struct {
        context: *T,
        callback: *const fn (*T, c_int) void,

        pub fn invoke(ctx: ?*anyopaque, data: c_int) callconv(.C) void {
            const self = @as(*@This(), @alignCast(@ptrCast(ctx.?)));
            self.callback(self.context, data);
        }
    };
}
```

---

## 四、导出 Zig 给 C

### 4.1 基本导出

```zig
// zig_lib.zig - 导出给 C 使用的库

const std = @import("std");

// 导出函数
export fn zig_add(a: c_int, b: c_int) c_int {
    return a + b;
}

export fn zig_process_data(data: [*c]const u8, len: usize) c_int {
    const slice = data[0..len];
    // 处理数据...
    _ = slice;
    return 0;
}

// 导出全局变量
export var zig_global_counter: c_int = 0;

// 导出类型
export const ZigPoint = extern struct {
    x: f64,
    y: f64,
};

export fn zig_point_distance(a: *const ZigPoint, b: *const ZigPoint) f64 {
    const dx = a.x - b.x;
    const dy = a.y - b.y;
    return std.math.sqrt(dx * dx + dy * dy);
}
```

### 4.2 生成 C 头文件

```zig
// tools/generate_header.zig

const std = @import("std");

const function_decls = [_][]const u8{
    "int zig_add(int a, int b);",
    "int zig_process_data(const char* data, size_t len);",
    "double zig_point_distance(const ZigPoint* a, const ZigPoint* b);",
};

const type_decls = [_][]const u8{
    "typedef struct { double x; double y; } ZigPoint;",
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var header = std.ArrayList(u8).init(allocator);
    defer header.deinit();

    const w = header.writer();

    try w.writeAll("#ifndef ZIG_LIB_H\n");
    try w.writeAll("#define ZIG_LIB_H\n\n");
    try w.writeAll("#include <stddef.h>\n\n");
    try w.writeAll("#ifdef __cplusplus\n");
    try w.writeAll('extern "C" {\n');
    try w.writeAll("#endif\n\n");

    try w.writeAll("/* Types */\n");
    for (type_decls) |decl| {
        try w.print("{s}\n", .{decl});
    }

    try w.writeAll("\n/* Functions */\n");
    for (function_decls) |decl| {
        try w.print("{s}\n", .{decl});
    }

    try w.writeAll("\n/* Global Variables */\n");
    try w.writeAll("extern int zig_global_counter;\n");

    try w.writeAll("\n#ifdef __cplusplus\n");
    try w.writeAll("}\n");
    try w.writeAll("#endif\n\n");
    try w.writeAll("#endif /* ZIG_LIB_H */\n");

    const stdout = std.io.getStdOut();
    try stdout.writeAll(header.items);
}
```

### 4.3 C 兼容 API 设计

```zig
// safe_api.zig - 安全的 C API

const std = @import("std");

// 不透明句柄
pub const DatabaseHandle = *opaque {};

// 错误码
pub const ErrorCode = enum(c_int) {
    success = 0,
    null_pointer = -1,
    invalid_argument = -2,
    out_of_memory = -3,
    io_error = -4,
};

// 创建数据库
export fn database_create(out_handle: ?*?*DatabaseHandle) callconv(.C) ErrorCode {
    const handle = out_handle orelse return .null_pointer;

    const db = std.heap.c_allocator.create(Database) catch return .out_of_memory;
    db.* = Database.init();

    handle.* = @ptrCast(db);
    return .success;
}

// 销毁数据库
export fn database_destroy(handle: ?*DatabaseHandle) callconv(.C) ErrorCode {
    const db = @as(?*Database, @ptrCast(@alignCast(handle))) orelse return .null_pointer;

    db.deinit();
    std.heap.c_allocator.destroy(db);

    return .success;
}

// 执行查询
export fn database_query(
    handle: ?*DatabaseHandle,
    query: ?[*:0]const u8,
    out_result: ?*?[*:0]u8,
) callconv(.C) ErrorCode {
    const db = @as(?*Database, @ptrCast(@alignCast(handle))) orelse return .null_pointer;
    const q = query orelse return .null_pointer;
    const result = out_result orelse return .null_pointer;

    const result_str = db.query(std.mem.span(q)) catch return .io_error;

    const c_str = std.heap.c_allocator.dupeZ(u8, result_str) catch return .out_of_memory;
    result.* = c_str.ptr;

    return .success;
}

// 释放查询结果
export fn database_free_result(result: ?[*:0]u8) callconv(.C) void {
    if (result) |r| {
        std.heap.c_allocator.free(std.mem.span(r));
    }
}

// 内部实现
const Database = struct {
    data: std.StringHashMap([]const u8),

    fn init() Database {
        return .{
            .data = std.StringHashMap([]const u8).init(std.heap.c_allocator),
        };
    }

    fn deinit(self: *Database) void {
        self.data.deinit();
    }

    fn query(self: *Database, q: []const u8) ![]const u8 {
        return self.data.get(q) orelse "NOT_FOUND";
    }
};
```

---

## 五、头文件翻译

### 5.1 使用 translate-c

```bash
# 命令行翻译
zig translate-c /usr/include/sqlite3.h > sqlite3.zig

# 翻译 Windows 头文件
zig translate-c -target x86_64-windows-gnu windows.h > windows.zig

# 带宏定义的翻译
zig translate-c -DFOO=1 -DBAR=2 header.h > output.zig
```

### 5.2 在构建中使用

```zig
// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 翻译 C 头文件
    const translate_c = b.addTranslateC(.{
        .root_source_file = .{ .path = "include/mylib.h" },
        .target = target,
        .optimize = optimize,
    });

    // 添加宏定义
    translate_c.defineCMacro("ENABLE_FEATURE_X", "1");
    translate_c.addIncludePath(.{ .path = "include" });

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 使用翻译后的模块
    exe.addModule("mylib", translate_c.createModule());

    exe.linkLibC();
    exe.linkSystemLibrary("mylib");

    b.installArtifact(exe);
}
```

### 5.3 手动优化翻译结果

```zig
// 自动翻译的原始代码 (translate-c 生成)
// pub const my_status_t = c_uint;
// pub const MY_STATUS_OK = @as(c_int, 0);
// pub const MY_STATUS_ERROR = @as(c_int, 1);

// 手动优化的版本
pub const Status = enum(c_uint) {
    ok = 0,
    error = 1,
    invalid_param = 2,
    out_of_memory = 3,

    pub fn toError(self: Status) !void {
        return switch (self) {
            .ok => {},
            .error => error.Generic,
            .invalid_param => error.InvalidParam,
            .out_of_memory => error.OutOfMemory,
            _ => error.Unknown,
        };
    }
};

// 原始结构体
// pub const my_buffer_t = extern struct {
//     data: [*c]u8,
//     len: usize,
//     capacity: usize,
// };

// 优化后的 Zig 风格 API
pub const Buffer = struct {
    inner: c.my_buffer_t,

    pub fn init(allocator: std.mem.Allocator, capacity: usize) !Buffer {
        const data = try allocator.alloc(u8, capacity);
        return .{
            .inner = .{
                .data = data.ptr,
                .len = 0,
                .capacity = capacity,
            },
        };
    }

    pub fn asSlice(self: Buffer) []u8 {
        return self.inner.data[0..self.inner.len];
    }

    pub fn append(self: *Buffer, data: []const u8) !void {
        if (self.inner.len + data.len > self.inner.capacity) {
            return error.NoSpace;
        }
        @memcpy(self.inner.data[self.inner.len..][0..data.len], data);
        self.inner.len += data.len;
    }
};
```

---

## 六、内存兼容性

### 6.1 内存分配器桥接

```zig
// C 分配器包装
pub const CAllocator = struct {
    pub fn allocator() std.mem.Allocator {
        return .{
            .ptr = undefined,
            .vtable = &vtable,
        };
    }

    const vtable = std.mem.Allocator.VTable{
        .alloc = alloc,
        .resize = resize,
        .free = free,
    };

    fn alloc(_: *anyopaque, len: usize, ptr_align: u8, _: usize) ?[*]u8 {
        const ptr = c.malloc(len);
        if (ptr_align > 1) {
            // 确保对齐
            const alignment = @as(usize, 1) << @intCast(ptr_align);
            _ = alignment;
        }
        return @ptrCast(ptr);
    }

    fn resize(_: *anyopaque, buf: []u8, _: u8, new_len: usize, _: usize) bool {
        if (new_len <= buf.len) return true;
        return false;
    }

    fn free(_: *anyopaque, buf: []u8, _: u8, _: usize) void {
        c.free(buf.ptr);
    }
};

// 使用示例
pub fn use_c_allocator() void {
    var list = std.ArrayList(u8).init(CAllocator.allocator());
    defer list.deinit();

    list.appendSlice("hello") catch return;
}
```

### 6.2 生命周期管理

```zig
// C 资源管理器
pub fn CResource(comptime T: type, comptime deinit_fn: fn (*T) callconv(.C) void) type {
    return struct {
        ptr: *T,

        pub fn init(ptr: *T) @This() {
            return .{ .ptr = ptr };
        }

        pub fn deinit(self: @This()) void {
            deinit_fn(self.ptr);
        }
    };
}

// 使用
const FileHandle = CResource(c.FILE, c.fclose);

pub fn read_file(path: []const u8) ![]u8 {
    const file = c.fopen(path.ptr, "r") orelse return error.FileNotFound;
    var handle = FileHandle.init(file);
    defer handle.deinit();

    // 读取文件...
    return &[_]u8{};
}
```

---

## 七、实战：SQLite 绑定

```zig
// sqlite_wrapper.zig - 完整的 SQLite Zig 绑定

const std = @import("std");
const c = @cImport({
    @cInclude("sqlite3.h");
});

pub const Error = error{
    Error,
    Internal,
    Perm,
    Abort,
    Busy,
    Locked,
    NoMem,
    ReadOnly,
    Interrupt,
    IOError,
    Corrupt,
    NotFound,
    Full,
    CantOpen,
    Protocol,
    Empty,
    Schema,
    TooBig,
    Constraint,
    Mismatch,
    Misuse,
    NoLFS,
    Auth,
    Format,
    Range,
    NotADB,
};

fn check(rc: c_int) !void {
    return switch (rc) {
        c.SQLITE_OK => {},
        c.SQLITE_ERROR => error.Error,
        c.SQLITE_INTERNAL => error.Internal,
        c.SQLITE_PERM => error.Perm,
        c.SQLITE_ABORT => error.Abort,
        c.SQLITE_BUSY => error.Busy,
        c.SQLITE_LOCKED => error.Locked,
        c.SQLITE_NOMEM => error.NoMem,
        c.SQLITE_READONLY => error.ReadOnly,
        c.SQLITE_INTERRUPT => error.Interrupt,
        c.SQLITE_IOERR => error.IOError,
        c.SQLITE_CORRUPT => error.Corrupt,
        c.SQLITE_NOTFOUND => error.NotFound,
        c.SQLITE_FULL => error.Full,
        c.SQLITE_CANTOPEN => error.CantOpen,
        c.SQLITE_PROTOCOL => error.Protocol,
        c.SQLITE_EMPTY => error.Empty,
        c.SQLITE_SCHEMA => error.Schema,
        c.SQLITE_TOOBIG => error.TooBig,
        c.SQLITE_CONSTRAINT => error.Constraint,
        c.SQLITE_MISMATCH => error.Mismatch,
        c.SQLITE_MISUSE => error.Misuse,
        c.SQLITE_NOLFS => error.NoLFS,
        c.SQLITE_AUTH => error.Auth,
        c.SQLITE_FORMAT => error.Format,
        c.SQLITE_RANGE => error.Range,
        c.SQLITE_NOTADB => error.NotADB,
        else => error.Error,
    };
}

pub const Database = struct {
    db: *c.sqlite3,

    pub fn open(path: []const u8) !Database {
        var db: ?*c.sqlite3 = null;
        try check(c.sqlite3_open(path.ptr, &db));
        return .{ .db = db.? };
    }

    pub fn close(self: Database) void {
        _ = c.sqlite3_close(self.db);
    }

    pub fn execute(self: Database, sql: []const u8) !void {
        try check(c.sqlite3_exec(self.db, sql.ptr, null, null, null));
    }

    pub fn prepare(self: Database, sql: []const u8) !Statement {
        var stmt: ?*c.sqlite3_stmt = null;
        try check(c.sqlite3_prepare_v2(self.db, sql.ptr, @intCast(sql.len + 1), &stmt, null));
        return .{ .stmt = stmt.? };
    }

    pub fn lastInsertRowId(self: Database) i64 {
        return c.sqlite3_last_insert_rowid(self.db);
    }

    pub fn changes(self: Database) usize {
        return @intCast(c.sqlite3_changes(self.db));
    }
};

pub const Statement = struct {
    stmt: *c.sqlite3_stmt,

    pub fn finalize(self: Statement) void {
        _ = c.sqlite3_finalize(self.stmt);
    }

    pub fn step(self: Statement) !bool {
        const rc = c.sqlite3_step(self.stmt);
        if (rc == c.SQLITE_ROW) return true;
        if (rc == c.SQLITE_DONE) return false;
        try check(rc);
        unreachable;
    }

    pub fn reset(self: Statement) !void {
        try check(c.sqlite3_reset(self.stmt));
    }

    // 参数绑定
    pub fn bindInt(self: Statement, idx: usize, value: i32) !void {
        try check(c.sqlite3_bind_int(self.stmt, @intCast(idx), value));
    }

    pub fn bindInt64(self: Statement, idx: usize, value: i64) !void {
        try check(c.sqlite3_bind_int64(self.stmt, @intCast(idx), value));
    }

    pub fn bindDouble(self: Statement, idx: usize, value: f64) !void {
        try check(c.sqlite3_bind_double(self.stmt, @intCast(idx), value));
    }

    pub fn bindText(self: Statement, idx: usize, value: []const u8) !void {
        try check(c.sqlite3_bind_text(self.stmt, @intCast(idx), value.ptr, @intCast(value.len), c.SQLITE_STATIC));
    }

    pub fn bindNull(self: Statement, idx: usize) !void {
        try check(c.sqlite3_bind_null(self.stmt, @intCast(idx)));
    }

    // 结果获取
    pub fn columnInt(self: Statement, idx: usize) i32 {
        return c.sqlite3_column_int(self.stmt, @intCast(idx));
    }

    pub fn columnInt64(self: Statement, idx: usize) i64 {
        return c.sqlite3_column_int64(self.stmt, @intCast(idx));
    }

    pub fn columnDouble(self: Statement, idx: usize) f64 {
        return c.sqlite3_column_double(self.stmt, @intCast(idx));
    }

    pub fn columnText(self: Statement, idx: usize) []const u8 {
        const ptr = c.sqlite3_column_text(self.stmt, @intCast(idx));
        const len = c.sqlite3_column_bytes(self.stmt, @intCast(idx));
        return ptr[0..len];
    }

    pub fn columnType(self: Statement, idx: usize) ColumnType {
        return @enumFromInt(c.sqlite3_column_type(self.stmt, @intCast(idx)));
    }
};

pub const ColumnType = enum(c_int) {
    integer = c.SQLITE_INTEGER,
    float = c.SQLITE_FLOAT,
    blob = c.SQLITE_BLOB,
    null = c.SQLITE_NULL,
    text = c.SQLITE3_TEXT,
};

// 使用示例
pub fn example() !void {
    const db = try Database.open("test.db");
    defer db.close();

    try db.execute(
        \\CREATE TABLE IF NOT EXISTS users (
        \\    id INTEGER PRIMARY KEY,
        \\    name TEXT,
        \\    age INTEGER
        \\)
    );

    const stmt = try db.prepare("INSERT INTO users (name, age) VALUES (?, ?)");
    defer stmt.finalize();

    try stmt.bindText(1, "Alice");
    try stmt.bindInt(2, 30);
    _ = try stmt.step();
    try stmt.reset();

    try stmt.bindText(1, "Bob");
    try stmt.bindInt(2, 25);
    _ = try stmt.step();
}
```

---

## 八、实战：OpenGL 绑定

```zig
// gl.zig - OpenGL Zig 绑定

const std = @import("std");
const c = @cImport({
    @cInclude("GL/glew.h");
    @cInclude("GLFW/glfw3.h");
});

// 类型定义
pub const GLenum = c_uint;
pub const GLuint = c_uint;
pub const GLint = c_int;
pub const GLfloat = f32;
pub const GLbitfield = c_uint;
pub const GLsizei = c_int;

// 常量
pub const GL_TRIANGLES = 0x0004;
pub const GL_VERTEX_SHADER = 0x8B31;
pub const GL_FRAGMENT_SHADER = 0x8B30;
pub const GL_COMPILE_STATUS = 0x8B81;
pub const GL_LINK_STATUS = 0x8B82;
pub const GL_COLOR_BUFFER_BIT = 0x00004000;

// 窗口管理
pub const Window = struct {
    handle: *c.GLFWwindow,

    pub fn create(width: i32, height: i32, title: [*:0]const u8) !Window {
        _ = c.glfwInit();

        c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 3);
        c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 3);
        c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);

        const handle = c.glfwCreateWindow(width, height, title, null, null) orelse {
            return error.WindowCreationFailed;
        };

        c.glfwMakeContextCurrent(handle);
        _ = c.glewInit();

        return .{ .handle = handle };
    }

    pub fn destroy(self: Window) void {
        c.glfwDestroyWindow(self.handle);
        c.glfwTerminate();
    }

    pub fn shouldClose(self: Window) bool {
        return c.glfwWindowShouldClose(self.handle) != 0;
    }

    pub fn swapBuffers(self: Window) void {
        c.glfwSwapBuffers(self.handle);
    }

    pub fn pollEvents() void {
        c.glfwPollEvents();
    }

    pub fn setKeyCallback(self: Window, callback: fn (*c.GLFWwindow, c_int, c_int, c_int, c_int) callconv(.C) void) void {
        c.glfwSetKeyCallback(self.handle, callback);
    }
};

// 着色器程序
pub const Shader = struct {
    id: GLuint,

    pub fn createVertex(source: []const u8) !Shader {
        return create(GL_VERTEX_SHADER, source);
    }

    pub fn createFragment(source: []const u8) !Shader {
        return create(GL_FRAGMENT_SHADER, source);
    }

    fn create(shader_type: GLenum, source: []const u8) !Shader {
        const id = c.glCreateShader(shader_type);

        const source_ptr: [*c]const u8 = source.ptr;
        const len: c_int = @intCast(source.len);
        c.glShaderSource(id, 1, &source_ptr, &len);
        c.glCompileShader(id);

        var success: c_int = 0;
        c.glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (success == 0) {
            var log: [512]u8 = undefined;
            c.glGetShaderInfoLog(id, 512, null, &log);
            std.log.err("Shader compilation failed: {s}", .{std.mem.sliceTo(&log, 0)});
            return error.ShaderCompilationFailed;
        }

        return .{ .id = id };
    }

    pub fn destroy(self: Shader) void {
        c.glDeleteShader(self.id);
    }
};

pub const ShaderProgram = struct {
    id: GLuint,

    pub fn create(vertex: Shader, fragment: Shader) !ShaderProgram {
        const id = c.glCreateProgram();
        c.glAttachShader(id, vertex.id);
        c.glAttachShader(id, fragment.id);
        c.glLinkProgram(id);

        var success: c_int = 0;
        c.glGetProgramiv(id, GL_LINK_STATUS, &success);

        if (success == 0) {
            var log: [512]u8 = undefined;
            c.glGetProgramInfoLog(id, 512, null, &log);
            std.log.err("Program linking failed: {s}", .{std.mem.sliceTo(&log, 0)});
            return error.ProgramLinkingFailed;
        }

        return .{ .id = id };
    }

    pub fn use(self: ShaderProgram) void {
        c.glUseProgram(self.id);
    }

    pub fn destroy(self: ShaderProgram) void {
        c.glDeleteProgram(self.id);
    }

    pub fn setFloat(self: ShaderProgram, name: [*:0]const u8, value: f32) void {
        const location = c.glGetUniformLocation(self.id, name);
        c.glUniform1f(location, value);
    }
};

// 缓冲区
pub const VBO = struct {
    id: GLuint,

    pub fn create() VBO {
        var id: GLuint = 0;
        c.glGenBuffers(1, &id);
        return .{ .id = id };
    }

    pub fn bind(self: VBO) void {
        c.glBindBuffer(c.GL_ARRAY_BUFFER, self.id);
    }

    pub fn unbind() void {
        c.glBindBuffer(c.GL_ARRAY_BUFFER, 0);
    }

    pub fn bufferData(self: VBO, data: []const f32, usage: GLenum) void {
        self.bind();
        c.glBufferData(c.GL_ARRAY_BUFFER, @intCast(data.len * @sizeOf(f32)), data.ptr, usage);
    }

    pub fn destroy(self: VBO) void {
        c.glDeleteBuffers(1, &self.id);
    }
};

// 渲染函数
pub fn clearColor(r: f32, g: f32, b: f32, a: f32) void {
    c.glClearColor(r, g, b, a);
}

pub fn clear(mask: GLbitfield) void {
    c.glClear(mask);
}

pub fn drawArrays(mode: GLenum, first: GLint, count: GLsizei) void {
    c.glDrawArrays(mode, first, count);
}

// 渲染循环示例
pub fn render_loop() !void {
    const window = try Window.create(800, 600, "Zig OpenGL");
    defer window.destroy();

    const vertex_shader = try Shader.createVertex(
        \\#version 330 core
        \\layout (location = 0) in vec3 aPos;
        \\void main() {
        \\    gl_Position = vec4(aPos, 1.0);
        \\}
    );
    defer vertex_shader.destroy();

    const fragment_shader = try Shader.createFragment(
        \\#version 330 core
        \\out vec4 FragColor;
        \\void main() {
        \\    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        \\}
    );
    defer fragment_shader.destroy();

    const program = try ShaderProgram.create(vertex_shader, fragment_shader);
    defer program.destroy();

    // 顶点数据
    const vertices = [_]f32{
        -0.5, -0.5, 0.0,
         0.5, -0.5, 0.0,
         0.0,  0.5, 0.0,
    };

    const vbo = VBO.create();
    defer vbo.destroy();
    vbo.bufferData(&vertices, c.GL_STATIC_DRAW);

    // 渲染循环
    while (!window.shouldClose()) {
        clearColor(0.2, 0.3, 0.3, 1.0);
        clear(GL_COLOR_BUFFER_BIT);

        program.use();
        c.glBindVertexArray(vbo.id);
        drawArrays(GL_TRIANGLES, 0, 3);

        window.swapBuffers();
        Window.pollEvents();
    }
}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 适用范围: Zig 0.11+


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
