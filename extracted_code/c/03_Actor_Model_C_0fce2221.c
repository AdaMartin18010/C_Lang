/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 883
 * Language: c
 * Block ID: 0fce2221
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 节点发现 */
typedef struct node {
    uint16_t id;
    char address[64];
    int port;
    _Atomic(bool) connected;
} node_t;

/* 远程消息序列化 */
void remote_send(aid_t target, uint16_t node_id, message_t* msg) {
    node_t* node = find_node(node_id);
    if (node == NULL || !atomic_load(&node->connected)) {
        return;
    }

    /* 序列化消息 */
    uint8_t buffer[4096];
    size_t len = serialize_message(msg, buffer, sizeof(buffer));

    /* 发送 */
    send_to_node(node, buffer, len);
}

/* 接收远程消息 */
void handle_remote_message(node_t* from, uint8_t* data, size_t len) {
    message_t msg;
    deserialize_message(&msg, data, len);

    /* 路由到本地Actor */
    actor_t* target = actor_find(msg.target);
    if (target != NULL) {
        mailbox_send(target->mailbox, &msg);
    }
}

/* 跨节点Actor发现 */
actor_ref_t lookup_remote_actor(const char* name, uint16_t node_id) {
    /* 发送查询 */
    message_t query = {
        .type = MSG_LOOKUP,
        .payload.lookup.name = name
    };

    /* 等待响应 */
    message_t* response = send_receive(name_server_id, &query, 5000);

    if (response && response->type == MSG_LOOKUP_RESULT) {
        return response->payload.lookup_result.ref;
    }

    return (actor_ref_t){0};
}
