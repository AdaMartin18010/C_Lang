//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 477
// Language: zig
// Block ID: 324ef027
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// uart.zig - 串口驱动

const mmio = @import("mmio.zig");

const USART1_BASE = 0x40011000;

const SR_OFFSET = 0x00;
const DR_OFFSET = 0x04;
const BRR_OFFSET = 0x08;
const CR1_OFFSET = 0x0C;

pub const UART = struct {
    base: usize,

    pub fn init(base: usize) UART {
        return .{ .base = base };
    }

    pub fn configure(self: UART, baudrate: u32, clock_freq: u32) void {
        // 计算波特率
        const div = (clock_freq + baudrate / 2) / baudrate;
        mmio.write_volatile(u32, self.base + BRR_OFFSET, div);

        // 使能 TX, RX, UART
        const cr1 = 0x0C | 0x2000;  // TE + RE + UE
        mmio.write_volatile(u32, self.base + CR1_OFFSET, cr1);
    }

    pub fn write_byte(self: UART, byte: u8) void {
        // 等待发送寄存器空
        while (mmio.read_volatile(u32, self.base + SR_OFFSET) & 0x80 == 0) {}

        mmio.write_volatile(u32, self.base + DR_OFFSET, byte);
    }

    pub fn read_byte(self: UART) ?u8 {
        // 检查接收数据
        if (mmio.read_volatile(u32, self.base + SR_OFFSET) & 0x20 != 0) {
            return @truncate(mmio.read_volatile(u32, self.base + DR_OFFSET));
        }
        return null;
    }

    pub fn write_string(self: UART, s: []const u8) void {
        for (s) |c| {
            self.write_byte(c);
        }
    }

    pub fn format(self: UART, comptime fmt: []const u8, args: anytype) void {
        var buf: [256]u8 = undefined;
        const printed = std.fmt.bufPrint(&buf, fmt, args) catch return;
        self.write_string(printed);
    }
};

// 全局 UART 实例
pub const uart1 = UART.init(USART1_BASE);
