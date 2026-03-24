/-
示例8: 数论
展示素数、模运算和数论函数
-/

import Mathlib.NumberTheory.Divisors
import Mathlib.Data.ZMod.Basic
import Mathlib.NumberTheory.Padics.PadicNumbers

namespace NumberTheoryExamples

/- ========== 整除与素数 ========== -/

-- 素数定义
example (p : ℕ) : Nat.Prime p ↔ (2 ≤ p ∧ ∀ m, m ∣ p → m = 1 ∨ m = p) :=
  Iff.rfl

-- 欧几里得定理: 素数无穷
theorem primes_infinite : ∀ n : ℕ, ∃ p > n, p.Prime := by
  intro n
  let m := Nat.factorial n + 1
  have hm : 0 < m := by positivity
  
  obtain ⟨p, hp_prime, hp_dvd⟩ := Nat.exists_prime_and_dvd (by linarith : m ≠ 0)
  
  use p
  constructor
  · -- 证明 p > n
    by_contra h
    push_neg at h
    have : p ∣ Nat.factorial n := by
      apply Nat.dvd_factorial
      · exact hp_prime.pos
      · exact h
    have : p ∣ 1 := by
      convert Nat.dvd_sub' hp_dvd this
      simp [m]
    exact hp_prime.not_dvd_one this
  · exact hp_prime

/- ========== 模运算 ========== -/

-- 模加法
example (a b n : ℕ) (hn : n > 0) : (a + b) % n = ((a % n) + (b % n)) % n :=
  Nat.add_mod a b n

-- 费马小定理
example (p : ℕ) [Fact p.Prime] (a : ZMod p) : a ^ p = a := by
  by_cases ha : a = 0
  · rw [ha]; simp
  · calc
      a ^ p = a ^ (p - 1) * a := by rw [← pow_succ]; simp; linarith
      _ = 1 * a := by rw [ZMod.pow_card_sub_one_eq_one ha]
      _ = a := by simp

-- 欧拉定理
theorem euler_theorem {n a : ℕ} (hcop : Nat.Coprime a n) (hn : n > 0) :
    a ^ Nat.totient n ≡ 1 [MOD n] := by
  rw [← ZMod.eq_iff_modEq_nat]
  simp [ZMod.pow_totient]
  exact (ZMod.unitOfCoprime a hcop).isUnit

/- ========== 二次剩余 ========== -/

-- Legendre符号
def legendreSymbol (a p : ℕ) : ℤ :=
  if p = 2 then 0
  else if a % p = 0 then 0
  else if IsSquare (ZMod p) (a : ZMod p) then 1
  else -1

-- 二次互反律 (陈述)
theorem quadratic_reciprocity {p q : ℕ} (hp : p.Prime) (hq : q.Prime)
    (hp_odd : p ≠ 2) (hq_odd : q ≠ 2) (hpq : p ≠ q) :
    legendreSymbol p q * legendreSymbol q p =
      (-1) ^ ((p - 1) / 2 * (q - 1) / 2) := by
  sorry

/- ========== 算术函数 ========== -/

-- 除数函数
def divisorCount (n : ℕ) : ℕ :=
  n.divisors.card

-- 除数和函数
def divisorSum (n : ℕ) : ℕ :=
  ∑ d in n.divisors, d

-- 欧拉函数性质
example (n : ℕ) (hn : n > 0) : ∑ d in n.divisors, Nat.totient d = n := by
  rw [Nat.sum_totient' n hn]

/- ========== p-adic数 ========== -/

-- p-adic范数
noncomputable def padicNorm (p : ℕ) [Fact p.Prime] : ℚ → ℚ
  | 0 => 0
  | q => (p : ℚ) ^ (-padicValRat p q)

-- p-adic数的性质
example (p : ℕ) [Fact p.Prime] (q : ℚ) : padicNorm p q ≤ 1 := by
  sorry

/- ========== 丢番图方程 ========== -/

-- Pell方程 x² - dy² = 1
def PellSolution (d : ℕ) :=
  { xy : ℤ × ℤ // xy.1 ^ 2 - d * xy.2 ^ 2 = 1 }

-- Pell方程总有非平凡解 (d为非平方正整数)
theorem pell_has_solution (d : ℕ) (hd : ¬ IsSquare d) (hd_pos : d > 0) :
    ∃ xy : PellSolution d, xy.1.1 ≠ 0 ∧ xy.1.2 ≠ 0 := by
  sorry

end NumberTheoryExamples
