//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 338
// Language: zig
// Block ID: 23b6a145
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 创建库模块
    const lib_mod = b.addModule("my_lib", .{
        .source_file = .{ .path = "src/lib.zig" },
    });

    // 创建静态库
    const lib = b.addStaticLibrary(.{
        .name = "my_lib",
        .root_source_file = .{ .path = "src/lib.zig" },
        .target = target,
        .optimize = optimize,
    });
    b.installArtifact(lib);

    // 单元测试
    const lib_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/lib.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_lib_tests = b.addRunArtifact(lib_tests);
    const test_step = b.step("test", "Run library tests");
    test_step.dependOn(&run_lib_tests.step);

    // 示例
    const example = b.addExecutable(.{
        .name = "example",
        .root_source_file = .{ .path = "examples/basic_usage.zig" },
        .target = target,
        .optimize = optimize,
    });
    example.addModule("my_lib", lib_mod);

    const run_example = b.addRunArtifact(example);
    const example_step = b.step("example", "Run example");
    example_step.dependOn(&run_example.step);

    // 文档
    const docs = b.addInstallDirectory(.{
        .source_dir = lib.getEmittedDocs(),
        .install_dir = .prefix,
        .install_subdir = "docs",
    });
    const docs_step = b.step("docs", "Generate documentation");
    docs_step.dependOn(&docs.step);
}
