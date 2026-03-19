//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 752
// Language: zig
// Block ID: e02de21e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 优化构建缓存

pub fn build(b: *std.Build) void {
    // 使用全局缓存
    b.cache_root = .{ .path = "/var/cache/zig-build" };

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 共享模块
    const shared_module = b.createModule(.{
        .source_file = .{ .path = "src/shared.zig" },
    });

    // 多个目标共享模块
    const lib = b.addStaticLibrary(.{
        .name = "mylib",
        .root_source_file = .{ .path = "src/lib.zig" },
        .target = target,
        .optimize = optimize,
    });
    lib.addModule("shared", shared_module);

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });
    exe.addModule("shared", shared_module);
    exe.linkLibrary(lib);

    b.installArtifact(lib);
    b.installArtifact(exe);
}
