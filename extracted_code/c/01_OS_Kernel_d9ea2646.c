/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\01_OS_Kernel.md
 * Line: 352
 * Language: c
 * Block ID: d9ea2646
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// IDT（中断描述符表）设置

#define IDT_ENTRIES 256

struct idt_entry {
    uint16_t offset_low;    // 处理函数低16位
    uint16_t selector;      // 代码段选择子
    uint8_t  ist;           // 中断栈表
    uint8_t  type_attr;     // 类型和属性
    uint16_t offset_mid;    // 中16位
    uint32_t offset_high;   // 高32位
    uint32_t zero;          // 保留
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

// 设置IDT条目
void idt_set_gate(int num, uint64_t handler, uint16_t sel, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    idt[num].type_attr = flags;
    idt[num].zero = 0;
}

// 通用中断处理入口（汇编）
__attribute__((naked)) void interrupt_handler_entry(void) {
    __asm__ (
        "pushq %rax\n\t"
        "pushq %rcx\n\t"
        "pushq %rdx\n\t"
        "pushq %rsi\n\t"
        "pushq %rdi\n\t"
        "pushq %r8\n\t"
        "pushq %r9\n\t"
        "pushq %r10\n\t"
        "pushq %r11\n\t"
        "movq %rsp, %rdi\n\t"  // 参数：寄存器帧
        "call do_interrupt_handler\n\t"
        "popq %r11\n\t"
        "popq %r10\n\t"
        "popq %r9\n\t"
        "popq %r8\n\t"
        "popq %rdi\n\t"
        "popq %rsi\n\t"
        "popq %rdx\n\t"
        "popq %rcx\n\t"
        "popq %rax\n\t"
        "iretq"
    );
}

// C语言中断处理
void do_interrupt_handler(struct pt_regs *regs) {
    int irq = regs->irq;

    // 发送EOI（结束中断）给PIC/APIC
    if (irq >= 32) {
        lapic_eoi();
    }

    // 调用注册的处理函数
    if (irq_handlers[irq]) {
        irq_handlers[irq](regs);
    }
}
