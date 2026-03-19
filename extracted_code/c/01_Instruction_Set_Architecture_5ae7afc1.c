/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 606
 * Language: c
 * Block ID: 5ae7afc1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// x86历史遗留：写入8位或16位寄存器会影响完整64位寄存器

mov rax, 0x123456789ABCDEF0
mov al, 0;           // RAX变为 0x123456789ABCDE00
// 不是 0x0000000000000000！

// 在AMD64上，写入32位寄存器会自动零扩展到64位
mov eax, 1;          // RAX变为 0x0000000000000001
