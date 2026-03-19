/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\01_Binary_Arithmetic.md
 * Line: 1878
 * Language: c
 * Block ID: 529b812c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言源代码
int a = 100, b = 200;
int sum = a + b;

// 编译后的汇编代码 (ARM)
// LDR R0, [SP, #a]      ; 加载a
// LDR R1, [SP, #b]      ; 加载b
// ADD R2, R0, R1        ; 加法运算 (由32位加法器执行)
// STR R2, [SP, #sum]    ; 存储结果

// 硬件层面：32位超前进位加法器执行 R0 + R1
