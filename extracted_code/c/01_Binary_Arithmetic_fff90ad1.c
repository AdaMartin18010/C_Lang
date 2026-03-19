/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\01_Binary_Arithmetic.md
 * Line: 1894
 * Language: c
 * Block ID: fff90ad1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 在硬件层面，加法是相同的！
// 区别仅在于溢出检测和结果解释

unsigned int ua = 0xFFFFFFFF;
unsigned int ub = 1;
unsigned int usum = ua + ub;  // 回绕到0，Cout=1

int sa = -1;      // 0xFFFFFFFF
int sb = 1;       // 0x00000001
int ssum = sa + sb;  // 结果为0，没有溢出

// 硬件使用相同的加法器，但溢出检测不同：
// 无符号：溢出 = Cout
// 有符号：溢出 = Cout[31] XOR Cout[30]
