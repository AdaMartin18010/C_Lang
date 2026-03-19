/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 838
 * Language: c
 * Block ID: 4b4054d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Actor行为版本控制 */
typedef struct behaviour_version {
    int version;
    void (*handler)(actor_t*, message_t*);
    struct behaviour_version* next;
} behaviour_version_t;

typedef struct {
    atomic_int current_version;
    behaviour_version_t* versions;
} hot_upgrade_t;

/* 升级Actor行为 */
void upgrade_actor_behaviour(aid_t actor_id,
                              void (*new_behaviour)(actor_t*, message_t*),
                              int new_version) {
    actor_t* actor = actor_find(actor_id);
    if (actor == NULL) return;

    /* 发送升级消息 */
    message_t msg = {
        .type = MSG_UPGRADE,
        .payload.upgrade.version = new_version,
        .payload.upgrade.handler = new_behaviour
    };

    mailbox_send(actor->mailbox, &msg);
}

/* Actor处理升级 */
void handle_upgrade(actor_t* actor, message_t* msg) {
    /* 保存状态 */
    void* state = actor->state;

    /* 切换行为 */
    actor->behaviour = msg->payload.upgrade.handler;

    /* 状态迁移 */
    actor->state = migrate_state(state, msg->payload.upgrade.version);
}
