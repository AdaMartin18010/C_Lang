/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 282
 * Language: c
 * Block ID: f022cd05
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 序列点之前的副作用必须完成
⟨x = 1, x = 2⟩  // 序列点分割两个完整表达式

// 但在一个表达式内：
⟨x++ + x++, σ⟩  // 未指定行为！
// 两个副作用之间没有序列点
