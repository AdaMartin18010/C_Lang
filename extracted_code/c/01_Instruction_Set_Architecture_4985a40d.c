/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 231
 * Language: c
 * Block ID: 4985a40d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ARM64 A64指令格式（32位定长）

typedef struct {
    uint32_t raw;  // 原始32位指令

    // 字段提取
    int opcode;    // 主要操作码
    int rd;        // 目标寄存器
    int rn;        // 第一源寄存器
    int rm;        // 第二源寄存器
    int imm;       // 立即数
    int shift;     // 移位类型
} ARM64_Instruction;

// 解码逻辑指令格式
// 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//  sf op  S  1  1  0  1  0  1  1  0  rm .................. imm6 ...... rn ........ rd
//
// sf: 0=32位, 1=64位
// op: 0=AND, 1=ORR, 2=EOR, 3=ANDS

void decode_arm64_logical(uint32_t inst, ARM64_Instruction *decoded) {
    decoded->raw = inst;
    decoded->rd = inst & 0x1F;
    decoded->rn = (inst >> 5) & 0x1F;
    decoded->rm = (inst >> 16) & 0x1F;
    decoded->imm = (inst >> 10) & 0x3F;  // 移位量

    int sf = (inst >> 31) & 1;
    int opc = (inst >> 29) & 0x3;

    const char *op_names[] = {"AND", "ORR", "EOR", "ANDS"};
    printf("%s %c%d, %c%d, %c%d, LSL #%d\n",
           op_names[opc],
           sf ? 'X' : 'W', decoded->rd,
           sf ? 'X' : 'W', decoded->rn,
           sf ? 'X' : 'W', decoded->rm,
           decoded->imm);
}
