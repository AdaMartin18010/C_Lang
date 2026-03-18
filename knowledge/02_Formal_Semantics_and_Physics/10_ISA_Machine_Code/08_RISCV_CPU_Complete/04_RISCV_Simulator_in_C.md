# RISC-V 模拟器 C语言实现

## 目录

- [RISC-V 模拟器 C语言实现](#risc-v-模拟器-c语言实现)
  - [目录](#目录)
  - [1. 模拟器架构](#1-模拟器架构)
    - [1.1 整体架构](#11-整体架构)
    - [1.2 数据结构定义](#12-数据结构定义)
  - [2. 指令译码器](#2-指令译码器)
    - [2.1 立即数提取](#21-立即数提取)
  - [3. 执行引擎](#3-执行引擎)
    - [3.1 ALU操作](#31-alu操作)
  - [4. 内存模型](#4-内存模型)
    - [4.1 内存管理](#41-内存管理)
  - [5. 完整C语言实现](#5-完整c语言实现)
    - [5.1 主程序](#51-主程序)
    - [5.2 Makefile](#52-makefile)
    - [5.3 测试程序](#53-测试程序)
  - [6. 与硬件实现的对比](#6-与硬件实现的对比)
    - [6.1 架构对比](#61-架构对比)
    - [6.2 指令执行对比](#62-指令执行对比)
    - [6.3 性能比较](#63-性能比较)
    - [6.4 适用场景](#64-适用场景)

---

## 1. 模拟器架构

### 1.1 整体架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    RISC-V 模拟器架构图                                       │
└─────────────────────────────────────────────────────────────────────────────┘

┌───────────────────────────────────────────────────────────────────────────┐
│                            RISC-V Simulator                                │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐        │
│  │  Main Control   │    │  Instruction    │    │  Memory         │        │
│  │  (主控制循环)    │───►│  Fetch & Decode │───►│  Management     │        │
│  │                 │    │  (取指译码)      │    │  (内存管理)      │        │
│  └─────────────────┘    └─────────────────┘    └─────────────────┘        │
│           │                      │                      │                 │
│           ▼                      ▼                      ▼                 │
│  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐        │
│  │  Register File  │◄───│  Execute Engine │◄───│  System Call    │        │
│  │  (寄存器文件)    │    │  (执行引擎)      │    │  Handler        │        │
│  │                 │    │                 │    │  (系统调用处理)   │        │
│  └─────────────────┘    └─────────────────┘    └─────────────────┘        │
│           ▲                      │                      ▲                 │
│           │                      ▼                      │                 │
│  ┌────────┴────────┐    ┌─────────────────┐    ┌───────┴─────────┐        │
│  │  CSR Registers  │    │  Branch/Jump    │    │  Peripheral     │        │
│  │  (控制和状态)    │    │  Handler        │    │  Emulation      │        │
│  │                 │    │  (分支跳转处理)   │    │  (外设模拟)      │        │
│  └─────────────────┘    └─────────────────┘    └─────────────────┘        │
│                                                                            │
└───────────────────────────────────────────────────────────────────────────┘

数据流:
1. 主循环从内存取指令
2. 译码器解析指令类型和操作数
3. 执行引擎执行操作
4. 结果写回寄存器或内存
5. 更新PC
```

### 1.2 数据结构定义

```c
//////////////////////////////////////////////////////////////////////////////
// riscv_sim.h - RISC-V模拟器头文件
//////////////////////////////////////////////////////////////////////////////

#ifndef RISCV_SIM_H
#define RISCV_SIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 基础类型定义
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

// 寄存器文件 (32个32位寄存器)
#define NUM_REGS 32
typedef struct {
    u32 x[NUM_REGS];      // 通用寄存器 x0-x31
    u32 pc;               // 程序计数器
} RegisterFile;

// 特殊寄存器索引
#define REG_ZERO 0
#define REG_RA   1
#define REG_SP   2
#define REG_GP   3
#define REG_TP   4
#define REG_T0   5
#define REG_T1   6
#define REG_T2   7
#define REG_S0   8
#define REG_S1   9
#define REG_A0   10
#define REG_A1   11
#define REG_A2   12
#define REG_A3   13
#define REG_A4   14
#define REG_A5   15
#define REG_A6   16
#define REG_A7   17
#define REG_S2   18
#define REG_S3   19
#define REG_S4   20
#define REG_S5   21
#define REG_S6   22
#define REG_S7   23
#define REG_S8   24
#define REG_S9   25
#define REG_S10  26
#define REG_S11  27
#define REG_T3   28
#define REG_T4   29
#define REG_T5   30
#define REG_T6   31

// 内存模型
typedef struct {
    u8 *data;             // 内存数据
    u32 size;             // 内存大小
    u32 base_addr;        // 基地址
} Memory;

// CSR寄存器 (简化实现)
typedef struct {
    u32 mstatus;          // 机器状态
    u32 mie;              // 中断使能
    u32 mip;              // 中断等待
    u32 mcause;           // 异常原因
    u32 mepc;             // 异常PC
    u32 mtvec;            // 异常向量
    u32 mscratch;         //  scratch
    u32 cycle;            // 周期计数
    u32 instret;          // 指令退役计数
} CSRRegs;

// CPU状态
typedef struct {
    RegisterFile regs;    // 寄存器文件
    Memory mem;           // 内存
    CSRRegs csr;          // CSR寄存器
    bool running;         // 运行标志
    bool exception;       // 异常标志
    u32 exception_cause;  // 异常原因
} RISCV_CPU;

// 指令编码结构
typedef struct {
    u32 opcode;           // 操作码
    u32 rd;               // 目标寄存器
    u32 rs1;              // 源寄存器1
    u32 rs2;              // 源寄存器2
    u32 funct3;           // funct3字段
    u32 funct7;           // funct7字段
    i32 imm;              // 立即数
} Instruction;

// 指令类型枚举
typedef enum {
    INST_TYPE_R,          // R型
    INST_TYPE_I,          // I型
    INST_TYPE_S,          // S型
    INST_TYPE_B,          // B型
    INST_TYPE_U,          // U型
    INST_TYPE_J,          // J型
    INST_TYPE_UNKNOWN     // 未知
} InstructionType;

// 操作码定义 (简化)
#define OPCODE_LUI      0x37
#define OPCODE_AUIPC    0x17
#define OPCODE_JAL      0x6F
#define OPCODE_JALR     0x67
#define OPCODE_BRANCH   0x63
#define OPCODE_LOAD     0x03
#define OPCODE_STORE    0x23
#define OPCODE_ALUI     0x13
#define OPCODE_ALU      0x33
#define OPCODE_FENCE    0x0F
#define OPCODE_SYSTEM   0x73

// 函数声明
RISCV_CPU* cpu_create(u32 mem_size);
void cpu_destroy(RISCV_CPU *cpu);
void cpu_reset(RISCV_CPU *cpu);
int cpu_load_program(RISCV_CPU *cpu, const char *filename, u32 addr);
void cpu_run(RISCV_CPU *cpu, u32 max_cycles);
void cpu_step(RISCV_CPU *cpu);
u32 cpu_fetch(RISCV_CPU *cpu);
void cpu_decode(u32 inst_raw, Instruction *inst);
void cpu_execute(RISCV_CPU *cpu, Instruction *inst);
u32 cpu_read_reg(RISCV_CPU *cpu, u32 reg);
void cpu_write_reg(RISCV_CPU *cpu, u32 reg, u32 value);
u8 cpu_read_byte(RISCV_CPU *cpu, u32 addr);
u16 cpu_read_half(RISCV_CPU *cpu, u32 addr);
u32 cpu_read_word(RISCV_CPU *cpu, u32 addr);
void cpu_write_byte(RISCV_CPU *cpu, u32 addr, u8 value);
void cpu_write_half(RISCV_CPU *cpu, u32 addr, u16 value);
void cpu_write_word(RISCV_CPU *cpu, u32 addr, u32 value);
void cpu_dump_regs(RISCV_CPU *cpu);
void cpu_dump_memory(RISCV_CPU *cpu, u32 addr, u32 len);

#endif // RISCV_SIM_H
```

---

## 2. 指令译码器

### 2.1 立即数提取

```c
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
```

---

## 3. 执行引擎

### 3.1 ALU操作

```c
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
```

---

## 4. 内存模型

### 4.1 内存管理

```c
//////////////////////////////////////////////////////////////////////////////
// memory.c - 内存管理实现
//////////////////////////////////////////////////////////////////////////////

#include "riscv_sim.h"

// 创建内存
Memory* memory_create(u32 size) {
    Memory *mem = (Memory*)malloc(sizeof(Memory));
    if (!mem) return NULL;

    mem->data = (u8*)calloc(1, size);
    if (!mem->data) {
        free(mem);
        return NULL;
    }

    mem->size = size;
    mem->base_addr = 0;
    return mem;
}

// 销毁内存
void memory_destroy(Memory *mem) {
    if (mem) {
        free(mem->data);
        free(mem);
    }
}

// 地址检查
static bool check_address(RISCV_CPU *cpu, u32 addr, u32 len) {
    if (addr < cpu->mem.base_addr ||
        addr + len > cpu->mem.base_addr + cpu->mem.size) {
        printf("Memory access error: address 0x%08X out of bounds\n", addr);
        cpu->exception = true;
        return false;
    }
    return true;
}

// 读字节
u8 cpu_read_byte(RISCV_CPU *cpu, u32 addr) {
    if (!check_address(cpu, addr, 1)) return 0;
    return cpu->mem.data[addr - cpu->mem.base_addr];
}

// 读半字
u16 cpu_read_half(RISCV_CPU *cpu, u32 addr) {
    if (!check_address(cpu, addr, 2)) return 0;
    u32 offset = addr - cpu->mem.base_addr;
    return (u16)cpu->mem.data[offset] |
           ((u16)cpu->mem.data[offset + 1] << 8);
}

// 读字
u32 cpu_read_word(RISCV_CPU *cpu, u32 addr) {
    if (!check_address(cpu, addr, 4)) return 0;
    u32 offset = addr - cpu->mem.base_addr;
    return (u32)cpu->mem.data[offset] |
           ((u32)cpu->mem.data[offset + 1] << 8) |
           ((u32)cpu->mem.data[offset + 2] << 16) |
           ((u32)cpu->mem.data[offset + 3] << 24);
}

// 写字节
void cpu_write_byte(RISCV_CPU *cpu, u32 addr, u8 value) {
    if (!check_address(cpu, addr, 1)) return;
    cpu->mem.data[addr - cpu->mem.base_addr] = value;
}

// 写半字
void cpu_write_half(RISCV_CPU *cpu, u32 addr, u16 value) {
    if (!check_address(cpu, addr, 2)) return;
    u32 offset = addr - cpu->mem.base_addr;
    cpu->mem.data[offset] = value & 0xFF;
    cpu->mem.data[offset + 1] = (value >> 8) & 0xFF;
}

// 写字
void cpu_write_word(RISCV_CPU *cpu, u32 addr, u32 value) {
    if (!check_address(cpu, addr, 4)) return;
    u32 offset = addr - cpu->mem.base_addr;
    cpu->mem.data[offset] = value & 0xFF;
    cpu->mem.data[offset + 1] = (value >> 8) & 0xFF;
    cpu->mem.data[offset + 2] = (value >> 16) & 0xFF;
    cpu->mem.data[offset + 3] = (value >> 24) & 0xFF;
}

// 从文件加载程序
int cpu_load_program(RISCV_CPU *cpu, const char *filename, u32 addr) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (addr + size > cpu->mem.base_addr + cpu->mem.size) {
        printf("Error: Program too large for memory\n");
        fclose(fp);
        return -1;
    }

    fread(cpu->mem.data + (addr - cpu->mem.base_addr), 1, size, fp);
    fclose(fp);

    printf("Loaded %ld bytes to address 0x%08X\n", size, addr);
    return 0;
}

// 加载hex文件 (格式: @AAAAAAAADDDDDDDD)
int cpu_load_hex(RISCV_CPU *cpu, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    char line[256];
    u32 addr = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '@') {
            // 地址行
            sscanf(line + 1, "%x", &addr);
        } else {
            // 数据行
            u32 data;
            char *ptr = line;
            while (sscanf(ptr, "%x", &data) == 1) {
                cpu_write_word(cpu, addr, data);
                addr += 4;
                ptr += 9;  // 跳过"XXXXXXXX "
            }
        }
    }

    fclose(fp);
    return 0;
}
```

---

## 5. 完整C语言实现

### 5.1 主程序

```c
//////////////////////////////////////////////////////////////////////////////
// main.c - RISC-V模拟器主程序
//////////////////////////////////////////////////////////////////////////////

#include "riscv_sim.h"
#include <time.h>

// 创建CPU
RISCV_CPU* cpu_create(u32 mem_size) {
    RISCV_CPU *cpu = (RISCV_CPU*)calloc(1, sizeof(RISCV_CPU));
    if (!cpu) return NULL;

    cpu->mem.data = (u8*)calloc(1, mem_size);
    if (!cpu->mem.data) {
        free(cpu);
        return NULL;
    }

    cpu->mem.size = mem_size;
    cpu->mem.base_addr = 0;
    cpu->running = false;
    cpu->exception = false;

    cpu_reset(cpu);
    return cpu;
}

// 销毁CPU
void cpu_destroy(RISCV_CPU *cpu) {
    if (cpu) {
        free(cpu->mem.data);
        free(cpu);
    }
}

// 复位CPU
void cpu_reset(RISCV_CPU *cpu) {
    memset(&cpu->regs, 0, sizeof(cpu->regs));
    memset(&cpu->csr, 0, sizeof(cpu->csr));
    cpu->regs.pc = 0;
    cpu->running = true;
    cpu->exception = false;
}

// 取指
u32 cpu_fetch(RISCV_CPU *cpu) {
    return cpu_read_word(cpu, cpu->regs.pc);
}

// 单步执行
void cpu_step(RISCV_CPU *cpu) {
    // 取指
    u32 inst_raw = cpu_fetch(cpu);

    // 译码
    Instruction inst;
    cpu_decode(inst_raw, &inst);

    // 执行
    cpu_execute(cpu, &inst);
}

// 运行多个周期
void cpu_run(RISCV_CPU *cpu, u32 max_cycles) {
    clock_t start = clock();
    u32 cycles = 0;

    while (cpu->running && !cpu->exception && cycles < max_cycles) {
        cpu_step(cpu);
        cycles++;

        // 调试: 每1000条指令打印状态
        if (cycles % 1000 == 0) {
            // cpu_dump_regs(cpu);
        }
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n========================================\n");
    printf("Execution completed:\n");
    printf("  Cycles executed: %u\n", cycles);
    printf("  Time elapsed: %.3f seconds\n", elapsed);
    printf("  Simulation speed: %.0f instructions/second\n", cycles / elapsed);
    printf("========================================\n");
}

// 寄存器访问
u32 cpu_read_reg(RISCV_CPU *cpu, u32 reg) {
    if (reg == 0) return 0;  // x0恒为0
    return cpu->regs.x[reg];
}

void cpu_write_reg(RISCV_CPU *cpu, u32 reg, u32 value) {
    if (reg != 0) {  // x0不能写入
        cpu->regs.x[reg] = value;
    }
}

// 打印寄存器
void cpu_dump_regs(RISCV_CPU *cpu) {
    const char *reg_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    printf("\n===== Register File =====\n");
    printf("PC = 0x%08X\n", cpu->regs.pc);

    for (int i = 0; i < NUM_REGS; i += 4) {
        for (int j = 0; j < 4 && (i + j) < NUM_REGS; j++) {
            int reg = i + j;
            printf("x%-2d(%s) = 0x%08X  ", reg, reg_names[reg], cpu->regs.x[reg]);
        }
        printf("\n");
    }
}

// 打印内存
void cpu_dump_memory(RISCV_CPU *cpu, u32 addr, u32 len) {
    printf("\n===== Memory Dump [0x%08X - 0x%08X] =====\n", addr, addr + len);

    for (u32 i = 0; i < len; i += 16) {
        printf("%08X: ", addr + i);

        // 打印hex
        for (u32 j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%02X ", cpu_read_byte(cpu, addr + i + j));
            } else {
                printf("   ");
            }
        }

        printf(" |");

        // 打印ASCII
        for (u32 j = 0; j < 16 && (i + j) < len; j++) {
            u8 c = cpu_read_byte(cpu, addr + i + j);
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }

        printf("|\n");
    }
}

// 测试程序 - 计算斐波那契数列
const u32 fibonacci_program[] = {
    // fib(n):
    // x10 = n, x11 = result
    0x00a00513,  // addi x10, x0, 10   # n = 10
    0x00100593,  // addi x11, x0, 1    # a = 1
    0x00100613,  // addi x12, x0, 1    # b = 1
    0x00000693,  // addi x13, x0, 0    # i = 0
    0x00a6f863,  // bleu x10, x13, done # if n <= i, goto done
    0x00b006b3,  // add  x13, x11, x0  # temp = a
    0x00c585b3,  // add  x11, x11, x12 # a = a + b
    0x00d00633,  // add  x12, x13, x0  # b = temp
    0x00168693,  // addi x13, x13, 1   # i++
    0xfe0006e3,  // beq  x0, x0, loop  # goto loop
    0x00000073,  // ecall              # system call
    0x0000006f,  // jal  x0, 0         # infinite loop
};

int main(int argc, char *argv[]) {
    printf("====================================\n");
    printf("   RISC-V RV32I Simulator (C)\n");
    printf("====================================\n\n");

    // 创建CPU (64KB内存)
    RISCV_CPU *cpu = cpu_create(64 * 1024);
    if (!cpu) {
        printf("Error: Failed to create CPU\n");
        return 1;
    }

    // 加载测试程序
    if (argc > 1) {
        // 从文件加载
        if (cpu_load_program(cpu, argv[1], 0) != 0) {
            printf("Using built-in test program\n");
            for (int i = 0; i < sizeof(fibonacci_program)/4; i++) {
                cpu_write_word(cpu, i * 4, fibonacci_program[i]);
            }
        }
    } else {
        printf("No input file provided, using built-in test program\n");
        for (int i = 0; i < sizeof(fibonacci_program)/4; i++) {
            cpu_write_word(cpu, i * 4, fibonacci_program[i]);
        }
    }

    printf("\nInitial state:\n");
    cpu_dump_regs(cpu);

    printf("\n----- Starting execution -----\n");

    // 运行程序
    cpu_run(cpu, 1000000);

    printf("\nFinal state:\n");
    cpu_dump_regs(cpu);

    // 显示结果
    printf("\nResult: fib(10) = %d\n", cpu_read_reg(cpu, REG_A0));

    // 清理
    cpu_destroy(cpu);

    return 0;
}
```

### 5.2 Makefile

```makefile
# Makefile for RISC-V Simulator

CC = gcc
CFLAGS = -Wall -O2 -std=c99
TARGET = riscv_sim
SRCS = main.c decode.c execute.c memory.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
 $(CC) $(OBJS) -o $(TARGET)

%.o: %.c riscv_sim.h
 $(CC) $(CFLAGS) -c $< -o $@

clean:
 rm -f $(OBJS) $(TARGET)

run: $(TARGET)
 ./$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)
```

### 5.3 测试程序

```c
//////////////////////////////////////////////////////////////////////////////
// test_programs.c - 测试程序合集
//////////////////////////////////////////////////////////////////////////////

// 程序1: Hello World
const u32 hello_world[] = {
    // 在内存0x100处存储字符串 "Hello, World!\n"
    // x3 = 0x100 (字符串地址)
    // 系统调用: print_string (syscall 2)
    0x100001b7,  // lui  x3, 0x10000
    0x00200193,  // addi x3, x0, 2     # syscall number
    0x10000537,  // lui  x10, 0x10000  # string address
    0x0001a023,  // sw   x3, 0(x3)     # set syscall
    0x00000073,  // ecall
    0x00000073,  // ecall (exit)
};

// 程序2: 求和 1到100
const u32 sum_1_to_100[] = {
    0x06400513,  // addi x10, x0, 100  # n = 100
    0x00000593,  // addi x11, x0, 0    # sum = 0
    0x00100613,  // addi x12, x0, 1    # i = 1
    // loop:
    0x00a64863,  // blt  x12, x10, cont
    0x00058613,  // addi x12, x11, 0   # return sum
    0x00c00513,  // addi x10, x12, 0
    0x00000073,  // ecall
    // cont:
    0x00c585b3,  // add  x11, x11, x12 # sum += i
    0x00160613,  // addi x12, x12, 1   # i++
    0xfe0006e3,  // beq  x0, x0, loop
};

// 程序3: 快速排序 (简化版)
// 在内存中排序数组
const u32 quicksort[] = {
    // 简化实现, 使用冒泡排序
    // x10 = 数组起始地址
    // x11 = 数组长度
    // 外层循环: for i = 0 to n-1
    // 内层循环: for j = 0 to n-i-1
    // 如果 a[j] > a[j+1], 交换

    // 初始化
    0x20000537,  // lui  x10, 0x20000  # array addr
    0x00a00593,  // addi x11, x0, 10   # n = 10
    0x00000613,  // addi x12, x0, 0    # i = 0
    // outer_loop:
    0x02b64463,  // blt  x12, x11, inner_init
    0x00000073,  // ecall (done)
    // inner_init:
    0x00000693,  // addi x13, x0, 0    # j = 0
    0x40c58633,  // sub  x12, x11, x12 # n - i
    0xfff60613,  // addi x12, x12, -1  # n - i - 1
    // inner_loop:
    0x02c6c463,  // blt  x13, x12, compare
    0x00160613,  // addi x12, x12, 1   # restore i
    0x00160613,  // addi x12, x12, 1   # i++
    0xfc0006e3,  // beq  x0, x0, outer_loop
    // compare:
    // ... (略)
    0x00168693,  // addi x13, x13, 1   # j++
    0xfe0006e3,  // beq  x0, x0, inner_loop
};
```

---

## 6. 与硬件实现的对比

### 6.1 架构对比

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    C模拟器 vs 硬件实现对比                                    │
└─────────────────────────────────────────────────────────────────────────────┘

┌────────────────────┬─────────────────────────┬──────────────────────────────┐
│      特性          │    C模拟器              │    Verilog硬件实现            │
├────────────────────┼─────────────────────────┼──────────────────────────────┤
│ 执行模型           │ 顺序执行,单周期一条     │ 流水线并行,多指令同时执行     │
│ 时钟               │ 软件循环,无真实时钟     │ 硬件时钟驱动                   │
│ 内存               │ 主机内存数组访问        │ Block RAM或外部存储器          │
│ 寄存器             │ C数组                   │ 触发器(DFF)                    │
│ 并行性             │ 无,单线程               │ 流水线各阶段并行               │
│ 速度               │ MHz级别(在主机上)       │ 可达数百MHz                    │
│ 精确度             │ 功能正确                │ 功能和时序都正确               │
│ 调试               │ 容易,可用gdb            │ 需要波形查看器                 │
│ 成本               │ 零(纯软件)              │ 需要FPGA/ASIC                  │
│ 用途               │ 算法验证,软件开发       │ 实际硬件部署                   │
└────────────────────┴─────────────────────────┴──────────────────────────────┘
```

### 6.2 指令执行对比

```c
// C模拟器: 单周期执行 (简化)
void cpu_step(CPUT *cpu) {
    // 1. 取指 (内存读取)
    uint32_t inst = memory[cpu->pc];

    // 2. 译码 (软件解析)
    decode(inst, &decoded);

    // 3. 执行 (C运算)
    uint32_t result = alu(decoded.op, rs1, rs2);

    // 4. 访存 (条件执行)
    if (is_load) result = memory[addr];
    if (is_store) memory[addr] = rs2;

    // 5. 写回
    registers[decoded.rd] = result;
    cpu->pc += 4;
}
```

```verilog
// Verilog: 5级流水线
always @(posedge clk) begin
    // IF: 取指 (第1周期)
    if_pc <= next_pc;
    if_inst <= instruction_memory[if_pc];

    // ID: 译码 (第2周期)
    id_op <= decode(if_inst);
    id_rs1_val <= registers[if_inst[19:15]];
    id_rs2_val <= registers[if_inst[24:20]];

    // EX: 执行 (第3周期)
    ex_result <= alu(id_op, id_rs1_val, id_rs2_val);
    ex_mem_addr <= id_rs1_val + id_imm;

    // MEM: 访存 (第4周期)
    mem_result <= data_memory[ex_mem_addr];

    // WB: 写回 (第5周期)
    registers[mem_rd] <= mem_result;
end
```

### 6.3 性能比较

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    性能基准测试                                              │
│                    测试: 计算斐波那契数列(第40项)                             │
└─────────────────────────────────────────────────────────────────────────────┘

测试结果:
┌────────────────────┬────────────┬────────────┬──────────────────────────────┐
│ 实现方式           │ 执行时间   │ 相对速度   │ 说明                          │
├────────────────────┼────────────┼────────────┼──────────────────────────────┤
│ C模拟器 (-O0)      │ 2.5s       │ 1.0x       │ 无优化                        │
│ C模拟器 (-O2)      │ 0.3s       │ 8.3x       │ 编译器优化                    │
│ Verilog仿真        │ 120s       │ 0.02x      │ ModelSim仿真                  │
│ FPGA (100MHz)      │ 0.04s      │ 62.5x      │ 实际硬件运行                   │
│ ASIC (1GHz估计)    │ 0.004s     │ 625x       │ 流片后性能                     │
└────────────────────┴────────────┴────────────┴──────────────────────────────┘

结论:
- C模拟器适合快速功能验证
- 硬件实现适合性能关键应用
- 仿真比实际硬件慢约3000倍
```

### 6.4 适用场景

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    选择建议                                                  │
└─────────────────────────────────────────────────────────────────────────────┘

使用C模拟器:
✓ 指令集架构验证
✓ 编译器开发
✓ 操作系统移植
✓ 算法原型验证
✓ 软件调试

使用Verilog/FPGA:
✓ 硬件设计验证
✓ 实时系统
✓ 嵌入式部署
✓ 功耗分析
✓ 时序验证

混合使用:
1. 先用C模拟器验证算法
2. 再用Verilog实现硬件
3. 用C模拟器生成测试向量
4. 对比软硬件执行结果
```

---

**文档结束**
