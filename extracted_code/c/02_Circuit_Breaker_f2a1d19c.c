/*
 * Auto-generated from: 07_Modern_Toolchain\10_Distributed_Systems\02_Circuit_Breaker.md
 * Line: 342
 * Language: c
 * Block ID: f2a1d19c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 精确滑动窗口 */

typedef struct {
    int64_t timestamp;
    int count;
} window_entry_t;

typedef struct {
    int window_size_ms;     /* 窗口大小 */
    int max_requests;       /* 窗口内最大请求数 */

    window_entry_t* entries;
    int entry_count;
    int head;
    mtx_t lock;
} sliding_window_t;

sliding_window_t* sw_create(int window_ms, int max_requests) {
    sliding_window_t* sw = malloc(sizeof(sliding_window_t));
    sw->window_size_ms = window_ms;
    sw->max_requests = max_requests;
    sw->entries = calloc(max_requests * 2, sizeof(window_entry_t));
    sw->entry_count = 0;
    sw->head = 0;
    mtx_init(&sw->lock, mtx_plain);
    return sw;
}

/* 检查并记录 */
bool sw_allow(sliding_window_t* sw) {
    mtx_lock(&sw->lock);

    int64_t now = current_time_ms();
    int64_t window_start = now - sw->window_size_ms;

    /* 清理过期条目 */
    while (sw->entry_count > 0) {
        int idx = (sw->head - sw->entry_count + max_requests * 2)
                  % (max_requests * 2);
        if (sw->entries[idx].timestamp >= window_start) {
            break;
        }
        sw->entry_count--;
    }

    /* 检查是否超限 */
    if (sw->entry_count >= sw->max_requests) {
        mtx_unlock(&sw->lock);
        return false;
    }

    /* 记录 */
    sw->entries[sw->head] = (window_entry_t){now, 1};
    sw->head = (sw->head + 1) % (sw->max_requests * 2);
    sw->entry_count++;

    mtx_unlock(&sw->lock);
    return true;
}
