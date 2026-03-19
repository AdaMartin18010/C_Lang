/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\02_Circuit_Breaker.md
 * Line: 237
 * Language: c
 * Block ID: 42c7263b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double capacity;        /* 桶容量 */
    double tokens;          /* 当前令牌数 */
    double fill_rate;       /* 每秒填充速率 */
    int64_t last_update;    /* 最后更新时间 */
    mtx_t lock;
} token_bucket_t;

token_bucket_t* tb_create(double capacity, double fill_rate) {
    token_bucket_t* tb = malloc(sizeof(token_bucket_t));
    tb->capacity = capacity;
    tb->tokens = capacity;  /* 初始满 */
    tb->fill_rate = fill_rate;
    tb->last_update = current_time_ms();
    mtx_init(&tb->lock, mtx_plain);
    return tb;
}

/* 尝试获取令牌 */
bool tb_consume(token_bucket_t* tb, double tokens) {
    mtx_lock(&tb->lock);

    /* 计算新令牌 */
    int64_t now = current_time_ms();
    double elapsed = (now - tb->last_update) / 1000.0;
    tb->tokens = fmin(tb->capacity, tb->tokens + elapsed * tb->fill_rate);
    tb->last_update = now;

    /* 尝试消费 */
    if (tb->tokens >= tokens) {
        tb->tokens -= tokens;
        mtx_unlock(&tb->lock);
        return true;
    }

    mtx_unlock(&tb->lock);
    return false;
}

/* 等待获取令牌 */
bool tb_consume_wait(token_bucket_t* tb, double tokens, int timeout_ms) {
    int waited = 0;
    while (!tb_consume(tb, tokens)) {
        if (waited >= timeout_ms) return false;

        /* 计算等待时间 */
        mtx_lock(&tb->lock);
        double needed = tokens - tb->tokens;
        int wait = (int)(needed / tb->fill_rate * 1000);
        mtx_unlock(&tb->lock);

        wait = fmin(wait, 100);  /* 最多等100ms */
        usleep(wait * 1000);
        waited += wait;
    }
    return true;
}
