//
// Auto-generated from: Zig\matrices\Language_Comparison_Matrix.md
// Line: 277
// Language: zig
// Block ID: 55d6492f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig - 错误联合类型
const FileError = error{
    NotFound,
    PermissionDenied,
};

fn readFile(path: []const u8) FileError![]u8 {
    const file = try std.fs.cwd().openFile(path, .{});
    defer file.close();

    return try file.readToEndAlloc(allocator, max_size);
}

// 使用
const content = readFile("data.txt") catch |err| {
    std.log.err("Failed to read: {}", .{err});
    return;
};
