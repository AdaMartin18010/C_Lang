/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 501
 * Language: c
 * Block ID: be323c1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 栈帧操作函数

void create_stack_frame(X86_64_State *state, int local_var_size) {
    // push rbp
    state->rsp -= 8;
    *(uint64_t*)(state->memory + state->rsp) = state->rbp;

    // mov rbp, rsp
    state->rbp = state->rsp;

    // sub rsp, local_var_size
    state->rsp -= local_var_size;
}

void destroy_stack_frame(X86_64_State *state) {
    // mov rsp, rbp
    state->rsp = state->rbp;

    // pop rbp
    state->rbp = *(uint64_t*)(state->memory + state->rsp);
    state->rsp += 8;
}
