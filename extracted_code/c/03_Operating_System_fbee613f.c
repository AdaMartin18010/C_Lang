/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\03_Operating_System.md
 * Line: 452
 * Language: c
 * Block ID: fbee613f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
