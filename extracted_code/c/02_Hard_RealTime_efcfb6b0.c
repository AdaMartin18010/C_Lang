/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 326
 * Language: c
 * Block ID: efcfb6b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 优先级抢占式调度器
// 固定优先级调度 (Rate Monotonic)
// ============================================================================

#define MAX_TASKS       32
#define IDLE_TASK_ID    0

// 就绪队列
typedef struct {
    TaskControlBlock *tasks[MAX_TASKS];
    uint8_t count;
} ReadyQueue;

// 调度器上下文
typedef struct {
    TaskControlBlock tcbs[MAX_TASKS];
    TaskControlBlock *current_task;
    TaskControlBlock *next_task;
    ReadyQueue ready_queue;
    uint32_t system_tick_ms;
    bool scheduling_locked;
} SchedulerContext;

// 调度锁 - 防止关键区被抢占
void lock_scheduling(SchedulerContext *ctx) {
    disable_interrupts();
    ctx->scheduling_locked = true;
}

void unlock_scheduling(SchedulerContext *ctx) {
    ctx->scheduling_locked = false;
    enable_interrupts();
    // 触发重新调度
    request_reschedule();
}

// ============================================================================
// 就绪队列管理 (按优先级排序)
// ============================================================================

void ready_queue_insert(ReadyQueue *rq, TaskControlBlock *tcb) {
    // 按优先级插入 (数值小的优先级高)
    int i;
    for (i = rq->count; i > 0; i--) {
        if (rq->tasks[i-1]->priority <= tcb->priority) {
            break;
        }
        rq->tasks[i] = rq->tasks[i-1];
    }
    rq->tasks[i] = tcb;
    rq->count++;
}

TaskControlBlock* ready_queue_get_highest(ReadyQueue *rq) {
    if (rq->count == 0) return NULL;
    return rq->tasks[0];
}

void ready_queue_remove(ReadyQueue *rq, TaskControlBlock *tcb) {
    for (int i = 0; i < rq->count; i++) {
        if (rq->tasks[i] == tcb) {
            for (int j = i; j < rq->count - 1; j++) {
                rq->tasks[j] = rq->tasks[j+1];
            }
            rq->count--;
            return;
        }
    }
}

// ============================================================================
// 调度决策
// ============================================================================

void schedule(SchedulerContext *ctx) {
    if (ctx->scheduling_locked) return;

    // 获取最高优先级就绪任务
    TaskControlBlock *highest = ready_queue_get_highest(&ctx->ready_queue);

    if (highest == NULL) {
        // 无就绪任务，运行空闲任务
        highest = &ctx->tcbs[IDLE_TASK_ID];
    }

    // 检查是否需要切换
    if (highest != ctx->current_task) {
        // 优先级比较
        if (ctx->current_task == NULL ||
            highest->priority < ctx->current_task->priority) {
            ctx->next_task = highest;
            context_switch(ctx);
        }
    }
}

// ============================================================================
// 上下文切换 (汇编实现或编译器内置)
// ============================================================================

void context_switch(SchedulerContext *ctx) {
    TaskControlBlock *from = ctx->current_task;
    TaskControlBlock *to = ctx->next_task;

    // 保存当前任务上下文
    if (from != NULL && from->state == TASK_RUNNING) {
        save_cpu_context(&from->stack_pointer);
        from->state = TASK_READY;
        ready_queue_insert(&ctx->ready_queue, from);
    }

    // 恢复新任务上下文
    ctx->current_task = to;
    to->state = TASK_RUNNING;
    ready_queue_remove(&ctx->ready_queue, to);

    // 更新时间统计
    to->last_start_time = ctx->system_tick_ms;

    // 恢复CPU上下文
    restore_cpu_context(&to->stack_pointer);
}

// 汇编实现示例 (概念)
/*
save_cpu_context:
    PUSH    R0-R12, LR
    STR     SP, [R0]        ; 保存栈指针到tcb
    BX      LR

restore_cpu_context:
    LDR     SP, [R0]        ; 恢复栈指针
    POP     R0-R12, LR
    BX      LR
*/
