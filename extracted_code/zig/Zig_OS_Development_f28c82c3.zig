//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 299
// Language: zig
// Block ID: f28c82c3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// raspi4.zig - Raspberry Pi 4 特定代码
const std = @import("std");

// MMIO 基址 (Raspberry Pi 4)
const MMIO_BASE = 0xFE000000;

// Mini UART 寄存器
const AUX_ENABLES = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215004));
const AUX_MU_IO = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215040));
const AUX_MU_LSR = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215054));
const AUX_MU_CNTL = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215060));
const AUX_MU_BAUD = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215068));

pub fn uartInit() void {
    AUX_ENABLES.* = 1;
    AUX_MU_CNTL.* = 0;
    AUX_MU_BAUD.* = 270; // 115200 @ 500MHz
    AUX_MU_CNTL.* = 3;   // 使能 TX/RX
}

pub fn uartWrite(ch: u8) void {
    while ((AUX_MU_LSR.* & 0x20) == 0) {}
    AUX_MU_IO.* = ch;
}

export fn kernel_main() callconv(.C) noreturn {
    uartInit();
    uartWrite('H');
    uartWrite('i');

    while (true) {
        asm volatile ("wfi");
    }
}
