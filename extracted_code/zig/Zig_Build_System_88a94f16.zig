//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 339
// Language: zig
// Block ID: 88a94f16
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// ARM Cortex-M4 (STM32F4)
const cortex_m4 = b.resolveTargetQuery(.{
    .cpu_arch = .thumb,
    .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_m4 },
    .os_tag = .freestanding,
    .abi = .none,
});

const embedded_exe = b.addExecutable(.{
    .name = "firmware.elf",
    .root_source_file = .{ .path = "src/main.zig" },
    .target = cortex_m4,
    .optimize = .ReleaseSmall,
});

// 链接脚本
embedded_exe.setLinkerScript(.{ .path = "linker.ld" });

// 入口点
embedded_exe.entry = .{ .symbol_name = "_start" };

// 生成二进制文件
const bin = b.addObjCopy(embedded_exe.getEmittedBin(), .{
    .format = .bin,
});
b.installArtifact(bin);
