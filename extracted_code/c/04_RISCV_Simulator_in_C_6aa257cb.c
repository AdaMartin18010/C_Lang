/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 73
 * Language: c
 * Block ID: 6aa257cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
