/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_CPU_Microarchitecture_Detail.md
 * Line: 181
 * Language: c
 * Block ID: 3a4502b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 寄存器重命名示例
// 架构寄存器动态映射到物理寄存器，消除WAR/WAW冲突

// 原始代码 (x86-64)
mov rax, 1      // RAX = 1
mov rax, 2      // RAX = 2 (WAW: Write-After-Write)
mov rbx, rax    // RBX = RAX (RAW: Read-After-Write)

// 重命名后
// 假设: 物理寄存器池 P0-Pn
mov P0, 1       // 分配P0给RAX
mov P1, 2       // 分配P1给RAX, P0可回收
mov P2, P1      // 分配P2给RBX

// RAT (寄存器别名表) 状态:
// RAX → P1
// RBX → P2

// 重命名消除WAW依赖，允许并行执行
