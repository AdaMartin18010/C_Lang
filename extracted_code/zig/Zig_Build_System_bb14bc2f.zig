//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 307
// Language: zig
// Block ID: bb14bc2f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// ARM Linux
const arm_linux = b.resolveTargetQuery(.{
    .cpu_arch = .arm,
    .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_a9 },
    .os_tag = .linux,
    .abi = .gnueabihf,
});

// RISC-V 64
const riscv64 = b.resolveTargetQuery(.{
    .cpu_arch = .riscv64,
    .os_tag = .linux,
    .abi = .gnu,
});

// macOS
const macos = b.resolveTargetQuery(.{
    .cpu_arch = .aarch64,
    .os_tag = .macos,
    .os_version_min = .{ .semver = .{ .major = 11, .minor = 0, .patch = 0 } },
});

// WASM
const wasm = b.resolveTargetQuery(.{
    .cpu_arch = .wasm32,
    .os_tag = .freestanding,
});
