/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\README.md
 * Line: 142
 * Language: c
 * Block ID: e673d645
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file simple_vm.c
 * @brief 简单栈式字节码虚拟机实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* 操作码定义 */
typedef enum {
    OP_NOP = 0,     /* 无操作 */
    OP_CONST,       /* 加载常量到栈 */
    OP_ADD,         /* 加法: 弹出两个值，压入和 */
    OP_SUB,         /* 减法 */
    OP_MUL,         /* 乘法 */
    OP_DIV,         /* 除法 */
    OP_NEG,         /* 取负 */
    OP_PRINT,       /* 打印栈顶 */
    OP_HALT         /* 停止 */
} OpCode;

/* 虚拟机状态 */
typedef struct {
    uint8_t *code;      /* 字节码 */
    int code_size;      /* 代码大小 */
    int pc;             /* 程序计数器 */

    int *stack;         /* 操作数栈 */
    int sp;             /* 栈指针 */
    int stack_size;     /* 栈大小 */

    int *constants;     /* 常量池 */
    int constant_count;
} VM;

/* 创建虚拟机 */
VM* vm_create(int stack_size, int max_constants) {
    VM *vm = malloc(sizeof(VM));
    vm->stack = malloc(sizeof(int) * stack_size);
    vm->stack_size = stack_size;
    vm->sp = 0;

    vm->constants = malloc(sizeof(int) * max_constants);
    vm->constant_count = 0;

    vm->pc = 0;
    return vm;
}

/* 压栈 */
void vm_push(VM *vm, int value) {
    if (vm->sp >= vm->stack_size) {
        fprintf(stderr, "Stack overflow!\n");
        exit(1);
    }
    vm->stack[vm->sp++] = value;
}

/* 弹栈 */
int vm_pop(VM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "Stack underflow!\n");
        exit(1);
    }
    return vm->stack[--vm->sp];
}

/* 执行字节码 */
void vm_run(VM *vm) {
    int a, b;

    while (vm->pc < vm->code_size) {
        uint8_t op = vm->code[vm->pc++];

        switch (op) {
            case OP_NOP:
                break;

            case OP_CONST: {
                /* 下一个字节是常量索引 */
                int const_idx = vm->code[vm->pc++];
                vm_push(vm, vm->constants[const_idx]);
                break;
            }

            case OP_ADD:
                b = vm_pop(vm);
                a = vm_pop(vm);
                vm_push(vm, a + b);
                break;

            case OP_SUB:
                b = vm_pop(vm);
                a = vm_pop(vm);
                vm_push(vm, a - b);
                break;

            case OP_MUL:
                b = vm_pop(vm);
                a = vm_pop(vm);
                vm_push(vm, a * b);
                break;

            case OP_DIV:
                b = vm_pop(vm);
                a = vm_pop(vm);
                if (b == 0) {
                    fprintf(stderr, "Division by zero!\n");
                    exit(1);
                }
                vm_push(vm, a / b);
                break;

            case OP_NEG:
                vm_push(vm, -vm_pop(vm));
                break;

            case OP_PRINT:
                printf("%d\n", vm_pop(vm));
                break;

            case OP_HALT:
                return;

            default:
                fprintf(stderr, "Unknown opcode: %d\n", op);
                exit(1);
        }
    }
}

/* 示例: 计算 (1 + 2) * 3 */
int main() {
    VM *vm = vm_create(256, 16);

    /* 设置常量池: [1, 2, 3] */
    vm->constants[0] = 1;
    vm->constants[1] = 2;
    vm->constants[2] = 3;
    vm->constant_count = 3;

    /* 字节码: (1 + 2) * 3 */
    uint8_t program[] = {
        OP_CONST, 0,    /* 加载常量0 (1) */
        OP_CONST, 1,    /* 加载常量1 (2) */
        OP_ADD,         /* 1 + 2 = 3 */
        OP_CONST, 2,    /* 加载常量2 (3) */
        OP_MUL,         /* 3 * 3 = 9 */
        OP_PRINT,       /* 打印结果 */
        OP_HALT
    };

    vm->code = program;
    vm->code_size = sizeof(program);

    printf("Running bytecode...\n");
    vm_run(vm);

    free(vm->stack);
    free(vm->constants);
    free(vm);

    return 0;
}
