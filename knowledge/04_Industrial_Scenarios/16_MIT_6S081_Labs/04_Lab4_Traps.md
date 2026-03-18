# MIT 6.S081 Lab 4: Traps

## 1. 实验概述

### 1.1 目标

- 理解RISC-V陷入机制
- 掌握用户态到内核态的切换
- 实现RISC-V汇编函数
- 理解调用栈布局

### 1.2 RISC-V陷入类型

```
陷入(Trap)类型:
├── 异常 (Exception) - 同步
│   ├── 指令页错误 (Instruction Page Fault)
│   ├── 加载页错误 (Load Page Fault)
│   ├── 存储页错误 (Store Page Fault)
│   ├── 非法指令 (Illegal Instruction)
│   ├── 断点 (Breakpoint)
│   └── 环境调用 (Environment Call)
│
└── 中断 (Interrupt) - 异步
    ├── 软件中断 (Software Interrupt)
    ├── 定时器中断 (Timer Interrupt)
    └── 外部中断 (External Interrupt)

xv6使用的陷入:
- ecall (用户态系统调用)
- 定时器中断 (进程调度)
- 页错误 (内存管理)
```

---

## 2. RISC-V陷入处理流程

### 2.1 陷入发生时的硬件行为

```
当ecall指令执行时:

1. 保存PC到sepc (Supervisor Exception PC)
2. 设置scause (陷入原因)
3. 保存stval (陷入相关信息)
4. 切换模式到S-mode
5. PC = stvec (陷入向量基地址)

用户态                    内核态
   │                        │
   │ ecall                  │
   ├───────────────────────>│
   │                        │
   │                        │ 1. 保存上下文
   │                        │ 2. 处理系统调用
   │                        │ 3. 恢复上下文
   │ sret                   │
   │<───────────────────────┤
   │                        │
```

### 2.2 xv6陷入处理代码

```c
// kernel/trap.c
void usertrap(void) {
    // 检查是否来自用户态
    if ((r_sstatus() & SSTATUS_SPP) != 0)
        panic("usertrap: not from user mode");

    // 发送中断到其他CPU
    w_stvec((uint64)kernelvec);

    struct proc *p = myproc();

    // 保存用户程序计数器
    p->trapframe->epc = r_sepc();

    // 检查陷入原因
    if (r_scause() == 8) {
        // 系统调用
        if (killed(p))
            exit(-1);

        // sepc指向下一条指令
        p->trapframe->epc += 4;

        // 启用中断
        intr_on();

        // 执行系统调用
        syscall();
    } else if ((which_dev = devintr()) != 0) {
        // 设备中断
        // OK
    } else {
        // 异常
        printf("usertrap(): unexpected scause %p pid=%d\n",
               r_scause(), p->pid);
        printf("            sepc=%p stval=%p\n",
               r_sepc(), r_stval());
        setkilled(p);
    }

    usertrapret();
}

void usertrapret(void) {
    struct proc *p = myproc();

    // 关闭中断
    intr_off();

    // 设置陷入向量为用户态处理
    w_stvec(TRAMPOLINE + (uservec - trampoline));

    // 设置trapframe字段
    p->trapframe->kernel_satp = r_satp();
    p->trapframe->kernel_sp = p->kstack + PGSIZE;
    p->trapframe->kernel_trap = (uint64)usertrap;
    p->trapframe->kernel_hartid = r_tp();

    // 设置sstatus
    unsigned long x = r_sstatus();
    x &= ~SSTATUS_SPP;  // 设置下一条sret进入用户态
    x |= SSTATUS_SPIE;  // 用户态启用中断
    w_sstatus(x);

    // 设置sepc为保存的用户PC
    w_sepc(p->trapframe->epc);

    // 计算satp值
    uint64 satp = MAKE_SATP(p->pagetable);

    // 跳转到userret
    uint64 fn = TRAMPOLINE + (userret - trampoline);
    ((void (*)(uint64, uint64))fn)(TRAPFRAME, satp);
}
```

---

## 3. Backtrace实现

### 3.1 任务

实现`backtrace()`函数，打印函数调用栈。

### 3.2 RISC-V栈帧布局

```
高地址
┌─────────────────┐
│  返回地址 (ra)  │ ← fp + 8
├─────────────────┤
│  上一个fp       │ ← fp (当前帧指针)
├─────────────────┤
│  局部变量       │
│  ...            │
├─────────────────┤
│  保存的寄存器   │
├─────────────────┤
│  返回地址       │
│  上一个fp       │ ← fp - 16 (上一个帧)
└─────────────────┘
低地址
```

### 3.3 代码实现

```c
// kernel/printf.c

// 获取当前帧指针
static inline uint64 r_fp() {
    uint64 x;
    asm volatile("mv %0, s0" : "=r" (x));
    return x;
}

void backtrace(void) {
    uint64 fp = r_fp();  // 当前帧指针
    uint64 top = PGROUNDUP(fp);  // 栈顶

    printf("backtrace:\n");

    // 遍历栈帧
    while (fp < top) {
        // 返回地址在fp - 8
        uint64 ra = *(uint64*)(fp - 8);
        printf("%p\n", ra);

        // 移动到上一个帧
        fp = *(uint64*)(fp - 16);
    }
}
```

### 3.4 使用

