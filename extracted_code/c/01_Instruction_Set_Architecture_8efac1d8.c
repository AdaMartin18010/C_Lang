/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 364
 * Language: c
 * Block ID: 8efac1d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指令执行主循环（解释器模式）

void execute_program(uint8_t *code, size_t code_size, uint64_t entry_point) {
    X86_64_State state = {0};
    state.memory = malloc(4ULL * 1024 * 1024 * 1024);  // 4GB虚拟内存
    state.mem_size = 4ULL * 1024 * 1024 * 1024;
    memcpy(state.memory + entry_point, code, code_size);
    state.rip = entry_point;
    state.rsp = state.mem_size - 8;  // 栈底

    Instruction_Semantics dispatch_table[256] = {0};
    dispatch_table[0x89] = sem_mov;  // MOV r/m, r
    dispatch_table[0x01] = sem_add;  // ADD r/m, r
    dispatch_table[0xEB] = sem_jmp;  // JMP rel8
    // ... 更多指令

    int max_steps = 1000000;
    for (int i = 0; i < max_steps; i++) {
        // 取指
        uint8_t *pc = state.memory + state.rip;

        // 译码
        X86_Instruction inst;
        int len = decode_instruction(pc, &inst);

        // 执行
        uint8_t opcode = inst.opcode[0];
        if (dispatch_table[opcode]) {
            dispatch_table[opcode](&state, &inst);
        } else {
            printf("Unknown opcode: 0x%02X\n", opcode);
            break;
        }

        // 检查停机条件
        if (state.rip == 0) break;  // 返回到0地址视为停机
    }

    printf("Execution finished. RAX = 0x%lx\n", state.rax);
}
