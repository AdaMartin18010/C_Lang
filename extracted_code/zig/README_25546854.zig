//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 44
// Language: zig
// Block ID: 25546854
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 使用 GPA (General Purpose Allocator) 检测内存问题
test "zig allocator pattern" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer {
        const check = gpa.deinit();
        if (check == .leak) @panic("内存泄漏检测到！");
    }
    const allocator = gpa.allocator();

    // 动态分配数组
    const slice = try allocator.alloc(u32, 100);
    defer allocator.free(slice);  // 确保释放

    // 使用 Arena 分配器进行批量分配
    var arena = std.heap.ArenaAllocator.init(allocator);
    defer arena.deinit();
    const arena_alloc = arena.allocator();

    // 多次分配，只需一次 deinit
    _ = try arena_alloc.alloc(u8, 100);
    _ = try arena_alloc.alloc(u8, 200);
    _ = try arena_alloc.alloc(u8, 300);
}
