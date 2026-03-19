//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 715
// Language: zig
// Block ID: 42cd07fb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 集成外部工具

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 运行静态分析
    const lint = b.addSystemCommand(&.{
        "zig", "fmt", "--check", "src/",
    });
    exe.step.dependOn(&lint.step);

    // 生成覆盖率报告
    const coverage = b.step("coverage", "Generate coverage report");
    const kcov = b.addSystemCommand(&.{
        "kcov", "--clean", "--include-path=src", "coverage/",
    });
    const test_exe = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
    });
    kcov.addArtifactArg(test_exe);
    coverage.dependOn(&kcov.step);

    b.installArtifact(exe);
}
