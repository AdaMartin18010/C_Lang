/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 76
 * Language: c
 * Block ID: b2b212c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define _XOPEN_SOURCE 600
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>

/* ucontext已被标记为废弃，但广泛用于教学 */

typedef struct {
    ucontext_t ctx;
    void (*fn)(void*);
    void* arg;
    void* stack;
    size_t stack_size;
    enum { CO_READY, CO_RUNNING, CO_SUSPENDED, CO_DEAD } status;
} coroutine_t;

static ucontext_t main_ctx;
static coroutine_t* current = NULL;

coroutine_t* co_create(void (*fn)(void*), void* arg, size_t stack_size) {
    coroutine_t* co = malloc(sizeof(coroutine_t));
    co->fn = fn;
    co->arg = arg;
    co->stack_size = stack_size ? stack_size : 64 * 1024;
    co->stack = malloc(co->stack_size);
    co->status = CO_READY;

    getcontext(&co->ctx);
    co->ctx.uc_stack.ss_sp = co->stack;
    co->ctx.uc_stack.ss_size = co->stack_size;
    co->ctx.uc_link = &main_ctx;

    makecontext(&co->ctx, (void (*)(void))co->fn, 1, co->arg);

    return co;
}

void co_resume(coroutine_t* co) {
    if (co->status == CO_DEAD) return;

    current = co;
    co->status = CO_RUNNING;
    swapcontext(&main_ctx, &co->ctx);
}

void co_yield(void) {
    if (current == NULL) return;

    current->status = CO_SUSPENDED;
    coroutine_t* co = current;
    current = NULL;
    swapcontext(&co->ctx, &main_ctx);
}

void co_exit(void) {
    if (current) {
        current->status = CO_DEAD;
        coroutine_t* co = current;
        current = NULL;
        swapcontext(&co->ctx, &main_ctx);
    }
}

/* 使用示例 */
void coroutine_func(void* arg) {
    int n = *(int*)arg;

    for (int i = 0; i < n; i++) {
        printf("Coroutine: %d\n", i);
        co_yield();  /* 挂起 */
    }

    co_exit();
}

void example(void) {
    int arg = 5;
    coroutine_t* co = co_create(coroutine_func, &arg, 0);

    /* 交替执行 */
    while (co->status != CO_DEAD) {
        printf("Main: before resume\n");
        co_resume(co);
        printf("Main: after yield\n");
    }

    free(co->stack);
    free(co);
}
