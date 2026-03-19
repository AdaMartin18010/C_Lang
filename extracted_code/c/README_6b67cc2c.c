/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\README.md
 * Line: 601
 * Language: c
 * Block ID: 6b67cc2c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 当优化假设失败时安全回退

void deoptimize(OptimizedFrame *frame) {
    // 1. 从优化代码状态恢复解释器状态
    InterpreterState *interp_state = allocate_interp_state();

    // 2. 寄存器 -> 栈帧映射
    for (int i = 0; i < frame->num_locals; i++) {
        interp_state->locals[i] = frame->registers[frame->reg_map[i]];
    }

    // 3. 恢复操作数栈
    interp_state->stack = reconstruct_stack(frame);

    // 4. 设置解释器PC
    interp_state->pc = frame->deopt_pc;

    // 5. 跳转到解释器
    enter_interpreter(interp_state);
}
