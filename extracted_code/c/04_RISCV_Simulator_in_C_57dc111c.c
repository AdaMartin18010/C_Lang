/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 342
 * Language: c
 * Block ID: 57dc111c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//////////////////////////////////////////////////////////////////////////////
// execute.c - 执行引擎实现
//////////////////////////////////////////////////////////////////////////////

#include "riscv_sim.h"

// ALU操作
static u32 alu_add(u32 a, u32 b) { return a + b; }
static u32 alu_sub(u32 a, u32 b) { return a - b; }
static u32 alu_and(u32 a, u32 b) { return a & b; }
static u32 alu_or(u32 a, u32 b)  { return a | b; }
static u32 alu_xor(u32 a, u32 b) { return a ^ b; }
static u32 alu_sll(u32 a, u32 b) { return a << (b & 0x1F); }
static u32 alu_srl(u32 a, u32 b) { return a >> (b & 0x1F); }
static u32 alu_sra(u32 a, u32 b) { return (i32)a >> (b & 0x1F); }
static u32 alu_slt(u32 a, u32 b) { return (i32)a < (i32)b ? 1 : 0; }
static u32 alu_sltu(u32 a, u32 b) { return a < b ? 1 : 0; }

// 执行R型ALU指令
static void execute_r_type(RISCV_CPU *cpu, Instruction *inst) {
    u32 rs1_val = cpu_read_reg(cpu, inst->rs1);
    u32 rs2_val = cpu_read_reg(cpu, inst->rs2);
    u32 result = 0;

    if (inst->funct7 == 0x00) {
        switch (inst->funct3) {
            case 0x0: result = alu_add(rs1_val, rs2_val); break;  // ADD
            case 0x1: result = alu_sll(rs1_val, rs2_val); break;  // SLL
            case 0x2: result = alu_slt(rs1_val, rs2_val); break;  // SLT
            case 0x3: result = alu_sltu(rs1_val, rs2_val); break; // SLTU
            case 0x4: result = alu_xor(rs1_val, rs2_val); break;  // XOR
            case 0x5: result = alu_srl(rs1_val, rs2_val); break;  // SRL
            case 0x6: result = alu_or(rs1_val, rs2_val); break;   // OR
            case 0x7: result = alu_and(rs1_val, rs2_val); break;  // AND
        }
    } else if (inst->funct7 == 0x20) {
        switch (inst->funct3) {
            case 0x0: result = alu_sub(rs1_val, rs2_val); break;  // SUB
            case 0x5: result = alu_sra(rs1_val, rs2_val); break;  // SRA
        }
    }
    // RV32M扩展
    else if (inst->funct7 == 0x01) {
        #ifdef RV32M_ENABLE
        i64 mul_result;
        switch (inst->funct3) {
            case 0x0: result = (u32)((i64)(i32)rs1_val * (i64)(i32)rs2_val); break; // MUL
            case 0x1: // MULH
                mul_result = (i64)(i32)rs1_val * (i64)(i32)rs2_val;
                result = (u32)(mul_result >> 32);
                break;
            case 0x2: // MULHSU
                mul_result = (i64)(i32)rs1_val * (u64)rs2_val;
                result = (u32)(mul_result >> 32);
                break;
            case 0x3: // MULHU
                mul_result = (i64)(u64)rs1_val * (u64)rs2_val;
                result = (u32)(mul_result >> 32);
                break;
            case 0x4: // DIV
                if (rs2_val == 0) result = 0xFFFFFFFF;
                else if (rs1_val == 0x80000000 && rs2_val == 0xFFFFFFFF) result = 0x80000000;
                else result = (u32)((i32)rs1_val / (i32)rs2_val);
                break;
            case 0x5: // DIVU
                if (rs2_val == 0) result = 0xFFFFFFFF;
                else result = rs1_val / rs2_val;
                break;
            case 0x6: // REM
                if (rs2_val == 0) result = rs1_val;
                else if (rs1_val == 0x80000000 && rs2_val == 0xFFFFFFFF) result = 0;
                else result = (u32)((i32)rs1_val % (i32)rs2_val);
                break;
            case 0x7: // REMU
                if (rs2_val == 0) result = rs1_val;
                else result = rs1_val % rs2_val;
                break;
        }
        #endif
    }

    cpu_write_reg(cpu, inst->rd, result);
}

