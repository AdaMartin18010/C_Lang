/-
# OperationalSemantics Aesop自动化测试

本模块测试Aesop在STLC操作语义中的自动化证明能力。
-/

import Operational.Syntax
import Operational.Semantics
import Operational.Properties
import Operational.AesopConfig

namespace Operational
namespace AesopTests

set_option maxHeartbeats 500000

-- ==================== 测试1: 值不可归约 ====================

/-- 常量值不可归约（自动化版本）-/
theorem value_const_irreducible_aesop (n : Nat) : 
  ¬(Tm.const (Sum.inl n) →ₛ Tm.const (Sum.inl n)) := by
  aesop_semantics
  all_goals simp [Tm.isValue] at *
  all_goals contradiction

/-- 抽象值不可归约（自动化版本）-/
theorem value_abs_irreducible_aesop {x τ t} : 
  ¬(Tm.abs x τ t →ₛ Tm.abs x τ t) := by
  aesop_semantics
  all_goals simp [Tm.isValue] at *
  all_goals contradiction

-- ==================== 测试2: 替换引理简化证明 ====================

/-- 变量替换简化证明 - 相同变量 -/
theorem subst_var_same_aesop {x t} : 
  (Tm.var x).subst x t = t := by
  aesop_simp

/-- 变量替换简化证明 - 不同变量 -/
theorem subst_var_diff_aesop {x y t} (h : x ≠ y) : 
  (Tm.var y).subst x t = Tm.var y := by
  aesop_simp

/-- 常量替换保持不变 -/
theorem subst_const_aesop {x s c} : 
  (Tm.const c).subst x s = Tm.const c := by
  aesop_simp

-- ==================== 测试3: 进展定理简化 ====================

/-- 常量有进展（自动化）-/
theorem progress_const_aesop (c : Tm.Const) :
  (Tm.const c).isValue ∨ ∃ t', (Tm.const c) →ₛ t' := by
  left
  aesop_simp

/-- 抽象有进展（自动化）-/
theorem progress_abs_aesop {x τ t} :
  (Tm.abs x τ t).isValue ∨ ∃ t', (Tm.abs x τ t) →ₛ t' := by
  left
  aesop_simp

-- ==================== 测试4: 类型保持简化 ====================

/-- if true then t else e 保持类型（自动化）-/
theorem preservation_ifTrue_aesop {t₂ t₃ τ} :
  [] ⊢ Tm.ifte (Tm.bool true) t₂ t₃ : τ →
  [] ⊢ t₂ : τ := by
  intro htype
  have hstep : Tm.ifte (Tm.bool true) t₂ t₃ →ₛ t₂ := SmallStep.ifTrue
  -- 使用简化的保持证明
  simp [typeCheck] at htype ⊢
  cases htype with
  | pure h =>
    cases h with
  | pure h₁ h₂ => exact h₂

/-- if false then t else e 保持类型（自动化）-/
theorem preservation_ifFalse_aesop {t₂ t₃ τ} :
  [] ⊢ Tm.ifte (Tm.bool false) t₂ t₃ : τ →
  [] ⊢ t₃ : τ := by
  intro htype
  simp [typeCheck] at htype ⊢
  cases htype with
  | pure h =>
    cases h with
  | pure h₁ h₂ =>
    cases h₂ with
    | pure h₃ => exact h₃

-- ==================== 测试5: 小步语义确定性（简化案例） ====================

/-- if true 的确定性 -/
theorem determinism_ifTrue_aesop {t₂ t₃ t'} :
  Tm.ifte (Tm.bool true) t₂ t₃ →ₛ t' → t' = t₂ := by
  intro h
  cases h with
  | ifTrue => rfl
  | _ => contradiction

/-- if false 的确定性 -/
theorem determinism_ifFalse_aesop {t₂ t₃ t'} :
  Tm.ifte (Tm.bool false) t₂ t₃ →ₛ t' → t' = t₃ := by
  intro h
  cases h with
  | ifFalse => rfl
  | _ => contradiction

-- ==================== 测试6: 大步语义等价性 ====================

/-- 常量的大步求值（自动化）-/
theorem bigstep_const_aesop {c} :
  (Tm.const c) ⇓ (Tm.const c) := by
  apply BigStep.const

/-- 抽象的大步求值（自动化）-/
theorem bigstep_abs_aesop {x τ t} :
  (Tm.abs x τ t) ⇓ (Tm.abs x τ t) := by
  apply BigStep.abs

-- ==================== 测试7: 类型健全性引理 ====================

/-- 良类型项不会卡住（常量）-/
theorem type_soundness_const_aesop {c} :
  ¬(∃ msg, eval (Tm.const c) = .stuck msg) := by
  simp [eval, evalN, Tm.isValue]
  intro msg h
  simp at h

-- ==================== 测试8: 归约关系传递性 ====================

/-- 多步归约自反性（自动化）-/
theorem multistep_refl_aesop {t} : t →* t := by
  apply MultiStep.refl

-- ==================== 测试9: 基本等式证明 ====================

/-- 替换 distributes over app（简化版本）-/
theorem subst_app_dist_aesop {x s t₁ t₂} :
  (Tm.app t₁ t₂).subst x s = Tm.app (t₁.subst x s) (t₂.subst x s) := by
  rfl

/-- 替换 distributes over binop -/
theorem subst_binop_dist_aesop {x s op t₁ t₂} :
  (Tm.binop op t₁ t₂).subst x s = Tm.binop op (t₁.subst x s) (t₂.subst x s) := by
  rfl

-- ==================== 测试10: 性能基准测试 ====================

/-- 复杂表达式的自动化证明性能测试 -/
theorem complex_expr_step_aesop :
  let expr := Tm.app (λ "x" : Ty.nat => Tm.var "x") (Tm.num 5)
  ∃ t', expr →ₛ t' := by
  exists Tm.var "x"
  apply SmallStep.appAbs
  simp [Tm.isValue]

end AesopTests
end Operational
