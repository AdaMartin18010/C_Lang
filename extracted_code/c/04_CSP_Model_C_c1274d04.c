/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 561
 * Language: c
 * Block ID: c1274d04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Future/Promise模式 */

typedef struct {
    void* value;
    atomic_bool ready;
    mtx_t lock;
    cnd_t cond;
    chan_t* waiters;  /* 等待结果的channel */
} future_t;

typedef struct {
    future_t* future;
} promise_t;

future_t* make_future(void) {
    future_t* f = calloc(1, sizeof(future_t));
    atomic_init(&f->ready, false);
    mtx_init(&f->lock, mtx_plain);
    cnd_init(&f->cond);
    return f;
}

promise_t make_promise(future_t* f) {
    return (promise_t){.future = f};
}

/* 设置结果 */
void promise_set(promise_t* p, void* value) {
    future_t* f = p->future;
    mtx_lock(&f->lock);
    f->value = value;
    atomic_store_explicit(&f->ready, true, memory_order_release);
    cnd_broadcast(&f->cond);
    mtx_unlock(&f->lock);
}

/* 获取结果 (阻塞) */
void* future_get(future_t* f) {
    mtx_lock(&f->lock);
    while (!atomic_load_explicit(&f->ready, memory_order_acquire)) {
        cnd_wait(&f->cond, &f->lock);
    }
    void* v = f->value;
    mtx_unlock(&f->lock);
    return v;
}

/* 使用 */
void async_task(void* arg) {
    future_t* f = arg;
    promise_t p = make_promise(f);

    /* 执行异步操作 */
    void* result = do_work();
    promise_set(&p, result);
}

void example(void) {
    future_t* f = make_future();
    go(async_task, f);

    /* 阻塞等待结果 */
    void* result = future_get(f);
}
