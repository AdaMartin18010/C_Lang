//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 139
// Language: zig
// Block ID: 9efa0478
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 完整配置示例

const std = @import("std");

pub fn build(b: *std.Build) !void {
    // ==================== 选项配置 ====================

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOptions(.{});

    // 自定义选项
    const enable_logging = b.option(bool, "enable-logging", "Enable debug logging") orelse false;
    const max_connections = b.option(u32, "max-connections", "Max concurrent connections") orelse 100;

    // ==================== 模块定义 ====================

    // 创建可执行文件
    const exe = b.addExecutable(.{
        .name = "server",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 添加模块
    const utils_module = b.createModule(.{
        .source_file = .{ .path = "src/utils.zig" },
    });
    exe.addModule("utils", utils_module);

    // 添加依赖模块
    const network_module = b.createModule(.{
        .source_file = .{ .path = "libs/network/src/lib.zig" },
        .dependencies = &.{
            .{ .name = "utils", .module = utils_module },
        },
    });
    exe.addModule("network", network_module);

    // ==================== 编译选项 ====================

    // 定义编译时选项
    const options = b.addOptions();
    options.addOption(bool, "enable_logging", enable_logging);
    options.addOption(u32, "max_connections", max_connections);
    options.addOption([]const u8, "version", "1.0.0");
    exe.addOptions("config", options);

    // C 源文件
    exe.addCSourceFile("src/sha256.c", &.{
        "-O3",
        "-Wall",
    });

    // 链接库
    exe.linkLibC();
    exe.linkSystemLibrary("ssl");
    exe.linkSystemLibrary("crypto");

    // 库搜索路径
    exe.addLibraryPath(.{ .path = "libs" });

    // 包含路径
    exe.addIncludePath(.{ .path = "include" });
    exe.addIncludePath(.{ .path = "libs/network/include" });

    // 链接时选项
    exe.link_gc_sections = true;  // 移除未使用代码
    exe.link_z_relro = true;      // 启用 RELRO

    // ==================== 安装配置 ====================

    // 安装可执行文件
    b.installArtifact(exe);

    // 安装额外文件
    b.installFile("config/default.conf", "etc/myapp/default.conf");
    b.installDirectory(.{
        .source_dir = "assets",
        .install_dir = .prefix,
        .install_subdir = "share/myapp/assets",
    });

    // ==================== 运行配置 ====================

    // zig run
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // ==================== 测试配置 ====================

    // 单元测试
    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    // 集成测试
    const integration_tests = b.addTest(.{
        .root_source_file = .{ .path = "tests/integration.zig" },
        .target = target,
        .optimize = optimize,
    });
    integration_tests.addModule("network", network_module);

    const run_integration_tests = b.addRunArtifact(integration_tests);

    // 测试步骤
    const test_step = b.step("test", "Run all tests");
    test_step.dependOn(&run_unit_tests.step);
    test_step.dependOn(&run_integration_tests.step);

    // ==================== 其他步骤 ====================

    // 格式化检查
    const fmt_step = b.addFmt(.{
        .paths = &.{
            "src",
            "tests",
            "build.zig",
        },
        .check = true,  // 检查模式，不实际修改
    });

    const check_step = b.step("check-fmt", "Check formatting");
    check_step.dependOn(&fmt_step.step);

    // 文档生成
    const docs = b.addInstallDirectory(.{
        .source_dir = exe.getEmittedDocs(),
        .install_dir = .prefix,
        .install_subdir = "docs",
    });

    const docs_step = b.step("docs", "Generate documentation");
    docs_step.dependOn(&docs.step);

    // 清理步骤
    const clean_step = b.step("clean", "Clean build artifacts");
    const clean_cmd = b.addSystemCommand(&.{ "rm", "-rf", "zig-out", "zig-cache" });
    clean_step.dependOn(&clean_cmd.step);
}
