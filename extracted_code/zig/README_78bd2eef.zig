//
// Auto-generated from: Zig\README.md
// Line: 1454
// Language: zig
// Block ID: 78bd2eef
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// STM32 GPIO 示例
const GPIOA_BASE = 0x4800_0000;
const GPIOA_MODER = @as(*volatile u32, @ptrFromInt(GPIOA_BASE + 0x00));
const GPIOA_ODR = @as(*volatile u32, @ptrFromInt(GPIOA_BASE + 0x14));

pub export fn main() void {
    // 配置 PA5 为输出
    GPIOA_MODER.* &= ~(0b11 << 10);
    GPIOA_MODER.* |= (0b01 << 10);

    // 闪烁 LED
    while (true) {
        GPIOA_ODR.* ^= (1 << 5);
        delay(500_000);
    }
}

fn delay(count: u32) void {
    var i: u32 = 0;
    while (i < count) : (i += 1) {
        asm volatile ("nop");
    }
}
