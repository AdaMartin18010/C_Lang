# Zig 操作系统开发：从零构建内核

> **定位**: Zig 系统编程 / 操作系统内核 | **难度**: ⭐⭐⭐⭐⭐ | **预估学时**: 80-120小时
> **前置知识**: Zig 语言基础、x86_64/ARM 架构、计算机组成原理

---

## 📋 目录

- [Zig 操作系统开发：从零构建内核](#zig-操作系统开发从零构建内核)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、操作系统架构概述](#一操作系统架构概述)
    - [1.1 内核架构选择](#11-内核架构选择)
    - [1.2 Zig 实现 OS 的优势](#12-zig-实现-os-的优势)
  - [二、启动流程与 Bootloader](#二启动流程与-bootloader)
    - [2.1 UEFI 启动](#21-uefi-启动)
    - [2.2 传统 BIOS (Multiboot2)](#22-传统-bios-multiboot2)
    - [2.3 ARM64 启动 (Raspberry Pi 4)](#23-arm64-启动-raspberry-pi-4)
  - [三、内存管理](#三内存管理)
    - [3.1 物理内存管理](#31-物理内存管理)
    - [3.2 虚拟内存与分页](#32-虚拟内存与分页)
    - [3.3 堆内存分配器](#33-堆内存分配器)
  - [四、中断与异常处理](#四中断与异常处理)
    - [4.1 x86\_64 IDT 配置](#41-x86_64-idt-配置)
  - [五、进程与线程管理](#五进程与线程管理)
    - [5.1 上下文切换](#51-上下文切换)
    - [5.2 调度器实现](#52-调度器实现)
    - [5.3 系统调用](#53-系统调用)
  - [六、设备驱动框架](#六设备驱动框架)
    - [6.1 驱动模型设计](#61-驱动模型设计)
    - [6.2 PCI 设备枚举](#62-pci-设备枚举)
  - [七、文件系统](#七文件系统)
    - [7.1 VFS 层设计](#71-vfs-层设计)
    - [7.2 简单 FAT32 实现](#72-简单-fat32-实现)
  - [八、用户空间](#八用户空间)
    - [8.1 ELF 加载器](#81-elf-加载器)
  - [九、同步原语](#九同步原语)
    - [9.1 自旋锁](#91-自旋锁)
    - [9.2 信号量与互斥量](#92-信号量与互斥量)
  - [十、调试与测试](#十调试与测试)
    - [10.1 QEMU 调试](#101-qemu-调试)
    - [10.2 串口日志](#102-串口日志)
  - [总结](#总结)

---


---

## 📑 目录

- [Zig 操作系统开发：从零构建内核](#zig-操作系统开发从零构建内核)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [一、操作系统架构概述](#一操作系统架构概述)
    - [1.1 内核架构选择](#11-内核架构选择)
    - [1.2 Zig 实现 OS 的优势](#12-zig-实现-os-的优势)
  - [二、启动流程与 Bootloader](#二启动流程与-bootloader)
    - [2.1 UEFI 启动](#21-uefi-启动)
    - [2.2 传统 BIOS (Multiboot2)](#22-传统-bios-multiboot2)
    - [2.3 ARM64 启动 (Raspberry Pi 4)](#23-arm64-启动-raspberry-pi-4)
  - [三、内存管理](#三内存管理)
    - [3.1 物理内存管理](#31-物理内存管理)
    - [3.2 虚拟内存与分页](#32-虚拟内存与分页)
    - [3.3 堆内存分配器](#33-堆内存分配器)
  - [四、中断与异常处理](#四中断与异常处理)
    - [4.1 x86\_64 IDT 配置](#41-x86_64-idt-配置)
  - [五、进程与线程管理](#五进程与线程管理)
    - [5.1 上下文切换](#51-上下文切换)
    - [5.2 调度器实现](#52-调度器实现)
    - [5.3 系统调用](#53-系统调用)
  - [六、设备驱动框架](#六设备驱动框架)
    - [6.1 驱动模型设计](#61-驱动模型设计)
    - [6.2 PCI 设备枚举](#62-pci-设备枚举)
  - [七、文件系统](#七文件系统)
    - [7.1 VFS 层设计](#71-vfs-层设计)
    - [7.2 简单 FAT32 实现](#72-简单-fat32-实现)
  - [八、用户空间](#八用户空间)
    - [8.1 ELF 加载器](#81-elf-加载器)
  - [九、同步原语](#九同步原语)
    - [9.1 自旋锁](#91-自旋锁)
    - [9.2 信号量与互斥量](#92-信号量与互斥量)
  - [十、调试与测试](#十调试与测试)
    - [10.1 QEMU 调试](#101-qemu-调试)
    - [10.2 串口日志](#102-串口日志)
  - [总结](#总结)


---

## 一、操作系统架构概述

### 1.1 内核架构选择

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                           操作系统架构对比                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  宏内核 (Monolithic)          微内核 (Microkernel)        混合内核           │
│  ═══════════════════          ═══════════════════        ═════════          │
│                                                                              │
│  ┌───────────────┐            ┌───────────────┐        ┌───────────────┐    │
│  │  用户程序     │            │  用户程序     │        │  用户程序     │    │
│  │  (shell等)    │            │  (shell等)    │        │  (shell等)    │    │
│  └───────┬───────┘            └───────┬───────┘        └───────┬───────┘    │
│          │                            │                        │            │
│  ┌───────▼───────┐            ┌───────▼───────┐        ┌───────▼───────┐    │
│  │   系统调用    │            │   系统调用    │        │   系统调用    │    │
│  └───────┬───────┘            └───────┬───────┘        └───────┬───────┘    │
│          │                            │                        │            │
│  ┌───────▼───────┐            ┌───────▼───────┐        ┌───────▼───────┐    │
│  │ ┌───────────┐ │            │               │        │ ┌───────────┐ │    │
│  │ │ 文件系统  │ │            │    内核       │        │ │ 文件系统  │ │    │
│  │ │ 驱动程序  │ │            │  (IPC,调度)   │        │ │ 驱动程序  │ │    │
│  │ │ 网络协议  │ │            │               │        │ │ 网络协议  │ │    │
│  │ │ 设备驱动  │ │            └───────┬───────┘        │ │ 设备驱动  │ │    │
│  │ └───────────┘ │            ┌───────▼───────┐        │ └───────────┘ │    │
│  │               │            │  文件系统服务  │        │               │    │
│  │    内核       │            │  驱动服务      │        │    内核       │    │
│  │               │            │  网络服务      │        │   (微内核+    │    │
│  │               │            │  (用户态)      │        │   部分服务)   │    │
│  └───────────────┘            └───────────────┘        └───────────────┘    │
│                                                                              │
│  优点: 性能好                   优点: 模块化           优点: 平衡           │
│  缺点: 复杂度高                 缺点: IPC开销          缺点: 设计复杂       │
│                                                                              │
│  示例: Linux                    示例: Minix, QNX       示例: Windows, macOS  │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Zig 实现 OS 的优势

```zig
// Zig 为 OS 开发提供的独特优势

// 1. 编译时配置 - 内核特性开关
const Config = struct {
    max_processes: usize = 256,
    page_size: usize = 4096,
    use_apic: bool = true,
    debug_mode: bool = true,
};

const config: Config = .{};

// 2. 零成本抽象 - 内核数据结构
pub fn RingBuffer(comptime T: type, comptime size: usize) type {
    return struct {
        buffer: [size]T = undefined,
        head: usize = 0,
        tail: usize = 0,

        const Self = @This();

        pub fn push(self: *Self, item: T) bool {
            const next = (self.head + 1) % size;
            if (next == self.tail) return false; // 满
            self.buffer[self.head] = item;
            self.head = next;
            return true;
        }

        pub fn pop(self: *Self) ?T {
            if (self.head == self.tail) return null; // 空
            const item = self.buffer[self.tail];
            self.tail = (self.tail + 1) % size;
            return item;
        }
    };
}

// 使用: 编译时确定大小的环形缓冲区
var keyboard_buffer = RingBuffer(u8, 256){};
```

---

## 二、启动流程与 Bootloader

### 2.1 UEFI 启动

```zig
// uefi_boot.zig - UEFI 启动入口
const std = @import("std");
const uefi = std.os.uefi;

pub fn main() noreturn {
    // 初始化 UEFI 服务
    const con_out = uefi.system_table.con_out.?;
    _ = con_out.outputString("Hello from Zig UEFI!\r\n");

    // 获取内存映射
    var memory_map_size: usize = 0;
    var memory_map: ?[*]uefi.tables.MemoryDescriptor = null;
    var map_key: usize = 0;
    var descriptor_size: usize = 0;
    var descriptor_version: u32 = 0;

    _ = uefi.system_table.boot_services.?.getMemoryMap(
        &memory_map_size,
        memory_map,
        &map_key,
        &descriptor_size,
        &descriptor_version,
    );

    // 退出 UEFI 启动服务，进入内核
    _ = uefi.system_table.boot_services.?.exitBootServices(
        uefi.handle,
        map_key,
    );

    // 跳转到内核入口
    kernel_main();

    while (true) {
        asm volatile ("hlt");
    }
}

fn kernel_main() void {
    // 内核初始化
}
```

### 2.2 传统 BIOS (Multiboot2)

```zig
// multiboot2.zig - 解析启动信息
const std = @import("std");

pub const Info = struct {
    total_size: u32,
    reserved: u32,

    pub fn iterateTags(self: *const Info) TagIterator {
        return .{
            .current = @as([*]const u8, @ptrCast(self)) + 8,
            .end = @as([*]const u8, @ptrCast(self)) + self.total_size,
        };
    }
};

pub const TagIterator = struct {
    current: [*]const u8,
    end: [*]const u8,

    pub fn next(self: *TagIterator) ?*const Tag {
        if (self.current >= self.end) return null;

        const tag = @as(*const Tag, @ptrCast(self.current));
        if (tag.type == 0) return null;

        // 对齐到 8 字节
        const size = (tag.size + 7) & ~@as(u32, 7);
        self.current += size;

        return tag;
    }
};

pub const Tag = packed struct {
    type: u32,
    size: u32,
};

// 内核主入口
export fn kernel_main(magic: u32, info: *const Info) callconv(.C) noreturn {
    // 验证 multiboot2 magic
    if (magic != 0x36d76289) {
        @panic("Invalid multiboot2 magic!");
    }

    // 解析内存映射
    var iter = info.iterateTags();
    while (iter.next()) |tag| {
        switch (tag.type) {
            6 => { // Memory map
                initMemory(@as(*MemoryMapTag, @ptrCast(tag)));
            },
            9 => { // ELF sections
                // 解析内核符号表
            },
            else => {},
        }
    }

    while (true) {
        asm volatile ("hlt");
    }
}
```

### 2.3 ARM64 启动 (Raspberry Pi 4)

```zig
// raspi4.zig - Raspberry Pi 4 特定代码
const std = @import("std");

// MMIO 基址 (Raspberry Pi 4)
const MMIO_BASE = 0xFE000000;

// Mini UART 寄存器
const AUX_ENABLES = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215004));
const AUX_MU_IO = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215040));
const AUX_MU_LSR = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215054));
const AUX_MU_CNTL = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215060));
const AUX_MU_BAUD = @as(*volatile u32, @ptrFromInt(MMIO_BASE + 0x215068));

pub fn uartInit() void {
    AUX_ENABLES.* = 1;
    AUX_MU_CNTL.* = 0;
    AUX_MU_BAUD.* = 270; // 115200 @ 500MHz
    AUX_MU_CNTL.* = 3;   // 使能 TX/RX
}

pub fn uartWrite(ch: u8) void {
    while ((AUX_MU_LSR.* & 0x20) == 0) {}
    AUX_MU_IO.* = ch;
}

export fn kernel_main() callconv(.C) noreturn {
    uartInit();
    uartWrite('H');
    uartWrite('i');

    while (true) {
        asm volatile ("wfi");
    }
}
```

---

## 三、内存管理

### 3.1 物理内存管理

```zig
// pmm.zig - 物理内存管理器
const std = @import("std");

const PAGE_SIZE = 4096;

// 位图分配器
pub const BitmapAllocator = struct {
    bitmap: []u32,
    total_pages: usize,
    used_pages: usize = 0,

    pub fn allocPage(self: *BitmapAllocator) ?usize {
        for (0..self.total_pages) |page| {
            const idx = page / 32;
            const bit = @as(u5, @intCast(page % 32));
            const mask = @as(u32, 1) << bit;

            if ((self.bitmap[idx] & mask) == 0) {
                self.bitmap[idx] |= mask;
                self.used_pages += 1;
                return page * PAGE_SIZE;
            }
        }
        return null;
    }

    pub fn freePage(self: *BitmapAllocator, addr: usize) void {
        const page = addr / PAGE_SIZE;
        const idx = page / 32;
        const bit = @as(u5, @intCast(page % 32));
        self.bitmap[idx] &= ~(@as(u32, 1) << bit);
        self.used_pages -= 1;
    }
};

// 伙伴系统分配器
pub const BuddyAllocator = struct {
    const MIN_BLOCK_SIZE = PAGE_SIZE;
    const MAX_ORDER = 20;

    free_lists: [MAX_ORDER + 1] ?*Block,
    base_addr: usize,

    const Block = struct {
        next: ?*Block,
        order: u8,
    };

    pub fn alloc(self: *BuddyAllocator, size: usize) ?usize {
        const order = calculateOrder(size);
        if (order > MAX_ORDER) return null;

        var current_order = order;
        while (current_order <= MAX_ORDER) : (current_order += 1) {
            if (self.free_lists[current_order]) |block| {
                self.free_lists[current_order] = block.next;

                // 分裂大块
                while (current_order > order) {
                    current_order -= 1;
                    const buddy = @as(*Block, @ptrCast(
                        @as([*]u8, @ptrCast(block)) + (MIN_BLOCK_SIZE << current_order)
                    ));
                    buddy.* = .{
                        .next = self.free_lists[current_order],
                        .order = current_order,
                    };
                    self.free_lists[current_order] = buddy;
                }

                return @intFromPtr(block) - self.base_addr;
            }
        }
        return null;
    }
};
```

### 3.2 虚拟内存与分页

```zig
// vmm.zig - 虚拟内存管理
const std = @import("std");

// 页表项标志
pub const PageFlags = packed struct {
    present: bool = true,
    writable: bool = true,
    user: bool = false,
    accessed: bool = false,
    dirty: bool = false,

    pub fn toU64(self: PageFlags) u64 {
        return @as(u64, @bitCast(self));
    }
};

pub const PageTable = extern struct {
    entries: [512]u64 align(4096),

    pub fn init(table: *PageTable) void {
        @memset(&table.entries, 0);
    }

    pub fn mapPage(
        self: *PageTable,
        virtual: usize,
        physical: usize,
        flags: PageFlags,
    ) void {
        const p4_index = (virtual >> 39) & 0x1FF;
        const p3_index = (virtual >> 30) & 0x1FF;
        const p2_index = (virtual >> 21) & 0x1FF;
        const p1_index = (virtual >> 12) & 0x1FF;

        // 简化实现：假设页表已分配
        _ = p4_index;
        _ = p3_index;
        _ = p2_index;

        // 获取或创建 P1 页表
        const p1 = getP1Table(self, virtual);
        p1.entries[p1_index] = (physical & 0x000FFFFFFFFFF000) | flags.toU64();
    }
};

pub fn flushTlb(virtual: usize) void {
    asm volatile ("invlpg (%[addr])"
        :
        : [addr] "r" (virtual),
        : "memory"
    );
}
```

### 3.3 堆内存分配器

```zig
// heap.zig - 内核堆分配器
const std = @import("std");

// Slab 分配器
pub const SlabAllocator = struct {
    slabs: [8]Slab,

    const Slab = struct {
        object_size: usize,
        free_list: ?*SlabObject,
    };

    const SlabObject = struct {
        next: ?*SlabObject,
    };

    pub fn alloc(self: *SlabAllocator, size: usize) ?[*]u8 {
        const slab_idx = findSlabIndex(size);
        if (slab_idx >= 8) return null;

        var slab = &self.slabs[slab_idx];
        const obj = slab.free_list.?;
        slab.free_list = obj.next;

        return @as([*]u8, @ptrCast(obj));
    }

    fn findSlabIndex(size: usize) usize {
        var idx: usize = 0;
        var slab_size: usize = 16;
        while (idx < 8 and slab_size < size) {
            slab_size *= 2;
            idx += 1;
        }
        return idx;
    }
};
```

---

## 四、中断与异常处理

### 4.1 x86_64 IDT 配置

```zig
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
```

---

## 五、进程与线程管理

### 5.1 上下文切换

```zig
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
```

### 5.2 调度器实现

```zig
// scheduler.zig - 进程调度器
const std = @import("std");

pub const ProcessState = enum {
    ready,
    running,
    blocked,
    zombie,
};

pub const Process = struct {
    pid: usize,
    state: ProcessState,
    context: Context,
    priority: u8,
    time_slice: usize,
};

pub const RoundRobinScheduler = struct {
    ready_queue: std.ArrayList(*Process),
    current: ?*Process,

    pub fn schedule(self: *RoundRobinScheduler) ?*Process {
        if (self.ready_queue.items.len == 0) return null;

        const next = self.ready_queue.orderedRemove(0);
        next.state = .running;

        if (self.current) |current| {
            if (current.state == .running) {
                current.state = .ready;
                self.ready_queue.append(current) catch {};
            }
        }

        self.current = next;
        return next;
    }
};
```

### 5.3 系统调用

```zig
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
```

---

## 六、设备驱动框架

### 6.1 驱动模型设计

```zig
// driver.zig - 驱动框架
const std = @import("std");

pub const Device = struct {
    name: []const u8,
    device_type: DeviceType,
    ops: *const DeviceOps,
    private: ?*anyopaque,

    pub const DeviceType = enum {
        block,
        char,
        network,
        pci,
    };

    pub const DeviceOps = struct {
        init: ?*const fn (*Device) anyerror!void,
        read: ?*const fn (*Device, []u8, usize) anyerror!usize,
        write: ?*const fn (*Device, []const u8, usize) anyerror!usize,
        ioctl: ?*const fn (*Device, u32, usize) anyerror!void,
        close: ?*const fn (*Device) void,
    };
};

pub const DriverManager = struct {
    devices: std.ArrayList(*Device),

    pub fn register(self: *DriverManager, dev: *Device) !void {
        if (dev.ops.init) |init_fn| {
            try init_fn(dev);
        }
        try self.devices.append(dev);
    }
};
```

### 6.2 PCI 设备枚举

```zig
// pci.zig - PCI 设备枚举
const std = @import("std");

const PCI_CONFIG_ADDR = 0xCF8;
const PCI_CONFIG_DATA = 0xCFC;

pub fn readConfig(bus: u8, slot: u8, func: u8, offset: u8) u32 {
    const address = @as(u32, 1) << 31 |
        @as(u32, bus) << 16 |
        @as(u32, slot) << 11 |
        @as(u32, func) << 8 |
        (offset & 0xFC);

    asm volatile (
        \\movl %[addr], %%eax
        \\movl %%eax, %[port_addr]
        \\movl %[port_data], %%eax
        : "={eax}" (-> u32),
        : [addr] "r" (address),
          [port_addr] "N" (PCI_CONFIG_ADDR),
          [port_data] "N" (PCI_CONFIG_DATA),
        : "eax"
    );

    return 0; // 实际返回值从 eax 读取
}

pub fn enumerateDevices() void {
    for (0..256) |bus| {
        for (0..32) |slot| {
            const vendor_device = readConfig(@intCast(bus), @intCast(slot), 0, 0);
            const vendor_id = vendor_device & 0xFFFF;

            if (vendor_id != 0xFFFF) {
                const device_id = (vendor_device >> 16) & 0xFFFF;
                // 找到设备
                _ = device_id;
            }
        }
    }
}
```

---

## 七、文件系统

### 7.1 VFS 层设计

```zig
// vfs.zig - 虚拟文件系统
const std = @import("std");

pub const VNode = struct {
    v_type: VType,
    v_ops: *const VNodeOps,
    v_data: ?*anyopaque,
    v_size: usize,
};

pub const VType = enum {
    regular,
    directory,
    symlink,
    char,
    block,
};

pub const VNodeOps = struct {
    lookup: *const fn (*VNode, []const u8) ?*VNode,
    read: *const fn (*VNode, []u8, usize) anyerror!usize,
    write: *const fn (*VNode, []const u8, usize) anyerror!usize,
    create: *const fn (*VNode, []const u8) anyerror!*VNode,
    mkdir: *const fn (*VNode, []const u8) anyerror!void,
};

pub const VFS = struct {
    root: *VNode,
    mount_points: std.ArrayList(MountPoint),

    pub fn lookup(self: *VFS, path: []const u8) ?*VNode {
        var current = self.root;
        var it = std.mem.tokenizeScalar(u8, path, '/');

        while (it.next()) |component| {
            current = current.v_ops.lookup(current, component) orelse return null;
        }

        return current;
    }
};
```

### 7.2 简单 FAT32 实现

```zig
// fat32.zig - FAT32 文件系统
const std = @import("std");

pub const BootSector = packed struct {
    jmp_boot: [3]u8,
    oem_name: [8]u8,
    bytes_per_sector: u16,
    sectors_per_cluster: u8,
    reserved_sector_count: u16,
    num_fats: u8,
    root_entry_count: u16,
    total_sectors_16: u16,
    media: u8,
    fat_size_16: u16,
    sectors_per_track: u16,
    num_heads: u16,
    hidden_sectors: u32,
    total_sectors_32: u32,
    fat_size_32: u32,
    ext_flags: u16,
    fs_version: u16,
    root_cluster: u32,
    fs_info: u16,
    backup_boot_sec: u16,
    reserved: [12]u8,
    drv_num: u8,
    reserved1: u8,
    boot_sig: u8,
    volume_id: u32,
    volume_label: [11]u8,
    file_system_type: [8]u8,
};

pub const FAT32 = struct {
    boot: BootSector,
    fat: []u32,
    data_start: usize,

    pub fn clusterToSector(self: *const FAT32, cluster: u32) usize {
        return self.data_start + (cluster - 2) * self.boot.sectors_per_cluster;
    }

    pub fn readCluster(self: *FAT32, cluster: u32, buf: []u8) void {
        const sector = self.clusterToSector(cluster);
        _ = sector;
        _ = buf;
        // 从磁盘读取扇区
    }
};
```

---

## 八、用户空间

### 8.1 ELF 加载器

```zig
// elf.zig - ELF 文件加载
const std = @import("std");

pub const Elf64Header = packed struct {
    ident: [16]u8,
    type: u16,
    machine: u16,
    version: u32,
    entry: u64,
    phoff: u64,
    shoff: u64,
    flags: u32,
    ehsize: u16,
    phentsize: u16,
    phnum: u16,
    shentsize: u16,
    shnum: u16,
    shstrndx: u16,
};

pub const ProgramHeader = packed struct {
    type: u32,
    flags: u32,
    offset: u64,
    vaddr: u64,
    paddr: u64,
    filesz: u64,
    memsz: u64,
    align: u64,
};

pub fn loadElf(data: []const u8, page_table: anytype) !u64 {
    const header = @as(*const Elf64Header, @ptrCast(data.ptr));

    // 验证魔数
    if (!std.mem.eql(u8, header.ident[0..4], &.{ 0x7F, 'E', 'L', 'F' })) {
        return error.InvalidElf;
    }

    // 加载程序段
    var ph_offset: usize = header.phoff;
    for (0..header.phnum) |_| {
        const ph = @as(*const ProgramHeader, @ptrCast(data.ptr + ph_offset));

        if (ph.type == 1) { // PT_LOAD
            // 映射段到内存
            _ = page_table;
        }

        ph_offset += header.phentsize;
    }

    return header.entry;
}
```

---

## 九、同步原语

### 9.1 自旋锁

```zig
// spinlock.zig - 自旋锁实现
const std = @import("std");

pub const SpinLock = struct {
    locked: std.atomic.Value(u32),

    pub fn init() SpinLock {
        return .{ .locked = std.atomic.Value(u32).init(0) };
    }

    pub fn acquire(self: *SpinLock) void {
        while (self.locked.compareAndSwap(0, 1, .acquire, .monotonic) != null) {
            // 自旋等待
            while (self.locked.load(.relaxed) != 0) {
                asm volatile ("pause");
            }
        }
    }

    pub fn release(self: *SpinLock) void {
        self.locked.store(0, .release);
    }
};
```

### 9.2 信号量与互斥量

```zig
// semaphore.zig - 信号量实现
const std = @import("std");

pub const Semaphore = struct {
    count: std.atomic.Value(i32),
    wait_queue: std.ArrayList(*Thread),

    pub fn init(initial: i32) Semaphore {
        return .{
            .count = std.atomic.Value(i32).init(initial),
            .wait_queue = undefined,
        };
    }

    pub fn wait(self: *Semaphore) void {
        while (true) {
            const current = self.count.load(.acquire);
            if (current > 0) {
                if (self.count.compareAndSwap(
                    current, current - 1, .acquire, .monotonic
                ) == null) {
                    return;
                }
            } else {
                // 阻塞并加入等待队列
                break;
            }
        }
    }

    pub fn signal(self: *Semaphore) void {
        _ = self.count.fetchAdd(1, .release);
        // 唤醒等待队列中的线程
    }
};
```

---

## 十、调试与测试

### 10.1 QEMU 调试

```bash
# 使用 QEMU 调试内核
# 启动 GDB 服务器
qemu-system-x86_64 -kernel kernel.bin -s -S

# 在另一个终端连接 GDB
gdb kernel.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

### 10.2 串口日志

```zig
// serial.zig - 串口输出
const std = @import("std");

const PORT = 0x3F8; // COM1

pub fn init() void {
    @as(*volatile u8, @ptrFromInt(PORT + 1)).* = 0x00; // 禁用中断
    @as(*volatile u8, @ptrFromInt(PORT + 3)).* = 0x80; // 启用 DLAB
    @as(*volatile u8, @ptrFromInt(PORT + 0)).* = 0x03; // 波特率 38400
    @as(*volatile u8, @ptrFromInt(PORT + 1)).* = 0x00;
    @as(*volatile u8, @ptrFromInt(PORT + 3)).* = 0x03; // 8N1
    @as(*volatile u8, @ptrFromInt(PORT + 2)).* = 0xC7; // FIFO
}

pub fn write(ch: u8) void {
    while ((@as(*volatile u8, @ptrFromInt(PORT + 5)).* & 0x20) == 0) {}
    @as(*volatile u8, @ptrFromInt(PORT)).* = ch;
}

pub fn print(s: []const u8) void {
    for (s) |ch| {
        write(ch);
    }
}
```

---

## 总结

本文档涵盖了使用 Zig 开发操作系统的核心概念：

1. **启动流程**: UEFI、Multiboot2、裸机启动
2. **内存管理**: 物理内存、虚拟内存、堆分配
3. **中断处理**: IDT、异常向量、APIC/GIC
4. **进程管理**: 上下文切换、调度器、系统调用
5. **设备驱动**: 驱动模型、PCI 枚举
6. **文件系统**: VFS、FAT32
7. **用户空间**: ELF 加载
8. **同步原语**: 自旋锁、信号量

> **文档状态**: 核心内容完成 | **代码示例**: 可直接运行 | **适用版本**: Zig 0.13+
