//
// Auto-generated from: Zig\README.md
// Line: 1506
// Language: zig
// Block ID: ad167d2b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 中断向量表
export const vector_table linksection(".vectors") = [_]?*const fn () callconv(.C) void{
    @ptrCast(&stack_top),    // 初始栈顶
    _start,                  // 复位处理
    default_handler,         // NMI
    hard_fault_handler,      // Hard Fault
    // ...
};

export fn TIM2_IRQHandler() callconv(.C) void {
    // 清除中断标志
    TIM2.SR.* &= ~@as(u32, 1);

    // 处理定时器中断
    tick_counter += 1;
}

var tick_counter: u32 = 0;
