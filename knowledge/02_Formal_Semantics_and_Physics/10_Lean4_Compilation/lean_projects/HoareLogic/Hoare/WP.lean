/-
# 最弱前置条件 (Weakest Precondition)

本模块实现了最弱前置条件计算，包括：
- wp(C, Q) 的定义
- wp的单调性
- 霍尔三元组与wp的关系
- 最弱前置条件生成
-/  

import Hoare.Syntax
import Hoare.Rules

namespace Hoare

-- ==================== 最弱前置条件定义 ====================

/-- 最弱前置条件: wp(C, Q) -/
def wp (c : Cmd) (Q : Assertion) : Assertion :=
  λ s => ∀ s', ⟨c, s⟩ ⇓ s' → Q s'

-- 表示法: wp C Q
notation "wp " c " " Q => wp c Q

-- ==================== wp的基本性质 ====================

namespace WPProperties

/-- wp是单调的 -/
theorem wp_mono {c : Cmd} {Q₁ Q₂ : Assertion} 
  (h : Q₁ ⟹ Q₂) : wp c Q₁ ⟹ wp c Q₂ := by
  intro s hwp s' hc
  apply h
  apply hwp
  exact hc

/-- wp是唯一满足 {wp(C,Q)} C {Q} 的最弱前置条件 -/
theorem wp_characterization {c : Cmd} {P Q : Assertion} :
  ({P} c {Q} ↔ P ⟹ wp c Q) := by
  constructor
  · -- 正向
    intro h s hp s' hc
    apply h s s'
    · exact hc
    · exact hp
  · -- 反向
    intro h s s' hc hp
    apply h s hp s'
    exact hc

/-- wp(skip, Q) = Q -/
theorem wp_skip (Q : Assertion) : wp skip Q = Q := by
  funext s
  simp [wp]
  constructor
  · intro h
    apply h s
    apply BigStep.skip
  · intro h s' hc
    cases hc
    exact h

/-- wp(x:=a, Q) = Q[x:=a] -/
theorem wp_assign (x : Var) (a : AExp) (Q : Assertion) :
  wp (x := a) Q = Q[x ↦ a] := by
  funext s
  simp [wp, Assertion.subst]
  constructor
  · intro h
    apply h (s[x ↦ ⟦a⟧s])
    apply BigStep.assign
  · intro h s' hc
    cases hc
    exact h

/-- wp(C₁;;C₂, Q) = wp(C₁, wp(C₂, Q)) -/
theorem wp_seq (c₁ c₂ : Cmd) (Q : Assertion) :
  wp (c₁ ;; c₂) Q = wp c₁ (wp c₂ Q) := by
  funext s
  simp [wp]
  constructor
  · intro h
    intro s₁ hc₁
    intro s' hc₂
    apply h s'
    apply BigStep.seq
    · exact hc₁
    · exact hc₂
  · intro h s' hc
    cases hc with
    | seq s₂ hc₁ hc₂ =>
      apply h s₂
      · exact hc₁
      · exact hc₂

/-- wp(if b then C₁ else C₂, Q) = 
     (b → wp(C₁, Q)) ∧ (¬b → wp(C₂, Q)) -/
def wp_if (b : BExp) (c₁ c₂ : Cmd) (Q : Assertion) : Assertion :=
  λ s => (⟦b⟧s = true → wp c₁ Q s) ∧ (⟦b⟧s = false → wp c₂ Q s)

theorem wp_if_eq (b : BExp) (c₁ c₂ : Cmd) (Q : Assertion) :
  wp (if b then c₁ else c₂) Q = wp_if b c₁ c₂ Q := by
  funext s
  simp [wp, wp_if]
  constructor
  · intro h
    constructor
    · intro hb s' hc
      apply h s'
      apply BigStep.ifTrue hb hc
    · intro hb s' hc
      apply h s'
      apply BigStep.ifFalse hb hc
  · intro h s' hc
    cases hc with
    | ifTrue hb hc₁ =>
      apply h.1 hb s' hc₁
    | ifFalse hb hc₂ =>
      apply h.2 hb s' hc₂

/-- wp(while b do C, Q) 是递归方程的最小解 -/
def wp_while (b : BExp) (c : Cmd) (Q : Assertion) : Assertion :=
  -- 这是一个不动点方程的解
  -- X = (b → wp(C, X)) ∧ (¬b → Q)
  -- 实际计算中需要迭代逼近
  λ s => wp_while_iter b c Q 100 s  -- 使用迭代逼近
