/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 283
 * Language: c
 * Block ID: 0d1f5331
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指令语义执行框架

typedef void (*Instruction_Semantics)(X86_64_State *state, X86_Instruction *inst);

// MOV指令语义
void sem_mov(X86_64_State *state, X86_Instruction *inst) {
    int64_t src_value;

    // 确定源操作数
    if (inst->imm_size > 0) {
        src_value = inst->immediate;
    } else {
        // 从寄存器或内存读取
        int reg = inst->modrm & 0x7;
        src_value = get_register(state, reg);
    }

    // 确定目标并写入
    if (inst->has_modrm) {
        int mod = (inst->modrm >> 6) & 0x3;
        if (mod == 3) {  // 寄存器到寄存器
            int dest_reg = inst->modrm & 0x7;
            set_register(state, dest_reg, src_value);
        } else {  // 到内存
            uint64_t addr = calculate_effective_address(state, inst);
            write_memory(state, addr, src_value, inst->imm_size);
        }
    }

    // 更新PC
    state->rip += instruction_length(inst);
}

// ADD指令语义（更新标志位）
void sem_add(X86_64_State *state, X86_Instruction *inst) {
    int64_t dest = get_operand_value(state, inst, DEST);
    int64_t src = get_operand_value(state, inst, SRC);

    int64_t result = dest + src;

    // 设置标志位
    state->rflags.zf = (result == 0);
    state->rflags.sf = (result < 0);
    state->rflags.cf = ((uint64_t)result < (uint64_t)dest);  // 无符号溢出
    state->rflags.of = (((dest ^ result) & (src ^ result)) < 0);  // 有符号溢出
    state->rflags.pf = parity(result & 0xFF);

    // 写入结果
    set_operand_value(state, inst, DEST, result);

    state->rip += instruction_length(inst);
}

// JMP指令语义
void sem_jmp(X86_64_State *state, X86_Instruction *inst) {
    int64_t target;

    if (inst->imm_size > 0) {
        // 相对跳转
        target = state->rip + inst->immediate;
    } else {
        // 绝对跳转（寄存器或内存）
        target = get_operand_value(state, inst, SRC);
    }

    state->rip = target;
}

// 条件跳转（以JE为例）
void sem_je(X86_64_State *state, X86_Instruction *inst) {
    if (state->rflags.zf) {
        state->rip += inst->immediate;  // 跳转
    } else {
        state->rip += instruction_length(inst);  // 继续
    }
}
