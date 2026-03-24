/-
示例5: 代数结构
展示环、域和多项式的定义
-/

import Mathlib.Algebra.Ring.Basic
import Mathlib.Data.Polynomial.Basic
import Mathlib.FieldTheory.Finite.Basic

namespace AlgebraicStructures

-- 使用Mathlib中的结构

-- 多项式环
open Polynomial

-- 定义多项式
def myPoly : Polynomial ℤ := X ^ 2 + 2 * X + 1

-- 多项式求值
#eval myPoly.eval 3  -- 16

-- 因式分解
example : X ^ 2 + 2 * X + 1 = (X + 1) ^ 2 := by
  ring

-- 多项式除法
def dividePoly (f g : Polynomial ℚ) (hg : g ≠ 0) :
    ∃ q r, f = g * q + r ∧ r.degree < g.degree :=
  ⟨f / g, f % g, by field_simp [ EuclideanDomain.div_add_mod ], 
   EuclideanDomain.mod_lt f hg⟩

-- 有限域F_p
section FiniteField

variable (p : ℕ) [Fact p.Prime]

-- ZMod p是一个域
example : Field (ZMod p) := by infer_instance

-- 费马小定理
theorem fermat_little_theorem (a : ZMod p) : a ^ p = a := by
  by_cases ha : a = 0
  · rw [ha]; simp
  · calc
      a ^ p = a ^ (p - 1) * a := by rw [← pow_succ]; simp; linarith
      _ = 1 * a := by rw [ZMod.pow_card_sub_one_eq_one ha]
      _ = a := by simp

-- 乘法群的阶
theorem multiplicative_group_order :
    Fintype.card (ZMod p)ˣ = p - 1 := by
  rw [ZMod.card_units p]

end FiniteField

-- 整数环的性质
section IntegerRing

-- 欧几里得算法
def gcd (a b : ℤ) : ℤ :=
  if b = 0 then a else gcd b (a % b)

-- 贝祖等式
theorem bezout (a b : ℤ) (ha : a ≠ 0) (hb : b ≠ 0) :
    ∃ x y : ℤ, a * x + b * y = gcd a b := by
  -- 扩展欧几里得算法
  sorry

-- 唯一分解定理
theorem unique_factorization (n : ℕ) (hn : n > 0) :
    ∃ (p : Finset (Nat × ℕ)),
      (∀ (q, k) ∈ p, q.Prime) ∧
      n = ∏ (q, k) ∈ p, q ^ k := by
  sorry

end IntegerRing

-- 矩阵代数
section MatrixAlgebra

open Matrix

variable {n : Type} [Fintype n] [DecidableEq n]

-- 单位矩阵
example : (1 : Matrix n n ℝ) i j = if i = j then 1 else 0 :=
  rfl

-- 矩阵乘法
def matrixMult (A B : Matrix n n ℝ) : Matrix n n ℝ :=
  A * B

-- 转置性质
theorem transpose_mult (A B : Matrix n n ℝ) :
    (A * B)ᵀ = Bᵀ * Aᵀ :=
  Matrix.transpose_mul A B

-- 可逆矩阵
def InvertibleMatrix (A : Matrix n n ℝ) : Prop :=
  ∃ B, A * B = 1 ∧ B * A = 1

-- 行列式非零等价于可逆
example (A : Matrix n n ℝ) :
    InvertibleMatrix A ↔ det A ≠ 0 := by
  sorry

end MatrixAlgebra

end AlgebraicStructures
