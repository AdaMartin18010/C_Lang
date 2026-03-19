/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\06_Embedded_System_Design.md
 * Line: 80
 * Language: c
 * Block ID: fc3e3377
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 发动机控制状态机
typedef enum {
    STATE_OFF,
    STATE_CRANKING,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_FAULT
} EngineState;

typedef enum {
    EVT_IGNITION_ON,
    EVT_IGNITION_OFF,
    EVT_ENGINE_STARTED,
    EVT_FAULT_DETECTED,
    EVT_FAULT_CLEARED
} EngineEvent;

typedef struct {
    EngineState state;
    uint32_t entry_time;
    uint16_t fault_code;
} EngineControl;

// 状态处理函数表
typedef void (*StateHandler)(EngineControl *ec, const EngineEvent *evt);

void handle_off(EngineControl *ec, const EngineEvent *evt) {
    switch (*evt) {
        case EVT_IGNITION_ON:
            ec->state = STATE_CRANKING;
            ec->entry_time = get_tick_ms();
            start_cranking();
            break;
        default:
            break;
    }
}

// 主状态机循环
void engine_fsm_run(EngineControl *ec) {
    static const StateHandler handlers[] = {
        [STATE_OFF] = handle_off,
        [STATE_CRANKING] = handle_cranking,
        [STATE_IDLE] = handle_idle,
        [STATE_RUNNING] = handle_running,
        [STATE_FAULT] = handle_fault
    };

    EngineEvent evt;
    if (poll_event(&evt)) {
        handlers[ec->state](ec, &evt);
    }
}
