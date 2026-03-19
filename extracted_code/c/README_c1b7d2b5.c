/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 364
 * Language: c
 * Block ID: c1b7d2b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单状态机 - 使用 switch-case
enum State { IDLE, RUNNING, PAUSED, STOPPED };
enum Event { START, PAUSE, RESUME, STOP };

void handle_event_simple(enum State* state, enum Event event) {
    switch (*state) {
        case IDLE:
            if (event == START) *state = RUNNING;
            break;
        case RUNNING:
            if (event == PAUSE) *state = PAUSED;
            else if (event == STOP) *state = STOPPED;
            break;
        case PAUSED:
            if (event == RESUME) *state = RUNNING;
            else if (event == STOP) *state = STOPPED;
            break;
        case STOPPED:
            if (event == START) *state = RUNNING;
            break;
    }
}
