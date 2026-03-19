/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\01_State_Machine_Patterns.md
 * Line: 16
 * Language: c
 * Block ID: 646ca75a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 基本状态机 */

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
    STATE_COUNT
} state_t;

typedef enum {
    EVENT_START,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_STOP,
    EVENT_COUNT
} event_t;

/* 状态处理函数 */
typedef void (*state_handler_t)(void* ctx);

void idle_handler(void* ctx) {
    printf("Idle... waiting for start\n");
}

void running_handler(void* ctx) {
    printf("Running... processing\n");
}

void paused_handler(void* ctx) {
    printf("Paused...\n");
}

/* 状态机 */
typedef struct {
    state_t current;
    state_handler_t handlers[STATE_COUNT];
    void* context;
} state_machine_t;

void sm_init(state_machine_t* sm) {
    sm->current = STATE_IDLE;
    sm->handlers[STATE_IDLE] = idle_handler;
    sm->handlers[STATE_RUNNING] = running_handler;
    sm->handlers[STATE_PAUSED] = paused_handler;
}

void sm_run(state_machine_t* sm) {
    sm->handlers[sm->current](sm->context);
}

/* 状态转换 */
void sm_transition(state_machine_t* sm, event_t event) {
    static const state_t transition[STATE_COUNT][EVENT_COUNT] = {
        /* IDLE */    { STATE_RUNNING, STATE_IDLE,   STATE_IDLE,   STATE_IDLE },
        /* RUNNING */ { STATE_RUNNING, STATE_PAUSED, STATE_PAUSED, STATE_STOPPED },
        /* PAUSED */  { STATE_PAUSED,  STATE_PAUSED, STATE_RUNNING, STATE_STOPPED }
    };

    state_t next = transition[sm->current][event];
    if (next != sm->current) {
        printf("Transition: %d -> %d\n", sm->current, next);
        sm->current = next;
    }
}
