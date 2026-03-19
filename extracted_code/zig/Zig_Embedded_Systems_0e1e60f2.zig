//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 363
// Language: zig
// Block ID: 0e1e60f2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// gpio.zig - STM32 GPIO 驱动

const mmio = @import("mmio.zig");

// GPIO 寄存器基址
const GPIOA_BASE = 0x40020000;
const GPIOB_BASE = 0x40020400;
const GPIOC_BASE = 0x40020800;

// 寄存器偏移
const MODER_OFFSET = 0x00;
const OTYPER_OFFSET = 0x04;
const OSPEEDR_OFFSET = 0x08;
const PUPDR_OFFSET = 0x0C;
const IDR_OFFSET = 0x10;
const ODR_OFFSET = 0x14;
const BSRR_OFFSET = 0x18;

pub const Port = enum {
    A, B, C, D, E, F, G, H, I,

    fn base(self: Port) usize {
        return switch (self) {
            .A => GPIOA_BASE,
            .B => GPIOB_BASE,
            .C => GPIOC_BASE,
            else => @panic("TODO"),
        };
    }
};

pub const Mode = enum(u2) {
    input = 0b00,
    output = 0b01,
    af = 0b10,
    analog = 0b11,
};

pub const Speed = enum(u2) {
    low = 0b00,
    medium = 0b01,
    high = 0b10,
    very_high = 0b11,
};

pub const Pull = enum(u2) {
    none = 0b00,
    up = 0b01,
    down = 0b10,
};

pub const Pin = struct {
    port: Port,
    number: u4,

    pub fn init(port: Port, number: u4) Pin {
        return .{ .port = port, .number = number };
    }

    pub fn set_mode(self: Pin, mode: Mode) void {
        const base = self.port.base();
        const shift = @as(u5, self.number) * 2;
        mmio.modify_volatile(u32, base + MODER_OFFSET, @as(u32, 0b11) << shift, @intFromEnum(mode) << shift);
    }

    pub fn set_speed(self: Pin, speed: Speed) void {
        const base = self.port.base();
        const shift = @as(u5, self.number) * 2;
        mmio.modify_volatile(u32, base + OSPEEDR_OFFSET, @as(u32, 0b11) << shift, @intFromEnum(speed) << shift);
    }

    pub fn set_pull(self: Pin, pull: Pull) void {
        const base = self.port.base();
        const shift = @as(u5, self.number) * 2;
        mmio.modify_volatile(u32, base + PUPDR_OFFSET, @as(u32, 0b11) << shift, @intFromEnum(pull) << shift);
    }

    pub fn write(self: Pin, value: bool) void {
        const base = self.port.base();
        const bit_num = @as(u5, self.number);
        if (value) {
            mmio.write_volatile(u32, base + BSRR_OFFSET, @as(u32, 1) << bit_num);
        } else {
            mmio.write_volatile(u32, base + BSRR_OFFSET, @as(u32, 1) << (bit_num + 16));
        }
    }

    pub fn read(self: Pin) bool {
        const base = self.port.base();
        const idr = mmio.read_volatile(u32, base + IDR_OFFSET);
        return (idr >> @as(u5, self.number)) & 1 == 1;
    }

    pub fn toggle(self: Pin) void {
        self.write(!self.read());
    }
};

// 便捷函数
pub fn configure_output(pin: Pin) void {
    pin.set_mode(.output);
    pin.set_speed(.high);
    pin.set_pull(.none);
}

pub fn configure_input(pin: Pin, pull: Pull) void {
    pin.set_mode(.input);
    pin.set_pull(pull);
}
