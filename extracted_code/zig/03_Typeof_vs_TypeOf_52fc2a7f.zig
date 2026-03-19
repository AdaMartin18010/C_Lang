//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 2267
// Language: zig
// Block ID: 52fc2a7f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 完整的类型特征萃取

const std = @import("std");

// 类型大小和对齐
fn sizeOf(comptime T: type) usize {
    return @sizeOf(T);
}

fn alignOf(comptime T: type) usize {
    return @alignOf(T);
}

// 检查是否为整数类型
fn isInteger(comptime T: type) bool {
    return @typeInfo(T) == .Int;
}

// 检查是否为浮点类型
fn isFloat(comptime T: type) bool {
    return @typeInfo(T) == .Float;
}

// 检查是否为指针类型
fn isPointer(comptime T: type) bool {
    return @typeInfo(T) == .Pointer;
}

// 检查是否为数组类型
fn isArray(comptime T: type) bool {
    return @typeInfo(T) == .Array;
}

// 获取数组元素类型
fn arrayElementType(comptime T: type) type {
    comptime {
        const info = @typeInfo(T);
        if (info != .Array) {
            @compileError("Expected array type");
        }
        return info.Array.child;
    }
}

// 获取数组长度
fn arrayLength(comptime T: type) usize {
    comptime {
        const info = @typeInfo(T);
        if (info != .Array) {
            @compileError("Expected array type");
        }
        return info.Array.len;
    }
}

// 获取指针目标类型
fn pointerTarget(comptime T: type) type {
    comptime {
        const info = @typeInfo(T);
        if (info != .Pointer) {
            @compileError("Expected pointer type");
        }
        return info.Pointer.child;
    }
}

// 获取类型的最大值
fn maxValue(comptime T: type) T {
    comptime {
        const info = @typeInfo(T);
        return switch (info) {
            .Int => |int| std.math.maxInt(T),
            .Float => std.math.floatMax(T),
            else => @compileError("Expected numeric type"),
        };
    }
}

// 获取类型的最小值
fn minValue(comptime T: type) T {
    comptime {
        const info = @typeInfo(T);
        return switch (info) {
            .Int => |int| std.math.minInt(T),
            .Float => std.math.floatMin(T),
            else => @compileError("Expected numeric type"),
        };
    }
}

// 检查类型是否有某个字段
fn hasField(comptime T: type, name: []const u8) bool {
    comptime {
        const info = @typeInfo(T);
        if (info != .Struct) return false;
        inline for (info.Struct.fields) |field| {
            if (std.mem.eql(u8, field.name, name)) return true;
        }
        return false;
    }
}

// 使用示例
fn demoTypeTraits() void {
    const Point = struct { x: i32, y: i32 };

    std.debug.print("i32 size: {}, align: {}\n", .{
        sizeOf(i32), alignOf(i32),
    });

    std.debug.print("isInteger(i32): {}\n", .{isInteger(i32)});
    std.debug.print("isFloat(f64): {}\n", .{isFloat(f64)});

    const Arr = [10]i32;
    std.debug.print("array element: {}, length: {}\n", .{
        @typeName(arrayElementType(Arr)),
        arrayLength(Arr),
    });

    std.debug.print("hasField(Point, 'x'): {}\n", .{hasField(Point, "x")});
    std.debug.print("i32 max: {}\n", .{maxValue(i32)});
}
