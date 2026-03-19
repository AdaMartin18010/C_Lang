/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\01_WP_Energy_Landscape.md
 * Line: 156
 * Language: c
 * Block ID: bc5725f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 最弱前置条件的核心规则：
 *
 * 1. wp(skip, Q) = Q
 *
 * 2. wp(x := E, Q) = Q[x/E]
 *    （Q中所有x替换为E）
 *
 * 3. wp(S1; S2, Q) = wp(S1, wp(S2, Q))
 *
 * 4. wp(if B then S1 else S2, Q) =
 *    (B ⇒ wp(S1, Q)) ∧ (¬B ⇒ wp(S2, Q))
 *
 * 5. wp(while B do S, Q) =
 *    ∃k≥0. I_k
 *    其中：
 *    I_0 = ¬B ∧ Q
 *    I_{k+1} = B ∧ wp(S, I_k)
 *
 *    实际使用循环不变式I：
 *    I ∧ (I ∧ B ⇒ wp(S, I)) ∧ (I ∧ ¬B ⇒ Q)
 */

// 示例：计算简单程序的WP
/*
程序：
  if x > 0 then
    y := x
  else
    y := -x

后条件Q: y ≥ 0

WP计算：
  WP = (x > 0 ⇒ WP[y:=x](y ≥ 0)) ∧ (x ≤ 0 ⇒ WP[y:=-x](y ≥ 0))
     = (x > 0 ⇒ x ≥ 0) ∧ (x ≤ 0 ⇒ -x ≥ 0)
     = (x > 0 ⇒ x ≥ 0) ∧ (x ≤ 0 ⇒ x ≤ 0)
     = true ∧ true
     = true

结论：程序总是满足y ≥ 0（绝对值计算）
*/
