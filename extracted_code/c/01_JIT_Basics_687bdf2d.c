/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 214
 * Language: c
 * Block ID: 687bdf2d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// x86-64指令编码器

typedef struct {
    uint8_t *code;
    size_t offset;
    size_t capacity;
} X86Encoder;

// 编码辅助函数
static void emit_u8(X86Encoder *enc, uint8_t byte) {
    if (enc->offset >= enc->capacity) {
        // 扩展缓冲区
        enc->capacity *= 2;
        enc->code = realloc(enc->code, enc->capacity);
    }
    enc->code[enc->offset++] = byte;
}

static void emit_u32(X86Encoder *enc, uint32_t val) {
    emit_u8(enc, val & 0xFF);
    emit_u8(enc, (val >> 8) & 0xFF);
    emit_u8(enc, (val >> 16) & 0xFF);
    emit_u8(enc, (val >> 24) & 0xFF);
}

// REX前缀（64位模式）
// W=1 (64位操作), R/X/B=0
#define REX_W 0x48

// ModR/M字节编码
// mod(2) | reg(3) | rm(3)
#define MODRM(mod, reg, rm) (((mod) << 6) | ((reg) << 3) | (rm))

// 寄存器编码
enum X86Reg {
    RAX = 0, RCX = 1, RDX = 2, RBX = 3,
    RSP = 4, RBP = 5, RSI = 6, RDI = 7,
    R8 = 8, R9 = 9, R10 = 10, R11 = 11,
    R12 = 12, R13 = 13, R14 = 14, R15 = 15
};

// mov rax, imm64
void emit_mov_rax_imm64(X86Encoder *enc, uint64_t imm) {
    emit_u8(enc, REX_W);
    emit_u8(enc, 0xB8);  // mov r64, imm64
    emit_u8(enc, imm & 0xFF);
    emit_u8(enc, (imm >> 8) & 0xFF);
    emit_u8(enc, (imm >> 16) & 0xFF);
    emit_u8(enc, (imm >> 24) & 0xFF);
    emit_u8(enc, (imm >> 32) & 0xFF);
    emit_u8(enc, (imm >> 40) & 0xFF);
    emit_u8(enc, (imm >> 48) & 0xFF);
    emit_u8(enc, (imm >> 56) & 0xFF);
}

// add rax, imm32
void emit_add_rax_imm32(X86Encoder *enc, int32_t imm) {
    emit_u8(enc, REX_W);
    emit_u8(enc, 0x05);  // add rax, imm32
    emit_u32(enc, imm);
}

// ret
void emit_ret(X86Encoder *enc) {
    emit_u8(enc, 0xC3);
}

// 生成简单函数：返回 x + 42
typedef int64_t (*JitFunc)(int64_t);

JitFunc jit_compile_add_42(void) {
    CodeBuffer *cb = code_buffer_create(4096);
    X86Encoder enc = {
        .code = cb->buffer,
        .offset = 0,
        .capacity = cb->size
    };

    // System V AMD64 ABI:
    // 第一个参数在RDI寄存器

    // mov rax, rdi  (保存参数到rax)
    emit_u8(&enc, 0x48);  // REX.W
    emit_u8(&enc, 0x89);  // mov r64, r/m64
    emit_u8(&enc, MODRM(3, RDI, RAX));  // mod=11, reg=RDI, rm=RAX

    // add rax, 42
    emit_add_rax_imm32(&enc, 42);

    // ret
    emit_ret(&enc);

    // 刷新指令缓存
    jit_flush_icache(cb->buffer, enc.offset);

    return (JitFunc)cb->buffer;
}

// 使用生成的函数
void test_jit(void) {
    JitFunc f = jit_compile_add_42();

    int64_t result = f(10);  // 返回 52
    printf("Result: %ld\n", result);

    // 清理
    munmap((void*)f, 4096);
}