```c
// 在panic或调试时调用
void panic(char *s) {
    printf("panic: ");
    printf(s);
    printf("\n");
    backtrace();  // 打印调用栈
    panicked = 1;
    for (;;)
        ;
}
```

---

## 4. Alarm实现

### 4.1 任务

实现`sigalarm`和`sigreturn`系统调用，实现定期调用用户处理函数。

### 4.2 设计

```
sigalarm(n, fn):
- 每隔n个ticks调用fn
- 保存当前执行上下文
- 调用fn
- 恢复上下文

数据结构:
struct proc {
    // ... 原有字段 ...
    int alarm_interval;      // 报警间隔
    uint64 alarm_handler;    // 处理函数地址
    int alarm_ticks;         // 当前计数
    struct trapframe *alarm_trapframe;  // 保存的上下文
    int alarm_goingoff;      // 是否正在处理
};
```

### 4.3 系统调用实现

```c
// kernel/sysproc.c

uint64 sys_sigalarm(void) {
    int interval;
    uint64 handler;
    struct proc *p = myproc();

    if (argint(0, &interval) < 0)
        return -1;
    if (argaddr(1, &handler) < 0)
        return -1;

    p->alarm_interval = interval;
    p->alarm_handler = handler;
    p->alarm_ticks = 0;

    return 0;
}

uint64 sys_sigreturn(void) {
    struct proc *p = myproc();

    // 恢复原始上下文
    *p->trapframe = *p->alarm_trapframe;
    p->alarm_goingoff = 0;

    return 0;
}
```

### 4.4 定时器中断处理

```c
// kernel/trap.c

void usertrap(void) {
    // ... 原有代码 ...

    // 处理定时器中断
    if (which_dev == 2) {
        struct proc *p = myproc();

        // 检查是否设置了alarm
        if (p->alarm_interval > 0 && !p->alarm_goingoff) {
            p->alarm_ticks++;

            if (p->alarm_ticks >= p->alarm_interval) {
                p->alarm_ticks = 0;
                p->alarm_goingoff = 1;

                // 保存当前上下文
                *p->alarm_trapframe = *p->trapframe;

                // 修改返回地址为处理函数
                p->trapframe->epc = p->alarm_handler;
            }
        }

        yield();
    }

    usertrapret();
}
```

### 4.5 用户态测试

```c
// user/alarmtest.c

#include "kernel/types.h"
#include "user/user.h"

volatile int alarm_count = 0;

void alarm_handler() {
    alarm_count++;
    printf("alarm! count=%d\n", alarm_count);
    sigreturn();
}

int main() {
    // 每10个ticks调用alarm_handler
    sigalarm(10, alarm_handler);

    // 长时间运行
    for (int i = 0; i < 100000000; i++) {
        if (i % 10000000 == 0) {
            printf("working... %d\n", i);
        }
    }

    printf("done. alarm_count=%d\n", alarm_count);
    exit(0);
}
```

---

## 5. RISC-V汇编基础

### 5.1 常用指令

```asm
# 寄存器操作
mv rd, rs       # rd = rs
li rd, imm      # rd = immediate

# 内存操作
lw rd, offset(rs)   # rd = mem[rs + offset]
sw rs, offset(rd)   # mem[rd + offset] = rs
ld/sd               # 64位版本

# 算术运算
add rd, rs1, rs2    # rd = rs1 + rs2
sub rd, rs1, rs2    # rd = rs1 - rs2
addi rd, rs, imm    # rd = rs + immediate

# 分支跳转
beq rs1, rs2, label    # if rs1 == rs2, jump
bne rs1, rs2, label    # if rs1 != rs2, jump
jal rd, label          # rd = pc + 4, jump to label
jr rs                  # jump to rs

# 特权指令
ecall               # 环境调用 (系统调用)
sret                # 从supervisor返回
```

### 5.2 xv6汇编示例

```asm
# kernel/trampoline.S
# 用户态陷入处理

.globl trampoline
trampoline:
.align 4
.globl uservec
uservec:
    # 保存用户寄存器到trapframe
    sd ra, 40(a0)
    sd sp, 48(a0)
    sd gp, 56(a0)
    # ... 保存其他寄存器

    # 恢复内核栈指针
    ld sp, 8(a0)

    # 跳转到内核处理
    ld t0, 16(a0)
    jr t0

.globl userret
userret:
    # 恢复用户寄存器
    ld ra, 40(a0)
    ld sp, 48(a0)
    # ... 恢复其他寄存器

    # 返回用户态
    sret
```

---

## 6. 调试技巧

### 6.1 查看寄存器

```bash
# GDB调试
(gdb) info registers      # 查看所有寄存器
(gdb) p/x $sepc          # 查看异常PC
(gdb) p/x $scause        # 查看异常原因
(gdb) p/x $stval         # 查看异常值
(gdb) p/x $sstatus       # 查看状态
```

### 6.2 常见问题

```
问题1: sepc指向错误地址
原因: 未正确保存/恢复epc
解决: 检查trapframe.epc处理

问题2: 陷入后死机
原因: 栈溢出或页表错误
解决: 检查kstack分配

问题3: alarm不触发
原因: 定时器中断未开启
解决: 检查init.c中的timerinit()
```

---

**难度**: ⭐⭐⭐
**预计时间**: 6-8小时
