# MIT 6.S081 Lab 4: Traps

## 1. 实验概述

### 1.1 目标

- 理解RISC-V的陷入(Trap)机制
- 掌握异常(Exception)、中断(Interrupt)和系统调用(System Call)的处理流程
- 实现用户态的backtrace功能
- 实现内核态的alarm功能

### 1.2 RISC-V陷入基础

```
RISC-V陷入类型:
├── 异常 (Exception) - 同步
│   ├── 指令地址未对齐
│   ├── 指令访问错误
│   ├── 非法指令
│   ├── 断点 (Breakpoint)
│   ├── 加载地址未对齐
│   ├── 加载访问错误
│   ├── 存储/AMO地址未对齐
│   ├── 存储/AMO访问错误
│   ├── 环境调用 (ECALL) - 来自U/S/M模式
│   └── 指令页面错误 / 加载页面错误 / 存储页面错误
│
└── 中断 (Interrupt) - 异步
    ├── 软件中断
    ├── 定时器中断
    └── 外部中断
```

### 1.3 关键控制寄存器

```
RISC-V陷入相关CSR寄存器:

sepc  (Supervisor Exception Program Counter)
    - 保存陷入时的PC值
    - sret指令会将PC设为sepc

scause (Supervisor Cause)
    - 记录陷入的原因
    - 最高位为1表示中断，0表示异常
    - 低63位表示具体的异常/中断代码

stval (Supervisor Trap Value)
    - 保存与陷入相关的额外信息
    - 例如：页面错误时的错误地址

sstatus (Supervisor Status)
    - SIE: 监督者中断使能
    - SPP: 陷入前的特权级别 (0=U, 1=S)
    - SPIE: 陷入前的中断使能状态

stvec (Supervisor Trap Vector Base Address)
    - 陷入向量表基地址
    - 可以配置为直接模式或向量模式

sscratch (Supervisor Scratch)
    - 临时寄存器，用于保存上下文
    - 通常指向进程的trapframe
```

---

## 2. 陷入处理流程

### 2.1 完整陷入流程

```
陷入发生时硬件自动完成:
┌─────────────────────────────────────────┐
│  1. 特权模式从U模式切换到S模式           │
│  2. 将PC保存到sepc                       │
│  3. 设置scause为陷入原因                 │
│  4. 设置stval (如有额外信息)              │
│  5. 将sstatus.SPP设为陷入前的特权级       │
│  6. 跳转到stvec指定的地址执行             │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│  软件处理 (kernel/trap.c):              │
│  1. 保存用户寄存器到trapframe            │
│  2. 切换到内核栈                         │
│  3. 调用trap()处理具体陷入               │
│  4. 恢复用户寄存器                       │
│  5. sret返回用户态                       │
└─────────────────────────────────────────┘
```

### 2.2 陷入入口代码 (kernel/kernelvec.S)

```asm
# kernel/kernelvec.S
# 内核陷入入口

.globl kerneltrap
.globl kernelvec
.align 4
kernelvec:
    # 保存所有寄存器到栈上
    addi sp, sp, -256

    sd ra, 0(sp)
    sd sp, 8(sp)
    sd gp, 16(sp)
    sd tp, 24(sp)
    sd t0, 32(sp)
    sd t1, 40(sp)
    sd t2, 48(sp)
    sd s0, 56(sp)
    sd s1, 64(sp)
    sd a0, 72(sp)
    sd a1, 80(sp)
    sd a2, 88(sp)
    sd a3, 96(sp)
    sd a4, 104(sp)
    sd a5, 112(sp)
    sd a6, 120(sp)
    sd a7, 128(sp)
    sd s2, 136(sp)
    sd s3, 144(sp)
    sd s4, 152(sp)
    sd s5, 160(sp)
    sd s6, 168(sp)
    sd s7, 176(sp)
    sd s8, 184(sp)
    sd s9, 192(sp)
    sd s10, 200(sp)
    sd s11, 208(sp)
    sd t3, 216(sp)
    sd t4, 224(sp)
    sd t5, 232(sp)
    sd t6, 240(sp)

    # 调用C语言陷入处理函数
    call kerneltrap

    # 恢复寄存器
    ld ra, 0(sp)
    ld sp, 8(sp)
    ld gp, 16(sp)
    ld tp, 24(sp)
    ld t0, 32(sp)
    ld t1, 40(sp)
    ld t2, 48(sp)
    ld s0, 56(sp)
    ld s1, 64(sp)
    ld a0, 72(sp)
    ld a1, 80(sp)
    ld a2, 88(sp)
    ld a3, 96(sp)
    ld a4, 104(sp)
    ld a5, 112(sp)
    ld a6, 120(sp)
    ld a7, 128(sp)
    ld s2, 136(sp)
    ld s3, 144(sp)
    ld s4, 152(sp)
    ld s5, 160(sp)
    ld s6, 168(sp)
    ld s7, 176(sp)
    ld s8, 184(sp)
    ld s9, 192(sp)
    ld s10, 200(sp)
    ld s11, 208(sp)
    ld t3, 216(sp)
    ld t4, 224(sp)
    ld t5, 232(sp)
    ld t6, 240(sp)

    addi sp, sp, 256

    # 返回
    sret
```

