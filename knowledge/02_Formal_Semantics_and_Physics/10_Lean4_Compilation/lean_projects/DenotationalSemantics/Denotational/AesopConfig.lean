/-
# DenotationalSemantics Aesop配置

本模块为指称语义项目配置Aesop自动化证明策略，
专注于域论、连续性和不动点相关证明。
-/

import Aesop

namespace Denotational

-- ==================== Aesop规则集配置 ====================

/-- DenotationalSemantics领域特定的Aesop规则集 -/
declare_aesop_rule_sets [DenotationalSemantics]

-- ==================== Safe规则 ====================

@[aesop safe (rule_sets [DenotationalSemantics])]
theorem le_refl_safe {D} [Poset D] {x : D} : x ⊑ x :=
  Poset.le_refl x

@[aesop safe (rule_sets [DenotationalSemantics])]
theorem bot_le_safe {D} [CPO D] {x : D} : ⊥ ⊑ x :=
  CPO.bot_le x

@[aesop safe (rule_sets [DenotationalSemantics])]
theorem monotone_apply {D E} [Poset D] [Poset E] {f : D → E} {hf : Monotone f} {x y : D} :
  x ⊑ y → f x ⊑ f y := by
  apply hf

-- ==================== Unsafe规则 ====================

@[aesop unsafe 30% (rule_sets [DenotationalSemantics])]
theorem le_trans_unsafe {D} [Poset D] {x y z : D} : x ⊑ y → y ⊑ z → x ⊑ z :=
  Poset.le_trans

@[aesop unsafe 30% (rule_sets [DenotationalSemantics])]
theorem le_antisymm_unsafe {D} [Poset D] {x y : D} : x ⊑ y → y ⊑ x → x = y :=
  Poset.le_antisymm

@[aesop unsafe 25% (rule_sets [DenotationalSemantics])]
theorem lub_upper_bound {D} [CPO D] {c : Nat → D} {h : IsChain c} {n : Nat} :
  c n ⊑ CPO.lub c h := by
  apply (CPO.lub_is_lub c h).1

@[aesop unsafe 20% (rule_sets [DenotationalSemantics])]
theorem lub_least {D} [CPO D] {c : Nat → D} {h : IsChain c} {ub : D} :
  (∀ n, c n ⊑ ub) → CPO.lub c h ⊑ ub := by
  intro hub
  apply (CPO.lub_is_lub c h).2
  exact hub

-- ==================== 连续性规则 ====================

@[aesop unsafe 25% (rule_sets [DenotationalSemantics])]
theorem continuous_mono {D E} [CPO D] [CPO E] {f : D → E} (hf : Continuous f) :
  Monotone f :=
  hf.1

@[aesop unsafe 25% (rule_sets [DenotationalSemantics])]
theorem continuous_lub {D E} [CPO D] [CPO E] {f : D → E} {c : Nat → D} {h : IsChain c} 
  (hf : Continuous f) :
  f (CPO.lub c h) = CPO.lub (λ n => f (c n)) _ := by
  apply hf.2

-- ==================== 不动点规则 ====================

@[aesop unsafe 30% (rule_sets [DenotationalSemantics])]
theorem fix_fp_unsafe {D} [CPO D] {f : D → D} {hf : Continuous f} :
  f (fix f hf) = fix f hf :=
  fix_fp hf

@[aesop unsafe 25% (rule_sets [DenotationalSemantics])]
theorem fix_least_unsafe {D} [CPO D] {f : D → D} {hf : Continuous f} {x : D} :
  f x = x → fix f hf ⊑ x :=
  fix_least hf

-- ==================== 归一化规则 ====================

@[aesop norm (rule_sets [DenotationalSemantics])]
theorem iterate_zero {D} [CPO D] {f : D → D} : f^[[0]] = ⊥ := by
  simp [iterate]

@[aesop norm (rule_sets [DenotationalSemantics])]
theorem iterate_succ {D} [CPO D] {f : D → D} {n : Nat} : f^[[n+1]] = f (f^[[n]]) := by
  simp [iterate]

@[aesop norm simp (rule_sets [DenotationalSemantics])]
def is_chain_def {D} [Poset D] (c : Nat → D) : IsChain c ↔ ∀ n, c n ⊑ c (n + 1) := by
  rfl

-- ==================== Aesop策略宏定义 ====================

/-- 域论专用aesop策略 -/
macro "aesop_domain" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 60
        maxRuleApplications := 600
        enableSimp := true })
      (rule_sets [DenotationalSemantics])
  )

/-- 单调性证明策略 -/
macro "aesop_mono" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := true
        maxRuleApplicationDepth := 30
        maxRuleApplications := 200
        enableSimp := true })
      (rule_sets [DenotationalSemantics])
  )

/-- 连续性证明策略 -/
macro "aesop_continuous" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 80
        maxRuleApplications := 800
        enableSimp := true })
      (rule_sets [DenotationalSemantics])
  )

/-- 不动点证明策略 -/
macro "aesop_fix" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 100
        maxRuleApplications := 1000
        enableSimp := true })
      (rule_sets [DenotationalSemantics])
  )

-- ==================== 便捷证明构造器 ====================

tactic_elab "prove_monotone" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      intro x y h
      try simp [Poset.le] at h ⊢
      aesop_mono
    )
  )

tactic_elab "prove_continuous" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      constructor
      · prove_monotone
      · intro c hchain
        try simp [Poset.le]
        aesop_continuous
    )
  )

end Denotational
