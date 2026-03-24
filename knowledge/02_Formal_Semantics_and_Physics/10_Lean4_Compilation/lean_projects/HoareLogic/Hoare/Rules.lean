/-
# 霍尔逻辑推理规则

本模块实现了霍尔逻辑的推理规则，包括：
- 霍尔三元组 {P} C {Q}
- 推理规则公理
- 循环不变式
- 规则的可证明性
-/  

import Hoare.Syntax

namespace Hoare

-- ==================== 断言 ====================

/-- 断言: 关于程序状态的谓词 -/
def Assertion := State → Prop

def Assertion.implies (P Q : Assertion) : Prop :=
  ∀ s, P s → Q s

-- 表示法: P ⟹ Q
infix:50 " ⟹ " => Assertion.implies

/-- 断言等价 -/
def Assertion.equiv (P Q : Assertion) : Prop :=
  P ⟹ Q ∧ Q ⟹ P

infix:50 " ⟺ " => Assertion.equiv

-- ==================== 霍尔三元组 ====================

/-- 霍尔三元组 {P} C {Q} -/
def HoareTriple (P : Assertion) (c : Cmd) (Q : Assertion) : Prop :=
  ∀ s s', ⟨c, s⟩ ⇓ s' → P s → Q s'

-- 表示法: {P} C {Q}
notation "{" P "} " c " {" Q "}" => HoareTriple P c Q

-- ==================== 霍尔逻辑规则 ====================

namespace Rules

/-- 跳过规则 (Skip) -/
-- {P} skip {P}
theorem skipRule (P : Assertion) : {P} skip {P} := by
  intro s s' h hp
  cases h
  exact hp

/-- 赋值规则 (Assignment) -/
-- {P[x:=a]} x := a {P}
def Assertion.subst (P : Assertion) (x : Var) (a : AExp) : Assertion :=
  λ s => P (s[x ↦ ⟦a⟧s])

notation P "[" x " ↦ " a "]" => Assertion.subst P x a

theorem assignRule (P : Assertion) (x : Var) (a : AExp) :
  {P[x ↦ a]} (x := a) {P} := by
  intro s s' h hp
  cases h
  exact hp

/-- 序列规则 (Sequence) -/
-- {P} C₁ {Q} → {Q} C₂ {R} → {P} C₁;;C₂ {R}
theorem seqRule {P Q R : Assertion} {c₁ c₂ : Cmd} 
  (h₁ : {P} c₁ {Q}) (h₂ : {Q} c₂ {R}) :
  {P} (c₁ ;; c₂) {R} := by
  intro s s' h hp
  cases h with
  | seq s₂ hc₁ hc₂ =>
    apply h₂ s₂ s' hc₂
    apply h₁ s s₂ hc₁
    exact hp

/-- 条件规则 (Conditional) -/
-- {P ∧ b} C₁ {Q} → {P ∧ ¬b} C₂ {Q} → {P} if b then C₁ else C₂ {Q}

def Assertion.andB (P : Assertion) (b : BExp) : Assertion :=
  λ s => P s ∧ ⟦b⟧s = true

def Assertion.andNotB (P : Assertion) (b : BExp) : Assertion :=
  λ s => P s ∧ ⟦b⟧s = false

notation P " ∧ " b => Assertion.andB P b
notation P " ∧ ¬" b => Assertion.andNotB P b

theorem ifRule {P Q : Assertion} {b : BExp} {c₁ c₂ : Cmd}
  (h₁ : {P ∧ b} c₁ {Q}) (h₂ : {P ∧ ¬b} c₂ {Q}) :
  {P} (if b then c₁ else c₂) {Q} := by
  intro s s' h hp
  cases h with
  | ifTrue hb hc₁ =>
    apply h₁ s s' hc₁
    exact ⟨hp, hb⟩
  | ifFalse hb hc₂ =>
    apply h₂ s s' hc₂
    exact ⟨hp, hb⟩

/-- While循环规则 -/
-- {I ∧ b} C {I} → {I} while b do C {I ∧ ¬b}
theorem whileRule {I : Assertion} {b : BExp} {c : Cmd}
  (h : {I ∧ b} c {I}) :
  {I} (while b do c) {I ∧ ¬b} := by
  intro s s' hbig hp
  -- 使用归纳法证明while的性质
  have hinv : ∀ s₁ s₂, ⟨while b do c, s₁⟩ ⇓ s₂ → I s₁ → (I ∧ ¬b) s₂ := by
    intro s₁ s₂ hwhile
    induction hwith with
    | whileFalse s₁ hb =>
      intro hi
      exact ⟨hi, hb⟩
    | whileTrue s₁ s₂ s₃ hb hc hwhile ih =>
      intro hi
      apply ih
      apply h s₁ s₂ hc
      exact ⟨hi, hb⟩
  apply hinv s s' hbig hp

