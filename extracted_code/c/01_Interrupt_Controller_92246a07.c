/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 651
 * Language: c
 * Block ID: 92246a07
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * x86_64 IDT门描述符结构
 * arch/x86/include/asm/desc_defs.h
 */

// 32位中断门描述符 (8字节)
struct gate_struct32 {
    u16 offset_low;
    u16 segment;
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    u16 offset_high;
} __attribute__((packed));

// 64位中断门描述符 (16字节)
struct gate_struct64 {
    u16 offset_low;
    u16 segment;
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    u16 offset_middle;
    u32 offset_high;
    u32 zero1;
} __attribute__((packed));

// IDT描述符类型
#define GATE_INTERRUPT  0xE     // 中断门 (禁止IF)
#define GATE_TRAP       0xF     // 陷阱门 (不禁止IF)
#define GATE_CALL       0xC     // 调用门
#define GATE_TASK       0x5     // 任务门

// Linux内核IDT初始化
typedef struct gate_struct gate_desc;

// 设置IDT门描述符的宏
#define _set_gate(gate_addr, type, dpl, addr, ist)          \
{                                                           \
    gate_desc *gate = (gate_desc *)(gate_addr);             \
    u64 a = (u64)(addr);                                    \
    gate->offset_low = (u16)(a);                            \
    gate->segment = __KERNEL_CS;                            \
    gate->ist = ist;                                        \
    gate->p = 1;                                            \
    gate->dpl = (dpl);                                      \
    gate->zero0 = 0;                                        \
    gate->type = (type);                                    \
    gate->offset_middle = (u16)((a) >> 16);                 \
    gate->offset_high = (u32)((a) >> 32);                   \
    gate->zero1 = 0;                                        \
}

// 设置中断门
#define set_intr_gate_notrace(n, addr)                      \
    _set_gate(&idt_table[n], GATE_INTERRUPT, 0, addr, 0)

// 设置系统门 (允许用户态调用，如int 0x80)
#define set_system_intr_gate(n, addr)                       \
    _set_gate(&idt_table[n], GATE_INTERRUPT, 3, addr, 0)

// 设置陷阱门
#define set_trap_gate(n, addr)                              \
    _set_gate(&idt_table[n], GATE_TRAP, 0, addr, 0)
