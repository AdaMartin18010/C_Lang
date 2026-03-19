/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 149
 * Language: c
 * Block ID: bd40f0da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 推导树示例：
// ⟨x = 1; while (x < 3) x = x + 1, σ⟩ ⇓ σ'
//
// ─────────────────────────  (x=1)
// ⟨x=1, σ⟩ ⇓ σ[x↦1]
//
// ─────────────────────────  (x<3 为真)
// ⟨x<3, σ₁⟩ →* 1
// ─────────────────────────  (x=x+1)
// ⟨x=x+1, σ₁⟩ ⇓ σ₂
//
// ...（循环展开）
