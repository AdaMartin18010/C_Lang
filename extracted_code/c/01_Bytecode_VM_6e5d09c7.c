/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\01_Bytecode_VM.md
 * Line: 272
 * Language: c
 * Block ID: 6e5d09c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单汇编器：文本 -> 字节码

typedef struct {
    char *labels[256];
    size_t label_addrs[256];
    size_t label_count;

    uint8_t code[65536];
    size_t code_size;

    int64_t constants[256];
    size_t const_count;
} Assembler;

// 指令编码
void emit_byte(Assembler *a, uint8_t byte) {
    a->code[a->code_size++] = byte;
}

void emit_u16(Assembler *a, uint16_t val) {
    emit_byte(a, (val >> 8) & 0xFF);
    emit_byte(a, val & 0xFF);
}

size_t add_constant(Assembler *a, int64_t val) {
    a->constants[a->const_count] = val;
    return a->const_count++;
}

// 汇编一行代码
void assemble_line(Assembler *a, const char *line) {
    char op[32];
    char arg[32];

    // 解析标签
    if (sscanf(line, "%[^:]:", op) == 1) {
        // 是标签
        a->labels[a->label_count] = strdup(op);
        a->label_addrs[a->label_count] = a->code_size;
        a->label_count++;
        return;
    }

    // 解析指令
    if (sscanf(line, "%s %s", op, arg) < 1) return;

    if (strcmp(op, "PUSH") == 0) {
        emit_byte(a, OP_PUSH);
        emit_u16(a, atoi(arg));
    } else if (strcmp(op, "CONST") == 0) {
        emit_byte(a, OP_CONST);
        size_t idx = add_constant(a, atoll(arg));
        emit_byte(a, idx);
    } else if (strcmp(op, "ADD") == 0) {
        emit_byte(a, OP_ADD);
    } else if (strcmp(op, "SUB") == 0) {
        emit_byte(a, OP_SUB);
    } else if (strcmp(op, "MUL") == 0) {
        emit_byte(a, OP_MUL);
    } else if (strcmp(op, "DIV") == 0) {
        emit_byte(a, OP_DIV);
    } else if (strcmp(op, "PRINT") == 0) {
        emit_byte(a, OP_PRINT);
    } else if (strcmp(op, "HALT") == 0) {
        emit_byte(a, OP_HALT);
    }
}

// 示例程序
const char *program =
    "CONST 10\n"
    "CONST 20\n"
    "ADD\n"
    "PRINT\n"
    "HALT\n";
