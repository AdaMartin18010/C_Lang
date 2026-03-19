/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 438
 * Language: c
 * Block ID: 550f9d34
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 两个入口的循环
goto L2;    // 从外部跳入循环中间
L1:
    ...
L2:
    ...
    if (cond) goto L1;
