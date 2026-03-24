/-
示例10: 高级策略技巧
展示linarith, ring, field_simp等策略
-/

import Mathlib.Tactic

namespace AdvancedTactics

/- ========== 线性算术 ========== -/

example (x y z : ℝ) (h1 : x + y + z = 6) (h2 : x + y = 3) (h3 : y + z = 5) :
    x = 1 ∧ y = 2 ∧ z = 3 := by
  constructor
  · linarith
  constructor
  · linarith
  · linarith

/- ========== 环和域推理 ========== -/

-- ring策略处理交换环表达式
example (x y : ℝ) : (x + y) ^ 2 = x ^ 2 + 2 * x * y + y ^ 2 := by
  ring

-- ring_nf规范化
example (x y : ℝ) : (x + y) * (x - y) + y ^ 2 = x ^ 2 := by
  ring_nf

-- field_simp处理分式
example (x y : ℝ) (hx : x ≠ 0) (hy : y ≠ 0) :
    1 / x + 1 / y = (x + y) / (x * y) := by
  field_simp
  ring

-- nlinarith处理非线性
example (x y : ℝ) (h : x ^ 2 + y ^ 2 = 0) : x = 0 ∧ y = 0 := by
  have hx : x ^ 2 ≥ 0 := sq_nonneg x
  have hy : y ^ 2 ≥ 0 := sq_nonneg y
  have h1 : x ^ 2 = 0 := by nlinarith
  have h2 : y ^ 2 = 0 := by nlinarith
  constructor
  · rwa [sq_eq_zero_iff] at h1
  · rwa [sq_eq_zero_iff] at h2

/- ========== 集合推理 ========== -/

example (A B C : Set ℝ) (h : A ⊆ B) (h' : B ⊆ C) : A ⊆ C := by
  intro x hx
  apply h'
  apply h
  exact hx

example (A B : Set ℝ) : A ∩ B = B ∩ A := by
  ext x
  simp [and_comm]

/- ========== 逻辑推理 ========== -/

example (p q r : Prop) : (p → q → r) ↔ (p ∧ q → r) := by
  constructor
  · intro h hpq
    apply h hpq.1 hpq.2
  · intro h hp hq
    apply h ⟨hp, hq⟩

-- tauto处理命题逻辑
example (p q r : Prop) : (p → q) → (q → r) → p → r := by
  tauto

/- ========== 存在量词和唯一性 ========== -/

example : ∃ n : ℕ, n > 10 ∧ n.Prime := by
  use 11
  constructor
  · norm_num
  · norm_num

example : ∃! n : ℕ, n > 10 ∧ n < 12 := by
  use 11
  constructor
  · constructor
    · norm_num
    · norm_num
  · intro n hn
    have h1 : n > 10 := hn.1
    have h2 : n < 12 := hn.2
    interval_cases n
    all_goals norm_num at *

/- ========== 归纳法变体 ========== -/

-- 强归纳法
example (P : ℕ → Prop) (h : ∀ n, (∀ m < n, P m) → P n) (n : ℕ) : P n := by
  induction' n using Nat.strongRecOn with n ih
  apply h
  exact ih

-- 课程值归纳
theorem course_of_values {P : ℕ → Prop}
    (h : ∀ n, (∀ m < n, P m) → P n) : ∀ n, P n := by
  intro n
  have : ∀ k ≤ n, P k := by
    induction n with
    | zero =>
      intro k hk
      interval_cases k
      apply h
      simp
    | succ n ih =>
      intro k hk
      by_cases h' : k ≤ n
      · exact ih k h'
      · have : k = n + 1 := by linarith
        rw [this]
        apply h
        intro m hm
        apply ih
        linarith
  exact this n (by simp)

/- ========== 自定义简化 ========== -/

-- 使用simp扩展
example (n : ℕ) : n + 0 = n := by
  simp

-- 指定引理简化
example (n : ℕ) : Nat.factorial (n + 1) = (n + 1) * Nat.factorial n := by
  simp [Nat.factorial]

-- 反向简化
example (n : ℕ) : n * 2 = n + n := by
  simp [Nat.mul_two]

/- ========== 决策过程 ========== -/

decide  -- 可判定命题
example : 2 + 2 = 4 := by decide

example : Nat.Prime 17 := by norm_num

-- finish处理有限域上的命题
example : ∀ x : Fin 3, x = 0 ∨ x = 1 ∨ x = 2 := by
  intro x
  fin_cases x <;> simp

end AdvancedTactics
