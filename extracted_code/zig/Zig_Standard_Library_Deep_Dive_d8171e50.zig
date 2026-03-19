//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 771
// Language: zig
// Block ID: d8171e50
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn directory_operations() !void {
    const cwd = std.fs.cwd();
    const allocator = std.heap.page_allocator;

    // 创建目录
    try cwd.makeDir("mydir");

    // 递归创建
    try cwd.makePath("parent/child/grandchild");

    // 打开目录
    var dir = try cwd.openDir("mydir", .{ .iterate = true });
    defer dir.close();

    // 列出目录内容
    var it = dir.iterate();
    while (try it.next()) |entry| {
        switch (entry.kind) {
            .file => std.log.info("File: {s}", .{entry.name}),
            .directory => std.log.info("Dir: {s}", .{entry.name}),
            else => std.log.info("Other: {s}", .{entry.name}),
        }
    }

    // 递归遍历
    var walker = try cwd.walk(allocator);
    defer walker.deinit();

    while (try walker.next()) |entry| {
        std.log.info("Path: {s}, Kind: {}", .{ entry.path, entry.kind });
    }

    // 重命名
    try cwd.rename("mydir", "mydir2");

    // 删除
    try cwd.deleteFile("test.txt");
    try cwd.deleteDir("mydir2");

    // 删除非空目录
    try cwd.deleteTree("parent");

    // 文件信息
    const stat = try cwd.statFile("build.zig");
    std.log.info("Size: {}, Modified: {}", .{ stat.size, stat.mtime });
}
