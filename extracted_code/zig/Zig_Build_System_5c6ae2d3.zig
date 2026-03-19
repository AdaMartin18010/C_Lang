//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 112
// Language: zig
// Block ID: 5c6ae2d3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 最小配置

const std = @import("std");

pub fn build(b: *std.Build) void {
    // 标准优化选项
    const optimize = b.standardOptimizeOption(.{});

    // 标准目标选项
    const target = b.standardTargetOptions(.{});

    // 创建可执行文件
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 安装可执行文件
    b.installArtifact(exe);
}
