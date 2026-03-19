//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 1131
// Language: zig
// Block ID: 71d7322c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 使用 @TypeOf 和 comptime 实现泛型

const std = @import("std");

// ========== 基础泛型函数 ==========

fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

fn min(comptime T: type, a: T, b: T) T {
    return if (a < b) a else b;
}

// 使用 @TypeOf 的自动类型推断版本
fn maxInferred(a: anytype, b: @TypeOf(a)) @TypeOf(a) {
    return if (a > b) a else b;
}

// ========== 类型安全交换 ==========

fn swap(comptime T: type, a: *T, b: *T) void {
    const tmp = a.*;
    a.* = b.*;
    b.* = tmp;
}

// 使用 @TypeOf 的版本
fn swapInferred(a: anytype, b: anytype) void {
    // 编译时验证类型相同
    comptime {
        if (@TypeOf(a) != @TypeOf(b)) {
            @compileError("swapInferred requires same types");
        }
        // 验证是指针
        const info = @typeInfo(@TypeOf(a));
        if (info != .Pointer) {
            @compileError("swapInferred requires pointers");
        }
    }

    const T = @typeInfo(@TypeOf(a)).Pointer.child;
    const tmp: T = a.*;
    a.* = b.*;
    b.* = tmp;
}

// ========== 容器类型 ==========

fn ArrayList(comptime T: type) type {
    return struct {
        items: []T,
        capacity: usize,
        allocator: std.mem.Allocator,

        const Self = @This();

        fn init(allocator: std.mem.Allocator) Self {
            return .{
                .items = &[_]T{},
                .capacity = 0,
                .allocator = allocator,
            };
        }

        fn append(self: *Self, item: T) !void {
            if (self.items.len >= self.capacity) {
                try self.grow();
            }
            self.items[self.items.len] = item;
            self.items.len += 1;
        }

        fn grow(self: *Self) !void {
            const new_cap = if (self.capacity == 0) 8 else self.capacity * 2;
            self.items = try self.allocator.realloc(self.items, new_cap);
            self.capacity = new_cap;
        }
    };
}

// ========== 类型特征萃取 ==========

fn ElementType(comptime Container: type) type {
    const info = @typeInfo(Container);
    return switch (info) {
        .Array => |arr| arr.child,
        .Pointer => |ptr| ptr.child,
        .Struct => @compileError("Cannot extract element type from arbitrary struct"),
        else => @compileError("Not a container type"),
    };
}

fn hasMethod(comptime T: type, name: []const u8) bool {
    const info = @typeInfo(T);
    if (info != .Struct) return false;

    inline for (info.Struct.decls) |decl| {
        if (std.mem.eql(u8, decl.name, name)) return true;
    }
    return false;
}

// ========== 使用示例 ==========

fn demoComptimeGenerics() void {
    // max/min 使用
    const m1 = max(i32, 10, 20);
    const m2 = maxInferred(3.14, 2.71);
    std.debug.print("max: {}, {}\n", .{ m1, m2 });

    // swap 使用
    var a: i32 = 1;
    var b: i32 = 2;
    swap(i32, &a, &b);
    std.debug.print("After swap: a={}, b={}\n", .{ a, b });

    // ArrayList 使用
    const IntList = ArrayList(i32);
    var list = IntList.init(std.heap.page_allocator);
    _ = list;

    // 类型萃取
    const Elem = ElementType([10]i32);
    std.debug.assert(Elem == i32);
}
