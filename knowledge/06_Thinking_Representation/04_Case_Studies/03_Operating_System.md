# 操作系统内核案例研究


---

## 📑 目录

- [操作系统内核案例研究](#操作系统内核案例研究)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [1. 内核架构](#1-内核架构)
    - [1.1 宏内核 vs 微内核](#11-宏内核-vs-微内核)
    - [1.2 混合内核架构](#12-混合内核架构)
  - [2. 内存管理](#2-内存管理)
    - [2.1 x86内存架构演进](#21-x86内存架构演进)
    - [2.2 分页机制](#22-分页机制)
  - [3. 进程调度](#3-进程调度)
    - [3.1 进程状态机](#31-进程状态机)
    - [3.2 调度算法对比](#32-调度算法对比)
  - [4. 简化OS内核实现](#4-简化os内核实现)
    - [4.1 引导加载程序 (Bootloader)](#41-引导加载程序-bootloader)
    - [4.2 内核主程序](#42-内核主程序)
    - [4.3 编译脚本和运行](#43-编译脚本和运行)
  - [5. 关键概念总结](#5-关键概念总结)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 概述

本章深入分析操作系统内核的设计与实现，涵盖内核架构、内存管理、进程调度等核心组件。
通过完整的代码示例展示如何从实模式切换到保护模式，并实现基本的操作系统功能。

---

## 1. 内核架构

### 1.1 宏内核 vs 微内核

```text
┌─────────────────────────────────────────────────────────────────┐
│                       内核架构对比                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                │
│     宏内核 (Monolithic)              微内核 (Microkernel)        │
│     ───────────────────              ───────────────────         │
│                                                                │
│  ┌─────────────────────┐          ┌─────────────────────┐       │
│  │    系统调用接口      │          │    系统调用接口      │       │
│  └──────────┬──────────┘          └──────────┬──────────┘       │
│             │                                │                 │
│  ┌──────────▼──────────┐          ┌──────────▼──────────┐       │
│  │                     │          │                     │       │
│  │  ┌───┐ ┌───┐ ┌───┐  │          │   进程间通信 (IPC)   │       │
│  │  │VFS│ │FS │ │MM │  │          │                     │       │
│  │  └───┘ └───┘ └───┘  │          └──────────┬──────────┘       │
│  │  ┌───┐ ┌───┐ ┌───┐  │                     │                 │
│  │  │网络│ │驱动│ │调度│ │          ┌──────────▼──────────┐       │
│  │  └───┘ └───┘ └───┘  │          │   微内核 (最小功能)   │       │
│  │                     │          │ • 地址空间管理        │       │
│  │   所有功能在内核态    │          │ • 线程/调度          │       │
│  │                     │          │ • 进程间通信          │       │
│  └──────────┬──────────┘          │ • 中断处理           │       │
│             │                     └─────────────────────┘       │
│  ┌──────────▼──────────┐                                      │
│  │      硬件层          │          文件系统、驱动等作为用户态服务   │
│  └─────────────────────┘          通过IPC与内核通信              │
│                                                                │
│  代表: Linux, BSD                  代表: Minix, QNX, L4         │
│  优点: 性能高, 通信开销小            优点: 稳定, 安全, 可扩展     │
│  缺点: 大内核, 一个bug影响全系统     缺点: IPC开销, 性能损失      │
│                                                                │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 混合内核架构

```text
┌─────────────────────────────────────────────────────────────┐
│                       混合内核 (Windows/macOS)               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  用户空间                                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  应用程序  │  系统服务  │  设备驱动(部分)  │  子系统   │    │
│  └─────────────────────────────────────────────────────┘    │
│  ─────────────────────────────────────────────────────────  │
│  内核空间                                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                    系统调用接口                      │    │
│  ├─────────────────────────────────────────────────────┤    │
│  │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐  │   │
│  │  │内存 │ │进程 │ │I/O  │ │文件 │ │网络 │ │安全 │  │   │
│  │  │管理 │ │调度 │ │管理 │ │系统 │ │协议 │ │子系统│  │   │
│  │  └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘ └──┬──┘  │   │
│  │     └───────┴───────┴───────┴───────┴───────┘      │   │
│  │                    微内核核心                        │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │         硬件抽象层 (HAL) / 引导程序                  │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 内存管理

### 2.1 x86内存架构演进

```text
┌─────────────────────────────────────────────────────────────┐
│                   x86 内存管理演进                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  实模式 (Real Mode)              保护模式 (Protected Mode)    │
│  ───────────────────             ────────────────────────   │
│                                                             │
│  段寄存器 << 4                  段选择子 ──▶ 段描述符表(GDT)   │
│       +                         ┌───────────┐               │
│  偏移地址  ──▶ 物理地址          │  段基址    │               │
│             │                   │  段限长    │               │
│             ▼                   │  属性(DPL) │               │
│        ┌────────┐               └─────┬─────┘               │
│        │物理内存│                     +                     │
│        │ 1MB   │              偏移地址 ──▶ 线性地址          │
│        └────────┘                     │                     │
│                                       ▼                     │
│  最大1MB内存                    ┌──────────┐               │
│  无保护                         │ 分页机制  │ 可选           │
│  无权限检查                     │ 页目录/表 │               │
│                                 └────┬─────┘               │
│                                      ▼                      │
│                                 ┌──────────┐               │
│                                 │ 物理内存  │               │
│                                 │ 4GB+     │               │
│                                 └──────────┘               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 分页机制

```text
┌─────────────────────────────────────────────────────────────┐
│                   二级分页机制 (x86)                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  线性地址 (32位)                                             │
│  ┌────────┬────────┬────────────────┐                       │
│  │ 目录索引 │ 表索引  │    页内偏移     │                       │
│  │  10位   │  10位   │     12位        │                       │
│  └────┬───┴────┬───┴────────┬────────┘                       │
│       │        │            │                               │
│       ▼        ▼            ▼                               │
│                                                             │
│  CR3 ──▶ ┌─────────────┐                                    │
│  页目录   │ 页目录表(PDE) │                                   │
│  基地址   │ [0-1023]    │                                   │
│          │ 每个4字节    │                                   │
│          └──────┬──────┘                                   │
│                 │                                          │
│                 ▼                                          │
│          ┌─────────────┐                                   │
│          │ 页表(PTE)   │                                    │
│          │ [0-1023]    │                                    │
│          │ 每个4字节    │                                    │
│          └──────┬──────┘                                   │
│                 │                                          │
│                 ▼                                          │
│          ┌─────────────┐                                   │
│          │ 物理页帧    │ 4KB页面                             │
│          │ (4KB对齐)   │                                    │
│          └─────────────┘                                   │
│                                                             │
│  页表项结构 (PTE):                                           │
│  ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐  │
│  │31-12│11-9│ 8  │ 7  │ 6  │ 5  │ 4  │ 3  │ 2  │ 1  │ 0  │  │
│  │页框 │AVL │ G  │PAT │ D  │ A  │PCD │PWT │U/S │R/W │ P  │  │
│  │地址 │    │全局│    │脏位│访问│    │    │用户│读写│存在│  │
│  └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 进程调度

### 3.1 进程状态机

```text
┌─────────────────────────────────────────────────────────────┐
│                    进程状态转换图                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                        创建                                 │
│                         │                                   │
│                         ▼                                   │
│  ┌─────────────────────────────────────────┐               │
│  │                                         │               │
│  │    ┌─────────┐      I/O/等待事件        │               │
│  │    │  运行   │ ─────────────────▶      │               │
│  │    │ Running │                        │               │
│  │    └────┬────┘                        │               │
│  │  调度  │  ▲   时间片用完/被抢占        │  就绪队列      │
│  │    │    │  │                           │               │
│  │    ▼    │  │                           │               │
│  │    ┌────┴────┐      I/O完成/          │               │
│  │    │  就绪   │ ◀──── 事件到达          │               │
│  │    │ Ready  │                          │               │
│  │    └─────────┘                          │               │
│  │                                         │               │
│  └─────────────────────────────────────────┘               │
│         │                      ▲                           │
│         │  等待事件            │                           │
│         ▼                      │                           │
│    ┌─────────┐                 │                           │
│    │  阻塞   │─────────────────┘                           │
│    │ Blocked │                                             │
│    └────┬────┘                                             │
│         │                                                   │
│         ▼ 终止                                              │
│    ┌─────────┐                                             │
│    │  僵尸   │                                             │
│    │ Zombie  │                                             │
│    └─────────┘                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 调度算法对比

| 算法 | 时间复杂度 | 特点 | 适用场景 |
|------|-----------|------|----------|
| **FIFO** | O(1) | 简单，可能护航 | 批处理 |
| **SJF** | O(n) | 最优平均等待时间 | 已知执行时间 |
| **优先级** | O(n) | 紧急任务优先 | 实时系统 |
| **RR** | O(1) | 时间片轮转 | 分时系统 |
| **多级反馈队列** | O(1) | 动态优先级调整 | 通用操作系统 |

---

## 4. 简化OS内核实现

### 4.1 引导加载程序 (Bootloader)

```nasm
; boot.asm - 引导扇区程序
; 将CPU从实模式切换到保护模式，然后加载内核

[BITS 16]           ; 16位实模式
[ORG 0x7C00]        ; BIOS加载引导扇区到0x7C00

start:
    ; 初始化段寄存器
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00  ; 设置栈顶

    ; 保存引导驱动器号
    mov [boot_drive], dl

    ; 清屏
    mov ax, 0x0003
    int 0x10

    ; 打印实模式消息
    mov si, msg_real_mode
    call print_string

    ; 加载内核到内存
    call load_kernel

    ; 启用A20地址线
    call enable_a20

    ; 加载GDT并切换到保护模式
    call switch_to_protected_mode

    jmp $           ; 死循环

; ============== 实模式函数 ==============

print_string:
    pusha
.loop:
    lodsb           ; AL = [SI], SI++
    or al, al
    jz .done
    mov ah, 0x0E    ; BIOS teletype
    mov bh, 0x00    ; 页号
    mov bl, 0x07    ; 颜色
    int 0x10
    jmp .loop
.done:
    popa
    ret

load_kernel:
    pusha
    mov si, msg_load_kernel
    call print_string

    ; BIOS INT 13h 读取磁盘
    mov ah, 0x02    ; 读扇区
    mov al, 15      ; 读15个扇区
    mov ch, 0       ; 柱面0
    mov cl, 2       ; 从扇区2开始(扇区1是引导扇区)
    mov dh, 0       ; 磁头0
    mov dl, [boot_drive]

    ; ES:BX = 0x0000:0x8000 = 0x8000
    mov bx, 0x8000
    int 0x13
    jc disk_error   ; 进位表示错误

    cmp al, 15      ; 检查读取的扇区数
    jne disk_error

    popa
    ret

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

enable_a20:
    pusha
    mov si, msg_a20
    call print_string

    ; 使用快速A20门
    in al, 0x92
    or al, 2
    out 0x92, al

    popa
    ret

; ============== GDT (全局描述符表) ==============

gdt_start:
    ; 空描述符 (必须)
    dq 0x0000000000000000

gdt_code:           ; 代码段描述符
    dw 0xFFFF       ; 段限长 0-15
    dw 0x0000       ; 段基址 0-15
    db 0x00         ; 段基址 16-23
    db 0x9A         ; 访问字节: 存在、特权0、代码段、可执行、可读
    db 0xCF         ; 段限长 16-19 + 标志 (4KB粒度、32位)
    db 0x00         ; 段基址 24-31

gdt_data:           ; 数据段描述符
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92         ; 访问字节: 存在、特权0、数据段、可写
    db 0xCF
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT大小
    dd gdt_start                ; GDT地址

; 段选择子
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; ============== 切换到保护模式 ==============

switch_to_protected_mode:
    mov si, msg_prot_mode
    call print_string

    cli             ; 关中断
    lgdt [gdt_descriptor]   ; 加载GDT

    ; 设置保护模式位
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; 远跳转到32位代码，刷新流水线
    jmp CODE_SEG:start_protected_mode

[BITS 32]           ; 32位保护模式

start_protected_mode:
    ; 初始化保护模式段寄存器
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000    ; 设置栈顶

    ; 清屏 (VGA文本模式)
    mov edi, 0xB8000
    mov ecx, 2000       ; 80x25
    mov ax, 0x0720      ; 空格，白色
    rep stosw

    ; 打印保护模式消息
    mov esi, msg_prot_mode_ok
    mov edi, 0xB8000
    call print_string_pm

    ; 跳转到内核
    jmp 0x8000

    jmp $

print_string_pm:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0F        ; 白色
    stosw
    jmp .loop
.done:
    popa
    ret

; ============== 数据 ==============

boot_drive:     db 0
msg_real_mode:  db "[Boot] Real mode started", 13, 10, 0
msg_load_kernel:db "[Boot] Loading kernel...", 13, 10, 0
msg_disk_error: db "[Boot] Disk error!", 13, 10, 0
msg_a20:        db "[Boot] Enabling A20...", 13, 10, 0
msg_prot_mode:  db "[Boot] Switching to protected mode...", 13, 10, 0
msg_prot_mode_ok:db "[Kernel] Protected mode enabled! ", 0

; 填充到510字节并添加引导签名
times 510-($-$$) db 0
dw 0xAA55           ; 引导扇区签名
```

### 4.2 内核主程序

```c
/* kernel.c - 简化操作系统内核 */
/* 编译: gcc -m32 -ffreestanding -c kernel.c -o kernel.o */
/* 链接: ld -m elf_i386 -Ttext 0x8000 -o kernel.bin kernel.o */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ============== 基本类型定义 ============== */

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* ============== 硬件寄存器操作 ============== */

static inline void outb(u16 port, u8 value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline u8 inb(u16 port) {
    u8 value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void cli(void) {
    __asm__ volatile ("cli");
}

static inline void sti(void) {
    __asm__ volatile ("sti");
}

static inline void hlt(void) {
    __asm__ volatile ("hlt");
}

/* ============== VGA显示驱动 ============== */

#define VGA_ADDRESS     0xB8000
#define VGA_WIDTH       80
#define VGA_HEIGHT      25
#define VGA_COLOR       0x0F    /* 白色 */

static u16 *vga_buffer = (u16 *)VGA_ADDRESS;
static int cursor_x = 0;
static int cursor_y = 0;

static void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (VGA_COLOR << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

static void vga_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        int index = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[index] = (VGA_COLOR << 8) | c;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        /* 滚动屏幕 */
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH;
             i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = (VGA_COLOR << 8) | ' ';
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

static void vga_print(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

static void vga_print_hex(u32 value) {
    const char *hex = "0123456789ABCDEF";
    char buf[9];
    for (int i = 7; i >= 0; i--) {
        buf[i] = hex[value & 0xF];
        value >>= 4;
    }
    buf[8] = '\0';
    vga_print(buf);
}

static void vga_print_dec(u32 value) {
    char buf[12];
    int i = 11;
    buf[--i] = '\0';
    do {
        buf[--i] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);
    vga_print(&buf[i]);
}

/* ============== 串口输出 (用于调试) ============== */

#define SERIAL_PORT     0x3F8   /* COM1 */

static void serial_init(void) {
    outb(SERIAL_PORT + 1, 0x00);    /* 禁用中断 */
    outb(SERIAL_PORT + 3, 0x80);    /* 启用DLAB */
    outb(SERIAL_PORT + 0, 0x03);    /* 波特率除数低字节 */
    outb(SERIAL_PORT + 1, 0x00);    /* 波特率除数高字节 */
    outb(SERIAL_PORT + 3, 0x03);    /* 8位数据，无奇偶校验 */
    outb(SERIAL_PORT + 2, 0xC7);    /* 启用FIFO */
    outb(SERIAL_PORT + 4, 0x0B);    /* 启用中断 */
}

static void serial_putchar(char c) {
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0); /* 等待发送就绪 */
    outb(SERIAL_PORT, c);
}

static void serial_print(const char *str) {
    while (*str) {
        serial_putchar(*str++);
    }
}

/* ============== 中断处理 ============== */

#define IDT_ENTRIES     256

typedef struct {
    u16 offset_low;
    u16 selector;
    u8  zero;
    u8  type_attr;
    u16 offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idtp;

/* 中断帧 */
typedef struct {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_num, err_code;
    u32 eip, cs, eflags;
} interrupt_frame_t;

void interrupt_handler(interrupt_frame_t *frame) {
    vga_print("\n[INT] Interrupt ");
    vga_print_dec(frame->int_num);
    vga_print(" occurred!\n");

    if (frame->int_num == 0x20) {
        /* 定时器中断 */
        static int ticks = 0;
        ticks++;
        if (ticks % 100 == 0) {
            serial_print(".");  /* 每秒打印一个点 */
        }
    } else if (frame->int_num == 0x21) {
        /* 键盘中断 */
        u8 scancode = inb(0x60);
        vga_print("[KEY] Scancode: ");
        vga_print_hex(scancode);
        vga_print("\n");
    }

    /* 发送EOI */
    if (frame->int_num >= 0x20) {
        outb(0x20, 0x20);   /* 主PIC EOI */
        if (frame->int_num >= 0x28) {
            outb(0xA0, 0x20);   /* 从PIC EOI */
        }
    }
}

/* 中断入口桩 (由汇编生成) */
extern void isr0(void);
extern void isr32(void);
extern void isr33(void);

static void idt_set_gate(int num, u32 handler, u16 selector, u8 flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
}

static void idt_init(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (u32)&idt;

    /* 初始化所有门为默认处理器 */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (u32)isr0, 0x08, 0x8E);
    }

    /* 设置特定中断 */
    idt_set_gate(0x20, (u32)isr32, 0x08, 0x8E);   /* 定时器 */
    idt_set_gate(0x21, (u32)isr33, 0x08, 0x8E);   /* 键盘 */

    /* 加载IDT */
    __asm__ volatile ("lidt %0" : : "m"(idtp));
}

/* ============== PIC可编程中断控制器 ============== */

static void pic_init(void) {
    /* 初始化主PIC */
    outb(0x20, 0x11);   /* ICW1: 边沿触发，级联 */
    outb(0x21, 0x20);   /* ICW2: 中断向量偏移 0x20 */
    outb(0x21, 0x04);   /* ICW3: 从片连接到IR2 */
    outb(0x21, 0x01);   /* ICW4: 8086模式 */

    /* 初始化从PIC */
    outb(0xA0, 0x11);   /* ICW1 */
    outb(0xA1, 0x28);   /* ICW2: 中断向量偏移 0x28 */
    outb(0xA1, 0x02);   /* ICW3 */
    outb(0xA1, 0x01);   /* ICW4 */

    /* 屏蔽所有中断，只启用定时器和键盘 */
    outb(0x21, 0xFC);   /* 主PIC: 允许IRQ0(定时器), IRQ1(键盘) */
    outb(0xA1, 0xFF);   /* 从PIC: 全部屏蔽 */
}

/* ============== 定时器 (PIT) ============== */

#define PIT_FREQ        1193182     /* PIT输入频率 */
#define TIMER_HZ        100         /* 100Hz = 10ms */

static void pit_init(void) {
    u32 divisor = PIT_FREQ / TIMER_HZ;
    outb(0x43, 0x36);   /* 通道0，访问方式：低字节/高字节，模式3 */
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

/* ============== 内存管理 ============== */

#define PAGE_SIZE       4096
#define PAGE_DIR_ADDR   0x100000    /* 页目录地址 1MB */
#define PAGE_TABLE_ADDR 0x101000    /* 页表地址 */

/* 页目录项和页表项标志 */
#define PAGE_PRESENT    0x001
#define PAGE_WRITABLE   0x002
#define PAGE_USER       0x004

static void *kmalloc_base = (void *)0x200000;   /* 2MB开始 */

static void paging_init(void) {
    u32 *page_dir = (u32 *)PAGE_DIR_ADDR;
    u32 *page_table = (u32 *)PAGE_TABLE_ADDR;

    /* 清零 */
    for (int i = 0; i < 1024; i++) {
        page_dir[i] = 0;
        page_table[i] = 0;
    }

    /* 设置第一个页表: 映射前4MB */
    for (int i = 0; i < 1024; i++) {
        page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* 页目录第一项指向页表 */
    page_dir[0] = (u32)page_table | PAGE_PRESENT | PAGE_WRITABLE;

    /* 启用分页 */
    __asm__ volatile (
        "mov %0, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :
        : "r"(page_dir)
        : "eax"
    );
}

static void *kmalloc(u32 size) {
    void *ptr = kmalloc_base;
    kmalloc_base = (void *)((u32)kmalloc_base + size);
    /* 4KB对齐 */
    kmalloc_base = (void *)(((u32)kmalloc_base + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    return ptr;
}

/* ============== 进程调度 ============== */

#define MAX_PROCESSES   8
#define STACK_SIZE      4096

typedef enum {
    PROC_UNUSED = 0,
    PROC_READY,
    PROC_RUNNING,
    PROC_BLOCKED
} proc_state_t;

typedef struct process {
    u32 pid;
    proc_state_t state;
    u32 esp;            /* 栈指针 */
    u32 ebp;            /* 基址指针 */
    u32 eip;            /* 指令指针 */
    u32 *stack;
    u32 sleep_ticks;
    char name[16];
} process_t;

static process_t processes[MAX_PROCESSES];
static process_t *current_proc = NULL;
static int next_pid = 1;
static volatile u32 ticks = 0;

/* 进程切换上下文 (汇编实现) */
extern void switch_context(u32 *old_esp, u32 new_esp);

static int process_create(void (*entry)(void), const char *name) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROC_UNUSED) {
            process_t *p = &processes[i];
            p->pid = next_pid++;
            p->state = PROC_READY;
            p->stack = kmalloc(STACK_SIZE);
            p->esp = (u32)p->stack + STACK_SIZE - 4;
            p->ebp = p->esp;
            p->eip = (u32)entry;
            p->sleep_ticks = 0;
            strncpy(p->name, name, 15);
            p->name[15] = '\0';

            /* 初始化栈 */
            u32 *stack = (u32 *)p->esp;
            *stack = (u32)entry;    /* 返回地址 */

            return p->pid;
        }
    }
    return -1;
}

static void scheduler(void) {
    if (!current_proc) {
        /* 选择第一个就绪进程 */
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].state == PROC_READY) {
                current_proc = &processes[i];
                current_proc->state = PROC_RUNNING;
                return;
            }
        }
    } else {
        current_proc->state = PROC_READY;

        /* 轮转选择下一个就绪进程 */
        int current_idx = current_proc - processes;
        for (int i = 1; i <= MAX_PROCESSES; i++) {
            int idx = (current_idx + i) % MAX_PROCESSES;
            if (processes[idx].state == PROC_READY) {
                current_proc = &processes[idx];
                current_proc->state = PROC_RUNNING;
                return;
            }
        }
    }
}

void timer_handler(void) {
    ticks++;

    /* 更新睡眠进程 */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROC_BLOCKED && processes[i].sleep_ticks > 0) {
            processes[i].sleep_ticks--;
            if (processes[i].sleep_ticks == 0) {
                processes[i].state = PROC_READY;
            }
        }
    }

    /* 每10ms调度一次 */
    if (ticks % 1 == 0) {
        scheduler();
    }
}

static void sleep(u32 ms) {
    if (current_proc) {
        current_proc->sleep_ticks = ms / 10;    /* 转换为ticks */
        current_proc->state = PROC_BLOCKED;
        while (current_proc->sleep_ticks > 0) {
            hlt();  /* 等待 */
        }
    }
}

/* ============== 示例进程 ============== */

static void process_a(void) {
    while (1) {
        vga_print("[A]");
        serial_print("[Process A running]\n");
        sleep(500);     /* 休眠500ms */
    }
}

static void process_b(void) {
    while (1) {
        vga_print("[B]");
        serial_print("[Process B running]\n");
        sleep(1000);    /* 休眠1000ms */
    }
}

static void idle_process(void) {
    while (1) {
        hlt();  /* 空闲时休眠 */
    }
}

/* ============== 内核主函数 ============== */

void kernel_main(void) {
    /* 初始化 */
    vga_clear();
    serial_init();

    vga_print("\n\n");
    vga_print("========================================\n");
    vga_print("   MinimalOS Kernel v0.1\n");
    vga_print("========================================\n\n");

    serial_print("\n\n========================================\n");
    serial_print("MinimalOS Kernel Starting...\n");
    serial_print("========================================\n\n");

    /* 初始化内存管理 */
    vga_print("[INIT] Initializing paging...\n");
    paging_init();
    vga_print("[INIT] Paging enabled\n");

    /* 初始化中断 */
    vga_print("[INIT] Initializing IDT...\n");
    idt_init();
    vga_print("[INIT] IDT loaded\n");

    vga_print("[INIT] Initializing PIC...\n");
    pic_init();
    vga_print("[INIT] PIC configured\n");

    vga_print("[INIT] Initializing PIT...\n");
    pit_init();
    vga_print("[INIT] Timer started (100Hz)\n");

    /* 创建进程 */
    vga_print("\n[INIT] Creating processes...\n");

    int pid_a = process_create(process_a, "ProcessA");
    vga_print("  Created Process A, PID=");
    vga_print_dec(pid_a);
    vga_print("\n");

    int pid_b = process_create(process_b, "ProcessB");
    vga_print("  Created Process B, PID=");
    vga_print_dec(pid_b);
    vga_print("\n");

    int pid_idle = process_create(idle_process, "Idle");
    vga_print("  Created Idle Process, PID=");
    vga_print_dec(pid_idle);
    vga_print("\n");

    /* 启用中断 */
    vga_print("\n[INIT] Enabling interrupts...\n");
    sti();

    vga_print("\n[KERNEL] System running!\n");
    vga_print("========================================\n\n");

    /* 进入第一个进程 */
    current_proc = &processes[0];
    current_proc->state = PROC_RUNNING;

    /* 跳转到进程入口 */
    __asm__ volatile (
        "mov %0, %%esp\n"
        "mov %0, %%ebp\n"
        "jmp *%1\n"
        :
        : "r"(current_proc->esp), "r"(current_proc->eip)
    );

    /* 永不返回 */
    while (1) {
        hlt();
    }
}

/* ============== 入口点 ============== */

void kernel_entry(void) {
    kernel_main();
}
```

### 4.3 编译脚本和运行

```bash
#!/bin/bash
# build.sh - 编译内核

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# 编译引导扇区
nasm -f bin boot.asm -o boot.bin

# 编译内核 (裸机，无标准库)
gcc -m32 -ffreestanding -nostdlib -nostartfiles \
    -fno-builtin -fno-exceptions -fno-rtti \
    -O2 -c kernel.c -o kernel.o

# 链接内核
ld -m elf_i386 -Ttext 0x8000 --oformat binary \
    -e kernel_entry -o kernel.bin kernel.o

# 创建软盘镜像 (1.44MB)
dd if=/dev/zero of=floppy.img bs=512 count=2880

# 写入引导扇区
dd if=boot.bin of=floppy.img bs=512 count=1 conv=notrunc

# 写入内核 (从第2扇区开始)
dd if=kernel.bin of=floppy.img bs=512 seek=1 conv=notrunc

echo "Build complete: floppy.img"
echo "Run with: qemu-system-i386 -fda floppy.img"
```

```bash
# 运行 (需要qemu)
qemu-system-i386 -fda floppy.img -serial stdio
```

---

## 5. 关键概念总结

```
┌─────────────────────────────────────────────────────────────┐
│                x86保护模式关键组件                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                  段式内存管理                         │   │
│  │  段选择子(16位) ──▶ GDT/LDT ──▶ 段描述符(64位)        │   │
│  │                                                      │   │
│  │  段描述符: 基址(32位) + 限长(20位) + 属性             │   │
│  │  属性: 存在(P)、特权级(DPL)、类型(S)、可执行(E)、可写(W) │   │
│  └─────────────────────────────────────────────────────┘   │
│                            │                                │
│                            ▼                                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                  页式内存管理 (可选)                  │   │
│  │                                                      │   │
│  │  线性地址 ──▶ 页目录(10位) ──▶ 页表(10位) ──▶ 页内偏移 │   │
│  │                      │             │                  │   │
│  │                      ▼             ▼                  │   │
│  │                  页目录项(PDE)   页表项(PTE)           │   │
│  │                  [物理页框地址]   [物理页框地址]        │   │
│  └─────────────────────────────────────────────────────┘   │
│                            │                                │
│                            ▼                                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                  中断和异常                           │   │
│  │                                                      │   │
│  │  中断来源: 外部中断(PIC) / 内部异常(CPU) / INT指令    │   │
│  │                                                      │   │
│  │  处理流程:                                            │   │
│  │  1. 获取中断向量号                                   │   │
│  │  2. IDT[向量号] ──▶ 段选择子:偏移地址                 │   │
│  │  3. 特权级检查 (CPL <= DPL)                          │   │
│  │  4. 保存上下文 (EFLAGS, CS, EIP)                      │   │
│  │  5. 执行中断处理程序                                  │   │
│  │  6. IRET恢复上下文                                    │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 总结

本案例展示了一个简化操作系统内核的完整实现，包括：

- **引导加载程序**：实模式到保护模式的切换
- **内存管理**：分页机制、堆分配器
- **中断处理**：IDT、PIC、定时器
- **进程调度**：轮转调度、上下文切换

该内核可作为学习操作系统原理的基础，进一步扩展包括：系统调用、文件系统、内存保护、多核支持等。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
