/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 272
 * Language: c
 * Block ID: d6dd2316
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 求值顺序未指定
⟨f(E₁, E₂), σ⟩  // 不保证 E₁ 先于 E₂

// C标准：函数参数求值顺序未指定
// 语义：允许 ⟨E₁, σ⟩ → 先于 ⟨E₂, σ⟩ 或相反
