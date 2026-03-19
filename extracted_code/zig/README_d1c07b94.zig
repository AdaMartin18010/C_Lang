//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\README.md
// Line: 99
// Language: zig
// Block ID: d1c07b94
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 使用Zig分配器分配C内存
fn zig_malloc(size: usize) callconv(.C) ?*anyopaque {
    const ptr = std.heap.c_allocator.alloc(u8, size) catch return null;
    return ptr.ptr;
}

// 释放C内存
fn zig_free(ptr: ?*anyopaque) callconv(.C) void {
    if (ptr) |p| {
        std.heap.c_allocator.free(@as([*]u8, @ptrCast(p))[0..1]);
    }
}
