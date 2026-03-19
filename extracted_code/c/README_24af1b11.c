/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 87
 * Language: c
 * Block ID: 24af1b11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的 switch-case 状态机
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

void fsm_handle_event(state_t* current, event_t event) {
    switch (*current) {
        case STATE_IDLE:
            if (event == EVENT_START) {
                *current = STATE_RUNNING;
            }
            break;
        case STATE_RUNNING:
            if (event == EVENT_PAUSE) {
                *current = STATE_PAUSED;
            } else if (event == EVENT_STOP) {
                *current = STATE_STOPPED;
            }
            break;
        case STATE_PAUSED:
            if (event == EVENT_RESUME) {
                *current = STATE_RUNNING;
            } else if (event == EVENT_STOP) {
                *current = STATE_STOPPED;
            }
            break;
        case STATE_STOPPED:
            if (event == EVENT_START) {
                *current = STATE_RUNNING;
            }
            break;
    }
}
