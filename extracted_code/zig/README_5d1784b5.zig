//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\README.md
// Line: 155
// Language: zig
// Block ID: 5d1784b5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 替代CMake/Makefile
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 编译C库
    const lib = b.addStaticLibrary(.{
        .name = "mylib",
        .target = target,
        .optimize = optimize,
    });

    lib.addCSourceFiles(&.{
        "src/core.c",
        "src/utils.c",
        "src/api.c",
    }, &.{
        "-Wall",
        "-Wextra",
        "-O3",
    });

    lib.linkLibC();
    b.installArtifact(lib);
}
