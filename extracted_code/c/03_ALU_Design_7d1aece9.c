/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\03_ALU_Design.md
 * Line: 1330
 * Language: c
 * Block ID: 7d1aece9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言条件语句
if (a < b) {
    // then分支
} else {
    // else分支
}

// 对应的汇编实现 (伪代码)
// CMP  R1, R2      ; 比较a和b (执行SUB，设置标志位)
// BGE  else_label  ; 如果a >= b (N==V)，跳转到else
// ...              ; then分支代码
// JMP  end_if
// else_label:
// ...              ; else分支代码
// end_if:

// 对应的ALU操作:
// CMP:  SUB R1, R2 (结果不保存，只设置标志位)
// 标志位: N = (R1-R2)的符号位
//        Z = (R1-R2)是否为零
//        C = 借位
//        V = 溢出
