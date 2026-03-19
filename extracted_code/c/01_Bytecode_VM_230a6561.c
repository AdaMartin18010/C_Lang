/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\01_Bytecode_VM.md
 * Line: 144
 * Language: c
 * Block ID: 230a6561
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指令执行循环
void vm_run(VM *vm) {
    #define PUSH(v) (vm->stack[vm->sp++] = (v))
    #define POP()   (vm->stack[--vm->sp])
    #define PEEK()  (vm->stack[vm->sp - 1])

    for (;;) {
        uint8_t op = vm->code[vm->pc++];

        switch (op) {
            case OP_NOP:
                break;

            case OP_CONST: {
                // 下一条字节是常量索引
                uint8_t idx = vm->code[vm->pc++];
                PUSH(vm->constants[idx]);
                break;
            }

            case OP_PUSH: {
                // 直接压入操作数（2字节）
                int16_t val = (vm->code[vm->pc] << 8) | vm->code[vm->pc + 1];
                vm->pc += 2;
                PUSH(val);
                break;
            }

            case OP_POP:
                (void)POP();
                break;

            case OP_ADD: {
                int64_t b = POP();
                int64_t a = POP();
                PUSH(a + b);
                break;
            }

            case OP_SUB: {
                int64_t b = POP();
                int64_t a = POP();
                PUSH(a - b);
                break;
            }

            case OP_MUL: {
                int64_t b = POP();
                int64_t a = POP();
                PUSH(a * b);
                break;
            }

            case OP_DIV: {
                int64_t b = POP();
                int64_t a = POP();
                if (b == 0) {
                    fprintf(stderr, "Error: Division by zero\n");
                    return;
                }
                PUSH(a / b);
                break;
            }

            case OP_EQ: {
                int64_t b = POP();
                int64_t a = POP();
                PUSH(a == b ? 1 : 0);
                break;
            }

            case OP_LT: {
                int64_t b = POP();
                int64_t a = POP();
                PUSH(a < b ? 1 : 0);
                break;
            }

            case OP_JMP: {
                int16_t offset = (int16_t)((vm->code[vm->pc] << 8) | vm->code[vm->pc + 1]);
                vm->pc += offset;
                break;
            }

            case OP_JZ: {
                int16_t offset = (int16_t)((vm->code[vm->pc] << 8) | vm->code[vm->pc + 1]);
                vm->pc += 2;
                if (POP() == 0) {
                    vm->pc += offset;
                }
                break;
            }

            case OP_LOAD: {
                uint8_t idx = vm->code[vm->pc++];
                PUSH(vm->locals[idx]);
                break;
            }

            case OP_STORE: {
                uint8_t idx = vm->code[vm->pc++];
                vm->locals[idx] = POP();
                break;
            }

            case OP_PRINT: {
                printf("%ld\n", POP());
                break;
            }

            case OP_HALT:
                return;

            default:
                fprintf(stderr, "Unknown opcode: %d\n", op);
                return;
        }
    }

    #undef PUSH
    #undef POP
    #undef PEEK
}
