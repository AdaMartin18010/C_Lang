/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 245
 * Language: c
 * Block ID: a4fd50f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 分离逻辑的Hoare三元组：{P} C {Q}
 * - P, Q是分离逻辑断言
 * - C是命令
 *
 * 关键规则：
 */

// 空堆规则
// {emp} skip {emp}

// 分配规则
// {emp} x := cons(a, b) {x ↦ a, b}

// 查找规则
// {x ↦ v} y := [x] {x ↦ v ∧ y = v}

// 赋值规则
// {x ↦ _} [x] := v {x ↦ v}

// 释放规则
// {x ↦ v} dispose(x) {emp}

/*
 * 关键：分离合取的局部性
 *
 * 如果 {P} C {Q} 且 C 只访问 P 中的地址
 * 则 {P * R} C {Q * R}  （帧规则）
 */
