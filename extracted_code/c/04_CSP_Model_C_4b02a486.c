/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 291
 * Language: c
 * Block ID: 4b02a486
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    SELECT_SEND,
    SELECT_RECV,
    SELECT_DEFAULT
} select_op_t;

typedef struct {
    chan_t* ch;
    void* ptr;          /* 发送/接收数据指针 */
    select_op_t op;
    bool selected;      /* 是否被选中 */
} select_case_t;

/* 非阻塞尝试 */
static bool try_op(select_case_t* c) {
    if (c->op == SELECT_SEND) {
        /* 尝试发送 */
        mtx_lock(&c->ch->lock);
        size_t send_idx = atomic_load(&c->ch->send_idx);
        size_t recv_idx = atomic_load(&c->ch->recv_idx);
        size_t count = (send_idx - recv_idx + c->ch->capacity + 1)
                       % (c->ch->capacity + 1);

        if (count < c->ch->capacity) {
            /* 可以发送 */
            char* slot = (char*)c->ch->data + send_idx * c->ch->elem_size;
            memcpy(slot, c->ptr, c->ch->elem_size);
            atomic_store(&c->ch->send_idx,
                        (send_idx + 1) % (c->ch->capacity + 1));
            cnd_signal(&c->ch->recv_ready);
            mtx_unlock(&c->ch->lock);
            return true;
        }
        mtx_unlock(&c->ch->lock);
        return false;

    } else if (c->op == SELECT_RECV) {
        /* 尝试接收 */
        mtx_lock(&c->ch->lock);
        size_t send_idx = atomic_load(&c->ch->send_idx);
        size_t recv_idx = atomic_load(&c->ch->recv_idx);

        if (send_idx != recv_idx) {
            /* 有数据 */
            char* slot = (char*)c->ch->data + recv_idx * c->ch->elem_size;
            memcpy(c->ptr, slot, c->ch->elem_size);
            atomic_store(&c->ch->recv_idx,
                        (recv_idx + 1) % (c->ch->capacity + 1));
            cnd_signal(&c->ch->send_ready);
            mtx_unlock(&c->ch->lock);
            return true;
        }
        mtx_unlock(&c->ch->lock);
        return false;
    }
    return true;  /* DEFAULT */
}

/* Select实现 */
int chan_select(select_case_t* cases, int n, int timeout_ms) {
    /* 1. 随机打乱顺序，避免饥饿 */
    int order[n];
    for (int i = 0; i < n; i++) order[i] = i;

    /* Fisher-Yates shuffle */
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }

    /* 2. 先尝试非阻塞 */
    for (int i = 0; i < n; i++) {
        int idx = order[i];
        if (cases[idx].op == SELECT_DEFAULT) {
            return idx;  /* 立即返回 */
        }
        if (try_op(&cases[idx])) {
            return idx;
        }
    }

    /* 3. 需要阻塞等待 */
    mtx_t poll_lock;
    cnd_t poll_cond;
    mtx_init(&poll_lock, mtx_plain);
    cnd_init(&poll_cond);

    /* 注册到所有channel的等待列表 */
    for (int i = 0; i < n; i++) {
        cases[i].selected = false;
        /* 实际实现需要维护每个channel的等待者列表 */
    }

    /* 等待直到某个channel就绪 */
    mtx_lock(&poll_lock);
    struct timespec deadline;
    timespec_get(&deadline, TIME_UTC);
    deadline.tv_sec += timeout_ms / 1000;
    deadline.tv_nsec += (timeout_ms % 1000) * 1000000;

    while (1) {
        /* 检查是否被唤醒 */
        for (int i = 0; i < n; i++) {
            if (cases[i].selected) {
                mtx_unlock(&poll_lock);
                return i;
            }
        }

        /* 超时检查 */
        if (timeout_ms >= 0) {
            if (cnd_timedwait(&poll_cond, &poll_lock, &deadline)
                == thrd_timeout) {
                mtx_unlock(&poll_lock);
                return -1;
            }
        } else {
            cnd_wait(&poll_cond, &poll_lock);
        }
    }
}
