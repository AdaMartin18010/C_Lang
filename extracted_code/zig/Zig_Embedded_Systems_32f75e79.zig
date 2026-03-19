//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 903
// Language: zig
// Block ID: 32f75e79
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// itm.zig - Instrumentation Trace Macrocell

const ITM_BASE = 0xE0000000;
const ITM_PORT = ITM_BASE + 0x000;

pub fn init() void {
    // 启用 ITM
    const DEMCR = 0xE000EDFC;
    mmio.write_volatile(u32, DEMCR, mmio.read_volatile(u32, DEMCR) | 0x01000000);

    // 解锁 ITM
    const ITM_LAR = ITM_BASE + 0xFB0;
    mmio.write_volatile(u32, ITM_LAR, 0xC5ACCE55);

    // 启用端口 0
    const ITM_TER0 = ITM_BASE + 0xE00;
    mmio.write_volatile(u32, ITM_TER0, 0x01);

    const ITM_TCR = ITM_BASE + 0xE80;
    mmio.write_volatile(u32, ITM_TCR, 0x00010001);
}

pub fn write_char(c: u8) void {
    const port = ITM_PORT;
    while (mmio.read_volatile(u32, port) == 0) {}
    mmio.write_volatile(u8, port, c);
}

pub fn print(s: []const u8) void {
    for (s) |c| {
        write_char(c);
    }
}
