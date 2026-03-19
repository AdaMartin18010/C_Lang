//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 388
// Language: zig
// Block ID: e7f9e1cf
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 混合C/Zig项目

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 编译C库
    const lib_c = b.addStaticLibrary(.{
        .name = "c_lib",
        .target = target,
        .optimize = optimize,
    });
    lib_c.addCSourceFiles(.{
        .files = &.{
            "src/c_lib/file1.c",
            "src/c_lib/file2.c",
        },
        .flags = &.{
            "-std=c11",
            "-Wall",
            "-Wextra",
        },
    });
    lib_c.addIncludePath(b.path("src/c_lib/include"));

    // 链接到Zig可执行文件
    const exe = b.addExecutable(.{
        .name = "hybrid_app",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    exe.linkLibrary(lib_c);
    exe.addIncludePath(b.path("src/c_lib/include"));

    b.installArtifact(exe);
}
