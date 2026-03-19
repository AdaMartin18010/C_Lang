/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 275
 * Language: c
 * Block ID: bb59a152
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* microkernel.c - 微内核核心实现 */
#include "microkernel.h"
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

/* 全局服务表 */
static service_t *service_list = NULL;
static atomic_uint next_service_id = 1;
static atomic_flag service_lock = ATOMIC_FLAG_INIT;

/* 自旋锁 */
static void spinlock_acquire(atomic_flag *lock) {
    while (atomic_flag_test_and_set(lock)) {
        /* 自旋等待 */
    }
}

static void spinlock_release(atomic_flag *lock) {
    atomic_flag_clear(lock);
}

/* 查找服务 */
static service_t *find_service(service_id_t id) {
    service_t *svc = service_list;
    while (svc) {
        if (svc->id == id) return svc;
        svc = svc->next;
    }
    return NULL;
}

/* 注册服务 */
service_id_t mk_register_service(const char *name, int (*handler)(message_t *)) {
    if (!name || !handler) return 0;

    service_t *svc = calloc(1, sizeof(service_t));
    if (!svc) return 0;

    svc->id = atomic_fetch_add(&next_service_id, 1);
    svc->name = name;
    svc->handler = handler;
    svc->priority = 10; /* 默认优先级 */

    spinlock_acquire(&service_lock);
    svc->next = service_list;
    service_list = svc;
    spinlock_release(&service_lock);

    return svc->id;
}

/* 消息处理循环示例 */
void service_worker_thread(void *arg) {
    service_t *self = (service_t *)arg;
    message_t msg_buffer[16];

    while (1) {
        int ret = mk_receive_message(msg_buffer, sizeof(msg_buffer), 1000);
        if (ret > 0) {
            /* 调用服务处理函数 */
            self->handler(msg_buffer);
        }
    }
}
