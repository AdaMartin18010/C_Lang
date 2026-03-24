/-
示例6: 分析基础
展示连续性、微分和积分
-/

import Mathlib.Analysis.Calculus.Deriv.Basic
import Mathlib.Analysis.Calculus.ContDiff.Defs
import Mathlib.Analysis.SpecialFunctions.Pow.Real
import Mathlib.MeasureTheory.Integral.IntervalIntegral

namespace AnalysisBasics

open Real Set

/- ========== 连续性与极限 ========== -/

-- 证明x^2在0处连续
example : Continuous (λ x : ℝ => x ^ 2) := by
  apply Continuous.pow
  exact continuous_id

-- 具体点的连续性
example : ContinuousAt (λ x : ℝ => x ^ 2) 0 := by
  apply Continuous.continuousAt
  apply Continuous.pow
  exact continuous_id

/- ========== 导数 ========== -/

-- 导数的定义
example : deriv (λ x : ℝ => x ^ 3) = (λ x => 3 * x ^ 2) := by
  funext x
  simp [deriv_pow]

-- 链式法则的应用
example (f g : ℝ → ℝ) (x : ℝ)
    (hf : DifferentiableAt ℝ f (g x)) (hg : DifferentiableAt ℝ g x) :
    deriv (f ∘ g) x = deriv f (g x) * deriv g x :=
  deriv.comp x hf hg

-- 乘积法则
example (f g : ℝ → ℝ) (x : ℝ)
    (hf : DifferentiableAt ℝ f x) (hg : DifferentiableAt ℝ g x) :
    deriv (fun y => f y * g y) x = deriv f x * g x + f x * deriv g x :=
  deriv_mul hf hg

/- ========== 积分 ========== -/

-- 定积分
example : ∫ x : ℝ in (0 : ℝ)..1, x ^ 2 = 1 / 3 := by
  norm_num

-- 基本初等函数的积分
example : ∫ x : ℝ in (0 : ℝ)..Real.pi, Real.sin x = 2 := by
  norm_num
  -- 使用sin的积分公式
  sorry

/- ========== 泰勒展开 ========== -/

-- e^x的泰勒级数
noncomputable def taylorExp (n : Nat) (x : ℝ) : ℝ :=
  ∑ k in Finset.range n, x ^ k / Nat.factorial k

-- 泰勒展开的误差估计
theorem taylor_exp_remainder (n : Nat) (x : ℝ) :
    ∃ c, |c| ≤ |x| ∧
    Real.exp x - taylorExp n x = Real.exp c * x ^ n / Nat.factorial n := by
  sorry

/- ========== 级数 ========== -/

-- 几何级数
example (r : ℝ) (hr : |r| < 1) : ∑' n : ℕ, r ^ n = 1 / (1 - r) := by
  rw [tsum_geometric_of_norm_lt_one hr]

-- 调和级数发散
example : ¬ Summable (λ n : ℕ => 1 / (n + 1) : ℕ → ℝ) := by
  sorry

-- p-级数收敛条件
example (p : ℝ) : Summable (λ n : ℕ => 1 / (n + 1) ^ p) ↔ p > 1 := by
  sorry

/- ========== 微分方程示例 ========== -/

-- 常微分方程: y' = ky的解
def exponentialGrowth (k : ℝ) (y₀ : ℝ) : ℝ → ℝ :=
  fun t => y₀ * Real.exp (k * t)

-- 验证这是解
theorem exp_growth_solution (k y₀ : ℝ) :
    let y := exponentialGrowth k y₀
    deriv y = fun t => k * y t ∧ y 0 = y₀ := by
  constructor
  · funext t
    simp [exponentialGrowth, deriv_const_mul, deriv_id'', mul_comm k]
  · simp [exponentialGrowth]

/- ========== 优化 ========== -/

-- 求函数的临界点
example (f : ℝ → ℝ) (x : ℝ) (hf : DifferentiableAt ℝ f x)
    (h : IsLocalMin f x) : deriv f x = 0 :=
  h.deriv_eq_zero

-- 二阶导数测试
theorem second_derivative_test (f : ℝ → ℝ) (x : ℝ)
    (hf : ContDiff ℝ 2 f) (h1 : deriv f x = 0) (h2 : deriv^[2] f x > 0) :
    IsLocalMin f x := by
  sorry

end AnalysisBasics