// 执行I型ALU指令
static void execute_i_type_alu(RISCV_CPU *cpu, Instruction *inst) {
    u32 rs1_val = cpu_read_reg(cpu, inst->rs1);
    u32 imm = (u32)inst->imm;
    u32 result = 0;

    switch (inst->funct3) {
        case 0x0: result = alu_add(rs1_val, imm); break;  // ADDI
        case 0x2: result = alu_slt(rs1_val, imm); break;  // SLTI
        case 0x3: result = alu_sltu(rs1_val, imm); break; // SLTIU
        case 0x4: result = alu_xor(rs1_val, imm); break;  // XORI
        case 0x6: result = alu_or(rs1_val, imm); break;   // ORI
        case 0x7: result = alu_and(rs1_val, imm); break;  // ANDI
        case 0x1: result = alu_sll(rs1_val, imm & 0x1F); break; // SLLI
        case 0x5:
            if ((imm >> 5) == 0x00) {
                result = alu_srl(rs1_val, imm & 0x1F);     // SRLI
            } else if ((imm >> 5) == 0x20) {
                result = alu_sra(rs1_val, imm & 0x1F);     // SRAI
            }
            break;
    }

    cpu_write_reg(cpu, inst->rd, result);
}

// 执行Load指令
static void execute_load(RISCV_CPU *cpu, Instruction *inst) {
    u32 addr = cpu_read_reg(cpu, inst->rs1) + (u32)inst->imm;
    u32 result = 0;

    switch (inst->funct3) {
        case 0x0: // LB
            result = (u32)(i32)(i8)cpu_read_byte(cpu, addr);
            break;
        case 0x1: // LH
            result = (u32)(i32)(i16)cpu_read_half(cpu, addr);
            break;
        case 0x2: // LW
            result = cpu_read_word(cpu, addr);
            break;
        case 0x4: // LBU
            result = (u32)cpu_read_byte(cpu, addr);
            break;
        case 0x5: // LHU
            result = (u32)cpu_read_half(cpu, addr);
            break;
    }

    cpu_write_reg(cpu, inst->rd, result);
}

// 执行Store指令
static void execute_store(RISCV_CPU *cpu, Instruction *inst) {
    u32 addr = cpu_read_reg(cpu, inst->rs1) + (u32)inst->imm;
    u32 rs2_val = cpu_read_reg(cpu, inst->rs2);

    switch (inst->funct3) {
        case 0x0: // SB
            cpu_write_byte(cpu, addr, (u8)rs2_val);
            break;
        case 0x1: // SH
            cpu_write_half(cpu, addr, (u16)rs2_val);
            break;
        case 0x2: // SW
            cpu_write_word(cpu, addr, rs2_val);
            break;
    }
}

// 执行分支指令
static void execute_branch(RISCV_CPU *cpu, Instruction *inst) {
    u32 rs1_val = cpu_read_reg(cpu, inst->rs1);
    u32 rs2_val = cpu_read_reg(cpu, inst->rs2);
    bool taken = false;

    switch (inst->funct3) {
        case 0x0: taken = (rs1_val == rs2_val); break;  // BEQ
        case 0x1: taken = (rs1_val != rs2_val); break;  // BNE
        case 0x4: taken = ((i32)rs1_val < (i32)rs2_val); break;  // BLT
        case 0x5: taken = ((i32)rs1_val >= (i32)rs2_val); break; // BGE
        case 0x6: taken = (rs1_val < rs2_val); break;   // BLTU
        case 0x7: taken = (rs1_val >= rs2_val); break;  // BGEU
    }

    if (taken) {
        cpu->regs.pc += (u32)inst->imm;
    } else {
        cpu->regs.pc += 4;
    }
}

