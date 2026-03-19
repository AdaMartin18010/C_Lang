//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 172
// Language: zig
// Block ID: 3ee1ac22
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 获取依赖
    const zap_dep = b.dependency("zap", .{
        .target = target,
        .optimize = optimize,
    });

    // 创建可执行文件
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 添加依赖模块
    exe.addModule("zap", zap_dep.module("zap"));

    // 链接依赖库
    exe.linkLibrary(zap_dep.artifact("facil.io"));

    b.installArtifact(exe);
}
