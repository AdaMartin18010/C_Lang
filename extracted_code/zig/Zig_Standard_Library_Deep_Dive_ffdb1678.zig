//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 822
// Language: zig
// Block ID: ffdb1678
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn path_operations() void {
    const path = "/home/user/documents/file.txt";

    // 获取文件名
    const basename = std.fs.path.basename(path);
    std.log.info("Basename: {s}", .{basename});  // "file.txt"

    // 获取目录
    const dirname = std.fs.path.dirname(path) orelse "/";
    std.log.info("Dirname: {s}", .{dirname});  // "/home/user/documents"

    // 分离扩展名
    const ext = std.fs.path.extension(path);
    std.log.info("Extension: {s}", .{ext});  // ".txt"

    // 连接路径
    var buf: [256]u8 = undefined;
    const joined = std.fs.path.join(&buf, &.{ "/home/user", "projects", "zig" }) catch unreachable;
    std.log.info("Joined: {s}", .{joined});

    // 规范化路径
    var norm_buf: [256]u8 = undefined;
    const normalized = std.fs.path.resolve(&norm_buf, &.{ "/home/user", "../alice", "./documents" }) catch unreachable;
    std.log.info("Normalized: {s}", .{normalized});  // "/home/alice/documents"

    // 判断绝对/相对
    const is_abs = std.fs.path.isAbsolute("/home/user");
    const is_rel = !std.fs.path.isAbsolute("relative/path");
    std.log.info("Absolute: {}, Relative: {}", .{ is_abs, is_rel });
}