/-- 后条件强化 (Consequence) -/
-- P' ⟹ P → {P} C {Q} → Q ⟹ Q' → {P'} C {Q'}
theorem consequenceRule {P P' Q Q' : Assertion} {c : Cmd}
  (hp : P' ⟹ P) (h : {P} c {Q}) (hq : Q ⟹ Q') :
  {P'} c {Q'} := by
  intro s s' hc hp'
  apply hq s'
  apply h s s' hc
  apply hp s
  exact hp'

/-- 前条件弱化 -/
theorem weakenPre {P P' Q : Assertion} {c : Cmd}
  (hp : P' ⟹ P) (h : {P} c {Q}) :
  {P'} c {Q} := by
  apply consequenceRule hp h
  intro s hs
  exact hs

/-- 后条件强化 -/
theorem strengthenPost {P Q Q' : Assertion} {c : Cmd}
  (h : {P} c {Q}) (hq : Q ⟹ Q') :
  {P} c {Q'} := by
  apply consequenceRule _ h hq
  intro s hs
  exact hs

end Rules

-- ==================== 循环不变式 ====================

namespace Invariant

/-- 循环不变式的定义 -/
def isInvariant (I : Assertion) (b : BExp) (c : Cmd) : Prop :=
  {I ∧ b} c {I}

/-- 验证循环不变式 -/
theorem verifyInvariant {I : Assertion} {b : BExp} {c : Cmd}
  (h : isInvariant I b c) :
  {I} (while b do c) {I ∧ ¬b} := by
  apply Rules.whileRule
  exact h

/-- 寻找阶乘循环的不变式 -/
def factInvariant : Assertion :=
  λ s => s "fact" * s "n" = s "n₀" ∧ s "n" ≥ 0
  -- 其中n₀是初始值（需要额外记录）

end Invariant

-- ==================== 派生规则 ====================

namespace DerivedRules

open Rules

/-- 常量赋值规则 -/
theorem constAssign {x : Var} {n : Int} {P : Assertion} :
  {(λ s => P (s[x ↦ n]))} (x := # n) {P} := by
  apply assignRule

/-- 交换赋值规则 -/
theorem swapAssign {x y : Var} {t : Var} :
  {λ s => s x = a ∧ s y = b}
  (t := $ x) ;;
  (x := $ y) ;;
  (y := $ t)
  {λ s => s x = b ∧ s y = a} := by
  -- 完整证明需要展开每一步
  sorry

/-- 如果条件为假，则跳过 -/
theorem ifFalseSkip {P : Assertion} {b : BExp} {c₁ c₂ : Cmd}
  (h : ∀ s, P s → ⟦b⟧s = false) :
  {P} (if b then c₁ else c₂) {P} := by
  apply ifRule
  · -- 真分支不可达
    intro s s' hc ⟨hp, hb⟩
    have hfalse := h s hp
    contradiction
  · -- 假分支
    apply skipRule

end DerivedRules

-- ==================== 霍尔逻辑的可靠性 ====================

namespace Soundness

/-- 霍尔逻辑是可靠的: 如果 ⊢ {P} C {Q}，则 ⊨ {P} C {Q} -/
theorem hoare_sound {P Q : Assertion} {c : Cmd} :
  {P} c {Q} → ∀ s s', ⟨c, s⟩ ⇓ s' → P s → Q s' := by
  intro h
  exact h

/-- 部分正确性: 如果程序终止，则后条件成立 -/
def PartialCorrectness (P : Assertion) (c : Cmd) (Q : Assertion) : Prop :=
  ∀ s s', ⟨c, s⟩ ⇓ s' → P s → Q s'

/-- 全正确性: 程序必然终止且后条件成立 -/
def TotalCorrectness (P : Assertion) (c : Cmd) (Q : Assertion) : Prop :=
  (∀ s, P s → ∃ s', ⟨c, s⟩ ⇓ s') ∧ PartialCorrectness P c Q

end Soundness

end Hoare
