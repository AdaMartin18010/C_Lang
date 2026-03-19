/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\01_WP_Energy_Landscape.md
 * Line: 661
 * Language: c
 * Block ID: abd8a48e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：不变式不足以推导出后条件
/*
程序：
  s := 0;
  i := 0;
  while i < n do
    s := s + i;
    i := i + 1

错误不变式：i ≤ n  （太弱！）

退出时：i ≥ n ∧ i ≤ n → i = n
但无法确定s的值

正确不变式：s = Σ_{k=0}^{i-1} k ∧ i ≤ n
退出时：s = Σ_{k=0}^{n-1} k = n*(n-1)/2
*/
