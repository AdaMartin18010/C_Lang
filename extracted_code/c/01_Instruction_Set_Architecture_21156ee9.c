/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\01_Instruction_Set_Architecture.md
 * Line: 593
 * Language: c
 * Block ID: 21156ee9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 某些指令在特定条件下的行为未定义
// 例如：移位量 >= 数据宽度

// 错误
shl eax, 64;  // 未定义行为（x86限制移位量为0-63）

// 正确
shl eax, 31;  // 最大安全移位
