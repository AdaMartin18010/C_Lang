/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\06_Lab6_Multithreading.md
 * Line: 162
 * Language: c
 * Block ID: 546833d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/uthread.c

void
thread_schedule(void)
{
    struct thread *t, *next_thread;

    // 找到下一个可运行的线程
    next_thread = 0;
    t = current_thread + 1;

    for (int i = 0; i < MAX_THREAD; i++) {
        if (t >= threads + MAX_THREAD)
            t = threads;
        if (t->state == RUNNABLE && t != current_thread) {
            next_thread = t;
            break;
        }
        t++;
    }

    if (next_thread == 0) {
        printf("thread_schedule: no runnable threads\n");
        exit(-1);
    }

    if (current_thread != next_thread) {
        struct thread *prev = current_thread;
        next_thread->state = RUNNING;
        current_thread = next_thread;

        // 上下文切换
        thread_switch(&prev->context, &next_thread->context);
    }
}
