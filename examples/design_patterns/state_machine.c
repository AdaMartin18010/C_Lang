/*
 * 有限状态机（函数指针表驱动）示例
 * 编译: gcc -Wall -Wextra -o state_machine state_machine.c
 */
#include <stdio.h>

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
    STATE_COUNT
} State;

typedef enum {
    EVT_START,
    EVT_PAUSE,
    EVT_RESUME,
    EVT_STOP,
    EVT_COUNT
} Event;

const char *state_name(State s) {
    static const char *names[] = {"IDLE", "RUNNING", "PAUSED", "STOPPED"};
    return names[s];
}

/* 状态处理函数 */
typedef State (*StateHandler)(Event evt);

static State state_idle(Event evt) {
    if (evt == EVT_START) return STATE_RUNNING;
    return STATE_IDLE;
}

static State state_running(Event evt) {
    if (evt == EVT_PAUSE)  return STATE_PAUSED;
    if (evt == EVT_STOP)   return STATE_STOPPED;
    return STATE_RUNNING;
}

static State state_paused(Event evt) {
    if (evt == EVT_RESUME) return STATE_RUNNING;
    if (evt == EVT_STOP)   return STATE_STOPPED;
    return STATE_PAUSED;
}

static State state_stopped(Event evt) {
    if (evt == EVT_START) return STATE_RUNNING;
    return STATE_STOPPED;
}

/* 状态表 */
static StateHandler state_table[STATE_COUNT] = {
    [STATE_IDLE]    = state_idle,
    [STATE_RUNNING] = state_running,
    [STATE_PAUSED]  = state_paused,
    [STATE_STOPPED] = state_stopped,
};

/* 状态机驱动 */
State fsm_step(State current, Event evt) {
    StateHandler handler = state_table[current];
    State next = handler(evt);
    if (next != current) {
        printf("  %s -> %s\n", state_name(current), state_name(next));
    }
    return next;
}

int main(void) {
    State s = STATE_IDLE;
    
    printf("Initial: %s\n", state_name(s));
    s = fsm_step(s, EVT_START);
    s = fsm_step(s, EVT_PAUSE);
    s = fsm_step(s, EVT_RESUME);
    s = fsm_step(s, EVT_STOP);
    s = fsm_step(s, EVT_START);
    s = fsm_step(s, EVT_STOP);
    
    return 0;
}
