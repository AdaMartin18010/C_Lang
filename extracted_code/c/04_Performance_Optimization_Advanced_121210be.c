/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 3295
 * Language: c
 * Block ID: 121210be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file lock_free.c
 * @brief 无锁数据结构与算法
 *
 * 无锁编程使用原子操作代替锁，避免：
 * - 锁竞争
 * - 线程阻塞/唤醒开销
 * - 优先级反转
 * - 死锁风险
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

/* 内存序说明：
 * memory_order_relaxed: 无顺序约束，仅保证原子性
 * memory_order_acquire: 加载操作，后续操作不会重排序到前面
 * memory_order_release: 存储操作，前面操作不会重排序到后面
 * memory_order_acq_rel: 读写操作都应用acquire/release语义
 * memory_order_seq_cst: 顺序一致性，最强约束
 */

/**
 * @brief 无锁栈（Treiber栈）
 */
typedef struct Node {
    void *data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) top;
} LockFreeStack;

void stack_init(LockFreeStack *stack) {
    atomic_init(&stack->top, NULL);
}

void stack_push(LockFreeStack *stack, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;

    Node *old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        atomic_store_explicit(&new_node->next, old_top, memory_order_relaxed);
        /* CAS: 如果top仍是old_top，则更新为new_node */
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, new_node,
        memory_order_release,  /* 成功时使用release */
        memory_order_relaxed   /* 失败时使用relaxed */
    ));
}

void* stack_pop(LockFreeStack *stack) {
    Node *old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_acquire);
        if (old_top == NULL) {
            return NULL;  /* 空栈 */
        }
        /* CAS: 如果top仍是old_top，则更新为next */
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top,
        atomic_load_explicit(&old_top->next, memory_order_relaxed),
        memory_order_release,
        memory_order_acquire   /* 失败时重新获取 */
    ));

    void *data = old_top->data;
    /* 注意：这里简化处理，实际需要安全内存回收（如Hazard Pointers）*/
    free(old_top);
    return data;
}

/**
 * @brief 无锁队列（Michael-Scott队列）
 */
typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_init(LockFreeQueue *queue) {
    Node *dummy = malloc(sizeof(Node));
    dummy->data = NULL;
    atomic_init(&dummy->next, NULL);
    atomic_init(&queue->head, dummy);
    atomic_init(&queue->tail, dummy);
}

void queue_enqueue(LockFreeQueue *queue, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;
    atomic_init(&new_node->next, NULL);

    Node *tail, *next;
    while (1) {
        tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
        next = atomic_load_explicit(&tail->next, memory_order_acquire);

        /* 检查tail是否仍然是tail */
        if (tail == atomic_load_explicit(&queue->tail, memory_order_relaxed)) {
            if (next == NULL) {
                /* 尝试链接新节点 */
                if (atomic_compare_exchange_weak_explicit(
                    &tail->next, &next, new_node,
                    memory_order_release, memory_order_relaxed)) {
                    break;  /* 入队成功 */
                }
            } else {
                /* tail落后，尝试推进 */
                atomic_compare_exchange_weak_explicit(
                    &queue->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            }
        }
    }
    /* 尝试推进tail */
    atomic_compare_exchange_weak_explicit(
        &queue->tail, &tail, new_node,
        memory_order_release, memory_order_relaxed);
}

void* queue_dequeue(LockFreeQueue *queue) {
    Node *head, *tail, *next;
    void *data;

    while (1) {
        head = atomic_load_explicit(&queue->head, memory_order_acquire);
        tail = atomic_load_explicit(&queue->tail, memory_order_acquire);
        next = atomic_load_explicit(&head->next, memory_order_acquire);

        if (head == atomic_load_explicit(&queue->head, memory_order_relaxed)) {
            if (head == tail) {
                if (next == NULL) {
                    return NULL;  /* 空队列 */
                }
                /* tail落后，推进它 */
                atomic_compare_exchange_weak_explicit(
                    &queue->tail, &tail, next,
                    memory_order_release, memory_order_relaxed);
            } else {
                data = next->data;
                /* 尝试推进head */
                if (atomic_compare_exchange_weak_explicit(
                    &queue->head, &head, next,
                    memory_order_release, memory_order_relaxed)) {
                    break;
                }
            }
        }
    }

    /* 注意：这里简化处理，实际需要延迟释放 */
    free(head);
    return data;
}

/**
 * @brief 有锁对比实现
 */
typedef struct {
    Node *top;
    pthread_mutex_t lock;
} LockedStack;

void locked_stack_init(LockedStack *stack) {
    stack->top = NULL;
    pthread_mutex_init(&stack->lock, NULL);
}

void locked_stack_push(LockedStack *stack, void *data) {
    Node *new_node = malloc(sizeof(Node));
    new_node->data = data;

    pthread_mutex_lock(&stack->lock);
    new_node->next = stack->top;
    stack->top = new_node;
    pthread_mutex_unlock(&stack->lock);
}

void* locked_stack_pop(LockedStack *stack) {
    pthread_mutex_lock(&stack->lock);
    Node *node = stack->top;
    if (node) {
        stack->top = node->next;
    }
    pthread_mutex_unlock(&stack->lock);

    if (node) {
        void *data = node->data;
        free(node);
        return data;
    }
    return NULL;
}

/**
 * @brief 性能基准测试
 */
#define NUM_THREADS 8
#define OPS_PER_THREAD 1000000

typedef struct {
    void *stack;
    int ops;
    bool use_lock_free;
} ThreadArg;

void* stack_worker(void *arg) {
    ThreadArg *targ = (ThreadArg*)arg;

    for (int i = 0; i < targ->ops; i++) {
        void *data = (void*)(size_t)i;
        if (targ->use_lock_free) {
            stack_push((LockFreeStack*)targ->stack, data);
            stack_pop((LockFreeStack*)targ->stack);
        } else {
            locked_stack_push((LockedStack*)targ->stack, data);
            locked_stack_pop((LockedStack*)targ->stack);
        }
    }
    return NULL;
}

void benchmark_lock_free(void) {
    printf("=== Lock-Free vs Locked Stack Benchmark ===\n\n");
    printf("Threads: %d, Ops per thread: %d\n\n", NUM_THREADS, OPS_PER_THREAD);

    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];

    /* 测试有锁栈 */
    LockedStack locked_stack;
    locked_stack_init(&locked_stack);

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].stack = &locked_stack;
        args[i].ops = OPS_PER_THREAD;
        args[i].use_lock_free = false;
    }

    clock_t start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stack_worker, &args[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t locked_time = clock() - start;

    printf("Locked stack:   %ld ms\n", locked_time * 1000 / CLOCKS_PER_SEC);

    /* 测试无锁栈 */
    LockFreeStack lock_free_stack;
    stack_init(&lock_free_stack);

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].stack = &lock_free_stack;
        args[i].use_lock_free = true;
    }

    start = clock();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, stack_worker, &args[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t lock_free_time = clock() - start;

    printf("Lock-free stack: %ld ms (%.2fx speedup)\n",
           lock_free_time * 1000 / CLOCKS_PER_SEC,
           (double)locked_time / lock_free_time);
}

int main(void) {
    benchmark_lock_free();
    return 0;
}
