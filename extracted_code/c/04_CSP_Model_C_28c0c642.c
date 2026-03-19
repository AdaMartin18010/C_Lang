/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 229
 * Language: c
 * Block ID: 28c0c642
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 无缓冲Channel - 同步点 */

typedef struct {
    void* sender_data;   /* 发送者数据 */
    void* recv_buffer;   /* 接收者缓冲区 */
    atomic_bool ready;   /* 就绪标志 */
    mtx_t lock;
    cnd_t cond;
} unbuf_chan_t;

unbuf_chan_t* make_unbuf_chan(void) {
    unbuf_chan_t* ch = calloc(1, sizeof(unbuf_chan_t));
    atomic_init(&ch->ready, false);
    mtx_init(&ch->lock, mtx_plain);
    cnd_init(&ch->cond);
    return ch;
}

/* 发送 - 阻塞直到有接收者 */
bool unbuf_send(unbuf_chan_t* ch, void* data, size_t size) {
    mtx_lock(&ch->lock);

    /* 等待接收者 */
    while (!atomic_load_explicit(&ch->ready, memory_order_acquire)) {
        cnd_wait(&ch->cond, &ch->lock);
    }

    /* 直接复制到接收者缓冲区 */
    memcpy(ch->recv_buffer, data, size);
    atomic_store_explicit(&ch->ready, false, memory_order_release);
    cnd_signal(&ch->cond);

    mtx_unlock(&ch->lock);
    return true;
}

/* 接收 - 阻塞直到有发送者 */
bool unbuf_recv(unbuf_chan_t* ch, void* buffer, size_t size) {
    mtx_lock(&ch->lock);

    /* 通知发送者我们准备好了 */
    ch->recv_buffer = buffer;
    atomic_store_explicit(&ch->ready, true, memory_order_release);
    cnd_signal(&ch->cond);

    /* 等待发送者完成 */
    while (atomic_load_explicit(&ch->ready, memory_order_acquire)) {
        cnd_wait(&ch->cond, &ch->lock);
    }

    mtx_unlock(&ch->lock);
    return true;
}
