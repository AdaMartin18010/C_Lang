//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 309
// Language: zig
// Block ID: 79f27ffd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

test "file operations" {
    const allocator = testing.allocator;

    // 创建临时目录
    var tmp_dir = testing.tmpDir(.{});
    defer tmp_dir.cleanup();

    // 在临时目录中操作
    const file = try tmp_dir.dir.createFile("test.txt", .{});
    defer file.close();

    try file.writeAll("hello world");

    // 读取验证
    const content = try tmp_dir.dir.readFileAlloc(allocator, "test.txt", 1024);
    defer allocator.free(content);

    try testing.expectEqualStrings("hello world", content);
}
