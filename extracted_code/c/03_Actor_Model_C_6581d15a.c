/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 210
 * Language: c
 * Block ID: 6581d15a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Actor引用 - 可本地可远程 */

typedef uint64_t aid_t;  /* Actor ID */

#define AID_LOCAL_BITS  48
#define AID_NODE_MASK   0xFFFF000000000000ULL
#define AID_LOCAL_MASK  0x0000FFFFFFFFFFFFULL

typedef struct actor_ref {
    aid_t id;

    /* 如果是本地Actor，直接指针 */
    actor_t* local;

    /* 如果是远程Actor，网络信息 */
    struct {
        uint16_t node_id;
        char address[64];
        int port;
    } remote;
} actor_ref_t;

/* 创建本地Actor引用 */
actor_ref_t local_ref(actor_t* actor) {
    return (actor_ref_t){
        .id = actor->id,
        .local = actor,
        .remote = {0}
    };
}

/* 发送消息到引用 */
void actor_ref_send(actor_ref_t* ref, message_t* msg) {
    if (ref->local != NULL) {
        /* 本地发送 */
        mailbox_send(ref->local->mailbox, msg);
    } else {
        /* 远程发送 - 序列化后网络发送 */
        remote_send(ref->id, ref->remote.node_id, msg);
    }
}
