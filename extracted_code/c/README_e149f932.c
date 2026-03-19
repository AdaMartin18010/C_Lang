/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 389
 * Language: c
 * Block ID: e149f932
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 表驱动状态机 - 使用状态转换表
typedef struct {
    enum State current;
    enum Event event;
    enum State next;
    void (*action)(void);
} Transition;

static const Transition transitions[] = {
    { IDLE,    START,  RUNNING, on_start },
    { RUNNING, PAUSE,  PAUSED,  on_pause },
    { RUNNING, STOP,   STOPPED, on_stop },
    { PAUSED,  RESUME, RUNNING, on_resume },
    { PAUSED,  STOP,   STOPPED, on_stop },
    { STOPPED, START,  RUNNING, on_start },
};

void handle_event_table(enum State* state, enum Event event) {
    for (size_t i = 0; i < sizeof(transitions)/sizeof(Transition); i++) {
        if (transitions[i].current == *state &&
            transitions[i].event == event) {
            if (transitions[i].action) transitions[i].action();
            *state = transitions[i].next;
            return;
        }
    }
}
