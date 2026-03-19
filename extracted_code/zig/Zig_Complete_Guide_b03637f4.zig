//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 466
// Language: zig
// Block ID: b03637f4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

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
