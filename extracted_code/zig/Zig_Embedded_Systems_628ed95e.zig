//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 266
// Language: zig
// Block ID: 628ed95e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .thumb,
        .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_m4 },
        .os_tag = .freestanding,
        .abi = .none,
    });

    const optimize = b.standardOptimizeOption(.{});

    const elf = b.addExecutable(.{
        .name = "firmware.elf",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 链接脚本
    elf.setLinkerScript(.{ .path = "memory.ld" });

    // 无默认入口
    elf.entry = .{ .symbol_name = "_start" };

    // 生成二进制文件
    const bin = b.addObjCopy(elf.getEmittedBin(), .{
        .format = .bin,
    });
    bin.step.dependOn(&elf.step);

    b.getInstallStep().dependOn(&bin.step);

    // 安装二进制文件
    b.installArtifact(elf);

    // 生成 hex 文件
    const hex = b.addObjCopy(elf.getEmittedBin(), .{
        .format = .hex,
    });
    hex.step.dependOn(&elf.step);

    // 调试目标
    const debug_cmd = b.addSystemCommand(&.{
        "gdb-multiarch",
        "-ex", "target remote localhost:3333",
        "-ex", "monitor reset halt",
        "-ex", "load",
        "-ex", "continue",
    });
    debug_cmd.addFileArg(elf.getEmittedBin());

    const debug_step = b.step("debug", "Start GDB debug session");
    debug_step.dependOn(&debug_cmd.step);
}
