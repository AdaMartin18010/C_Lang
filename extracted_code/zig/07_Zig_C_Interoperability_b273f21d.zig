//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 280
// Language: zig
// Block ID: b273f21d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "sqlite_example",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 链接SQLite库
    exe.linkLibC();  // 链接libc
    exe.linkSystemLibrary("sqlite3");  // 链接sqlite3

    // 或者链接本地静态库
    exe.addObjectFile(b.path("libsqlite3.a"));

    // 添加头文件搜索路径
    exe.addIncludePath(b.path("include"));

    b.installArtifact(exe);

    // 运行命令
    const run_cmd = b.addRunArtifact(exe);
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
