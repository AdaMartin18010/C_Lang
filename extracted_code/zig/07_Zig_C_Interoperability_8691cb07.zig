//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 588
// Language: zig
// Block ID: 8691cb07
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 分配器接口
const std = @import("std");
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

// Zig分配，C使用，Zig释放
export fn create_buffer(size: usize) ?[*]u8 {
    const buf = allocator.alloc(u8, size) catch return null;
    return buf.ptr;
}

export fn free_buffer(ptr: ?[*]u8, size: usize) void {
    if (ptr) |p| {
        allocator.free(p[0..size]);
    }
}

// C调用方
// uint8_t *buf = create_buffer(1024);
// ... use buf ...
// free_buffer(buf, 1024);
