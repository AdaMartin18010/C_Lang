/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 634
 * Language: c
 * Block ID: cd76cca8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Go风格的Context */

typedef struct context {
    atomic_bool cancelled;
    chan_t* done;       /* 关闭时通知 */
    int64_t deadline;   /* 截止时间 */
    struct context* parent;
} context_t;

context_t* background(void) {
    static context_t bg = {0};
    return &bg;
}

context_t* with_cancel(context_t* parent) {
    context_t* ctx = calloc(1, sizeof(context_t));
    ctx->parent = parent;
    ctx->done = make_chan(sizeof(int), 0);
    return ctx;
}

context_t* with_timeout(context_t* parent, int ms) {
    context_t* ctx = with_cancel(parent);
    ctx->deadline = current_time_ms() + ms;

    /* 启动定时器goroutine */
    go((void(*)(void*))timeout_cancel, ctx);

    return ctx;
}

void cancel(context_t* ctx) {
    if (atomic_compare_exchange_strong(&ctx->cancelled,
                                       &(bool){false}, true)) {
        chan_close(ctx->done);

        /* 取消子context */
        /* 实际实现需要维护子context列表 */
    }
}

/* 检查是否取消 */
bool ctx_done(context_t* ctx) {
    return atomic_load(&ctx->cancelled);
}

/* 使用 */
void worker(context_t* ctx, chan_t* input, chan_t* output) {
    while (!ctx_done(ctx)) {
        select_case_t cases[] = {
            {input, &data, SELECT_RECV},
            {ctx->done, NULL, SELECT_RECV}
        };
        int chosen = chan_select(cases, 2, -1);

        if (chosen == 1) {
            return;  /* 取消 */
        }

        /* 处理数据 */
        process(data);
    }
}
