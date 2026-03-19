//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 323
// Language: zig
// Block ID: d85342c8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 高级构建配置
pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 条件编译选项
    const enable_tracing = b.option(bool, "tracing", "Enable tracing") orelse false;
    const backend = b.option(Backend, "backend", "Select backend") orelse .default;

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    // 添加构建选项
    const options = b.addOptions();
    options.addOption(bool, "enable_tracing", enable_tracing);
    options.addOption(Backend, "backend", backend);
    exe.root_module.addOptions("build_options", options);

    // 并行构建配置
    exe.use_lld = true;  // 使用lld链接器
    exe.want_lto = optimize == .ReleaseFast;

    b.installArtifact(exe);
}
