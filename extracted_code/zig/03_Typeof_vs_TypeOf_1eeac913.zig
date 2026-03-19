//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 2046
// Language: zig
// Block ID: 1eeac913
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 类型安全函数实现

const std = @import("std");

// 类型安全最大值
fn max(a: anytype, b: @TypeOf(a)) @TypeOf(a) {
    return if (a > b) a else b;
}

// 类型安全最小值
fn min(a: anytype, b: @TypeOf(a)) @TypeOf(a) {
    return if (a < b) a else b;
}

// 类型安全交换
fn swap(a: anytype, b: anytype) void {
    comptime {
        if (@TypeOf(a) != @TypeOf(b)) {
            @compileError("swap requires same types");
        }
        const info = @typeInfo(@TypeOf(a));
        if (info != .Pointer) {
            @compileError("swap requires pointers");
        }
    }
    const T = @typeInfo(@TypeOf(a)).Pointer.child;
    const tmp: T = a.*;
    a.* = b.*;
    b.* = tmp;
}

// 数组长度 (编译时计算)
fn arraySize(comptime T: type, arr: []const T) usize {
    return arr.len;
}

// 偏移量计算
fn offsetOf(comptime T: type, comptime field: []const u8) usize {
    return @offsetOf(T, field);
}

// 容器遍历
fn forEach(container: anytype, func: anytype) void {
    for (container) |*item| {
        func(item);
    }
}

// 使用示例
fn demoTypeSafeUtils() void {
    // max/min
    const m = max(10, 20);
    std.debug.print("max: {}\n", .{m});

    // swap
    var a: i32 = 1;
    var b: i32 = 2;
    swap(&a, &b);
    std.debug.print("a={}, b={}\n", .{ a, b });

    // array size
    const arr = [_]i32{ 1, 2, 3, 4, 5 };
    std.debug.print("size: {}\n", .{arr.len});

    // offsetOf
    const Point = struct { x: i32, y: i32 };
    std.debug.print("offset of y: {}\n", .{offsetOf(Point, "y")});
}
