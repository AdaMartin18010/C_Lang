//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 195
// Language: zig
// Block ID: ef3509e8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// riscv_startup.zig

export fn _start() linksection(".text.boot") callconv(.Naked) noreturn {
    asm volatile (
        \\la sp, _stack_top
        \\la t0, _bss_start
        \\la t1, _bss_end
        \\1:
        \\bgeu t0, t1, 2f
        \\sw zero, 0(t0)
        \\addi t0, t0, 4
        \\j 1b
        \\2:
        \\call main
        \\3:
        \\wfi
        \\j 3b
    );
    unreachable;
}
