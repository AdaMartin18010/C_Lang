//
// Auto-generated from: Zig\2026_latest\Zig_2026_Roadmap.md
// Line: 97
// Language: zig
// Block ID: 46630d89
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026 新特性: 细粒度增量编译
// 只重新编译变化的部分，大幅提升大项目构建速度

// build.zig 配置
pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "myapp",
        .root_source_file = b.path("src/main.zig"),
        // 启用增量编译 (默认开启)
        .incremental = true,
    });

    // 编译缓存配置
    exe.use_lld = true;  // 使用 LLD 链接器
    exe.cache_root = ".zig-cache";
}
