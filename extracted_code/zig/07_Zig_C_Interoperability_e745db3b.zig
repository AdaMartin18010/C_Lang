//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 547
// Language: zig
// Block ID: e745db3b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 编译C代码并链接Zig库
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 编译Zig库
    const zig_lib = b.addStaticLibrary(.{
        .name = "zigmath",
        .root_source_file = b.path("src/math.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 编译C可执行文件
    const exe = b.addExecutable(.{
        .name = "c_app",
        .target = target,
        .optimize = optimize,
    });
    exe.addCSourceFile(.{
        .file = b.path("src/main.c"),
        .flags = &.{"-Wall", "-Wextra"},
    });
    exe.addIncludePath(b.path("include"));
    exe.linkLibrary(zig_lib);
    exe.linkLibC();

    b.installArtifact(exe);
}
