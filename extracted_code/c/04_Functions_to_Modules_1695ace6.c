/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md
 * Line: 93
 * Language: c
 * Block ID: 1695ace6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例函数
int multiply_add(int a, int b, int c) {
    int prod = a * b;
    int sum = prod + c;
    return sum;
}

// 硬件实现对应
/*
┌───────────────────────────────────────────────────────────────┐
│                  multiply_add硬件模块                          │
├───────────────────────────────────────────────────────────────┤
│                                                                │
│   Inputs: a[31:0], b[31:0], c[31:0]                           │
│   Output: return_val[31:0]                                    │
│                                                                │
│   ┌─────────┐      ┌─────────┐                               │
│   │  a      │─────▶│         │                               │
│   │  b      │─────▶│ MUL     │────┐                          │
│   └─────────┘      │ (DSP48) │    │                          │
│                    └─────────┘    │      ┌─────────┐         │
│                                   └─────▶│         │         │
│                    ┌─────────┐     ┌────▶│  ADD    │────▶    │
│                    │    c    │─────┘     │         │ return  │
│                    └─────────┘           └─────────┘         │
│                                                                │
│   Latency: 2 cycles (MUL 1 cycle + ADD 1 cycle)               │
│   II: 1 (可每周期启动一次新计算)                               │
│                                                                │
└───────────────────────────────────────────────────────────────┘
*/
