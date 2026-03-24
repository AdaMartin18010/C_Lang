/-
# OperationalSemantics Aesop配置

本模块为STLC操作语义项目配置Aesop自动化证明策略，
包含领域特定的规则集和搜索配置。
-/

import Aesop

namespace Operational

-- ==================== Aesop规则集配置 ====================

/-- OperationalSemantics领域特定的Aesop规则集 -/
declare_aesop_rule_sets [OperationalSemantics]

-- ==================== Safe规则（总是应用）====================

@[aesop safe (rule_sets [OperationalSemantics])]
theorem type_check_safe {Γ t τ} : Γ ⊢ t : τ → True := by trivial

@[aesop safe (rule_sets [OperationalSemantics])]
theorem value_is_nf {v : Tm} : v.isValue → ¬(∃ t', v →ₛ t') := by
  intro hv
  simp [Tm.isValue] at hv
  cases v <;> simp [SmallStep] at * <;> contradiction

-- ==================== Unsafe规则（尝试应用）====================

@[aesop unsafe 30% (rule_sets [OperationalSemantics])]
theorem app1_step {t₁ t₁' t₂} : t₁ →ₛ t₁' → Tm.app t₁ t₂ →ₛ Tm.app t₁' t₂ :=
  SmallStep.app1

@[aesop unsafe 30% (rule_sets [OperationalSemantics])]
theorem app2_step {t₁ t₂ t₂'} : t₁.isValue → t₂ →ₛ t₂' → Tm.app t₁ t₂ →ₛ Tm.app t₁ t₂' :=
  SmallStep.app2

@[aesop unsafe 30% (rule_sets [OperationalSemantics])]
theorem appAbs_step {x τ t v} : v.isValue → Tm.app (Tm.abs x τ t) v →ₛ t.subst x v :=
  SmallStep.appAbs

@[aesop unsafe 30% (rule_sets [OperationalSemantics])]
theorem ifTrue_step {t₂ t₃} : Tm.ifte (Tm.bool true) t₂ t₃ →ₛ t₂ :=
  SmallStep.ifTrue

@[aesop unsafe 30% (rule_sets [OperationalSemantics])]
theorem ifFalse_step {t₂ t₃} : Tm.ifte (Tm.bool false) t₂ t₃ →ₛ t₃ :=
  SmallStep.ifFalse

@[aesop unsafe 30% (rule_sets [OperationalSemantics])]
theorem letVal_step {x v t} : v.isValue → Tm.letb x v t →ₛ t.subst x v :=
  SmallStep.letVal

@[aesop unsafe 20% (rule_sets [OperationalSemantics])]
theorem binop_step {op n₁ n₂} : Tm.binop op (Tm.num n₁) (Tm.num n₂) →ₛ Tm.num (SmallStep.evalBinOp op n₁ n₂) :=
  SmallStep.binop

-- ==================== 归一化规则 ====================

@[aesop norm (rule_sets [OperationalSemantics])]
theorem subst_var_self {x t} : (Tm.var x).subst x t = t := by
  simp [Tm.subst]

@[aesop norm (rule_sets [OperationalSemantics])]
theorem subst_var_other {x y t} (h : x ≠ y) : (Tm.var y).subst x t = Tm.var y := by
  simp [Tm.subst, h]

@[aesop norm (rule_sets [OperationalSemantics])]
theorem subst_const {x s c} : (Tm.const c).subst x s = Tm.const c := by
  simp [Tm.subst]

-- ==================== Aesop策略宏定义 ====================

/-- OperationalSemantics专用aesop策略 -/
macro "aesop_semantics" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 50
        maxRuleApplications := 500
        enableSimp := true })
      (rule_sets [OperationalSemantics])
  )

/-- 快速简化策略 -/
macro "aesop_simp" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := true
        maxRuleApplicationDepth := 20
        maxRuleApplications := 100
        enableSimp := true })
      (rule_sets [-default, OperationalSemantics])
  )

/-- 深度搜索策略 -/
macro "aesop_deep" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 100
        maxRuleApplications := 1000
        enableSimp := true })
      (rule_sets [OperationalSemantics])
  )

-- ==================== 便捷证明构造器 ====================

/-- 使用Aesop自动证明类型保持 -/
tactic_elab "prove_preservation" : tactic =>
  Lean.Elab.Tactic.evalTactic (← 
    `(tactic|
      intros
      try induction hstep
      all_goals aesop_semantics
    )
  )

/-- 使用Aesop自动证明进展 -/
tactic_elab "prove_progress" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      intros
      try induction t
      all_goals try { left; simp [Tm.isValue] }
      all_goals try { right; aesop_semantics }
    )
  )

end Operational
