//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 2571
// Language: zig
// Block ID: 9f0b00e8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 编译时接口检查
const std = @import("std");

// ========== 编译时接口定义 ==========

// 使用 comptime 检查实现接口
fn ShapeInterface(comptime T: type) type {
    comptime {
        // 验证类型实现所需方法
        if (!@hasDecl(T, "draw")) {
            @compileError(@typeName(T) ++ " must implement draw()");
        }
        if (!@hasDecl(T, "area")) {
            @compileError(@typeName(T) ++ " must implement area()");
        }

        // 验证方法签名
        const draw_info = @typeInfo(@TypeOf(T.draw));
        if (draw_info != .Fn or draw_info.Fn.params.len != 1) {
            @compileError(@typeName(T) ++ ".draw must take exactly 1 parameter");
        }
    }

    return struct {
        pub const Impl = T;

        pub fn draw(self: T) void {
            self.draw();
        }

        pub fn area(self: T) f64 {
            return self.area();
        }
    };
}

// ========== 具体实现 ==========

const Circle = struct {
    radius: f64,

    pub fn draw(self: Circle) void {
        std.debug.print("Circle(r={d:.2})\n", .{self.radius});
    }

    pub fn area(self: Circle) f64 {
        return std.math.pi * self.radius * self.radius;
    }
};

const Rectangle = struct {
    width: f64,
    height: f64,

    pub fn draw(self: Rectangle) void {
        std.debug.print("Rectangle({d:.2}x{d:.2})\n", .{ self.width, self.height });
    }

    pub fn area(self: Rectangle) f64 {
        return self.width * self.height;
    }
};

// 编译时验证接口实现
const CircleShape = ShapeInterface(Circle);  // OK
const RectShape = ShapeInterface(Rectangle);  // OK

// const BadType = struct { x: i32 };
// const BadShape = ShapeInterface(BadType);  // 编译错误

// ========== 运行时多态 ==========

const ShapeUnion = union(enum) {
    circle: Circle,
    rectangle: Rectangle,

    pub fn draw(self: ShapeUnion) void {
        switch (self) {
            inline else => |shape| shape.draw(),
        }
    }

    pub fn area(self: ShapeUnion) f64 {
        return switch (self) {
            inline else => |shape| shape.area(),
        };
    }
};

// ========== 泛型算法 ==========

fn totalArea(comptime T: type, shapes: []const T) f64 {
    comptime ShapeInterface(T);  // 验证 T 实现 Shape 接口

    var total: f64 = 0;
    for (shapes) |shape| {
        total += shape.area();
    }
    return total;
}

// 使用
fn example() void {
    const circles = [_]Circle{
        .{ .radius = 1.0 },
        .{ .radius = 2.0 },
    };
    const total = totalArea(Circle, &circles);
    std.debug.print("Total area: {d:.2}\n", .{total});
}
