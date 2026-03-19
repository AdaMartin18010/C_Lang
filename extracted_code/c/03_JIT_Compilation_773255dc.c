/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\03_JIT_Compilation.md
 * Line: 401
 * Language: c
 * Block ID: 773255dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// vm.c - 包含解释器和JIT执行引擎
#include "bytecode.h"
#include "jit_compiler.h"
#include <stdio.h>
#include <stdlib.h>

// 解释器执行函数
int interpret(Bytecode *bc) {
    int stack[256];
    int sp = 0;  // 栈指针
    int pc = 0;  // 程序计数器
    int variables[64] = {0};

    while (pc < bc->code_size) {
        OpCode op = bc->code[pc];

        switch (op) {
            case OP_PUSH: {
                int const_idx = bc->code[++pc];
                stack[sp++] = bc->constants[const_idx];
                break;
            }

            case OP_POP:
                sp--;
                break;

            case OP_ADD: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a + b;
                break;
            }

            case OP_SUB: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a - b;
                break;
            }

            case OP_MUL: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a * b;
                break;
            }

            case OP_DIV: {
                int b = stack[--sp];
                int a = stack[--sp];
                stack[sp++] = a / b;
                break;
            }

            case OP_NEG:
                stack[sp-1] = -stack[sp-1];
                break;

            case OP_PRINT: {
                printf("%d\n", stack[sp-1]);
                break;
            }

            case OP_HALT:
                return stack[sp-1];

            default:
                fprintf(stderr, "Unknown opcode: %d\n", op);
                return -1;
        }
        pc++;
    }

    return stack[sp-1];
}

// 创建字节码示例
Bytecode* create_sample_bytecode(void) {
    Bytecode *bc = malloc(sizeof(Bytecode));
    bc->code_size = 16;
    bc->const_count = 4;

    bc->code = malloc(bc->code_size);
    bc->constants = malloc(bc->const_count * sizeof(int));

    // 常量: 10, 20, 3, 5
    bc->constants[0] = 10;
    bc->constants[1] = 20;
    bc->constants[2] = 3;
    bc->constants[3] = 5;

    // 计算: ((10 + 20) * 3) - 5 = 85
    int i = 0;
    bc->code[i++] = OP_PUSH; bc->code[i++] = 0;  // push 10
    bc->code[i++] = OP_PUSH; bc->code[i++] = 1;  // push 20
    bc->code[i++] = OP_ADD;                      // add
    bc->code[i++] = OP_PUSH; bc->code[i++] = 2;  // push 3
    bc->code[i++] = OP_MUL;                      // mul
    bc->code[i++] = OP_PUSH; bc->code[i++] = 3;  // push 5
    bc->code[i++] = OP_SUB;                      // sub
    bc->code[i++] = OP_PRINT;                    // print
    bc->code[i++] = OP_HALT;                     // halt

    return bc;
}

void free_bytecode(Bytecode *bc) {
    if (bc) {
        free(bc->code);
        free(bc->constants);
        free(bc);
    }
}

// 性能测试
void benchmark(Bytecode *bc, int iterations) {
    clock_t start, end;
    volatile int result;

    // 解释器性能测试
    start = clock();
    for (int i = 0; i < iterations; i++) {
        result = interpret(bc);
    }
    end = clock();
    printf("Interpreter: %.3f ms\n",
           (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    // JIT编译
    JITContext *jit = jit_init();
    JITFunction jit_func = jit_compile(jit, bc);

    // JIT执行测试
    start = clock();
    for (int i = 0; i < iterations; i++) {
        result = jit_func();
    }
    end = clock();
    printf("JIT: %.3f ms\n",
           (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    jit_free(jit);
}

int main(void) {
    printf("=== JIT Compiler Demo ===\n\n");

    Bytecode *bc = create_sample_bytecode();

    printf("Running interpreter...\n");
    int result = interpret(bc);
    printf("Interpreter result: %d\n\n", result);

    printf("Compiling with JIT...\n");
    JITContext *jit = jit_init();
    if (!jit) {
        fprintf(stderr, "Failed to initialize JIT\n");
        return 1;
    }

    JITFunction jit_func = jit_compile(jit, bc);
    printf("Running JIT compiled code...\n");
    result = jit_func();
    printf("JIT result: %d\n\n", result);

    printf("Running benchmark (1,000,000 iterations)...\n");
    benchmark(bc, 1000000);

    free_bytecode(bc);
    jit_free(jit);

    return 0;
}
