//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 198
// Language: zig
// Block ID: ebd9375c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 统一构建配置
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 创建 C 库静态链接
    const c_lib = b.addStaticLibrary(.{
        .name = "legacy",
        .target = target,
        .optimize = optimize,
    });

    // 添加 C 源文件
    c_lib.addCSourceFiles(.{
        .files = &.{
            "legacy/src/core.c",
            "legacy/src/utils.c",
            "legacy/src/parser.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-O2",
        },
    });

    c_lib.addIncludePath(b.path("legacy/include"));
    c_lib.linkLibC();

    // 创建 Zig 可执行文件
    const exe = b.addExecutable(.{
        .name = "migrated_app",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 链接 C 库
    exe.linkLibrary(c_lib);
    exe.addIncludePath(b.path("legacy/include"));
    exe.linkLibC();

    // 启用 translate-c 进行头文件导入
    exe.addAnonymousModule("c_headers", .{
        .root_source_file = b.path("legacy/include/legacy.h"),
    });

    b.installArtifact(exe);
}
