/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 577
 * Language: c
 * Block ID: adaddfb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 有限状态机的逻辑门实现

#define NUM_STATES 4
#define NUM_INPUTS 2
#define NUM_OUTPUTS 1

// 状态编码（使用触发器）
typedef enum { S0 = 0, S1 = 1, S2 = 2, S3 = 3 } State;

// 次态逻辑：next_state = f(current_state, input)
State next_state_logic(State current, uint8_t input) {
    // 状态转移表实现为组合逻辑
    switch (current) {
        case S0: return (input == 0) ? S0 : S1;
        case S1: return (input == 0) ? S0 : S2;
        case S2: return (input == 0) ? S1 : S3;
        case S3: return (input == 0) ? S2 : S3;
    }
    return S0;
}

// 输出逻辑：output = g(current_state, input)
Bool output_logic(State current, uint8_t input) {
    // Moore机或Mealy机
    return (current == S3) || (current == S2 && input == 1);
}

// FSM实现为触发器+组合逻辑
void fsm_implementation(Bool *state_ff, Bool input, Bool *output) {
    // 状态寄存器（D触发器）
    State current = (state_ff[1] << 1) | state_ff[0];

    // 组合逻辑计算次态
    State next = next_state_logic(current, input);

    // 输出逻辑
    *output = output_logic(current, input);

    // 下一时钟沿更新状态
    state_ff[0] = next & 1;
    state_ff[1] = (next >> 1) & 1;
}
