/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 664
 * Language: c
 * Block ID: 0b6f12a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 专门的监督者行为 */
void supervisor_behaviour(actor_t* self, message_t* msg) {
    switch (msg->type) {
        case MSG_EXIT:
            handle_child_exit(self, msg);
            break;

        case MSG_SPAWN_MANAGED: {
            /* 创建受监督的Actor */
            actor_t* child = actor_spawn(
                msg->payload.spawn.behaviour,
                msg->payload.spawn.arg,
                self  /* 监督者是当前Actor */
            );

            /* 记录配置 */
            add_child_spec(self, child->id, &msg->payload.spawn.spec);

            /* 回复创建的Actor ID */
            if (msg->reply_to) {
                message_t reply = {0};
                reply.type = MSG_SPAWN_RESULT;
                reply.payload.spawn_result.id = child->id;
                mailbox_send(msg->reply_to, &reply);
            }
            break;
        }
    }
}

/* 创建根监督者 */
actor_t* create_root_supervisor(supervisor_spec_t* spec) {
    return actor_spawn(supervisor_behaviour, spec, NULL);
}
