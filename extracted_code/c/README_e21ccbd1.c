/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\README.md
 * Line: 256
 * Language: c
 * Block ID: e21ccbd1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的代码生成器

typedef struct {
    uint8_t *code;      // 代码缓冲区
    size_t size;        // 当前大小
    size_t capacity;    // 容量
} CodeGen;

CodeGen* codegen_create(void) {
    CodeGen *cg = malloc(sizeof(CodeGen));
    cg->capacity = 4096;
    cg->code = mmap(NULL, cg->capacity,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    cg->size = 0;
    return cg;
}

// _emit helpers
void emit_u8(CodeGen *cg, uint8_t b) {
    cg->code[cg->size++] = b;
}

void emit_u32(CodeGen *cg, uint32_t v) {
    memcpy(&cg->code[cg->size], &v, 4);
    cg->size += 4;
}

// x86-64指令生成
void emit_mov_rax_imm(CodeGen *cg, uint64_t imm) {
    if (imm <= 0xFFFFFFFF) {
        // mov eax, imm32 (零扩展)
        emit_u8(cg, 0xB8);  // opcode
        emit_u32(cg, (uint32_t)imm);
    } else {
        // mov rax, imm64
        emit_u8(cg, 0x48);  // REX.W
        emit_u8(cg, 0xB8);  // opcode
        memcpy(&cg->code[cg->size], &imm, 8);
        cg->size += 8;
    }
}

void emit_ret(CodeGen *cg) {
    emit_u8(cg, 0xC3);
}

// 生成函数
void* codegen_finalize(CodeGen *cg) {
    __builtin___clear_cache(cg->code, cg->code + cg->size);
    return cg->code;
}
