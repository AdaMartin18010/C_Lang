/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\01_MMU_Architecture.md
 * Line: 59
 * Language: c
 * Block ID: 2257c614
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 有MMU系统的解决方案
// 程序A和程序B使用相同的虚拟地址，但映射到不同物理地址

// 程序A的页表：VA 0x1000 -> PA 0x100000
// 程序B的页表：VA 0x1000 -> PA 0x200000

int* ptr_a = (int*)0x1000;  // 虚拟地址
*ptr_a = 100;                // 实际写入物理地址0x100000

int* ptr_b = (int*)0x1000;  // 相同的虚拟地址
*ptr_b = 200;                // 实际写入物理地址0x200000，互不干扰！
