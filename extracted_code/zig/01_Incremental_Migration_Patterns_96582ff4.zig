//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 598
// Language: zig
// Block ID: 96582ff4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 使用 Zig 构建 C 项目

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 1. 创建 C 库（保持现有代码）
    const c_lib = b.addStaticLibrary(.{
        .name = "legacy_core",
        .target = target,
        .optimize = optimize,
    });

    // 添加 C 源文件
    c_lib.addCSourceFiles(.{
        .files = &.{
            "src/core/parser.c",
            "src/core/lexer.c",
            "src/core/ast.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-O2",
        },
    });

    c_lib.addIncludePath(b.path("include"));
    c_lib.linkLibC();

    // 2. 创建 Zig 模块（新增功能）
    const zig_module = b.addModule("new_features", .{
        .root_source_file = b.path("src/zig/modern_api.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 3. 创建主可执行文件（混合 C + Zig）
    const exe = b.addExecutable(.{
        .name = "hybrid_app",
        .root_source_file = b.path("src/zig/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 链接 C 库
    exe.linkLibrary(c_lib);
    exe.addIncludePath(b.path("include"));
    exe.root_module.addImport("new_features", zig_module);

    // 4. 安装和运行
    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the hybrid app");
    run_step.dependOn(&run_cmd.step);

    // 5. 测试（混合测试）
    const unit_tests = b.addTest(.{
        .root_source_file = b.path("src/zig/tests.zig"),
        .target = target,
        .optimize = optimize,
    });
    unit_tests.linkLibrary(c_lib);
    unit_tests.addIncludePath(b.path("include"));

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&b.addRunArtifact(unit_tests).step);
}
