/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 541
 * Language: c
 * Block ID: b4dfc553
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
