/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 464
 * Language: c
 * Block ID: d2fd0c8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 发送消息到Actor */
bool send(aid_t target_id, message_t* msg) {
    actor_t* target = actor_find(target_id);
    if (target == NULL) {
        return false;  /* Actor不存在 */
    }

    msg->target = target_id;
    return mailbox_send(target->mailbox, msg);
}

/* 带超时的发送 */
bool send_timeout(aid_t target_id, message_t* msg, int timeout_ms) {
    /* 如果邮箱满，等待 */
    actor_t* target = actor_find(target_id);
    if (target == NULL) return false;

    int waited = 0;
    while (!mailbox_send(target->mailbox, msg)) {
        if (waited >= timeout_ms) return false;
        thrd_sleep(&(struct timespec){0, 10000000}, NULL);  /* 10ms */
        waited += 10;
    }
    return true;
}

/* 发送并等待回复 */
message_t* send_receive(aid_t target_id, message_t* msg, int timeout_ms) {
    /* 创建临时邮箱 */
    mailbox_t reply_box;
    mailbox_init(&reply_box, 1);

    /* 在消息中包含回复地址 */
    msg->reply_to = &reply_box;

    if (!send(target_id, msg)) {
        return NULL;
    }

    /* 等待回复 */
    message_t* reply = malloc(sizeof(message_t));
    if (!mailbox_recv(&reply_box, reply, timeout_ms)) {
        free(reply);
        return NULL;
    }

    return reply;
}
