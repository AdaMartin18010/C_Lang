/-
示例2: 归纳证明
展示自然数归纳法和结构归纳法
-/

import Mathlib.Data.Nat.Basic
import Mathlib.Algebra.BigOperators.Ring

namespace InductionExamples

-- 求和公式: 1 + 2 + ... + n = n(n+1)/2
theorem sum_first_n (n : Nat) : ∑ i in Finset.range n, (i + 1) = n * (n + 1) / 2 := by
  induction n with
  | zero =>
    -- 基本情况: n = 0, 两边都是0
    simp
  | succ n ih =>
    -- 归纳步骤: 假设对n成立，证明对n+1成立
    rw [Finset.sum_range_succ, ih]
    -- 代数化简
    ring_nf
    -- 处理除法
    omega

-- 阶乘的单调性
example (n : Nat) : Nat.factorial n ≤ Nat.factorial (n + 1) := by
  rw [Nat.factorial_succ]
  apply Nat.le_mul_of_pos_right
  apply Nat.factorial_pos

-- 斐波那契数列性质
def fib : Nat → Nat
  | 0 => 0
  | 1 => 1
  | n + 2 => fib n + fib (n + 1)

-- 强归纳法示例
theorem fib_pos (n : Nat) : 0 < fib (n + 1) := by
  match n with
  | 0 => simp [fib]
  | 1 => simp [fib]
  | n + 2 =>
      have h1 : 0 < fib (n + 1) := fib_pos n
      have h2 : 0 < fib (n + 2) := fib_pos (n + 1)
      simp [fib]
      linarith

-- 列表的归纳
inductive MyList (α : Type) where
  | nil : MyList α
  | cons : α → MyList α → MyList α

def MyList.length {α} : MyList α → Nat
  | nil => 0
  | cons _ t => 1 + length t

def MyList.append {α} : MyList α → MyList α → MyList α
  | nil, ys => ys
  | cons x xs, ys => cons x (append xs ys)

theorem append_length {α} (xs ys : MyList α) :
    (xs.append ys).length = xs.length + ys.length := by
  induction xs with
  | nil => simp [MyList.append, MyList.length]
  | cons x xs ih =>
    simp [MyList.append, MyList.length, ih]
    ring

end InductionExamples
