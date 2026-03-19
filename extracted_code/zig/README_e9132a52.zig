//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 311
// Language: zig
// Block ID: e9132a52
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 保存和恢复浮点寄存器状态
export fn save_fp_state() void {
    var state: [64]u8 align(16) = undefined;  // FXSAVE 缓冲区
    asm volatile (
        \\ fxsave (%[buf])
        :
        : [buf] "r" (&state)
        : "memory"
    );

    // 执行可能修改 FP 寄存器的 C 代码...

    asm volatile (
        \\ fxrstor (%[buf])
        :
        : [buf] "r" (&state)
        : "memory"
    );
}
