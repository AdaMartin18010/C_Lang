//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 765
// Language: zig
// Block ID: 0095e33c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// pico_main.zig - RP2040 驱动

const mmio = @import("mmio.zig");

// RP2040 寄存器基址
const SIO_BASE = 0xD0000000;
const GPIO_OE = SIO_BASE + 0x020;
const GPIO_OUT = SIO_BASE + 0x014;
const GPIO_OUT_XOR = SIO_BASE + 0x01C;

const RESETS_BASE = 0x4000C000;

fn reset_peripheral(peripheral: u32) void {
    const RESET = RESETS_BASE + 0x00;
    const RESET_DONE = RESETS_BASE + 0x08;

    // 复位
    mmio.modify_volatile(u32, RESET, peripheral, peripheral);

    // 等待复位完成
    while (mmio.read_volatile(u32, RESET_DONE) & peripheral == 0) {}
}

pub export fn main() noreturn {
    // 复位 IO _BANK0
    reset_peripheral(1 << 5);

    // 配置 GPIO25 (板载 LED)
    const GPIO25_CTRL = 0x40014000 + 0x0CC;
    mmio.write_volatile(u32, GPIO25_CTRL, 5);  // SIO 功能

    // 设置为输出
    mmio.write_volatile(u32, GPIO_OE, mmio.read_volatile(u32, GPIO_OE) | (1 << 25));

    // 闪烁 LED
    while (true) {
        // 翻转输出
        mmio.write_volatile(u32, GPIO_OUT_XOR, 1 << 25);

        // 延时
        var delay: u32 = 0;
        while (delay < 500000) : (delay += 1) {
            asm volatile ("nop");
        }
    }
}
