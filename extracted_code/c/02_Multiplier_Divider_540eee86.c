/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\02_Multiplier_Divider.md
 * Line: 1964
 * Language: c
 * Block ID: 540eee86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言源代码
int a = 100, b = 25;
int prod = a * b;    // 乘法
int quot = a / b;    // 除法
int rem = a % b;     // 取模

// ARM64汇编代码
// MUL X2, X0, X1      ; 乘法 (可能用Wallace树或Booth)
// SDIV X3, X0, X1     ; 有符号除法 (SRT除法器)
// UDIV X4, X0, X1     ; 无符号除法
