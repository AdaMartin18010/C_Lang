/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 337
 * Language: c
 * Block ID: 811ec951
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单栈式虚拟机

typedef enum {
    OP_PUSH, OP_POP,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_JMP, OP_JZ, OP_JNZ,
    OP_CALL, OP_RET,
    OP_LOAD, OP_STORE,
    OP_PRINT, OP_HALT
} OpCode;

typedef struct {
    uint8_t *code;
    size_t code_size;
    int *stack;
    size_t stack_size;
    int sp, ip;
} VM;

void vm_load(VM *vm, const uint8_t *bytecode, size_t len);
void vm_run(VM *vm);
