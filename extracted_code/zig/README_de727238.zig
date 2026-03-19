//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 163
// Language: zig
// Block ID: de727238
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026年新特性：更强大的comptime
// 编译时序列化/反序列化

const std = @import("std");

// 编译时生成类型安全的序列化代码
pub fn Serializer(comptime T: type) type {
    return struct {
        pub fn serialize(value: T, writer: anytype) !void {
            inline for (@typeInfo(T).Struct.fields) |field| {
                try writer.writeAll(@as([]const u8, field.name));
                try writer.writeByte(':');

                const field_value = @field(value, field.name);
                switch (@typeInfo(field.type)) {
                    .Int => try writer.print("{d}", .{field_value}),
                    .Float => try writer.print("{d:.2}", .{field_value}),
                    .Pointer => |p| if (p.size == .Slice and p.child == u8)
                        try writer.print("\"{s}\"", .{field_value}),
                    else => @compileError("Unsupported type"),
                }
                try writer.writeByte(',');
            }
        }
    };
}

const User = struct {
    name: []const u8,
    age: u32,
    score: f64,
};

// 编译时生成专门的序列化器
const UserSerializer = Serializer(User);
