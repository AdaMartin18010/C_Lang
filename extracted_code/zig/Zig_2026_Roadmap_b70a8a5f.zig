//
// Auto-generated from: Zig\2026_latest\Zig_2026_Roadmap.md
// Line: 342
// Language: zig
// Block ID: b70a8a5f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026: build.zig 新特性

const std = @import("std");

pub fn build(b: *std.Build) void {
    // 1. 自动依赖管理
    const dep = b.dependency("zap", .{
        .version = "^2.0.0",
        .features = &.{"ssl", "compression"},
    });

    // 2. 条件编译配置
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 3. 工作区支持 (多包管理)
    const workspace = b.createWorkspace(.{
        .name = "my_project",
        .members = &.{
            "crates/core",
            "crates/server",
            "crates/client",
        },
    });

    // 4. 自定义构建步骤
    const test_step = b.step("test", "Run all tests");
    test_step.dependOn(&workspace.member("core").test_step);
    test_step.dependOn(&workspace.member("server").test_step);

    // 5. 代码生成集成
    const generated = b.addGenerated(.{
        .generator = "src/gen_protocol.zig",
        .inputs = &.{
            "protocols/api.proto",
            "protocols/events.proto",
        },
        .output = "src/generated/",
    });

    // 6. 发布配置
    const release = b.addRelease(.{
        .name = "v1.0.0",
        .artifacts = &.{
            exe,
            lib,
        },
        .targets = &.{
            "x86_64-linux",
            "x86_64-windows",
            "aarch64-macos",
        },
    });
}
