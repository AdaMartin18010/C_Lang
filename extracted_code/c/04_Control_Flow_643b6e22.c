/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 325
 * Language: c
 * Block ID: 643b6e22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct State State;
struct State {
    const char *name;
    State *(*handle)(Context *ctx, Event evt);
};

State *state_idle(Context *ctx, Event evt) {
    switch (evt) {
        case EVT_START: return &state_running;
        default: return NULL;  // 保持当前状态
    }
}

State *state_running(Context *ctx, Event evt) {
    switch (evt) {
        case EVT_STOP: return &state_idle;
        case EVT_PAUSE: return &state_paused;
        default: return NULL;
    }
}

// 运行状态机
void run_fsm(Context *ctx, State *initial) {
    State *current = initial;
    while (current) {
        Event evt = get_event(ctx);
        State *next = current->handle(ctx, evt);
        if (next) {
            printf("Transition: %s -> %s\n", current->name, next->name);
            current = next;
        }
    }
}