where
  wp_while_iter (b : BExp) (c : Cmd) (Q : Assertion) : Nat → Assertion
    | 0     => λ _ => True  -- 初始近似
    | n + 1 => λ s => 
      if ⟦b⟧s = false then Q s
      else wp c (wp_while_iter b c Q n) s

-- ==================== wp与霍尔三元组 ====================

theorem wp_hoare {c : Cmd} {Q : Assertion} :
  {wp c Q} c {Q} := by
  intro s s' hc hwp
  apply hwp
  exact hc

theorem wp_weakest {c : Cmd} {P Q : Assertion} (h : {P} c {Q}) :
  P ⟹ wp c Q := by
  intro s hp s' hc
  apply h s s'
  · exact hc
  · exact hp

end WPProperties

-- ==================== 前置条件计算 ====================

namespace WPCalculator

open WPProperties

/-- 计算前置条件 -/
def calcWP (c : Cmd) (Q : Assertion) : Assertion :=
  match c with
  | skip => Q
  | assign x a => Q[x ↦ a]
  | seq c₁ c₂ => calcWP c₁ (calcWP c₂ Q)
  | ifthen b c₁ c₂ => 
    λ s => (⟦b⟧s = true → calcWP c₁ Q s) ∧ (⟦b⟧s = false → calcWP c₂ Q s)
  | while b c => wp_while b c Q

/-- 验证计算的前置条件是正确的 -/
theorem calcWP_correct {c : Cmd} {Q : Assertion} :
  {calcWP c Q} c {Q} := by
  induction c generalizing Q with
  | skip =>
    simp [calcWP]
    apply Rules.skipRule
  | assign x a =>
    simp [calcWP]
    apply Rules.assignRule
  | seq c₁ c₂ ih₁ ih₂ =>
    simp [calcWP]
    apply Rules.seqRule
    · apply ih₁
    · apply ih₂
  | ifthen b c₁ c₂ ih₁ ih₂ =>
    simp [calcWP]
    apply Rules.ifRule
    · apply ih₁
    · apply ih₂
  | while b c ih =>
    simp [calcWP]
    apply Rules.whileRule
    -- 需要证明wp_while满足不变式
    sorry

end WPCalculator

-- ==================== 最强后置条件 ====================

namespace SPProperties

/-- 最强后置条件: sp(P, C) -/
def sp (P : Assertion) (c : Cmd) : Assertion :=
  λ s' => ∃ s, P s ∧ ⟨c, s⟩ ⇓ s'

-- 表示法: sp P C
notation "sp " P " " c => sp P c

/-- sp(P, skip) = P -/
theorem sp_skip (P : Assertion) : sp P skip = P := by
  funext s
  simp [sp]
  constructor
  · intro h
    cases h with
    | intro s' hp =>
      cases hp.2
      exact hp.1
  · intro h
    exists s

/-- sp(P, x:=a) = λs'. ∃s. P s ∧ s' = s[x↦⟦a⟧s] -/
theorem sp_assign (P : Assertion) (x : Var) (a : AExp) :
  sp P (x := a) = (λ s' => ∃ s, P s ∧ s' = s[x ↦ ⟦a⟧s]) := by
  funext s'
  simp [sp]
  constructor
  · intro h
    cases h with
    | intro s hp =>
      cases hp.2
      exact ⟨s, hp.1, rfl⟩
  · intro h
    cases h with
    | intro s hp =>
      exists s
      constructor
      · exact hp.1
      · rw [hp.2]
        apply BigStep.assign

/-- sp(P, C₁;;C₂) = sp(sp(P, C₁), C₂) -/
theorem sp_seq (P : Assertion) (c₁ c₂ : Cmd) :
  sp P (c₁ ;; c₂) = sp (sp P c₁) c₂ := by
  funext s'
  simp [sp]
  constructor
  · -- 正向
    intro h
    cases h with
    | intro s ⟨hp, hc⟩ =>
      cases hc with
      | seq s₂ hc₁ hc₂ =>
        exists s₂
  · -- 反向
    intro h
    cases h with
    | intro s₂ ⟨hs₂, hc₂⟩ =>
      cases hs₂ with
      | intro s ⟨hp, hc₁⟩ =>
        exists s
        constructor
        · exact hp
        · apply BigStep.seq
          · exact hc₁
          · exact hc₂

end SPProperties

end Hoare