### 2.3 用户陷入入口 (kernel/trampoline.S)

```asm
# kernel/trampoline.S
# 用户陷入入口 - 在TRAMPOLINE页面映射

.section trampsec
.globl trampoline
trampoline:
.align 4
.globl uservec
uservec:
    # 从用户态陷入时，satp指向用户页表
    # sscratch指向进程的trapframe

    # 保存用户寄存器到trapframe
    csrrw a0, sscratch, a0    # 交换a0和sscratch
    sd ra, 40(a0)
    sd sp, 48(a0)
    sd gp, 56(a0)
    sd tp, 64(a0)
    sd t0, 72(a0)
    sd t1, 80(a0)
    sd t2, 88(a0)
    sd s0, 96(a0)
    sd s1, 104(a0)
    sd a1, 120(a0)
    sd a2, 128(a0)
    sd a3, 136(a0)
    sd a4, 144(a0)
    sd a5, 152(a0)
    sd a6, 160(a0)
    sd a7, 168(a0)
    sd s2, 176(a0)
    sd s3, 184(a0)
    sd s4, 192(a0)
    sd s5, 200(a0)
    sd s6, 208(a0)
    sd s7, 216(a0)
    sd s8, 224(a0)
    sd s9, 232(a0)
    sd s10, 240(a0)
    sd s11, 248(a0)
    sd t3, 256(a0)
    sd t4, 264(a0)
    sd t5, 272(a0)
    sd t6, 280(a0)

    # 切换页表为内核页表
    ld t0, 24(a0)             # t0 = trapframe->kernel_satp
    csrw satp, t0
    sfence.vma zero, zero

    # 进入C代码
    ld t0, 0(a0)              # t0 = trapframe->kernel_satp (函数指针)
    jr t0

.globl userret
userret:
    # 返回用户态
    # a0 = 用户页表, a1 = TRAPFRAME

    # 切换到用户页表
    csrw satp, a0
    sfence.vma zero, zero

    # 从trapframe恢复用户寄存器
    # ... (类似保存的逆过程)

    sret                      # 返回用户态
```

---

## 3. Backtrace实现

### 3.1 任务描述

实现一个backtrace()函数，打印函数调用栈。这有助于调试时了解程序执行路径。

### 3.2 RISC-V栈帧结构

```
RISC-V栈帧布局:
高地址
┌─────────────────┐
│  返回地址 (ra)   │ <- fp + 8 (调用者的返回地址)
├─────────────────┤
│  前一个fp       │ <- fp (指向调用者的栈帧)
├─────────────────┤
│  局部变量       │
│  保存的寄存器   │
│  ...            │
├─────────────────┤
│  参数传递区     │ <- sp (当前栈指针)
└─────────────────┘
低地址

调用约定:
- fp (s0/x8): 帧指针，指向当前栈帧的保存位置
- ra (x1): 返回地址
- 每个函数序言保存: ra, fp 到栈中
- 然后设置: fp = sp + 16
```

### 3.3 Backtrace实现代码

```c
// kernel/printf.c

#include "kernel/riscv.h"
#include "kernel/defs.h"

// 获取当前帧指针
static inline uint64
r_fp()
{
    uint64 x;
    asm volatile("mv %0, s0" : "=r" (x));
    return x;
}

void
backtrace(void)
{
    printf("backtrace:\n");

    // 获取当前帧指针
    uint64 fp = r_fp();

    // 栈范围检查 (确保不越界)
    // PGROUNDUP用于获取栈顶，栈向下增长
    uint64 stack_top = PGROUNDUP(fp);

    while (fp < stack_top && fp > stack_top - PGSIZE) {
        // 获取返回地址: 保存在fp + 8处
        uint64 ra = *(uint64*)(fp - 8);

        printf("%p\n", ra);

        // 移动到上一帧
        fp = *(uint64*)(fp - 16);
    }
}
```

### 3.4 在panic中调用

```c
// kernel/printf.c
void
panic(char *s)
{
    // ... 原有代码 ...

    // 打印调用栈
    backtrace();

    // ... 原有代码 ...
}
```

### 3.5 测试backtrace

