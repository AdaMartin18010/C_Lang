//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 550
// Language: zig
// Block ID: afa049cf
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    // 可用的目标平台
    const targets: []const std.Target.Query = &.{
        .{ // Linux x86_64
            .cpu_arch = .x86_64,
            .os_tag = .linux,
            .abi = .gnu,
        },
        .{ // Linux ARM64
            .cpu_arch = .aarch64,
            .os_tag = .linux,
            .abi = .gnu,
        },
        .{ // macOS x86_64
            .cpu_arch = .x86_64,
            .os_tag = .macos,
        },
        .{ // macOS ARM64
            .cpu_arch = .aarch64,
            .os_tag = .macos,
        },
        .{ // Windows x86_64
            .cpu_arch = .x86_64,
            .os_tag = .windows,
            .abi = .gnu,
        },
        .{ // WASM
            .cpu_arch = .wasm32,
            .os_tag = .freestanding,
        },
    };

    // 为每个目标构建
    for (targets, 0..) |target_query, i| {
        const target = b.resolveTargetQuery(target_query);
        const optimize = .ReleaseFast;

        const exe_name = b.fmt("app_{}", .{i});
        const exe = b.addExecutable(.{
            .name = exe_name,
            .root_source_file = .{ .path = "src/main.zig" },
            .target = target,
            .optimize = optimize,
        });

        // 平台特定配置
        if (target_query.os_tag == .windows) {
            exe.subsystem = .Windows;
        }

        // 安装
        const install = b.addInstallArtifact(exe, .{
            .dest_dir = .{ .override = .{ .custom = b.fmt("bin/{s}", .{@tagName(target_query.os_tag)}) } },
        });
        b.getInstallStep().dependOn(&install.step);
    }
}
