/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 1016
 * Language: c
 * Block ID: 485c1eb8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* GenServer风格的请求响应 */

typedef struct {
    aid_t caller;
    int request_id;
    void* data;
} request_ctx_t;

void server_behaviour(actor_t* self, message_t* msg) {
    switch (msg->type) {
        case MSG_CALL: {
            /* 同步调用 */
            request_ctx_t ctx = {
                .caller = msg->sender,
                .request_id = msg->payload.call.request_id,
                .data = msg->payload.call.data
            };

            /* 处理请求 */
            void* result = handle_call(self, &ctx);

            /* 发送回复 */
            message_t reply = {
                .type = MSG_REPLY,
                .target = ctx.caller,
                .payload.reply.request_id = ctx.request_id,
                .payload.reply.result = result
            };
            send(ctx.caller, &reply);
            break;
        }

        case MSG_CAST: {
            /* 异步调用，无回复 */
            handle_cast(self, msg->payload.cast.data);
            break;
        }
    }
}

/* 客户端调用 */
void* call(aid_t server, void* request, int timeout_ms) {
    static atomic_int request_id = 0;

    message_t msg = {
        .type = MSG_CALL,
        .sender = self_id,
        .payload.call.request_id = atomic_fetch_add(&request_id, 1),
        .payload.call.data = request
    };

    /* 发送并等待 */
    send(server, &msg);

    message_t reply;
    while (receive(&reply, timeout_ms)) {
        if (reply.type == MSG_REPLY &&
            reply.payload.reply.request_id == msg.payload.call.request_id) {
            return reply.payload.reply.result;
        }
        /* 其他消息入队列稍后处理 */
        queue_message(&reply);
    }

    return NULL;  /* 超时 */
}

void cast(aid_t server, void* request) {
    message_t msg = {
        .type = MSG_CAST,
        .sender = self_id,
        .payload.cast.data = request
    };
    send(server, &msg);
}
