/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 113
 * Language: c
 * Block ID: a1a3a67a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 复杂示例：*(&x + 1) = 42
// ⟨*(&x + 1) = 42, σ⟩
// → ⟨*(a + 1) = 42, σ⟩      其中 a = address(x)
// → σ[a+1 ↦ 42]