```c
// user/bttest.c (测试程序)
#include "kernel/types.h"
#include "user/user.h"

void f3() {
    backtrace();  // 应该看到f3, f2, f1, main的调用链
}

void f2() {
    f3();
}

void f1() {
    f2();
}

int main() {
    f1();
    exit(0);
}
```

---

## 4. Alarm系统调用实现

### 4.1 任务描述

实现`sigalarm(interval, handler)`和`sigreturn()`系统调用，允许进程周期性执行处理函数。

### 4.2 设计思路

```
sigalarm工作原理:
1. 设置alarm间隔(ticks)和处理函数指针
2. 每次时钟中断，检查是否到达间隔
3. 到达时，保存当前trapframe，修改返回地址为handler
4. handler执行完成后，调用sigreturn恢复原状态

关键问题:
- 防止重入: 当handler正在执行时，不应触发新的alarm
- 正确恢复: 必须完全恢复原trapframe，包括所有寄存器
```

### 4.3 数据结构扩展

```c
// kernel/proc.h

struct proc {
    // ... 原有字段 ...

    // Alarm相关字段
    int alarm_interval;        // alarm间隔(ticks)
    void (*alarm_handler)();   // alarm处理函数
    int alarm_ticks;           // 距离下次alarm的ticks数
    int alarm_goingoff;        // 是否正在执行handler (防止重入)
    struct trapframe *alarm_trapframe;  // 保存的trapframe副本
};
```

### 4.4 系统调用实现

```c
// kernel/sysproc.c

uint64
sys_sigalarm(void)
{
    int interval;
    uint64 handler;

    // 获取参数
    if (argint(0, &interval) < 0)
        return -1;
    if (argaddr(1, &handler) < 0)
        return -1;

    struct proc *p = myproc();

    // 设置alarm
    p->alarm_interval = interval;
    p->alarm_handler = (void(*)())handler;
    p->alarm_ticks = interval;  // 初始化为间隔值
    p->alarm_goingoff = 0;

    return 0;
}

uint64
sys_sigreturn(void)
{
    struct proc *p = myproc();

    // 恢复原来的trapframe
    if (p->alarm_trapframe) {
        memmove(p->trapframe, p->alarm_trapframe, sizeof(struct trapframe));
        kfree((void*)p->alarm_trapframe);
        p->alarm_trapframe = 0;
    }

    // 重置alarm_goingoff标志
    p->alarm_goingoff = 0;

    // 返回原trapframe中的a0值
    return p->trapframe->a0;
}
```

### 4.5 修改usertrap

```c
// kernel/trap.c

void
usertrap(void)
{
    // ... 原有代码 ...

    // 处理系统调用
    if (r_scause() == 8) {
        // 系统调用
        // ... 原有代码 ...
    }

    // ... 其他异常处理 ...
}

// 时钟中断处理 (在usertrapret中调用或时钟中断时)
void
usertrapret(void)
{
    struct proc *p = myproc();

    // 检查是否需要触发alarm
    if (p->alarm_interval > 0 && !p->alarm_goingoff) {
        p->alarm_ticks--;

        if (p->alarm_ticks <= 0) {
            // 重置计数器
            p->alarm_ticks = p->alarm_interval;

            // 保存trapframe
            p->alarm_trapframe = (struct trapframe *)kalloc();
            if (p->alarm_trapframe) {
                memmove(p->alarm_trapframe, p->trapframe,
                        sizeof(struct trapframe));

                // 设置handler为返回地址
                p->trapframe->epc = (uint64)p->alarm_handler;

                // 标记正在执行handler
                p->alarm_goingoff = 1;
            }
        }
    }

    // ... 原有返回代码 ...
}
```

### 4.6 用户态测试程序

```c
// user/alarmtest.c
#include "kernel/types.h"
#include "user/user.h"

int ticks = 0;

void alarm_handler() {
    ticks++;
    printf("alarm! ticks=%d\n", ticks);
    sigreturn();
}

int main() {
    // 每10个ticks触发一次alarm
    sigalarm(10, alarm_handler);

    // 执行一些计算
    for (int i = 0; i < 100000000; i++) {
        // 消耗时间
    }

    printf("total alarms: %d\n", ticks);
    exit(0);
}
```

---

## 5. 陷入处理的关键代码

### 5.1 异常处理函数

