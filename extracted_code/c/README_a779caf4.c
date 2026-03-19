/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1107
 * Language: c
 * Block ID: a779caf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void (*func)(void* ctx);
    void* context;
    void (*free_context)(void* ctx);
} Closure;

Closure* closure_create(void (*func)(void*), void* ctx,
                        void (*free_ctx)(void*)) {
    Closure* c = malloc(sizeof(Closure));
    c->func = func;
    c->context = ctx;
    c->free_context = free_ctx;
    return c;
}

void closure_call(Closure* c) {
    c->func(c->context);
}

void closure_destroy(Closure* c) {
    if (c->free_context) {
        c->free_context(c->context);
    }
    free(c);
}

// 使用: 创建带状态的回调
void counter_increment(void* ctx) {
    int* count = ctx;
    (*count)++;
    printf("Count: %d\n", *count);
}

Closure* create_counter(void) {
    int* count = malloc(sizeof(int));
    *count = 0;
    return closure_create(counter_increment, count, free);
}
