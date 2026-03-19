/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\01_OS_Kernel.md
 * Line: 267
 * Language: c
 * Block ID: 3b61ae63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的优先级调度器

#define NR_PRIO 140
#define MAX_TASKS 1024

enum task_state {
    TASK_RUNNING = 0,
    TASK_INTERRUPTIBLE,
    TASK_UNINTERRUPTIBLE,
    TASK_STOPPED,
    TASK_ZOMBIE,
};

struct task_struct {
    pid_t pid;
    char comm[16];
    volatile enum task_state state;
    int prio;                    // 优先级 (0-139, 越低越优先)
    int static_prio;             // 静态优先级
    int nice;                    // nice值 (-20 to 19)

    struct list_head run_list;   // 运行队列链表
    struct list_head tasks;      // 所有任务链表

    uint64_t utime;              // 用户态时间
    uint64_t stime;              // 内核态时间
    uint64_t start_time;         // 启动时间

    struct mm_struct *mm;        // 内存描述符
    struct thread_info *thread_info;

    // 上下文
    struct cpu_context cpu_context;
};

struct runqueue {
    struct list_head queue[NR_PRIO];
    int nr_running;
    int curr_prio;               // 当前最高优先级
};

// 调度器主循环
void schedule(void) {
    struct task_struct *prev = current;
    struct task_struct *next;
    struct runqueue *rq = &cpu_rq(smp_processor_id());

    // 选择下一个任务（O(1)调度）
    int prio = sched_find_first_bit(rq->bitmap);
    next = list_entry(rq->queue[prio].next, struct task_struct, run_list);

    if (prev != next) {
        context_switch(prev, next);
    }
}

// 上下文切换（汇编实现）
__attribute__((naked)) void context_switch(struct task_struct *prev,
                                            struct task_struct *next) {
    // 保存prev上下文
    __asm__ __volatile__ (
        "pushq %%rbp\n\t"
        "pushq %%rbx\n\t"
        "pushq %%r12\n\t"
        "pushq %%r13\n\t"
        "pushq %%r14\n\t"
        "pushq %%r15\n\t"
        "movq %%rsp, %[prev_sp]\n\t"
        "movq %[next_sp], %%rsp\n\t"
        "popq %%r15\n\t"
        "popq %%r14\n\t"
        "popq %%r13\n\t"
        "popq %%r12\n\t"
        "popq %%rbx\n\t"
        "popq %%rbp\n\t"
        : [prev_sp] "=m" (prev->cpu_context.sp)
        : [next_sp] "m" (next->cpu_context.sp)
        : "memory"
    );
}
