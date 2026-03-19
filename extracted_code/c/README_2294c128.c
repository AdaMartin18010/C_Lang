/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 221
 * Language: c
 * Block ID: 2294c128
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C代码
int sum(int a, int b) {
    return a + b;
}

// RISC-V汇编 (RV64I)
// add a0, a0, a1
// ret

// 机器码: 0x00b50533
// 0000000 01011 01010 000 01010 0110011
// funct7  rs2   rs1   f3  rd    opcode
