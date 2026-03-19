/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 444
 * Language: c
 * Block ID: 7e05cab2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * x86特权指令列表
 * 这些指令只能在CPL=0时执行，否则会触发#GP异常
 */

// 控制寄存器操作
mov cr0, reg        // 修改CR0 (PG, PE等控制位)
mov reg, cr0        // 读取CR0
mov cr3, reg        // 切换页表 (TLB刷新)
mov cr4, reg        // 修改CR4 (PAE, PSE等)
mov cr8, reg        // 读取/修改TSS (任务优先级)

// 调试寄存器
mov dr0-dr7, reg    // 调试寄存器
mov reg, dr0-dr7

// MSR操作 (Model Specific Registers)
rdmsr               // 读取MSR (ECX=索引, EDX:EAX=值)
wrmsr               // 写入MSR

// 系统段/门操作
lgdt [mem]          // 加载GDT
lidt [mem]          // 加载IDT
lldt reg/mem        // 加载LDT
ltr reg/mem         // 加载任务寄存器

// 控制保护机制
cli                 // 清除中断使能 (IF=0)
sti                 // 设置中断使能 (IF=1)
in al/ax/eax, imm   // I/O端口输入 (如果IOPL < CPL)
out imm, al/ax/eax  // I/O端口输出
hlt                 // 停机
invlpg [mem]        // 使TLB条目失效

// 系统调用相关
swapgs              // 交换GS基址 (快速内核入口)
sysenter            // 快速系统调用 (Intel)
sysexit             // 快速系统调用返回 (Intel)
syscall             // 快速系统调用 (AMD/标准)
sysret              // 快速系统调用返回 (AMD/标准)

/*
 * Linux内核中的特权检查
 */

// 检查当前特权级别
static inline int user_mode(struct pt_regs *regs)
{
    return !!(regs->cs & 3);  // CPL = CS[1:0]
}

// 检查是否来自用户空间
#define user_mode(regs) (((regs)->cs & SEGMENT_RPL_MASK) == USER_RPL)

// 安全策略: 系统调用参数必须从用户空间复制
static inline bool access_ok(const void __user *addr, size_t size)
{
    // 检查地址是否在用户空间范围内
    return (unsigned long)addr < TASK_SIZE &&
           (unsigned long)addr + size >= (unsigned long)addr &&
           (unsigned long)addr + size <= TASK_SIZE;
}
