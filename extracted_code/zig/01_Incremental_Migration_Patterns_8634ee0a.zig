//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 2278
// Language: zig
// Block ID: 8634ee0a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 完整的混合项目配置

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // ═══════════════════════════════════════════════════════════
    // C 库配置
    // ═══════════════════════════════════════════════════════════

    const c_lib = b.addStaticLibrary(.{
        .name = "c_core",
        .target = target,
        .optimize = optimize,
    });

    // C 源文件 - 使用 Zig 编译器作为 C 编译器
    c_lib.addCSourceFiles(.{
        .files = &.{
            "src/c/legacy_parser.c",
            "src/c/data_structures.c",
            "src/c/io_utils.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-Werror=implicit-function-declaration",
            "-O2",
        },
    });

    // 添加第三方 C 库
    c_lib.addIncludePath(b.path("deps/libcurl/include"));
    c_lib.addIncludePath(b.path("deps/sqlite"));

    // 链接系统库
    c_lib.linkSystemLibrary("curl");
    c_lib.linkSystemLibrary("sqlite3");
    c_lib.linkLibC();

    b.installArtifact(c_lib);

    // ═══════════════════════════════════════════════════════════
    // Zig 模块配置
    // ═══════════════════════════════════════════════════════════

    const zig_mod = b.addModule("app", .{
        .root_source_file = b.path("src/zig/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 导出 Zig 模块供 C 使用
    const zig_lib = b.addSharedLibrary(.{
        .name = "zig_bridge",
        .root_source_file = b.path("src/zig/exports.zig"),
        .target = target,
        .optimize = optimize,
    });
    zig_lib.linkLibC();
    b.installArtifact(zig_lib);

    // ═══════════════════════════════════════════════════════════
    // 主可执行文件
    // ═══════════════════════════════════════════════════════════

    const exe = b.addExecutable(.{
        .name = "hybrid_app",
        .root_source_file = b.path("src/zig/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 链接 C 库
    exe.linkLibrary(c_lib);
    exe.addIncludePath(b.path("src/c/include"));

    // 链接系统库
    exe.linkSystemLibrary("pthread");
    exe.linkSystemLibrary("m");

    b.installArtifact(exe);

    // ═══════════════════════════════════════════════════════════
    // 测试配置
    // ═══════════════════════════════════════════════════════════

    const unit_tests = b.addTest(.{
        .root_source_file = b.path("src/zig/tests.zig"),
        .target = target,
        .optimize = optimize,
    });
    unit_tests.linkLibrary(c_lib);
    unit_tests.addIncludePath(b.path("src/c/include"));

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&b.addRunArtifact(unit_tests).step);

    // ═══════════════════════════════════════════════════════════
    // 模糊测试配置
    // ═══════════════════════════════════════════════════════════

    const fuzz_exe = b.addExecutable(.{
        .name = "fuzz_parser",
        .root_source_file = b.path("src/zig/fuzz.zig"),
        .target = target,
        .optimize = .ReleaseSafe,  // 模糊测试需要安全检查
    });
    fuzz_exe.linkLibrary(c_lib);
    fuzz_exe.linkLibFuzzer(true);
    b.installArtifact(fuzz_exe);

    // ═══════════════════════════════════════════════════════════
    // 运行配置
    // ═══════════════════════════════════════════════════════════

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
