//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 676
// Language: zig
// Block ID: 04fb632f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 创建不同构建变体

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});

    // 调试版本
    const debug_exe = b.addExecutable(.{
        .name = "app-debug",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = .Debug,
    });

    const debug_options = b.addOptions();
    debug_options.addOption(bool, "enable_asserts", true);
    debug_options.addOption(bool, "enable_logging", true);
    debug_exe.addOptions("build_config", debug_options);

    // 发布版本
    const release_exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = .ReleaseFast,
    });

    const release_options = b.addOptions();
    release_options.addOption(bool, "enable_asserts", false);
    release_options.addOption(bool, "enable_logging", false);
    release_exe.addOptions("build_config", release_options);

    b.installArtifact(debug_exe);
    b.installArtifact(release_exe);
}
