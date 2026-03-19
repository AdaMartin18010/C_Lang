//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1179
// Language: zig
// Block ID: 15bb4226
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn build(b: *std.Build) void {
    // 标准目标选项
    const target = b.standardTargetOptions(.{});

    // 标准优化选项
    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .ReleaseFast,
    });

    // 模块定义
    const mylib_module = b.addModule("mylib", .{
        .root_source_file = .{ .path = "src/mylib.zig" },
    });

    // 可执行文件
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 添加模块依赖
    exe.root_module.addImport("mylib", mylib_module);

    // 链接选项
    exe.linkLibC();
    exe.linkSystemLibrary("m");  // 数学库

    // 添加 C 源码
    exe.addCSourceFile(.{
        .file = .{ .path = "src/helper.c" },
        .flags = &.{"-Wall"},
    });

    // 包含路径
    exe.addIncludePath(.{ .path = "include" });

    // 链接库
    exe.addLibraryPath(.{ .path = "lib" });
    exe.linkSystemLibrary("mylib");

    // 定义宏
    exe.defineCMacro("DEBUG", "1");

    // 安装
    b.installArtifact(exe);

    // 运行命令
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // 单元测试
    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_unit_tests.step);

    // 集成测试
    const integration_tests = b.addTest(.{
        .root_source_file = .{ .path = "tests/integration.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_integration_tests = b.addRunArtifact(integration_tests);

    const integration_test_step = b.step("integration-test", "Run integration tests");
    integration_test_step.dependOn(&run_integration_tests.step);

    // 格式化检查
    const fmt_step = b.addFmt(.{
        .paths = &.{
            "src",
            "tests",
            "build.zig",
        },
        .check = true,
    });

    const check_step = b.step("check-fmt", "Check formatting");
    check_step.dependOn(&fmt_step.step);
}
