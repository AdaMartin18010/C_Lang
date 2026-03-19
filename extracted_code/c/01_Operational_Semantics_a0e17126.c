/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 212
 * Language: c
 * Block ID: a0e17126
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整推导：factorial(3)
//
// 程序：
// n = 3; r = 1;
// while (n > 0) {
//     r = r * n;
//     n = n - 1;
// }

// 初始状态 σ₀ = {n:undef, r:undef}

// 推导序列：
⟨n=3; r=1; while(n>0){r=r*n;n=n-1;}, σ₀⟩

→ ⟨r=1; while(...), σ₁⟩      其中 σ₁ = σ₀[n↦3]

→ ⟨while(...), σ₂⟩           其中 σ₂ = σ₁[r↦1]

→ ⟨r=r*n; n=n-1; while(...), σ₂⟩    (n>0 为真)

→ ⟨n=n-1; while(...), σ₃⟩    其中 σ₃ = σ₂[r↦3]

→ ⟨while(...), σ₄⟩           其中 σ₄ = σ₃[n↦2]

...（继续展开直到n=0）

→ σ₅                         其中 σ₅ = {n:0, r:6}
