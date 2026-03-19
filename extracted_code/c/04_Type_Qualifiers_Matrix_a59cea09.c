/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\04_Type_Qualifiers_Matrix.md
 * Line: 176
 * Language: c
 * Block ID: a59cea09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 硬件状态寄存器：只读且可能随时变化
const volatile uint32_t* STATUS_REG = (void*)0x40001000;

// 控制寄存器：可写，写入后立即生效
volatile uint32_t* CTRL_REG = (void*)0x40001004;
