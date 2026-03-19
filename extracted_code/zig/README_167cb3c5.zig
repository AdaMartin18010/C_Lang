//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 241
// Language: zig
// Block ID: 167cb3c5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026年：更灵活的模块系统

// 条件编译模块
pub usingnamespace switch (builtin.target.os.tag) {
    .linux => @import("platform/linux.zig"),
    .windows => @import("platform/windows.zig"),
    .macos => @import("platform/macos.zig"),
    else => @compileError("Unsupported platform"),
};

// 模块化构建
// build.zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    // 创建模块
    const utils = b.createModule(.{
        .root_source_file = b.path("src/utils/mod.zig"),
    });

    const network = b.createModule(.{
        .root_source_file = b.path("src/network/mod.zig"),
        .imports = &.{
            .{ .name = "utils", .module = utils },
        },
    });

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = b.path("src/main.zig"),
    });

    exe.root_module.addImport("network", network);
    exe.root_module.addImport("utils", utils);
}
