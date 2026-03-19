/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 143
 * Language: c
 * Block ID: 756f59d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用函数指针实现状态机
typedef struct StateMachine StateMachine;
typedef void (*StateFunc)(StateMachine *);

struct StateMachine {
    StateFunc current_state;
    int data;
};

void state_idle(StateMachine *sm);
void state_running(StateMachine *sm);
void state_paused(StateMachine *sm);
void state_stopped(StateMachine *sm);
