/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 173
 * Language: c
 * Block ID: 3076dd13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分层状态机 - 支持状态嵌套和继承
typedef struct hsm_state {
    const char* name;
    struct hsm_state* parent;
    void (*on_entry)(void* context);
    void (*on_exit)(void* context);
    int (*handle)(void* context, int event);
} hsm_state_t;

typedef struct {
    hsm_state_t* current;
    void* context;
} hsm_t;

// 事件分发 - 支持事件冒泡
int hsm_dispatch(hsm_t* hsm, int event) {
    for (hsm_state_t* s = hsm->current; s; s = s->parent) {
        if (s->handle) {
            int result = s->handle(hsm->context, event);
            if (result == 0) return 0;  // 已处理
        }
    }
    return -1;  // 未处理
}
