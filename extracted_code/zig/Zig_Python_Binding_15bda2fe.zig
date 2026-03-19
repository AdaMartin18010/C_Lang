//
// Auto-generated from: Zig\Zig_Python_Binding.md
// Line: 140
// Language: zig
// Block ID: 15bda2fe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // 获取 Python 配置
    const python_cflags = b.run(&.{"python3-config", "--cflags"});
    const python_ldflags = b.run(&.{"python3-config", "--ldflags"});

    // 创建共享库 (.so / .pyd)
    const lib = b.addSharedLibrary(.{
        .name = "mymodule",
        .root_source_file = .{ .path = "src/mymodule.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 链接 Python
    lib.linkLibC();
    lib.addIncludePath(.{ .path = "/usr/include/python3.11" });

    // 添加链接选项
    var it = std.mem.split(u8, python_ldflags, " ");
    while (it.next()) |flag| {
        if (flag.len > 2 and flag[0] == '-' and flag[1] == 'l') {
            lib.linkSystemLibrary(flag[2..]);
        }
    }

    b.installArtifact(lib);
}
