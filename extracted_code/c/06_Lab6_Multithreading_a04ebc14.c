/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 202
 * Language: c
 * Block ID: a04ebc14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/uthread.c

void
thread_create(void (*func)())
{
    struct thread *t;

    // 寻找空闲线程槽
    for (t = threads; t < threads + MAX_THREAD; t++) {
        if (t->state == FREE) break;
    }

    if (t >= threads + MAX_THREAD) {
        printf("thread_create: no free slots\n");
        exit(-1);
    }

    // 初始化上下文
    memset(&t->context, 0, sizeof(t->context));

    // 设置栈指针 (栈从高地址向低地址增长)
    t->context.sp = (uint64)(t->stack + STACK_SIZE);

    // 设置返回地址为线程函数
    t->context.ra = (uint64)func;

    t->state = RUNNABLE;
}

void
thread_yield(void)
{
    current_thread->state = RUNNABLE;
    thread_schedule();
}
