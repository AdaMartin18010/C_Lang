/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\02_Circuit_Breaker.md
 * Line: 299
 * Language: c
 * Block ID: 288c76ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    double capacity;        /* 桶容量 */
    double water;           /* 当前水量 */
    double leak_rate;       /* 漏出速率 */
    int64_t last_update;
    mtx_t lock;
} leaky_bucket_t;

leaky_bucket_t* lb_create(double capacity, double leak_rate) {
    leaky_bucket_t* lb = malloc(sizeof(leaky_bucket_t));
    lb->capacity = capacity;
    lb->water = 0;
    lb->leak_rate = leak_rate;
    lb->last_update = current_time_ms();
    mtx_init(&lb->lock, mtx_plain);
    return lb;
}

/* 添加水 (请求) */
bool lb_add(leaky_bucket_t* lb, double water) {
    mtx_lock(&lb->lock);

    /* 先漏水 */
    int64_t now = current_time_ms();
    double elapsed = (now - lb->last_update) / 1000.0;
    lb->water = fmax(0, lb->water - elapsed * lb->leak_rate);
    lb->last_update = now;

    /* 检查是否溢出 */
    if (lb->water + water <= lb->capacity) {
        lb->water += water;
        mtx_unlock(&lb->lock);
        return true;
    }

    mtx_unlock(&lb->lock);
    return false;  /* 桶满，拒绝 */
}
