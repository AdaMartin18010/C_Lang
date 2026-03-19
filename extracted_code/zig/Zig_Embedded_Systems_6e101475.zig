//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 682
// Language: zig
// Block ID: 6e101475
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// main.zig - STM32F4 LED 闪烁

const std = @import("std");
const gpio = @import("gpio.zig");
const uart = @import("uart.zig");
const nvic = @import("nvic.zig");

// 系统时钟配置
fn system_clock_config() void {
    // 启用 HSE
    const RCC_CR = 0x40023800;
    mmio.write_volatile(u32, RCC_CR, mmio.read_volatile(u32, RCC_CR) | 0x00010000);

    // 等待 HSE 就绪
    while (mmio.read_volatile(u32, RCC_CR) & 0x00020000 == 0) {}

    // 配置 PLL
    const RCC_PLLCFGR = 0x40023804;
    mmio.write_volatile(u32, RCC_PLLCFGR, 0x27405408);

    // 启用 PLL
    mmio.write_volatile(u32, RCC_CR, mmio.read_volatile(u32, RCC_CR) | 0x01000000);

    // 等待 PLL 就绪
    while (mmio.read_volatile(u32, RCC_CR) & 0x02000000 == 0) {}

    // 切换系统时钟到 PLL
    const RCC_CFGR = 0x40023808;
    mmio.modify_volatile(u32, RCC_CFGR, 0b11, 0b10);

    // 等待切换完成
    while ((mmio.read_volatile(u32, RCC_CFGR) >> 2) & 0b11 != 0b10) {}

    // 更新 SystemCoreClock 变量
    SystemCoreClock = 168000000;
}

var SystemCoreClock: u32 = 16000000;

pub export fn main() noreturn {
    // 配置系统时钟
    system_clock_config();

    // 启用 GPIOA 时钟
    const RCC_AHB1ENR = 0x40023830;
    mmio.write_volatile(u32, RCC_AHB1ENR, mmio.read_volatile(u32, RCC_AHB1ENR) | 0x01);

    // 配置 PA5 (LED) 为输出
    const led = gpio.Pin.init(.A, 5);
    gpio.configure_output(led);

    // 配置串口用于调试
    uart.uart1.configure(115200, SystemCoreClock);
    uart.uart1.write_string("STM32 Zig Boot!\r\n");

    // 主循环
    var counter: u32 = 0;
    while (true) {
        led.toggle();

        uart.uart1.format("Tick: {}\r\n", .{counter});
        counter += 1;

        delay_ms(500);
    }
}

fn delay_ms(ms: u32) void {
    // 简单忙等待
    const cycles_per_ms = SystemCoreClock / 1000;
    for (0..ms) |_| {
        for (0..cycles_per_ms / 3) |_| {
            asm volatile ("nop");
        }
    }
}
