//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 526
// Language: zig
// Block ID: 2f107c4e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// idt.zig - 中断描述符表
const std = @import("std");

const IDTEntry = packed struct {
    offset_low: u16,
    selector: u16,
    ist: u3,
    zero: u5 = 0,
    type: u4,
    zero2: u1 = 0,
    dpl: u2,
    present: u1,
    offset_mid: u16,
    offset_high: u32,
    reserved: u32 = 0,
};

var idt: [256]IDTEntry align(16) = undefined;

pub fn init() void {
    @memset(&idt, std.mem.zeroes(IDTEntry));

    // 设置异常处理程序
    setHandler(0, divideByZero);
    setHandler(14, pageFault);

    // 加载 IDT
    const idtr = packed struct { limit: u16, base: u64 }{
        .limit = @sizeOf(@TypeOf(idt)) - 1,
        .base = @intFromPtr(&idt),
    };

    asm volatile ("lidt %[idtr]"
        :
        : [idtr] "m" (idtr),
    );
}

fn setHandler(vector: u8, handler: *const fn () callconv(.Naked) void) void {
    const handler_addr = @intFromPtr(handler);
    idt[vector] = .{
        .offset_low = @truncate(handler_addr),
        .offset_mid = @truncate(handler_addr >> 16),
        .offset_high = @truncate(handler_addr >> 32),
        .selector = 0x08,
        .ist = 0,
        .type = 0xE,
        .dpl = 0,
        .present = 1,
    };
}

export fn divideByZero() callconv(.Naked) noreturn {
    asm volatile (
        \\cli
        \\hlt
    );
    unreachable;
}
