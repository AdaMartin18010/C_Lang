/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 137
 * Language: c
 * Block ID: 2461fb0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 表驱动状态机 - 将转换逻辑分离到表中
typedef void (*action_fn_t)(void* context);
typedef bool (*guard_fn_t)(void* context);

typedef struct {
    state_t from_state;
    event_t event;
    state_t to_state;
    guard_fn_t guard;
    action_fn_t action;
    const char* description;
} transition_t;

static const transition_t transitions[] = {
    {STATE_IDLE,    EVENT_START,  STATE_RUNNING, NULL,       on_start,   "开始运行"},
    {STATE_RUNNING, EVENT_PAUSE,  STATE_PAUSED,  can_pause,  on_pause,   "暂停"},
    {STATE_RUNNING, EVENT_STOP,   STATE_STOPPED, NULL,       on_stop,    "停止"},
    {STATE_PAUSED,  EVENT_RESUME, STATE_RUNNING, NULL,       on_resume,  "恢复"},
    {STATE_PAUSED,  EVENT_STOP,   STATE_STOPPED, NULL,       on_stop,    "从暂停停止"},
    {STATE_STOPPED, EVENT_START,  STATE_RUNNING, can_restart, on_restart, "重新启动"},
};

const transition_t* find_transition(state_t from, event_t event) {
    for (size_t i = 0; i < sizeof(transitions)/sizeof(transition_t); i++) {
        if (transitions[i].from_state == from &&
            transitions[i].event == event) {
            return &transitions[i];
        }
    }
    return NULL;
}
