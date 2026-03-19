//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 678
// Language: zig
// Block ID: 52153a37
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// syscall.zig - 系统调用处理
pub const Syscall = enum(usize) {
    exit = 0,
    fork = 1,
    read = 6,
    write = 7,
};

export fn syscallHandler() callconv(.Naked) void {
    asm volatile (
        \\swapgs
        \\mov %%rsp, %%gs:8
        \\mov %%gs:0, %%rsp
        \\call dispatchSyscall
        \\mov %%gs:8, %%rsp
        \\swapgs
        \\sysretq
    );
}

export fn dispatchSyscall(number: usize, arg1: usize, arg2: usize) usize {
    const syscall = @as(Syscall, @enumFromInt(number));
    return switch (syscall) {
        .exit => 0,
        .write => arg1 + arg2,
        else => 0,
    };
}
