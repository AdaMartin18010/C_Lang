//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 613
// Language: zig
// Block ID: 6f7562af
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    // ... 其他配置

    // 单元测试
    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    // 集成测试
    const integration_tests = b.addTest(.{
        .root_source_file = .{ .path = "tests/integration_tests.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_integration_tests = b.addRunArtifact(integration_tests);

    // 测试步骤
    const test_step = b.step("test", "Run all tests");
    test_step.dependOn(&run_unit_tests.step);
    test_step.dependOn(&run_integration_tests.step);

    // 覆盖率测试 (使用 kcov)
    const coverage_step = b.step("coverage", "Generate coverage report");
    const kcov = b.addSystemCommand(&.{
        "kcov", "--clean",
        "--include-path=src",
        "coverage/",
    });
    kcov.addArtifactArg(unit_tests);
    coverage_step.dependOn(&kcov.step);
}
