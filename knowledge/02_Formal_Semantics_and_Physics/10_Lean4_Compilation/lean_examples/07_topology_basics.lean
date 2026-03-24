/-
示例7: 拓扑基础
展示拓扑空间、连续性和紧性
-/

import Mathlib.Topology.Basic
import Mathlib.Topology.Compactness.Compact
import Mathlib.Topology.MetricSpace.Basic

namespace TopologyBasics

open Topology Filter

/- ========== 拓扑空间定义 ========== -/

-- 实数上的标准拓扑
#check (inferInstance : TopologicalSpace ℝ)

-- 开区间是开集
example : IsOpen (Ioo 0 1 : Set ℝ) := by
  apply isOpen_Ioo

/- ========== 连续映射 ========== -/

-- 多项式函数连续
example : Continuous (λ x : ℝ => x ^ 2 + 2 * x + 1) := by
  continuity

-- 复合函数连续
example (f g : ℝ → ℝ) (hf : Continuous f) (hg : Continuous g) :
    Continuous (λ x => f (g x)) :=
  Continuous.comp hf hg

/- ========== 紧性 ========== -/

-- 闭区间是紧集
theorem closed_interval_compact {a b : ℝ} (h : a ≤ b) :
    IsCompact (Icc a b) := by
  apply isCompact_Icc

-- Heine-Borel定理的特殊情况
example {s : Set ℝ} :
    IsCompact s ↔ IsClosed s ∧ Bornology.IsBounded s := by
  constructor
  · intro h
    constructor
    · exact h.isClosed
    · exact h.isBounded
  · rintro ⟨hclosed, hbdd⟩
    apply isCompact_of_isClosed_isBounded
    · exact hclosed
    · exact hbdd

/- ========== 连通性 ========== -/

-- 区间是连通的
example {a b : ℝ} (h : a ≤ b) : IsConnected (Icc a b) := by
  apply isConnected_Icc

-- 连通空间的连续像是连通的
theorem connected_image {X Y} [TopologicalSpace X] [TopologicalSpace Y]
    (f : X → Y) (hf : Continuous f) (hs : IsConnected s) :
    IsConnected (f '' s) :=
  IsConnected.image hs f hf

/- ========== 度量空间 ========== -/

-- 实数是度量空间
example : MetricSpace ℝ := by infer_instance

-- 距离函数
example (x y : ℝ) : dist x y = |x - y| :=
  Real.dist_eq x y

-- 开球的定义
example (x : ℝ) (r : ℝ) (hr : 0 < r) :
    Metric.ball x r = {y | |y - x| < r} := by
  ext y
  simp [Metric.mem_ball, Real.dist_eq]

/- ========== 收敛与极限 ========== -/

-- 序列收敛的定义
def SeqConverges (a : ℕ → ℝ) (L : ℝ) : Prop :=
  ∀ ε > 0, ∃ N, ∀ n ≥ N, |a n - L| < ε

-- 收敛序列有界
theorem convergent_implies_bounded (a : ℕ → ℝ) (L : ℝ)
    (h : SeqConverges a L) : Bornology.IsBounded (Set.range a) := by
  sorry

-- 柯西序列完备性 (实数)
theorem real_complete (a : ℕ → ℝ)
    (h : ∀ ε > 0, ∃ N, ∀ m n ≥ N, |a m - a n| < ε) :
    ∃ L, SeqConverges a L := by
  sorry

/- ========== 同胚 ========== -/

-- (0, 1) 同胚于 ℝ
example : Nonempty (Ioo 0 1 ≃ₜ ℝ) := by
  sorry

end TopologyBasics
