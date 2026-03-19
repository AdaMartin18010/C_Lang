/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\01_State_Machine_Patterns.md
 * Line: 86
 * Language: c
 * Block ID: 7fab799f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* HSM: 支持状态继承 */

typedef struct hsm_state {
    const char* name;
    struct hsm_state* parent;
    void (*entry)(void* ctx);
    void (*exit)(void* ctx);
    void (*handle)(void* ctx, int event);
} hsm_state_t;

/* 顶层状态 */
hsm_state_t state_top = {
    .name = "TOP",
    .parent = NULL,
    .entry = NULL,
    .exit = NULL,
    .handle = default_handler
};

/* 运行状态 */
void running_entry(void* ctx) { printf("Enter RUNNING\n"); }
void running_exit(void* ctx) { printf("Exit RUNNING\n"); }

hsm_state_t state_running = {
    .name = "RUNNING",
    .parent = &state_top,
    .entry = running_entry,
    .exit = running_exit,
    .handle = running_handler
};

/* 运行子状态: 处理中 */
void processing_entry(void* ctx) { printf("Enter PROCESSING\n"); }

hsm_state_t state_processing = {
    .name = "PROCESSING",
    .parent = &state_running,
    .entry = processing_entry,
    .exit = NULL,
    .handle = processing_handler
};

/* 查找事件处理器 (LCA算法) */
void hsm_dispatch(hsm_state_t** current, void* ctx, int event) {
    hsm_state_t* s = *current;

    while (s != NULL) {
        if (s->handle != NULL) {
            s->handle(ctx, event);
            return;
        }
        s = s->parent;  /* 向上查找 */
    }
}

/* 状态转换 */
void hsm_transition(hsm_state_t** current, hsm_state_t* target, void* ctx) {
    hsm_state_t* src = *current;
    hsm_state_t* dst = target;

    /* 找到最近公共祖先 */
    hsm_state_t* lca = find_lca(src, dst);

    /* 退出源状态树 */
    hsm_state_t* s = src;
    while (s != lca) {
        if (s->exit) s->exit(ctx);
        s = s->parent;
    }

    /* 记录路径 */
    hsm_state_t* path[16];
    int path_len = 0;
    s = dst;
    while (s != lca) {
        path[path_len++] = s;
        s = s->parent;
    }

    /* 进入目标状态树 (逆序) */
    for (int i = path_len - 1; i >= 0; i--) {
        if (path[i]->entry) path[i]->entry(ctx);
    }

    *current = target;
}
