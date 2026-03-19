/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 63
 * Language: c
 * Block ID: 5d50da30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Actor的基本特性 */

typedef struct actor {
    /* 唯一标识 */
    aid_t id;

    /* 邮箱 (消息队列) */
    mailbox_t* mailbox;

    /* 状态和行为 */
    void* state;
    void (*behaviour)(struct actor*, message_t*);

    /* 监督 */
    struct actor* parent;       /* 监督者 */
    struct actor** children;    /* 被监督者 */
    int child_count;

    /* 生命周期 */
    atomic_int status;          /* running/paused/stopped */
} actor_t;

/* Actor处理消息 */
void actor_receive(actor_t* self, message_t* msg) {
    /* 根据消息类型分发 */
    switch (msg->type) {
        case MSG_USER:
            self->behaviour(self, msg);  /* 用户定义行为 */
            break;
        case MSG_SUPERVISE:
            handle_supervision(self, msg);  /* 监督消息 */
            break;
        case MSG_SYSTEM:
            handle_system(self, msg);  /* 系统消息 */
            break;
    }
}
