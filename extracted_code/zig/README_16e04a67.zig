//
// Auto-generated from: Zig\README.md
// Line: 1374
// Language: zig
// Block ID: 16e04a67
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 裸机入口点
export fn _start() callconv(.Naked) noreturn {
    // 初始化栈
    asm volatile (
        \\ mov rsp, %[stack_top]
        :
        : [stack_top] "r" (@intFromPtr(&stack) + stack.len),
    );

    main();

    // 系统调用退出
    asm volatile (
        \\ mov rax, 60
        \\ xor rdi, rdi
        \\ syscall
    );
    unreachable;
}

var stack: [16 * 1024]u8 align(16) = undefined;

fn main() void {
    // 内核主函数
}
