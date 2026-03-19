//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 306
// Language: zig
// Block ID: fa7850b3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// c_wrapper.zig
const std = @import("std");

// 导入C头文件
const c = @cImport({
    @cInclude("clib.h");
});

pub const Person = extern struct {
    name: [64]u8,
    age: c_int,

    // Zig风格的方法
    pub fn create(name: []const u8, age: i32) !*Person {
        const c_name = try std.heap.c_allocator.dupeZ(u8, name);
        defer std.heap.c_allocator.free(c_name);

        const p = c.person_create(c_name.ptr, age);
        if (p == null) return error.OutOfMemory;
        return @ptrCast(p);
    }

    pub fn destroy(self: *Person) void {
        c.person_free(@ptrCast(self));
    }

    pub fn greet(self: *const Person) void {
        c.person_greet(@ptrCast(self));
    }

    pub fn getName(self: *const Person) []const u8 {
        return std.mem.sliceTo(&self.name, 0);
    }
};

// 更安全的包装
pub const SafePerson = struct {
    ptr: *Person,

    pub fn init(name: []const u8, age: i32) !SafePerson {
        return .{ .ptr = try Person.create(name, age) };
    }

    pub fn deinit(self: SafePerson) void {
        self.ptr.destroy();
    }

    pub fn greet(self: SafePerson) void {
        self.ptr.greet();
    }
};
