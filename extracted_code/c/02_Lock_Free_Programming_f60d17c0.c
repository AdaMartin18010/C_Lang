/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\02_Lock_Free_Programming.md
 * Line: 728
 * Language: c
 * Block ID: f60d17c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    _Atomic(node_t*) top;
} safe_stack_t;

void safe_push(safe_stack_t* stack, void* data) {
    node_t* node = malloc(sizeof(node_t));
    node->data = data;

    node_t* old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        node->next = old_top;
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, node,
        memory_order_release, memory_order_relaxed));
}

bool safe_pop(safe_stack_t* stack, void** data) {
    hp_thread_init();

    node_t* old_top;
    do {
        old_top = atomic_load_explicit(&stack->top, memory_order_relaxed);
        if (old_top == NULL) {
            return false;
        }

        /* 设置 hazard pointer */
        hp_set(0, old_top);

        /* 再次检查 */
        if (old_top != atomic_load_explicit(&stack->top,
                                            memory_order_acquire)) {
            continue;  /* 已变化，重试 */
        }
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old_top, old_top->next,
        memory_order_release, memory_order_relaxed));

    /* 清除 hazard pointer */
    hp_clear(0);

    *data = old_top->data;

    /* 退休节点，延迟释放 */
    hp_retire(old_top);

    return true;
}
