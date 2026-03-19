//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 579
// Language: zig
// Block ID: 1402621d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 翻译 C 头文件
    const translate_c = b.addTranslateC(.{
        .root_source_file = .{ .path = "include/mylib.h" },
        .target = target,
        .optimize = optimize,
    });

    // 添加宏定义
    translate_c.defineCMacro("ENABLE_FEATURE_X", "1");
    translate_c.addIncludePath(.{ .path = "include" });

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 使用翻译后的模块
    exe.addModule("mylib", translate_c.createModule());

    exe.linkLibC();
    exe.linkSystemLibrary("mylib");

    b.installArtifact(exe);
}
