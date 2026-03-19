/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\README.md
 * Line: 367
 * Language: c
 * Block ID: 4d6d5db7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file register_vm.c
 * @brief 简单寄存器式虚拟机
 */

#include <stdio.h>
#include <stdint.h>

#define NUM_REGS 16

typedef enum {
    R_OP_NOP = 0,
    R_OP_LOAD,      /* LOAD r, imm: r = imm */
    R_OP_ADD,       /* ADD r1, r2, r3: r1 = r2 + r3 */
    R_OP_SUB,       /* SUB r1, r2, r3: r1 = r2 - r3 */
    R_OP_MUL,       /* MUL r1, r2, r3: r1 = r2 * r3 */
    R_OP_DIV,       /* DIV r1, r2, r3: r1 = r2 / r3 */
    R_OP_MOV,       /* MOV r1, r2: r1 = r2 */
    R_OP_PRINT,     /* PRINT r: print r */
    R_OP_HALT
} R_OpCode;

/* 指令格式: [opcode:8] [reg1:8] [reg2:8] [reg3:8] [imm:32] */
typedef struct {
    uint8_t opcode;
    uint8_t r1, r2, r3;
    int32_t imm;
} Instruction;

typedef struct {
    int32_t regs[NUM_REGS];
    Instruction *code;
    int pc;
    int code_size;
} R_VM;

void r_vm_run(R_VM *vm) {
    while (vm->pc < vm->code_size) {
        Instruction *instr = &vm->code[vm->pc++];

        switch (instr->opcode) {
            case R_OP_NOP:
                break;

            case R_OP_LOAD:
                vm->regs[instr->r1] = instr->imm;
                break;

            case R_OP_ADD:
                vm->regs[instr->r1] = vm->regs[instr->r2] + vm->regs[instr->r3];
                break;

            case R_OP_SUB:
                vm->regs[instr->r1] = vm->regs[instr->r2] - vm->regs[instr->r3];
                break;

            case R_OP_MUL:
                vm->regs[instr->r1] = vm->regs[instr->r2] * vm->regs[instr->r3];
                break;

            case R_OP_DIV:
                vm->regs[instr->r1] = vm->regs[instr->r2] / vm->regs[instr->r3];
                break;

            case R_OP_MOV:
                vm->regs[instr->r1] = vm->regs[instr->r2];
                break;

            case R_OP_PRINT:
                printf("R%d = %d\n", instr->r1, vm->regs[instr->r1]);
                break;

            case R_OP_HALT:
                return;
        }
    }
}

/* 计算: r0 = (10 + 20) * 3 */
int main() {
    Instruction program[] = {
        {R_OP_LOAD, 0, 0, 0, 10},   /* r0 = 10 */
        {R_OP_LOAD, 1, 0, 0, 20},   /* r1 = 20 */
        {R_OP_ADD, 0, 0, 1, 0},     /* r0 = r0 + r1 = 30 */
        {R_OP_LOAD, 2, 0, 0, 3},    /* r2 = 3 */
        {R_OP_MUL, 0, 0, 2, 0},     /* r0 = r0 * r2 = 90 */
        {R_OP_PRINT, 0, 0, 0, 0},   /* print r0 */
        {R_OP_HALT, 0, 0, 0, 0}
    };

    R_VM vm = {
        .regs = {0},
        .code = program,
        .pc = 0,
        .code_size = sizeof(program) / sizeof(Instruction)
    };

    printf("Register VM running...\n");
    r_vm_run(&vm);

    return 0;
}
