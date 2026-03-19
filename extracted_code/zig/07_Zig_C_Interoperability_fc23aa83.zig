//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 440
// Language: zig
// Block ID: fc23aa83
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    const lib = b.addSharedLibrary(.{
        .name = "zigmath",
        .root_source_file = b.path("src/math.zig"),
        .target = b.standardTargetOptions(.{}),
        .optimize = b.standardOptimizeOption(.{}),
    });

    // 生成C头文件
    const generate_header = b.addSystemCommand(&.{
        "zig", "translate-c",
        "-lc",
        "--enable-cache",
    });
    // 或者使用zig ffi工具生成

    b.installArtifact(lib);
}