// 执行跳转指令
static void execute_jal(RISCV_CPU *cpu, Instruction *inst) {
    cpu_write_reg(cpu, inst->rd, cpu->regs.pc + 4);
    cpu->regs.pc += (u32)inst->imm;
}

static void execute_jalr(RISCV_CPU *cpu, Instruction *inst) {
    u32 target = (cpu_read_reg(cpu, inst->rs1) + (u32)inst->imm) & ~1;
    cpu_write_reg(cpu, inst->rd, cpu->regs.pc + 4);
    cpu->regs.pc = target;
}

// 执行LUI/AUIPC
static void execute_lui(RISCV_CPU *cpu, Instruction *inst) {
    cpu_write_reg(cpu, inst->rd, (u32)inst->imm);
    cpu->regs.pc += 4;
}

static void execute_auipc(RISCV_CPU *cpu, Instruction *inst) {
    cpu_write_reg(cpu, inst->rd, cpu->regs.pc + (u32)inst->imm);
    cpu->regs.pc += 4;
}

// 执行系统指令
static void execute_system(RISCV_CPU *cpu, Instruction *inst) {
    if (inst->funct3 == 0x0) {
        if (inst->imm == 0x000) {  // ECALL
            // 系统调用处理
            u32 syscall_num = cpu_read_reg(cpu, REG_A7);
            u32 arg0 = cpu_read_reg(cpu, REG_A0);

            switch (syscall_num) {
                case 1:  // exit
                    cpu->running = false;
                    printf("Program exit with code: %d\n", arg0);
                    break;
                case 2:  // print string
                    {
                        u32 addr = cpu_read_reg(cpu, REG_A0);
                        while (1) {
                            char c = (char)cpu_read_byte(cpu, addr++);
                            if (c == '\0') break;
                            putchar(c);
                        }
                    }
                    break;
                case 3:  // print int
                    printf("%d", (i32)arg0);
                    break;
            }
        } else if (inst->imm == 0x001) {  // EBREAK
            printf("Breakpoint at PC=0x%08X\n", cpu->regs.pc);
            cpu->running = false;
        }
    } else {
        // CSR操作 (简化)
        u32 csr_addr = (u32)inst->imm & 0xFFF;
        u32 rs1_val = cpu_read_reg(cpu, inst->rs1);
        u32 csr_val = 0;

        switch (csr_addr) {
            case 0xC00: csr_val = cpu->csr.cycle; break;
            case 0xC02: csr_val = cpu->csr.instret; break;
        }

        cpu_write_reg(cpu, inst->rd, csr_val);

        switch (inst->funct3) {
            case 0x1: // CSRRW
            case 0x5: // CSRRWI
                // 写CSR
                break;
        }
    }

    cpu->regs.pc += 4;
}

// 主执行函数
void cpu_execute(RISCV_CPU *cpu, Instruction *inst) {
    // 更新统计
    cpu->csr.cycle++;
    cpu->csr.instret++;

    switch (inst->opcode) {
        case OPCODE_LUI:
            execute_lui(cpu, inst);
            break;
        case OPCODE_AUIPC:
            execute_auipc(cpu, inst);
            break;
        case OPCODE_JAL:
            execute_jal(cpu, inst);
            break;
        case OPCODE_JALR:
            execute_jalr(cpu, inst);
            break;
        case OPCODE_BRANCH:
            execute_branch(cpu, inst);
            break;
        case OPCODE_LOAD:
            execute_load(cpu, inst);
            cpu->regs.pc += 4;
            break;
        case OPCODE_STORE:
            execute_store(cpu, inst);
            cpu->regs.pc += 4;
            break;
        case OPCODE_ALUI:
            execute_i_type_alu(cpu, inst);
            cpu->regs.pc += 4;
            break;
        case OPCODE_ALU:
            execute_r_type(cpu, inst);
            cpu->regs.pc += 4;
            break;
        case OPCODE_SYSTEM:
            execute_system(cpu, inst);
            break;
        default:
            printf("Unknown opcode: 0x%02X at PC=0x%08X\n", inst->opcode, cpu->regs.pc);
            cpu->exception = true;
            cpu->running = false;
            break;
    }
}
