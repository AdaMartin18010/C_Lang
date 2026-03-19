/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 220
 * Language: c
 * Block ID: 3598ec95
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define MAX_COROUTINES 1024
#define DEFAULT_STACK (64 * 1024)

typedef struct coroutine {
    int id;
    context_t ctx;
    void* stack;
    size_t stack_size;
    void (*fn)(void*);
    void* arg;
    enum { READY, RUNNING, SUSPENDED, DEAD } status;
    struct coroutine* next;
} coroutine;

static coroutine* coroutines[MAX_COROUTINES];
static coroutine* current_co = NULL;
static coroutine* ready_queue_head = NULL;
static coroutine* ready_queue_tail = NULL;
static int next_id = 0;
static context_t main_context;

/* 调度器 */
void schedule(void) {
    while (ready_queue_head != NULL) {
        /* 取出就绪协程 */
        coroutine* co = ready_queue_head;
        ready_queue_head = co->next;
        if (ready_queue_head == NULL) {
            ready_queue_tail = NULL;
        }

        if (co->status == DEAD) continue;

        /* 执行 */
        current_co = co;
        co->status = RUNNING;
        context_switch(&main_context, &co->ctx);

        /* 协程挂起或完成 */
        if (co->status != DEAD) {
            co->status = SUSPENDED;
        }
    }
    current_co = NULL;
}

/* 创建 */
int co_spawn(void (*fn)(void*), void* arg) {
    int id = next_id++;
    coroutine* co = malloc(sizeof(coroutine));

    co->id = id;
    co->fn = fn;
    co->arg = arg;
    co->status = READY;
    co->stack_size = DEFAULT_STACK;
    co->stack = malloc(co->stack_size);
    co->next = NULL;

    /* 初始化上下文 */
    co->ctx.rsp = (char*)co->stack + co->stack_size - sizeof(void*);
    co->ctx.rip = (void*)fn;
    co->ctx.rbp = NULL;

    coroutines[id] = co;

    /* 加入就绪队列 */
    if (ready_queue_tail) {
        ready_queue_tail->next = co;
    } else {
        ready_queue_head = co;
    }
    ready_queue_tail = co;

    return id;
}

/* 挂起 */
void co_yield(void) {
    if (current_co == NULL) return;

    coroutine* co = current_co;
    current_co = NULL;

    /* 保存当前上下文，切换到主上下文 */
    context_switch(&co->ctx, &main_context);
}

/* 恢复 */
void co_resume_id(int id) {
    coroutine* co = coroutines[id];
    if (co == NULL || co->status == DEAD) return;

    /* 加入就绪队列 */
    co->status = READY;
    if (ready_queue_tail) {
        ready_queue_tail->next = co;
    } else {
        ready_queue_head = co;
    }
    ready_queue_tail = co;
}

/* 完成 */
void co_done(void) {
    if (current_co) {
        current_co->status = DEAD;
        context_switch(&current_co->ctx, &main_context);
    }
}
