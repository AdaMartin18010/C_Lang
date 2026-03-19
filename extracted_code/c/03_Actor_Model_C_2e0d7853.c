/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\03_Actor_Model_C.md
 * Line: 570
 * Language: c
 * Block ID: 2e0d7853
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    STRATEGY_ONE_FOR_ONE,    /* 一个失败只重启它 */
    STRATEGY_ONE_FOR_ALL,    /* 一个失败重启所有 */
    STRATEGY_REST_FOR_ONE    /* 一个失败重启后面的 */
} supervision_strategy_t;

typedef struct {
    supervision_strategy_t strategy;
    int max_restarts;        /* 最大重启次数 */
    int restart_window;      /* 时间窗口(秒) */
    int backoff_base;        /* 退避基数(ms) */
} supervisor_spec_t;

typedef struct {
    aid_t child_id;
    int restart_count;
    time_t last_restart;
    supervisor_spec_t spec;
} child_spec_t;

/* 处理子Actor退出 */
void handle_child_exit(actor_t* supervisor, message_t* msg) {
    aid_t child_id = msg->payload.exit.child;
    int reason = msg->payload.exit.reason;

    /* 根据退出原因决定 */
    if (reason == REASON_NORMAL) {
        /* 正常退出，不需要重启 */
        remove_child(supervisor, child_id);
        return;
    }

    /* 查找子Actor配置 */
    child_spec_t* spec = find_child_spec(supervisor, child_id);
    if (spec == NULL) return;

    /* 检查重启频率 */
    time_t now = time(NULL);
    if (now - spec->last_restart < spec->spec.restart_window) {
        spec->restart_count++;
        if (spec->restart_count > spec->spec.max_restarts) {
            /* 重启过于频繁，停止监督 */
            actor_kill(supervisor, REASON_TOO_MANY_RESTARTS);
            return;
        }
    } else {
        spec->restart_count = 1;
    }

    spec->last_restart = now;

    /* 根据策略处理 */
    switch (spec->spec.strategy) {
        case STRATEGY_ONE_FOR_ONE:
            /* 只重启失败的子Actor */
            restart_child(supervisor, child_id);
            break;

        case STRATEGY_ONE_FOR_ALL:
            /* 停止所有子Actor，然后全部重启 */
            for (int i = 0; i < supervisor->child_count; i++) {
                actor_kill(supervisor->children[i], REASON_SUPERVISOR_RESTART);
            }
            restart_all_children(supervisor);
            break;

        case STRATEGY_REST_FOR_ONE:
            /* 停止并重启失败Actor之后的所有Actor */
            restart_child_and_later(supervisor, child_id);
            break;
    }
}

/* 带退避的重启 */
void restart_child(actor_t* supervisor, aid_t child_id) {
    child_spec_t* spec = find_child_spec(supervisor, child_id);

    /* 计算退避时间 */
    int backoff = spec->spec.backoff_base * (1 << spec->restart_count);
    if (backoff > 60000) backoff = 60000;  /* 最大60秒 */

    thrd_sleep(&(struct timespec){backoff / 1000,
                                   (backoff % 1000) * 1000000}, NULL);

    /* 重新创建Actor */
    actor_t* new_child = actor_spawn(spec->behaviour, spec->initial_state,
                                     supervisor);
    spec->child_id = new_child->id;
}
