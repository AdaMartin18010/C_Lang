//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 110
// Language: zig
// Block ID: b62e844c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// startup.zig - ARM Cortex-M 启动代码
const std = @import("std");

// 链接脚本导出的符号
extern const _stack_top: u32;
extern const _text_start: u32;
extern const _text_end: u32;
extern const _data_load: u32;
extern const _data_start: u32;
extern const _data_end: u32;
extern const _bss_start: u32;
extern const _bss_end: u32;

// 主函数入口 (用户实现)
extern fn main() noreturn;

// 默认中断处理
fn default_handler() callconv(.C) void {
    while (true) {
        asm volatile ("wfi");
    }
}

// 系统调用处理
fn svcall_handler() callconv(.C) void {
    // 处理 SVC
}

// 中断向量表
export const vector_table linksection(".vectors") = [_]fn () callconv(.C) void{
    @ptrCast(&_stack_top),  // 栈顶指针
    reset_handler,           // 复位
    default_handler,         // NMI
    default_handler,         // HardFault
    default_handler,         // MemManage
    default_handler,         // BusFault
    default_handler,         // UsageFault
    default_handler,         // 保留
    default_handler,         // 保留
    default_handler,         // 保留
    default_handler,         // 保留
    svcall_handler,          // SVCall
    default_handler,         // DebugMonitor
    default_handler,         // 保留
    default_handler,         // PendSV
    default_handler,         // SysTick
    // 外设中断...
};

// 复位处理
export fn reset_handler() callconv(.C) noreturn {
    // 1. 复制 .data 从 Flash 到 RAM
    const data_src = @as([*]const u32, @ptrCast(&_data_load));
    const data_dst = @as([*]u32, @ptrCast(&_data_start));
    const data_size = @intFromPtr(&_data_end) - @intFromPtr(&_data_start);

    for (0..data_size / 4) |i| {
        data_dst[i] = data_src[i];
    }

    // 2. 清零 .bss
    const bss_dst = @as([*]u32, @ptrCast(&_bss_start));
    const bss_size = @intFromPtr(&_bss_end) - @intFromPtr(&_bss_start);

    for (0..bss_size / 4) |i| {
        bss_dst[i] = 0;
    }

    // 3. 设置时钟 (可选)
    system_init();

    // 4. 跳转到主程序
    main();
}

fn system_init() void {
    // 初始化时钟系统
    // 配置 PLL
    // 设置 Flash 等待状态
}
