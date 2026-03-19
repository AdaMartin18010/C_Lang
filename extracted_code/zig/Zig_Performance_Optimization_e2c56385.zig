//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 750
// Language: zig
// Block ID: e2c56385
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig
const exe = b.addExecutable(.{
    .name = "app",
    .root_source_file = .{ .path = "src/main.zig" },
    .target = target,
    .optimize = .ReleaseFast,
});

// 启用 LTO
exe.want_lto = true;
