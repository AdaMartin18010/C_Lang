/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 233
 * Language: c
 * Block ID: a486ff79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//////////////////////////////////////////////////////////////////////////////
// decode.c - 指令译码实现
//////////////////////////////////////////////////////////////////////////////

#include "riscv_sim.h"

// 提取立即数
static i32 extract_imm_i(u32 inst) {
    return ((i32)(inst & 0xFFF00000)) >> 20;
}

static i32 extract_imm_s(u32 inst) {
    i32 imm = ((inst >> 7) & 0x1F) |           // imm[4:0]
              ((inst >> 20) & 0xFE0);          // imm[11:5]
    return (imm << 20) >> 20;                   // 符号扩展
}

static i32 extract_imm_b(u32 inst) {
    i32 imm = ((inst >> 7) & 0x1E) |           // imm[4:1]
              ((inst >> 20) & 0x7E0) |         // imm[10:5]
              ((inst << 4) & 0x800) |          // imm[11]
              ((inst >> 19) & 0x1000);         // imm[12]
    return (imm << 19) >> 19;                   // 符号扩展
}

static i32 extract_imm_u(u32 inst) {
    return (i32)(inst & 0xFFFFF000);
}

static i32 extract_imm_j(u32 inst) {
    i32 imm = ((inst >> 20) & 0x7FE) |         // imm[10:1]
              ((inst >> 9) & 0x800) |          // imm[11]
              (inst & 0xFF000) |               // imm[19:12]
              ((inst >> 11) & 0x100000);       // imm[20]
    return (imm << 11) >> 11;                   // 符号扩展
}

// 获取指令类型
InstructionType get_instruction_type(u32 opcode) {
    switch (opcode) {
        case OPCODE_ALU:     return INST_TYPE_R;
        case OPCODE_ALUI:
        case OPCODE_LOAD:
        case OPCODE_JALR:
        case OPCODE_FENCE:
        case OPCODE_SYSTEM:  return INST_TYPE_I;
        case OPCODE_STORE:   return INST_TYPE_S;
        case OPCODE_BRANCH:  return INST_TYPE_B;
        case OPCODE_LUI:
        case OPCODE_AUIPC:   return INST_TYPE_U;
        case OPCODE_JAL:     return INST_TYPE_J;
        default:             return INST_TYPE_UNKNOWN;
    }
}

// 完整译码函数
void cpu_decode(u32 inst_raw, Instruction *inst) {
    // 提取通用字段
    inst->opcode = inst_raw & 0x7F;
    inst->rd     = (inst_raw >> 7) & 0x1F;
    inst->funct3 = (inst_raw >> 12) & 0x7;
    inst->rs1    = (inst_raw >> 15) & 0x1F;
    inst->rs2    = (inst_raw >> 20) & 0x1F;
    inst->funct7 = (inst_raw >> 25) & 0x7F;
    inst->imm    = 0;

    // 根据类型提取立即数
    InstructionType type = get_instruction_type(inst->opcode);

    switch (type) {
        case INST_TYPE_I:
            inst->imm = extract_imm_i(inst_raw);
            break;
        case INST_TYPE_S:
            inst->imm = extract_imm_s(inst_raw);
            break;
        case INST_TYPE_B:
            inst->imm = extract_imm_b(inst_raw);
            break;
        case INST_TYPE_U:
            inst->imm = extract_imm_u(inst_raw);
            break;
        case INST_TYPE_J:
            inst->imm = extract_imm_j(inst_raw);
            break;
        default:
            break;
    }
}

// 打印指令 (调试用)
void print_instruction(Instruction *inst) {
    const char *type_str[] = {"R", "I", "S", "B", "U", "J", "?"};
    InstructionType type = get_instruction_type(inst->opcode);

    printf("Inst: opcode=0x%02X, type=%s, rd=%d, rs1=%d, rs2=%d, "
           "funct3=%d, funct7=%d, imm=%d\n",
           inst->opcode, type_str[type], inst->rd, inst->rs1, inst->rs2,
           inst->funct3, inst->funct7, inst->imm);
}
