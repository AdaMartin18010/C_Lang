---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# Zig 设计模式：惯用法的艺术

> **定位**: Zig 核心 / 设计模式 | **难度**: ⭐⭐⭐⭐ | **目标**: 掌握 Zig 惯用设计

---

## 📑 目录

- [📑 目录](#-目录)
- [一、面向数据设计](#一面向数据设计)
- [二、类型状态模式](#二类型状态模式)
- [三、编译时多态](#三编译时多态)
- [四、资源管理](#四资源管理)
- [五、错误处理模式](#五错误处理模式)
- [六、构建器模式](#六构建器模式)
- [七、策略模式](#七策略模式)
- [深入理解](#深入理解)
  - [核心原理](#核心原理)
  - [实践应用](#实践应用)
  - [最佳实践](#最佳实践)


---

## 一、面向数据设计

```zig
// 传统 OOP: 对象包含方法和数据
// DoD: 数据连续存储，逻辑分离

// 实体组件系统 (ECS)
const Entity = u32;

const Position = struct {
    x: f32,
    y: f32,
    z: f32,
};

const Velocity = struct {
    vx: f32,
    vy: f32,
    vz: f32,
};

const World = struct {
    positions: std.ArrayList(Position),
    velocities: std.ArrayList(Velocity),
    active: std.ArrayList(bool),

    pub fn init(allocator: std.mem.Allocator) World {
        return .{
            .positions = std.ArrayList(Position).init(allocator),
            .velocities = std.ArrayList(Velocity).init(allocator),
            .active = std.ArrayList(bool).init(allocator),
        };
    }

    pub fn deinit(self: *World) void {
        self.positions.deinit();
        self.velocities.deinit();
        self.active.deinit();
    }

    pub fn createEntity(self: *World, pos: Position, vel: Velocity) !Entity {
        const id = @as(Entity, @intCast(self.positions.items.len));
        try self.positions.append(pos);
        try self.velocities.append(vel);
        try self.active.append(true);
        return id;
    }

    pub fn update(self: *World, dt: f32) void {
        // 连续内存访问，缓存友好
        for (self.positions.items, self.velocities.items, self.active.items) |*pos, vel, active| {
            if (!active) continue;

            pos.x += vel.vx * dt;
            pos.y += vel.vy * dt;
            pos.z += vel.vz * dt;
        }
    }
};
```

---

## 二、类型状态模式

```zig
// 使用类型系统编码状态机

const std = @import("std");

// 未连接状态
const Disconnected = struct {};

// 已连接状态
const Connected = struct {
    socket: std.net.tcp.Connection,
};

// 状态转换的 Connection
fn Connection(comptime State: type) type {
    return struct {
        state: State,

        pub fn init() Connection(Disconnected) {
            return .{ .state = .{} };
        }

        // 只能在 Disconnected 状态下调用
        pub fn connect(self: Connection(Disconnected), address: std.net.Address) !Connection(Connected) {
            const socket = try std.net.tcp.Connection.connect(address);
            return .{ .state = .{ .socket = socket } };
        }

        // 只能在 Connected 状态下调用
        pub fn send(self: Connection(Connected), data: []const u8) !void {
            _ = try self.state.socket.write(data);
        }

        pub fn receive(self: Connection(Connected), buffer: []u8) !usize {
            return try self.state.socket.read(buffer);
        }

        pub fn disconnect(self: Connection(Connected)) Connection(Disconnected) {
            self.state.socket.close();
            return .{ .state = .{} };
        }
    };
}

// 使用
pub fn example() !void {
    var conn = Connection(Disconnected).init();

    // 类型错误: conn.send(...)  // 编译错误！

    conn = try conn.connect(try std.net.Address.parseIp("127.0.0.1", 8080));

    try conn.send("Hello");  // OK

    conn = conn.disconnect();
    // conn.send(...)  // 编译错误！
}
```

---

## 三、编译时多态

```zig
// 使用 anytype 实现静态多态

// 可排序接口
fn Comparable(comptime T: type) type {
    return struct {
        pub fn lessThan(a: T, b: T) bool {
            return a < b;
        }
    };
}

// 泛型排序 (编译时单态化)
fn bubbleSort(comptime T: type, items: []T) void {
    const N = items.len;
    var i: usize = 0;
    while (i < N) : (i += 1) {
        var j: usize = 0;
        while (j < N - i - 1) : (j += 1) {
            if (items[j] > items[j + 1]) {
                const temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }
}

// 鸭子类型接口
fn drawAll(items: anytype) void {
    const Item = @TypeOf(items);
    const info = @typeInfo(Item);

    if (info != .Pointer or info.Pointer.size != .Slice) {
        @compileError("Expected slice");
    }

    for (items) |item| {
        item.draw();
    }
}

// VTable 动态分发
const Drawable = struct {
    ptr: *anyopaque,
    vtable: *const VTable,

    const VTable = struct {
        draw: *const fn (*anyopaque) void,
        getArea: *const fn (*anyopaque) f32,
    };

    pub fn draw(self: Drawable) void {
        self.vtable.draw(self.ptr);
    }

    pub fn getArea(self: Drawable) f32 {
        return self.vtable.getArea(self.ptr);
    }
};

const Circle = struct {
    radius: f32,

    pub fn draw(self: *Circle) void {
        std.log.info("Drawing circle with radius {}", .{self.radius});
    }

    pub fn getArea(self: *Circle) f32 {
        return std.math.pi * self.radius * self.radius;
    }

    pub fn asDrawable(self: *Circle) Drawable {
        return .{
            .ptr = self,
            .vtable = &.{
                .draw = @ptrCast(&draw),
                .getArea = @ptrCast(&getArea),
            },
        };
    }
};
```

---

## 四、资源管理

```zig
// RAII 模式

fn Resource(comptime T: type, comptime deinit_fn: fn (*T) void) type {
    return struct {
        value: T,

        pub fn init(val: T) @This() {
            return .{ .value = val };
        }

        pub fn deinit(self: *@This()) void {
            deinit_fn(&self.value);
        }

        pub fn get(self: *@This()) *T {
            return &self.value;
        }
    };
}

// 使用
const FileResource = Resource(std.fs.File, std.fs.File.close);

fn readFile(path: []const u8) !void {
    var file = FileResource.init(try std.fs.cwd().openFile(path, .{}));
    defer file.deinit();

    // 使用 file.get() 访问
    var buffer: [1024]u8 = undefined;
    _ = try file.get().read(&buffer);
}

// 作用域守卫
fn ScopeGuard(comptime T: type) type {
    return struct {
        resource: *T,
        active: bool,

        pub fn init(r: *T) @This() {
            return .{ .resource = r, .active = true };
        }

        pub fn dismiss(self: *@This()) void {
            self.active = false;
        }

        pub fn deinit(self: *@This(), comptime cleanup: fn (*T) void) void {
            if (self.active) {
                cleanup(self.resource);
            }
        }
    };
}
```

---

## 五、错误处理模式

```zig
// 错误传播
fn mayFail() !void {
    try operation1();
    try operation2();
}

// 错误恢复
fn withRecovery() void {
    doPrimary() catch |err| switch (err) {
        error.NotFound => doFallback(),
        error.OutOfMemory => handleOOM(),
        else => |e| return e,
    };
}

// 错误累积
fn validate(input: []const u8) ValidationError!void {
    var errors = std.ArrayList([]const u8).init(allocator);
    defer errors.deinit();

    if (input.len < 8) try errors.append("Too short");
    if (!hasUpper(input)) try errors.append("No uppercase");
    if (!hasDigit(input)) try errors.append("No digit");

    if (errors.items.len > 0) {
        return ValidationError{ .messages = errors.toOwnedSlice() };
    }
}

// 可选值链
fn findUser(db: *Database, id: u64) ?User {
    return db.findById(id) orelse
           db.findByEmail(id) orelse
           db.findByPhone(id);
}

// 错误和可选值转换
fn getOrDefault(map: *Map, key: []const u8, default: Value) Value {
    return map.get(key) catch default;
}
```

---

## 六、构建器模式

```zig
const Request = struct {
    method: []const u8,
    url: []const u8,
    headers: std.StringHashMap([]const u8),
    body: ?[]const u8,

    pub const Builder = struct {
        method: []const u8 = "GET",
        url: []const u8 = "",
        headers: std.StringHashMap([]const u8),
        body: ?[]const u8 = null,
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) Builder {
            return .{
                .headers = std.StringHashMap([]const u8).init(allocator),
                .allocator = allocator,
            };
        }

        pub fn method(self: *Builder, m: []const u8) *Builder {
            self.method = m;
            return self;
        }

        pub fn url(self: *Builder, u: []const u8) *Builder {
            self.url = u;
            return self;
        }

        pub fn header(self: *Builder, key: []const u8, value: []const u8) !*Builder {
            try self.headers.put(key, value);
            return self;
        }

        pub fn body(self: *Builder, b: []const u8) *Builder {
            self.body = b;
            return self;
        }

        pub fn build(self: *Builder) Request {
            return .{
                .method = self.method,
                .url = self.url,
                .headers = self.headers,
                .body = self.body,
            };
        }
    };
};

// 使用
fn builderExample(allocator: std.mem.Allocator) !void {
    var builder = Request.Builder.init(allocator);

    const request = builder
        .method("POST")
        .url("/api/users")
        .header("Content-Type", "application/json") catch unreachable
        .header("Authorization", "Bearer token") catch unreachable
        .body("{\"name\":\"John\"}")
        .build();

    _ = request;
}
```

---

## 七、策略模式

```zig
// 编译时策略
fn Sorter(comptime compare: fn (i32, i32) bool) type {
    return struct {
        pub fn sort(items: []i32) void {
            // 使用 compare 进行排序
            std.sort.sort(i32, items, {}, struct {
                pub fn lessThan(_: void, a: i32, b: i32) bool {
                    return compare(a, b);
                }
            }.lessThan);
        }
    };
}

fn ascending(a: i32, b: i32) bool { return a < b; }
fn descending(a: i32, b: i32) bool { return a > b; }

const AscendingSort = Sorter(ascending);
const DescendingSort = Sorter(descending);

// 运行时策略
const CompressionStrategy = union(enum) {
    none,
    gzip: GzipCompressor,
    zlib: ZlibCompressor,

    pub fn compress(self: CompressionStrategy, data: []const u8, out: []u8) !usize {
        switch (self) {
            .none => {
                @memcpy(out[0..data.len], data);
                return data.len;
            },
            .gzip => |c| return c.compress(data, out),
            .zlib => |c| return c.compress(data, out),
        }
    }
};
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10


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
