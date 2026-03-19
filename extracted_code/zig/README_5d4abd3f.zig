//
// Auto-generated from: Zig\README.md
// Line: 1482
// Language: zig
// Block ID: 5d4abd3f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用 packed struct 定义寄存器
const UartRegs = packed struct {
    data: u8,
    _padding1: [3]u8,
    status: packed struct {
        tx_ready: u1,
        rx_ready: u1,
        _reserved: u6,
    },
    _padding2: [3]u8,
    control: u8,
};

const UART0 = @as(*volatile UartRegs, @ptrFromInt(0x4000_1000));

pub fn uart_putc(c: u8) void {
    while (UART0.status.tx_ready == 0) {}
    UART0.data = c;
}
