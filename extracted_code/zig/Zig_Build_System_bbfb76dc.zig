//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 640
// Language: zig
// Block ID: bbfb76dc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 根据目标平台条件编译

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 平台特定代码
    if (target.result.os.tag == .windows) {
        exe.addCSourceFile("src/win32_specific.c", &.{});
        exe.linkSystemLibrary("kernel32");
    } else if (target.result.os.tag == .linux) {
        exe.addCSourceFile("src/linux_specific.c", &.{});
        exe.linkSystemLibrary("pthread");
    }

    // 架构特定优化
    if (target.result.cpu.arch == .x86_64) {
        exe.target.cpu_features_add = std.Target.x86.featureSet(&.{.sse4_2});
    }

    b.installArtifact(exe);
}
