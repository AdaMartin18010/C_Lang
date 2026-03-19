/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\01_SR_Latch_and_D_FlipFlop.md
 * Line: 1050
 * Language: c
 * Block ID: ae74ce7a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言代码
int counter = 0;        // 映射到寄存器 R0

void increment() {
    counter++;          // 映射为：ADD R0, R0, #1
}

/* 硬件实现对应关系：
 *
 * C变量counter ──→ 寄存器文件中的某个寄存器
 *                    │
 *                    ▼
 *              ┌───────────┐
 *              │  D触发器  │ ←── 存储1位
 *              │  × 32位   │
 *              └───────────┘
 *                    │
 *                    ▼
 *              ┌───────────┐
 *              │  寄存器   │ ←── 32位寄存器
 *              │   文件    │
 *              └───────────┘
 */
