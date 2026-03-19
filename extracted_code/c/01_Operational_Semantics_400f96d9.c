/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 100
 * Language: c
 * Block ID: 400f96d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 取地址
⟨&x, σ⟩ → a    其中 a = address(x)

// 解引用
⟨*E, σ⟩ → ⟨*E', σ'⟩    如果 ⟨E, σ⟩ → ⟨E', σ'⟩
⟨*a, σ⟩ → v            如果 σ(a) = v

// 赋值语义
⟨x = E, σ⟩ → ⟨x = E', σ'⟩    如果 ⟨E, σ⟩ → ⟨E', σ'⟩
⟨x = n, σ⟩ → σ[x ↦ n]        // 存储更新
