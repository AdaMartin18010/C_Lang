/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 189
 * Language: c
 * Block ID: 0470ad60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// UB的语义规则

// 1. 整数溢出（有符号）
⟨INT_MAX + 1, σ⟩ → UB

// 2. 空指针解引用
⟨*NULL, σ⟩ → UB

// 3. 数组越界
⟨a[n], σ⟩ → UB    如果 n < 0 或 n ≥ size(a)

// 4. 未初始化读取
⟨x, σ⟩ → UB       如果 σ(x) = undef

// 5. 重复释放
⟨free(a); free(a), σ⟩ → UB
