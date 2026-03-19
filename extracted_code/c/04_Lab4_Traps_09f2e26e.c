/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 456
 * Language: c
 * Block ID: 09f2e26e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
