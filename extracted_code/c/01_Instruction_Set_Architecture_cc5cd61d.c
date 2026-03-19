/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 619
 * Language: c
 * Block ID: cc5cd61d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 某些指令要求内存操作数对齐

// MOVAPS要求16字节对齐
movaps xmm0, [unaligned_addr];  // 可能产生#GP异常

// 使用MOVDQU处理未对齐数据
movdqu xmm0, [unaligned_addr];  // 安全
