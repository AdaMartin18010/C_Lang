//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 1184
// Language: zig
// Block ID: d0b29720
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 模拟 C 匿名成员
const std = @import("std");

// Zig 不支持匿名成员，需显式命名

// 方案 1: 扁平化 - 所有字段提升到父结构体
const WithFlattenedStruct = extern struct {
    type: c_int,
    // 匿名结构体成员展开
    x: c_int,
    y: c_int,
    // 极坐标视图通过方法提供

    fn setPolar(self: *WithFlattenedStruct, r: f32, theta: f32) void {
        self.x = @intFromFloat(r * @cos(theta));
        self.y = @intFromFloat(r * @sin(theta));
    }

    fn getR(self: WithFlattenedStruct) f32 {
        return @sqrt(@as(f32, @floatFromInt(self.x * self.x + self.y * self.y)));
    }
};

// 方案 2: 显式命名联合体成员
const WithNamedUnion = extern struct {
    type: c_int,
    data: extern union {
        cartesian: extern struct {
            x: c_int,
            y: c_int,
        },
        polar: extern struct {
            r: f32,
            theta: f32,
        },
    },
};

// 方案 3: 使用 packed struct 模拟 Variant
const Variant = extern struct {
    type: u32,
    value: extern union {
        i: c_int,
        f: f32,
        p: ?*anyopaque,
    },
};

// 方案 4: 使用 @ptrCast 实现灵活的类型双关
fn typePunDemo() void {
    var f: f32 = 3.14159;
    const i_ptr: *const i32 = @ptrCast(&f);
    std.debug.print("float {d} as int: 0x{x}\n", .{f, i_ptr.*});
}

// 方案 5: 使用 @bitCast (更安全的类型双关)
fn bitCastDemo() void {
    const f: f32 = 3.14159;
    const i: i32 = @bitCast(f);
    std.debug.print("float {d} as int: 0x{x}\n", .{f, i});

    // 双向转换
    const f_back: f32 = @bitCast(i);
    std.debug.print("back to float: {d}\n", .{f_back});
}

pub fn main() void {
    // 演示方案 1
    var p: WithFlattenedStruct = .{
        .type = 0,
        .x = 10,
        .y = 20,
    };
    std.debug.print("x = {d}, y = {d}\n", .{p.x, p.y});
    std.debug.print("r = {d}\n", .{p.getR()});

    // 演示方案 2
    var p2: WithNamedUnion = .{
        .type = 0,
        .data = .{ .cartesian = .{ .x = 10, .y = 20 } },
    };
    std.debug.print("via union: x = {d}\n", .{p2.data.cartesian.x});

    // 类型双关演示
    typePunDemo();
    bitCastDemo();
}
