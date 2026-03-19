/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\README.md
 * Line: 205
 * Language: c
 * Block ID: 8c616ef7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的 x86-64 汇编器

typedef struct {
    uint8_t* buffer;
    size_t capacity;
    size_t size;
} Assembler;

Assembler* assembler_create(size_t capacity) {
    Assembler* a = malloc(sizeof(Assembler));
    a->buffer = malloc(capacity);
    a->capacity = capacity;
    a->size = 0;
    return a;
}

void emit_u8(Assembler* a, uint8_t b) {
    if (a->size < a->capacity) {
        a->buffer[a->size++] = b;
    }
}

void emit_u32(Assembler* a, uint32_t v) {
    emit_u8(a, v & 0xff);
    emit_u8(a, (v >> 8) & 0xff);
    emit_u8(a, (v >> 16) & 0xff);
    emit_u8(a, (v >> 24) & 0xff);
}

void emit_u64(Assembler* a, uint64_t v) {
    for (int i = 0; i < 8; i++) {
        emit_u8(a, (v >> (i * 8)) & 0xff);
    }
}

// 寄存器编码
#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9

// 编码 REX 前缀
void emit_rex(Assembler* a, int w, int r, int x, int b) {
    emit_u8(a, 0x40 | (w << 3) | (r << 2) | (x << 1) | b);
}

// mov reg, imm64 (仅 64 位寄存器)
void emit_mov_reg_imm64(Assembler* a, int reg, uint64_t imm) {
    emit_rex(a, 1, 0, 0, reg >= 8);
    emit_u8(a, 0xb8 + (reg & 7));
    emit_u64(a, imm);
}

// mov r/m64, r64
void emit_mov_mem_reg(Assembler* a, int base_reg, int offset, int src_reg) {
    emit_rex(a, 1, src_reg >= 8, 0, base_reg >= 8);
    emit_u8(a, 0x89);

    // ModR/M + SIB 编码
    if (offset == 0 && base_reg != RBP) {
        // [base]
        emit_u8(a, 0x00 | ((src_reg & 7) << 3) | (base_reg & 7));
    } else if (offset >= -128 && offset <= 127) {
        // disp8[base]
        emit_u8(a, 0x40 | ((src_reg & 7) << 3) | (base_reg & 7));
        emit_u8(a, (int8_t)offset);
    } else {
        // disp32[base]
        emit_u8(a, 0x80 | ((src_reg & 7) << 3) | (base_reg & 7));
        emit_u32(a, (int32_t)offset);
    }
}

// add r64, imm32
void emit_add_reg_imm32(Assembler* a, int reg, int32_t imm) {
    emit_rex(a, 1, 0, 0, reg >= 8);
    if (imm >= -128 && imm <= 127) {
        emit_u8(a, 0x83);
        emit_u8(a, 0xc0 | (reg & 7));
        emit_u8(a, (int8_t)imm);
    } else {
        emit_u8(a, 0x81);
        emit_u8(a, 0xc0 | (reg & 7));
        emit_u32(a, (uint32_t)imm);
    }
}

// ret
void emit_ret(Assembler* a) {
    emit_u8(a, 0xc3);
}

// 生成函数：void set_value(int64_t* ptr, int64_t val) { *ptr = val + 42; }
void* generate_set_value_plus_42() {
    Assembler* a = assembler_create(256);

    // System V AMD64 ABI:
    // %rdi = ptr (第一个参数)
    // %rsi = val (第二个参数)

    // mov %rsi, %rax (保存 val)
    emit_rex(a, 1, 0, 0, 0);
    emit_u8(a, 0x89);
    emit_u8(a, 0xf0);  // ModR/M for mov rsi, rax

    // add $42, %rax
    emit_add_reg_imm32(a, RAX, 42);

    // mov %rax, (%rdi)
    emit_rex(a, 1, 0, 0, 0);
    emit_u8(a, 0x89);
    emit_u8(a, 0x07);  // ModR/M for mov rax, [rdi]

    // ret
    emit_ret(a);

    // 复制到可执行内存
    JitCode* jit = jit_code_create(a->size);
    memcpy(jit->code_buffer, a->buffer, a->size);
    jit_code_finalize(jit);
    flush_instruction_cache(jit->code_buffer, jit->size);

    free(a->buffer);
    free(a);

    return jit->code_buffer;
}