```c
// kernel/trap.c

void
usertrap(void)
{
    int which_dev = 0;

    // 确认是从用户态陷入
    if ((r_sstatus() & SSTATUS_SPP) != 0)
        panic("usertrap: not from user mode");

    // 发送中断到stvec
    w_stvec((uint64)kernelvec);

    struct proc *p = myproc();
    p->trapframe->epc = r_sepc();

    uint64 cause = r_scause();

    if (cause == 8) {
        // 系统调用
        if (killed(p))
            exit(-1);

        p->trapframe->epc += 4;  // 跳过ecall指令
        intr_on();  // 系统调用期间允许中断
        syscall();
    } else if ((which_dev = devintr()) != 0) {
        // 设备中断
        // ok
    } else {
        // 未处理的异常
        printf("usertrap(): unexpected scause %p pid=%d\n",
               cause, p->pid);
        printf("            sepc=%p stval=%p\n",
               r_sepc(), r_stval());
        setkilled(p);
    }

    if (killed(p))
        exit(-1);

    usertrapret();
}

void
usertrapret(void)
{
    struct proc *p = myproc();

    // 关闭中断直到返回用户态
    intr_off();

    // 设置stvec为用户陷入向量
    w_stvec(TRAMPOLINE + (uservec - trampoline));

    // 设置trapframe字段
    p->trapframe->kernel_satp = r_satp();
    p->trapframe->kernel_sp = p->kstack + PGSIZE;
    p->trapframe->kernel_trap = (uint64)usertrap;
    p->trapframe->kernel_hartid = r_tp();

    // 设置sstatus
    unsigned long x = r_sstatus();
    x &= ~SSTATUS_SPP;  // 清除SPP位，下次返回U模式
    x |= SSTATUS_SPIE;  // 启用用户态中断
    w_sstatus(x);

    // 设置返回地址
    w_sepc(p->trapframe->epc);

    // 复制寄存器到用户空间
    uint64 satp = MAKE_SATP(p->pagetable);

    // 跳转到trampoline.S中的userret
    uint64 fn = TRAMPOLINE + (userret - trampoline);
    ((void (*)(uint64, uint64))fn)(TRAPFRAME, satp);
}
```

### 5.2 设备中断处理

```c
// kernel/trap.c

int
devintr(void)
{
    uint64 scause = r_scause();

    if ((scause & 0x8000000000000000L) &&
        (scause & 0xff) == 9) {
        //  supervisor外部中断 (PLIC)
        int irq = plic_claim();

        if (irq == UART0_IRQ) {
            uartintr();
        } else if (irq == VIRTIO0_IRQ) {
            virtio_disk_intr();
        } else if (irq) {
            printf("unexpected interrupt irq=%d\n", irq);
        }

        if (irq)
            plic_complete(irq);

        return 1;
    } else if (scause == 0x8000000000000001L) {
        //  supervisor定时器中断
        if (cpuid() == 0) {
            clockintr();
        }
        w_sip(r_sip() & ~2);  // 清除中断
        return 2;
    }

    return 0;
}
```

---

## 6. 调试技巧与常见问题

### 6.1 常见错误

```
错误1: panic("usertrap: not from user mode")
原因: 系统调用处理时sstatus.SPP设置错误
解决: 检查是否从正确的模式陷入

错误2: 时钟中断不触发
原因: sstatus.SIE未启用或stvec设置错误
解决: 确保中断使能位正确设置

错误3: alarm不触发或触发多次
原因: alarm_ticks计算错误或重入保护不当
解决: 检查递减逻辑和alarm_goingoff标志

错误4: sigreturn后程序崩溃
原因: trapframe恢复不完整
解决: 确保所有寄存器正确恢复
```

### 6.2 调试命令

```bash
# 在陷入处设置断点
(gdb) b usertrap
(gdb) b kerneltrap

# 查看陷入原因
(gdb) p/x $scause

# 查看陷入地址
(gdb) p/x $sepc

# 查看trapframe
(gdb) p *myproc()->trapframe

# 单步执行陷入处理
(gdb) stepi
```

### 6.3 打印陷入信息

```c
// 在usertrap中添加调试输出
void usertrap() {
    uint64 cause = r_scause();
    printf("trap: cause=%p epc=%p stval=%p\n",
           cause, r_sepc(), r_stval());
    // ...
}
```

---

## 7. 总结

### 7.1 核心概念回顾

| 概念 | 说明 |
|------|------|
| **陷入(Trap)** | 从用户态切换到内核态的机制 |
| **异常(Exception)** | 同步陷入，由指令执行触发 |
| **中断(Interrupt)** | 异步陷入，由外部事件触发 |
| **系统调用** | 用户请求内核服务的ecall指令 |
| **上下文保存** | 保存/恢复用户态寄存器状态 |
| **栈帧** | 函数调用的栈结构，用于backtrace |

### 7.2 关键学习点

1. **RISC-V陷入机制**: 理解硬件自动完成的操作和软件需要处理的工作
2. **trapframe**: 用户态上下文的保存和恢复
3. **trampoline页面**: 用户/内核态切换的关键
4. **栈帧结构**: 理解编译器生成的函数调用约定

---

**难度**: ⭐⭐⭐
**预计时间**: 8-10小时
**依赖**: Lab 3 (页表)


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
