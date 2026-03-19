/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 89
 * Language: c
 * Block ID: 1a6af2c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 编译器代码生成器的C实现框架

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_LOAD, OP_STORE, OP_CONST
} OpCode;

typedef struct Instruction {
    OpCode op;
    int dst;
    int src1;
    int src2;
    int imm;
} Instruction;

// 简单表达式编译：生成三地址码
void compile_expr(Expr *e, int *reg_counter, Instruction **code, int *count) {
    switch (e->type) {
        case EXPR_CONST:
            // 常量加载：R = imm
            (*code)[(*count)++] = (Instruction){
                .op = OP_CONST,
                .dst = (*reg_counter)++,
                .imm = e->value
            };
            break;

        case EXPR_VAR:
            // 变量加载：从栈帧加载到寄存器
            (*code)[(*count)++] = (Instruction){
                .op = OP_LOAD,
                .dst = (*reg_counter)++,
                .src1 = e->var_offset
            };
            break;

        case EXPR_ADD: {
            compile_expr(e->left, reg_counter, code, count);
            int left_reg = *reg_counter - 1;
            compile_expr(e->right, reg_counter, code, count);
            int right_reg = *reg_counter - 1;

            (*code)[(*count)++] = (Instruction){
                .op = OP_ADD,
                .dst = left_reg,      // 复用左操作数寄存器
                .src1 = left_reg,
                .src2 = right_reg
            };
            (*reg_counter)--;         // 释放右操作数寄存器
            break;
        }
    }
}
