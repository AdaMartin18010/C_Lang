//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 255
// Language: zig
// Block ID: b6e2944a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig - 测试配置

/// 运行 C 测试套件
const c_tests = b.addExecutable(.{
    .name = "c_tests",
    .target = target,
    .optimize = optimize,
});
c_tests.addCSourceFile(.{
    .file = b.path("tests/c_tests/runner.c"),
    .flags = &.{"-DTEST_ZIG_INTEROP"},
});
c_tests.linkLibrary(c_lib);

/// Zig 集成测试
const integration_tests = b.addTest(.{
    .root_source_file = b.path("tests/integration/main.zig"),
    .target = target,
    .optimize = optimize,
});
integration_tests.linkLibrary(c_lib);

const run_integration = b.addRunArtifact(integration_tests);
const test_step = b.step("integration-test", "运行集成测试");
test_step.dependOn(&run_integration.step);
