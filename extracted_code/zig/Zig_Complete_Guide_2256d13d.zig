//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1713
// Language: zig
// Block ID: 2256d13d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// kernel.zig - 简化版操作系统内核
// 目标: x86_64, bare metal

const std = @import("std");

// 内核入口点
export fn _start() callconv(.Naked) noreturn {
    asm volatile (
        \\mov $0x200000, %rsp  // 设置栈指针
        \\call kernel_main
        \\cli
        \\hlt
    );
    unreachable;
}

// VGA 文本模式输出
const VGA_BUFFER: [*]volatile u16 = @ptrFromInt(0xB8000);
const VGA_WIDTH = 80;
const VGA_HEIGHT = 25;

var cursor_x: usize = 0;
var cursor_y: usize = 0;

fn vga_putchar(c: u8) void {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 1;
    } else {
        const index = cursor_y * VGA_WIDTH + cursor_x;
        VGA_BUFFER[index] = vga_entry(c, 0x0F);  // 白字黑底
        cursor_x += 1;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y += 1;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll();
    }
}

fn vga_entry(c: u8, color: u8) u16 {
    return @as(u16, c) | (@as(u16, color) << 8);
}

fn scroll() void {
    // 简化实现
    cursor_y = VGA_HEIGHT - 1;
}

fn print(s: []const u8) void {
    for (s) |c| {
        vga_putchar(c);
    }
}

// 中断处理
const IDTEntry = packed struct {
    offset_low: u16,
    selector: u16,
    ist: u8,
    type_attr: u8,
    offset_mid: u16,
    offset_high: u32,
    reserved: u32,
};

var idt: [256]IDTEntry = undefined;

fn init_idt() void {
    const idt_descriptor = packed struct {
        size: u16,
        offset: u64,
    }{
        .size = @sizeOf(@TypeOf(idt)) - 1,
        .offset = @intFromPtr(&idt),
    };

    // 加载 IDT
    asm volatile ("lidt (%[desc])"
        :
        : [desc] "r" (&idt_descriptor),
    );
}

// 内存管理
const PAGE_SIZE = 4096;

fn init_memory() void {
    // 简单的页表设置
    // ...
}

// 调度器
const Task = struct {
    id: u32,
    stack: [1024]u8,
    context: Context,
    state: TaskState,
};

const TaskState = enum {
    ready,
    running,
    blocked,
};

const Context = struct {
    rsp: u64,
    rbp: u64,
    rip: u64,
};

var tasks: [10]Task = undefined;
var current_task: *Task = undefined;
var task_count: u32 = 0;

fn scheduler_init() void {
    // 初始化第一个任务 (当前)
    tasks[0] = Task{
        .id = 0,
        .stack = undefined,
        .context = undefined,
        .state = .running,
    };
    current_task = &tasks[0];
    task_count = 1;
}

fn schedule() void {
    // 轮询调度
    const next_id = (current_task.id + 1) % task_count;
    const next = &tasks[next_id];

    if (next.state == .ready) {
        const prev = current_task;
        current_task = next;
        switch_context(&prev.context, &next.context);
    }
}

fn switch_context(old: *Context, new: *const Context) void {
    // 汇编实现上下文切换
    asm volatile (
        \\push %rbp
        \\push %rbx
        \\push %r12
        \\push %r13
        \\push %r14
        \\push %r15
        \\mov %rsp, (%[old_rsp])
        \\mov (%[new_rsp]), %rsp
        \\pop %r15
        \\pop %r14
        \\pop %r13
        \\pop %r12
        \\pop %rbx
        \\pop %rbp
        \\ret
        :
        : [old_rsp] "r" (&old.rsp),
          [new_rsp] "r" (&new.rsp),
    );
}

// 系统调用
export fn syscall_handler() callconv(.Naked) void {
    asm volatile (
        \\push %rcx
        \\push %r11
        \\call handle_syscall
        \\pop %r11
        \\pop %rcx
        \\sysretq
    );
}

export fn handle_syscall(number: u64, arg1: u64, arg2: u64) u64 {
    return switch (number) {
        0 => 0,  // sys_exit
        1 => blk: {  // sys_write
            const buf: [*]const u8 = @ptrFromInt(arg1);
            const len: usize = @intCast(arg2);
            for (0..len) |i| {
                vga_putchar(buf[i]);
            }
            break :blk len;
        },
        else => 0,
    };
}

// 内核主函数
export fn kernel_main() void {
    print("Zig Kernel Booting...\n");

    init_idt();
    print("IDT initialized\n");

    init_memory();
    print("Memory initialized\n");

    scheduler_init();
    print("Scheduler initialized\n");

    print("Welcome to Zig OS!\n");

    // 进入空闲循环
    while (true) {
        asm volatile ("hlt");
    }
}
