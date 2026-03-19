/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 944
 * Language: c
 * Block ID: 37943847
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 简单的计数器Actor */

typedef struct {
    int count;
} counter_state_t;

void counter_behaviour(actor_t* self, message_t* msg) {
    counter_state_t* state = self->state;

    switch (msg->payload.user.user_type) {
        case CMD_INCREMENT:
            state->count++;
            break;

        case CMD_GET: {
            /* 发送当前值给请求者 */
            message_t reply = {
                .type = MSG_USER,
                .sender = self->id,
                .payload.user.user_type = RESP_COUNT,
                .payload.user.data = &state->count,
                .payload.user.size = sizeof(int)
            };
            send(msg->sender, &reply);
            break;
        }
    }
}

/* 使用 */
void example_counter(void) {
    /* 创建监督者 */
    supervisor_spec_t spec = {
        .strategy = STRATEGY_ONE_FOR_ONE,
        .max_restarts = 5,
        .restart_window = 60
    };
    actor_t* supervisor = create_root_supervisor(&spec);

    /* 创建计数器 */
    counter_state_t initial = {0};
    actor_t* counter = actor_spawn(counter_behaviour, &initial, supervisor);

    /* 发送增加消息 */
    message_t inc = {
        .type = MSG_USER,
        .payload.user.user_type = CMD_INCREMENT
    };
    send(counter->id, &inc);
    send(counter->id, &inc);
    send(counter->id, &inc);

    /* 获取值 */
    message_t get = {
        .type = MSG_USER,
        .sender = self_id,  /* 自己的Actor ID */
        .payload.user.user_type = CMD_GET
    };
    send(counter->id, &get);

    /* 等待回复 */
    message_t reply;
    if (receive(&reply, 1000)) {
        int* count = reply.payload.user.data;
        printf("Count: %d\n", *count);  /* 输出 3 */
    }
}
