//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 1837
// Language: zig
// Block ID: 36e0c09a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 完整的反射能力
const std = @import("std");

// ========== 类型内省 ==========

const Point = struct {
    x: f32,
    y: f32,
    z: f32,

    pub fn magnitude(self: Point) f32 {
        return @sqrt(self.x * self.x + self.y * self.y + self.z * self.z);
    }
};

// 获取类型信息
const point_info = @typeInfo(Point);

// 遍历结构体字段
fn printStructInfo(comptime T: type) void {
    const info = @typeInfo(T);
    std.debug.print("Type: {s}\n", .{@typeName(T)});
    std.debug.print("Size: {} bytes\n", .{@sizeOf(T)});
    std.debug.print("Alignment: {}\n", .{@alignOf(T)});

    if (info == .Struct) {
        std.debug.print("Fields:\n", .{});
        inline for (info.Struct.fields) |field| {
            std.debug.print("  {s}: {s} (offset: {})\n", .{
                field.name,
                @typeName(field.type),
                @offsetOf(T, field.name),
            });
        }
    }
}

// ========== 字段操作 ==========

// 编译时字段访问
fn getFieldOffset(comptime T: type, comptime field_name: []const u8) usize {
    return @offsetOf(T, field_name);
}

// 根据名称获取字段值（运行时）
fn getFieldValue(obj: anytype, comptime field_name: []const u8) !@TypeOf(@field(obj, field_name)) {
    // 编译时验证字段存在
    comptime {
        const info = @typeInfo(@TypeOf(obj));
        if (info != .Struct and info != .Union) {
            @compileError("Expected struct or union");
        }
        var found = false;
        inline for (info.Struct.fields) |field| {
            if (std.mem.eql(u8, field.name, field_name)) {
                found = true;
                break;
            }
        }
        if (!found) {
            @compileError("Field not found: " ++ field_name);
        }
    }
    return @field(obj, field_name);
}

// ========== 方法枚举 ==========

// 检查类型是否有特定声明
fn hasMethod(comptime T: type, comptime name: []const u8) bool {
    return @hasDecl(T, name);
}

// 检查类型是否有特定字段
fn hasField(comptime T: type, comptime name: []const u8) bool {
    const info = @typeInfo(T);
    if (info != .Struct) return false;
    inline for (info.Struct.fields) |field| {
        if (std.mem.eql(u8, field.name, name)) return true;
    }
    return false;
}

// ========== 接口检查 ==========

trait Drawable(comptime T: type) bool {
    return @hasDecl(T, "draw") and
           @typeInfo(@TypeOf(@field(T, "draw"))).Fn.params.len == 1;
}

fn drawIfSupported(obj: anytype) void {
    comptime {
        if (!Drawable(@TypeOf(obj))) {
            @compileError("Type must implement draw() method");
        }
    }
    obj.draw();
}

// ========== 序列化代码生成 ==========

fn makeSerializer(comptime T: type) type {
    const info = @typeInfo(T);
    if (info != .Struct) {
        @compileError("Only structs supported");
    }

    return struct {
        pub fn serialize(obj: T, writer: anytype) !void {
            inline for (info.Struct.fields) |field| {
                try writer.writeAll(field.name);
                try writer.writeByte('=');
                try serializeValue(@field(obj, field.name), writer);
                try writer.writeByte('\n');
            }
        }

        fn serializeValue(value: anytype, writer: anytype) !void {
            const V = @TypeOf(value);
            switch (@typeInfo(V)) {
                .Int => try writer.print("{}", .{value}),
                .Float => try writer.print("{d}", .{value}),
                .Bool => try writer.writeAll(if (value) "true" else "false"),
                else => @compileError("Unsupported field type: " ++ @typeName(V)),
            }
        }
    };
}

const PointSerializer = makeSerializer(Point);
