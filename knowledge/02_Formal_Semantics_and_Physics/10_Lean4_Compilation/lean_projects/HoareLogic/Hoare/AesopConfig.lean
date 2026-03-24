/-
# HoareLogic Aesop配置

本模块为霍尔逻辑项目配置Aesop自动化证明策略，
专注于验证条件生成、WP计算和程序验证。
-/

import Aesop

namespace Hoare

-- ==================== Aesop规则集配置 ====================

/-- HoareLogic领域特定的Aesop规则集 -/
declare_aesop_rule_sets [HoareLogic]

-- ==================== Safe规则 ====================

@[aesop safe (rule_sets [HoareLogic])]
theorem skipRule_safe {P : Assertion} : {P} skip {P} := by
  intro s s' h hp
  cases h
  exact hp

@[aesop safe (rule_sets [HoareLogic])]
theorem wp_hoare_safe {c : Cmd} {Q : Assertion} : {wp c Q} c {Q} := by
  intro s s' hc hwp
  apply hwp
  exact hc

@[aesop safe (rule_sets [HoareLogic])]
theorem implies_refl {P : Assertion} : P ⟹ P := by
  intro s hp
  exact hp

-- ==================== Unsafe规则 ====================

@[aesop unsafe 30% (rule_sets [HoareLogic])]
theorem seqRule_unsafe {P Q R : Assertion} {c₁ c₂ : Cmd} 
  (h₁ : {P} c₁ {Q}) (h₂ : {Q} c₂ {R}) : {P} (c₁ ;; c₂) {R} :=
  Rules.seqRule h₁ h₂

@[aesop unsafe 30% (rule_sets [HoareLogic])]
theorem assignRule_unsafe {P : Assertion} {x : Var} {a : AExp} :
  {P[x ↦ a]} (x := a) {P} :=
  Rules.assignRule P x a

@[aesop unsafe 30% (rule_sets [HoareLogic])]
theorem consequenceRule_unsafe {P P' Q Q' : Assertion} {c : Cmd}
  (hp : P' ⟹ P) (h : {P} c {Q}) (hq : Q ⟹ Q') : {P'} c {Q'} :=
  Rules.consequenceRule hp h hq

@[aesop unsafe 25% (rule_sets [HoareLogic])]
theorem weakenPre_unsafe {P P' Q : Assertion} {c : Cmd}
  (hp : P' ⟹ P) (h : {P} c {Q}) : {P'} c {Q} :=
  Rules.weakenPre hp h

@[aesop unsafe 25% (rule_sets [HoareLogic])]
theorem strengthenPost_unsafe {P Q Q' : Assertion} {c : Cmd}
  (h : {P} c {Q}) (hq : Q ⟹ Q') : {P} c {Q'} :=
  Rules.strengthenPost h hq

@[aesop unsafe 20% (rule_sets [HoareLogic])]
theorem ifRule_unsafe {P Q : Assertion} {b : BExp} {c₁ c₂ : Cmd}
  (h₁ : {P ∧ b} c₁ {Q}) (h₂ : {P ∧ ¬b} c₂ {Q}) : {P} (if b then c₁ else c₂) {Q} :=
  Rules.ifRule h₁ h₂

@[aesop unsafe 20% (rule_sets [HoareLogic])]
theorem whileRule_unsafe {I : Assertion} {b : BExp} {c : Cmd}
  (h : {I ∧ b} c {I}) : {I} (while b do c) {I ∧ ¬b} :=
  Rules.whileRule h

-- ==================== WP相关规则 ====================

@[aesop unsafe 25% (rule_sets [HoareLogic])]
theorem wp_mono_unsafe {c : Cmd} {Q₁ Q₂ : Assertion} 
  (h : Q₁ ⟹ Q₂) : wp c Q₁ ⟹ wp c Q₂ :=
  WPProperties.wp_mono h

@[aesop unsafe 25% (rule_sets [HoareLogic])]
theorem wp_characterization_unsafe {c : Cmd} {P Q : Assertion} :
  ({P} c {Q} ↔ P ⟹ wp c Q) :=
  WPProperties.wp_characterization

-- ==================== 归一化规则 ====================

@[aesop norm (rule_sets [HoareLogic])]
theorem wp_skip_norm {Q : Assertion} : wp skip Q = Q :=
  WPProperties.wp_skip Q

@[aesop norm (rule_sets [HoareLogic])]
theorem wp_assign_norm {x : Var} {a : AExp} {Q : Assertion} :
  wp (x := a) Q = Q[x ↦ a] :=
  WPProperties.wp_assign x a Q

@[aesop norm (rule_sets [HoareLogic])]
theorem wp_seq_norm {c₁ c₂ : Cmd} {Q : Assertion} :
  wp (c₁ ;; c₂) Q = wp c₁ (wp c₂ Q) :=
  WPProperties.wp_seq c₁ c₂ Q

@[aesop norm (rule_sets [HoareLogic])]
theorem subst_assertion_def {P : Assertion} {x : Var} {a : AExp} :
  (P[x ↦ a]) = (λ s => P (s[x ↦ ⟦a⟧s])) := by
  rfl

-- ==================== Aesop策略宏定义 ====================

/-- 霍尔逻辑专用aesop策略 -/
macro "aesop_hoare" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 50
        maxRuleApplications := 500
        enableSimp := true })
      (rule_sets [HoareLogic])
  )

/-- WP计算策略 -/
macro "aesop_wp" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := true
        maxRuleApplicationDepth := 40
        maxRuleApplications := 400
        enableSimp := true })
      (rule_sets [HoareLogic])
  )

/-- 验证条件生成策略 -/
macro "aesop_vc" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 60
        maxRuleApplications := 600
        enableSimp := true })
      (rule_sets [HoareLogic])
  )

/-- 循环不变式证明策略 -/
macro "aesop_inv" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 80
        maxRuleApplications := 800
        enableSimp := true })
      (rule_sets [HoareLogic])
  )

-- ==================== 便捷证明构造器 ====================

tactic_elab "verify_assign" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      apply assignRule_unsafe
    )
  )

tactic_elab "verify_seq" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      apply seqRule_unsafe
      · verify_assign
      · verify_assign
    )
  )

tactic_elab "verify_while" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      apply whileRule_unsafe
      try apply weakenPre_unsafe _ assignRule_unsafe
      all_goals aesop_hoare
    )
  )

tactic_elab "auto_verify" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      try simp only [wp_skip_norm, wp_assign_norm, wp_seq_norm]
      try aesop_hoare
    )
  )

end Hoare
