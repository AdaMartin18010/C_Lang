//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 594
// Language: zig
// Block ID: 28ffa837
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// context.zig - 上下文切换
const std = @import("std");

pub const Context = extern struct {
    r15: u64, r14: u64, r13: u64, r12: u64,
    r11: u64, r10: u64, r9: u64, r8: u64,
    rbp: u64, rdi: u64, rsi: u64, rdx: u64,
    rcx: u64, rbx: u64, rax: u64,
    rip: u64, cs: u64, rflags: u64,
    rsp: u64, ss: u64,
};

export fn contextSwitch(prev: **Context, next: *Context) void {
    asm volatile (
        \\push %%rax
        \\push %%rbx
        \\push %%r12
        \\push %%r13
        \\push %%r14
        \\push %%r15
        \\mov %%rsp, (%[prev])
        \\mov %[next], %%rsp
        \\pop %%r15
        \\pop %%r14
        \\pop %%r13
        \\pop %%r12
        \\pop %%rbx
        \\pop %%rax
        \\ret
        :
        : [prev] "r" (prev),
          [next] "r" (next),
        : "memory"
    );
}
