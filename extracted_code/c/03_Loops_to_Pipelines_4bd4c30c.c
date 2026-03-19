/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 83
 * Language: c
 * Block ID: 4bd4c30c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C循环
for (int i = 0; i < 100; i++) {
    sum += array[i];
}

// 对应的硬件结构
/*
┌─────────────────────────────────────────────────────────────┐
│                    循环计数器硬件                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│    ┌──────────┐     ┌──────────┐     ┌──────────┐          │
│    │ 计数器   │────▶│ 比较器   │────▶│ 控制逻辑 │          │
│    │ (i_reg)  │     │ (i<100)  │     │          │          │
│    └────┬─────┘     └──────────┘     └────┬─────┘          │
│         │                                 │                 │
│         │         ┌──────────┐            │                 │
│         └────────▶│  +1      │◀───────────┘                 │
│                   │ 加法器   │                               │
│                   └──────────┘                               │
│                                                              │
│  状态机：IDLE ──▶ INIT ──▶ EXEC ──▶ INC ──▶ CHECK ──▶ DONE  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
*/
