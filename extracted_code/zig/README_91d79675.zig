//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 122
// Language: zig
// Block ID: 91d79675
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 增量编译缓存配置
// build.zig

pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = b.path("src/main.zig"),
        .target = b.standardTargetOptions(.{}),
        .optimize = b.standardOptimizeOption(.{}),
    });

    // 启用增量编译
    exe.use_llvm = false;  // 使用自研后端获得更快增量编译
    exe.enable_cache = true;

    b.installArtifact(exe);
}
