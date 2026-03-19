//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 2412
// Language: zig
// Block ID: f006798b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 高级混合构建技巧

const std = @import("std");

/// 条件编译：根据目标平台选择实现
fn addPlatformSources(b: *std.Build, lib: *std.Build.Step.Compile) void {
    const target = lib.root_module.resolved_target.orelse return;

    if (target.result.isNative()) {
        // 本机开发：更多 Zig 代码
        lib.addCSourceFiles(.{
            .files = &.{
                "src/c/minimal_shim.c",
            },
            .flags = &.{
                "-std=c11",
            },
        });
    } else if (target.result.os.tag == .linux) {
        // Linux 发布：优化 C 代码
        lib.addCSourceFiles(.{
            .files = &.{
                "src/c/linux_optimized.c",
            },
            .flags = &.{
                "-std=c11",
                "-O3",
                "-DNDEBUG",
            },
        });
    }
}

/// 交叉编译支持
fn setupCrossCompilation(b: *std.Build) void {
    const targets: []const std.Target.Query = &.{
        .{ .cpu_arch = .x86_64, .os_tag = .linux },
        .{ .cpu_arch = .aarch64, .os_tag = .linux },
        .{ .cpu_arch = .x86_64, .os_tag = .macos },
        .{ .cpu_arch = .aarch64, .os_tag = .macos },
    };

    for (targets) |t| {
        const target = b.resolveTargetQuery(t);
        const exe = b.addExecutable(.{
            .name = b.fmt("app_{s}_{s}", .{
                @tagName(t.cpu_arch.?),
                @tagName(t.os_tag.?),
            }),
            .root_source_file = b.path("src/main.zig"),
            .target = target,
            .optimize = .ReleaseSmall,
        });
        b.installArtifact(exe);
    }
}
