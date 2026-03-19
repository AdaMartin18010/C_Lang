/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 77
 * Language: c
 * Block ID: 30df762c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数据冒险示例
// 指令1: R1 = R2 + R3
// 指令2: R4 = R1 + R5  (依赖于指令1的结果)

// 解决方案：数据前递(Forwarding)
//          EX/MEM寄存器直接传递到ALU输入
//          无需等待WB阶段
