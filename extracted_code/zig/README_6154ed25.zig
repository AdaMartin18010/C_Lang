//
// Auto-generated from: Zig\README.md
// Line: 967
// Language: zig
// Block ID: 6154ed25
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 创建可执行文件
    const exe = b.addExecutable(.{
        .name = "my-project",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 添加 C 源文件
    exe.addCSourceFile(.{
        .file = b.path("c_src/legacy.c"),
        .flags = &.{"-Wall", "-O2"},
    });

    // 添加头文件搜索路径
    exe.addIncludePath(b.path("c_src"));
    exe.addIncludePath(b.path("vendor/some_c_lib/include"));

    // 链接 C 库
    exe.linkLibC();
    exe.linkSystemLibrary("some_c_lib");

    b.installArtifact(exe);

    // 运行命令
    const run_cmd = b.addRunArtifact(exe);
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
