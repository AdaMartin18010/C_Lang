//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1127
// Language: zig
// Block ID: d90061fe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 编译 C 代码
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 编译 C 库
    const lib = b.addStaticLibrary(.{
        .name = "myclib",
        .target = target,
        .optimize = optimize,
    });

    lib.addCSourceFiles(.{
        .files = &.{
            "src/c_lib/file1.c",
            "src/c_lib/file2.c",
        },
        .flags = &.{
            "-Wall",
            "-Wextra",
            "-O2",
        },
    });

    lib.addIncludePath(.{ .path = "src/c_lib/include" });
    lib.linkLibC();

    // 主程序
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    exe.addIncludePath(.{ .path = "src/c_lib/include" });
    exe.linkLibrary(lib);
    exe.linkLibC();

    b.installArtifact(exe);
}
