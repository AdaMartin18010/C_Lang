/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 517
 * Language: c
 * Block ID: f5990bb7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Actor的主循环 */
void actor_loop(actor_t* actor) {
    message_t msg;

    while (atomic_load(&actor->status) == ACTOR_RUNNING) {
        /* 接收消息 */
        if (mailbox_recv(actor->mailbox, &msg, 100)) {
            /* 处理消息 */
            switch (msg.type) {
                case MSG_USER:
                    actor->behaviour(actor, &msg);
                    break;

                case MSG_SPAWN:
                    /* 创建子Actor */
                    actor_spawn(msg.payload.spawn.behaviour,
                               msg.payload.spawn.arg,
                               actor);
                    break;

                case MSG_KILL:
                    actor_kill(actor, REASON_NORMAL);
                    return;

                case MSG_EXIT:
                    /* 子Actor退出 */
                    handle_child_exit(actor, &msg);
                    break;

                case MSG_LINK:
                    /* 建立链接 */
                    break;

                default:
                    break;
            }

            /* 清理消息资源 */
            if (msg.user.data != NULL && msg.user.free_data) {
                free(msg.user.data);
            }
        }
    }
}
