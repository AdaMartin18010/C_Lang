//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 217
// Language: zig
// Block ID: fb8f6145
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig

const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "app",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 链接 C 标准库
    exe.linkLibC();

    // 链接系统库
    exe.linkSystemLibrary("sqlite3");
    exe.linkSystemLibrary("ssl");
    exe.linkSystemLibrary("crypto");

    // 链接静态库
    exe.addObjectFile(.{ .path = "libs/libcustom.a" });

    // 链接动态库
    exe.addObjectFile(.{ .path = "libs/libcustom.so" });

    // 库搜索路径
    exe.addLibraryPath(.{ .path = "/usr/local/lib" });
    exe.addLibraryPath(.{ .path = "libs" });

    // 包含路径
    exe.addIncludePath(.{ .path = "/usr/local/include" });
    exe.addIncludePath(.{ .path = "include" });

    // 链接选项
    exe.linkSystemLibrary("pthread");
    exe.linkSystemLibrary("m");  // 数学库

    b.installArtifact(exe);
}
