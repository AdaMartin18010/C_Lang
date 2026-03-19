/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 725
 * Language: c
 * Block ID: 64b2bc00
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 状态机实现，使用CBMC验证属性

typedef enum { STATE_IDLE, STATE_RUNNING, STATE_PAUSED, STATE_STOPPED } State;

typedef struct {
    State state;
    int counter;
    bool initialized;
} StateMachine;

// 使用CBMC验证以下属性：
// 1. 状态转换总是有效的
// 2. counter不会溢出
// 3. 必须先initialize才能start
// 4. stop后不能resume

void sm_init(StateMachine *sm);
void sm_start(StateMachine *sm);
void sm_pause(StateMachine *sm);
void sm_resume(StateMachine *sm);
void sm_stop(StateMachine *sm);

// CBMC验证桩
#ifdef CBMC
void verify_state_machine(void) {
    StateMachine sm;

    // 非确定性选择操作
    int choice;
    __CPROVER_assume(choice >= 0 && choice < 5);

    switch (choice) {
        case 0: sm_init(&sm); break;
        case 1: sm_start(&sm); break;
        case 2: sm_pause(&sm); break;
        case 3: sm_resume(&sm); break;
        case 4: sm_stop(&sm); break;
    }

    // 验证不变量
    __CPROVER_assert(sm.counter >= 0, "Counter non-negative");
    __CPROVER_assert(sm.state >= STATE_IDLE && sm.state <= STATE_STOPPED,
                     "Valid state");
}
#endif
