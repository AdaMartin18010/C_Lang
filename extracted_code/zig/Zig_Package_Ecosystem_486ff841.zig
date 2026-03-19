//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 207
// Language: zig
// Block ID: 486ff841
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 本地开发时使用本地路径

pub fn build(b: *std.Build) void {
    // 检查环境变量
    const local_zap = b.option([]const u8, "local-zap", "Path to local zap");

    const zap = if (local_zap) |path|
        b.createModule(.{
            .source_file = .{ .path = b.pathJoin(&.{ path, "src/zap.zig" }) },
        })
    else
        b.dependency("zap", .{}).module("zap");

    // ... 使用 zap
}
