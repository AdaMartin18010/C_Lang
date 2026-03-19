/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 153
 * Language: c
 * Block ID: dc3c5bb7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// x86-64指令解码器

typedef struct {
    uint8_t prefixes[4];
    int num_prefixes;

    uint8_t rex;           // REX前缀（64位扩展）
    bool has_rex;

    uint8_t opcode[3];
    int opcode_len;

    uint8_t modrm;         // ModR/M字节
    bool has_modrm;

    uint8_t sib;           // SIB字节
    bool has_sib;

    int32_t displacement;
    int disp_size;

    int64_t immediate;
    int imm_size;
} X86_Instruction;

// 解码REX前缀
void decode_rex(uint8_t rex, bool *w, bool *r, bool *x, bool *b) {
    *w = (rex >> 3) & 1;  // 操作数宽度（0=32位，1=64位）
    *r = (rex >> 2) & 1;  // ModR/M reg字段扩展
    *x = (rex >> 1) & 1;  // SIB index字段扩展
    *b = rex & 1;         // ModR/M r/m字段扩展
}

// 解码ModR/M字节
void decode_modrm(uint8_t modrm, int *mod, int *reg, int *rm) {
    *mod = (modrm >> 6) & 0x3;  // 寻址模式
    *reg = (modrm >> 3) & 0x7;  // 寄存器/操作码扩展
    *rm = modrm & 0x7;          // r/m寄存器或SIB/位移
}

// 示例：解码 "mov rax, 0x1234"
void example_decode() {
    // 机器码：48 C7 C0 34 12 00 00
    // 48: REX.W (64位操作)
    // C7: MOV Ev, Iz (移动立即数到r/m)
    // C0: ModR/M = 11 000 000 (mod=3, reg=0, rm=0)
    // 34 12 00 00: 立即数 0x00001234 (小端)

    uint8_t code[] = {0x48, 0xC7, 0xC0, 0x34, 0x12, 0x00, 0x00};

    X86_Instruction inst = {0};
    int pos = 0;

    // 解析REX前缀
    if ((code[pos] & 0xF0) == 0x40) {
        inst.rex = code[pos++];
        inst.has_rex = true;
    }

    // 解析操作码
    inst.opcode[0] = code[pos++];
    inst.opcode_len = 1;

    // 解析ModR/M
    inst.modrm = code[pos++];
    inst.has_modrm = true;

    // 解析立即数
    inst.immediate = *(int32_t*)&code[pos];
    inst.imm_size = 4;

    printf("Decoded: MOV RAX, 0x%lx\n", inst.immediate);
}
