/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 72
 * Language: c
 * Block ID: ff5e533c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 表达式文法（简化）
// E ::= n | x | E + E | E * E | &E | *E

// 数字求值（已完成）
⟨n, σ⟩ → n

// 变量查找
⟨x, σ⟩ → σ(x)    如果 x ∈ dom(σ)

// 加法（左优先）
⟨E₁ + E₂, σ⟩ → ⟨E₁' + E₂, σ'⟩    如果 ⟨E₁, σ⟩ → ⟨E₁', σ'⟩
⟨n + E₂, σ⟩ → ⟨n + E₂', σ'⟩      如果 ⟨E₂, σ⟩ → ⟨E₂', σ'⟩
⟨n₁ + n₂, σ⟩ → n₃                其中 n₃ = n₁ + n₂

// 乘法
⟨E₁ * E₂, σ⟩ → ...（类似加法）
