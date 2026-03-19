//
// Auto-generated from: Zig\README.md
// Line: 1168
// Language: zig
// Block ID: db031b76
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() !void {
    // 使用 GPA（通用分配器）
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 显式分配
    const buffer = try allocator.alloc(u8, 1024);
    defer allocator.free(buffer);  // 显式释放

    // 使用内存...
    @memcpy(buffer[0..5], "hello");
}
