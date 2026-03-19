/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 379
 * Language: c
 * Block ID: 139c2bad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    ACTOR_INIT,
    ACTOR_RUNNING,
    ACTOR_PAUSED,
    ACTOR_STOPPING,
    ACTOR_DEAD
} actor_status_t;

/* 创建Actor */
actor_t* actor_spawn(void (*behaviour)(actor_t*, message_t*),
                     void* initial_state,
                     actor_t* parent) {
    actor_t* actor = calloc(1, sizeof(actor_t));

    actor->id = atomic_fetch_add(&g_system.next_id, 1);
    actor->state = initial_state;
    actor->behaviour = behaviour;
    actor->parent = parent;
    actor->children = NULL;
    actor->child_count = 0;
    atomic_init(&actor->status, ACTOR_INIT);

    actor->mailbox = malloc(sizeof(mailbox_t));
    mailbox_init(actor->mailbox, 10000);  /* 默认10k消息上限 */

    /* 添加到父Actor的监督列表 */
    if (parent != NULL) {
        mtx_lock(&parent->lock);
        parent->children = realloc(parent->children,
                                   sizeof(actor_t*) * (parent->child_count + 1));
        parent->children[parent->child_count++] = actor;
        mtx_unlock(&parent->lock);
    }

    actor_register(actor);
    atomic_store(&actor->status, ACTOR_RUNNING);

    return actor;
}

/* 终止Actor */
void actor_kill(actor_t* actor, int reason) {
    actor_status_t old = ACTOR_RUNNING;
    if (!atomic_compare_exchange_strong(&actor->status, &old, ACTOR_STOPPING)) {
        return;  /* 已经在停止 */
    }

    /* 通知父Actor */
    if (actor->parent != NULL) {
        message_t msg = make_message(MSG_EXIT, actor->id, actor->parent->id);
        msg.payload.exit.child = actor->id;
        msg.payload.exit.reason = reason;
        mailbox_send(actor->parent->mailbox, &msg);
    }

    /* 终止所有子Actor */
    mtx_lock(&actor->lock);
    for (int i = 0; i < actor->child_count; i++) {
        actor_kill(actor->children[i], REASON_PARENT_EXIT);
    }
    mtx_unlock(&actor->lock);

    atomic_store(&actor->status, ACTOR_DEAD);
    actor_unregister(actor->id);
}

/* 链接Actor (监督关系) */
void actor_link(actor_t* a, actor_t* b) {
    /* 互相监控 */
    message_t msg = make_message(MSG_LINK, a->id, b->id);
    mailbox_send(b->mailbox, &msg);

    msg.target = a->id;
    msg.sender = b->id;
    mailbox_send(a->mailbox, &msg);
}
