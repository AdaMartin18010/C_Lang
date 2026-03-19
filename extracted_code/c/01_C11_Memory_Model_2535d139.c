/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 690
 * Language: c
 * Block ID: 2535d139
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ABA问题示例: 栈顶从A->B->A，CAS检测不到 */
/* 解决方案: 使用带版本的指针 (Tagged Pointer) */

typedef struct {
    node_t* ptr;
    uint64_t version;
} tagged_ptr_t;

typedef struct {
    _Atomic(tagged_ptr_t) top;
} safe_stack_t;

bool stack_pop_safe(safe_stack_t* stack, int* value) {
    tagged_ptr_t old, new;
    do {
        old = atomic_load_explicit(&stack->top, memory_order_acquire);
        if (old.ptr == NULL) return false;

        new.ptr = old.ptr->next;
        new.version = old.version + 1;  /* 版本递增 */
    } while (!atomic_compare_exchange_weak_explicit(
        &stack->top, &old, new,
        memory_order_release, memory_order_relaxed));

    *value = old.ptr->data;
    /* 延迟释放或使用Hazard Pointer */
    return true;
}
