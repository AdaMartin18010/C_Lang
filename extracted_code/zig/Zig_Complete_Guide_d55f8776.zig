//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 684
// Language: zig
// Block ID: d55f8776
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() !void {
    // GPA 自带泄漏检测
    var gpa = std.heap.GeneralPurposeAllocator(.{
        .safety = true,      // 安全检查
        .verbose_log = true, // 详细日志
    }){};
    defer {
        const leaked = gpa.deinit();
        if (leaked) {
            std.debug.print("Memory leak detected!\n", .{});
        }
    }
    const allocator = gpa.allocator();

    // 分配但不释放 (故意泄漏)
    const leaked_ptr = try allocator.alloc(u8, 100);
    _ = leaked_ptr;  // 忘记 free！
}
