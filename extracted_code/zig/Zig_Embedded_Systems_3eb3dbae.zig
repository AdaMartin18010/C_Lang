//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 941
// Language: zig
// Block ID: 3eb3dbae
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub fn assert(condition: bool, msg: []const u8) void {
    if (!condition) {
        uart.uart1.write_string("ASSERTION FAILED: ");
        uart.uart1.write_string(msg);
        uart.uart1.write_string("\r\n");

        // 触发断点
        asm volatile ("bkpt #0");

        // 或者重启
        const AIRCR = 0xE000ED0C;
        mmio.write_volatile(u32, AIRCR, 0x05FA0004);

        while (true) {}
    }
}

pub fn check_stack_overflow() void {
    // 检查栈底魔法值
    const stack_bottom: *volatile u32 = @ptrFromInt(0x20000000);
    if (stack_bottom.* != 0xDEADBEEF) {
        @panic("Stack overflow detected!");
    }
}
