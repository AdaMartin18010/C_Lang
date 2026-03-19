/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 122
 * Language: c
 * Block ID: d41917f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 语句文法
// S ::= skip | x = E | S₁; S₂ | if E S₁ S₂ | while E S

// 空语句
⟨skip, σ⟩ ⇓ σ

// 赋值
⟨x = E, σ⟩ ⇓ σ[x ↦ n]    如果 ⟨E, σ⟩ →* n

// 顺序
⟨S₁; S₂, σ⟩ ⇓ σ''        如果 ⟨S₁, σ⟩ ⇓ σ' 且 ⟨S₂, σ'⟩ ⇓ σ''

// 条件（真）
⟨if E S₁ S₂, σ⟩ ⇓ σ'     如果 ⟨E, σ⟩ →* n, n ≠ 0, ⟨S₁, σ⟩ ⇓ σ'

// 条件（假）
⟨if E S₁ S₂, σ⟩ ⇓ σ'     如果 ⟨E, σ⟩ →* 0, ⟨S₂, σ⟩ ⇓ σ'

// While（假）
⟨while E S, σ⟩ ⇓ σ       如果 ⟨E, σ⟩ →* 0

// While（真）
⟨while E S, σ⟩ ⇓ σ''     如果 ⟨E, σ⟩ →* n, n ≠ 0,
                          ⟨S, σ⟩ ⇓ σ', ⟨while E S, σ'⟩ ⇓ σ''
