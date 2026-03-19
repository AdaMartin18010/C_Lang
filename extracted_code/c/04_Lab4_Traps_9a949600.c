/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 408
 * Language: c
 * Block ID: 9a949600
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
