/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\01_MMU_Architecture.md
 * Line: 38
 * Language: c
 * Block ID: 76e88cca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 无MMU系统的问题示例
// 程序A和程序B都尝试访问地址0x1000

// 程序A
int* ptr_a = (int*)0x1000;  // 直接使用物理地址
*ptr_a = 100;                // 写入物理内存0x1000

// 程序B
int* ptr_b = (int*)0x1000;  // 同样的物理地址
*ptr_b = 200;                // 覆盖程序A的数据！

// 问题：
// 1. 没有内存隔离 - 程序可以互相破坏
// 2. 无法运行比物理内存大的程序
// 3. 内存碎片无法解决
// 4. 程序无法在内存中移动
