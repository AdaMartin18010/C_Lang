//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 452
// Language: zig
// Block ID: 33b13be4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn processData(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    var buffer = try allocator.alloc(u8, input.len);
    errdefer allocator.free(buffer);  // 失败时自动释放

    // 处理...

    return buffer;  // 调用者负责释放
}

// 使用
const result = try processData(arena.allocator(), input);
defer arena.allocator().free(result);
