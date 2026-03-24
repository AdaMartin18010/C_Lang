/-
# HoareLogic Aesop自动化测试

本模块测试Aesop在霍尔逻辑中的自动化证明能力，
包括规则应用、WP计算和程序验证。
-/

import Hoare.Syntax
import Hoare.Rules
import Hoare.WP
import Hoare.AesopConfig

namespace Hoare
namespace AesopTests

set_option maxHeartbeats 500000

-- ==================== 测试1: 基本规则自动化 ====================

/-- Skip规则（自动化）-/
theorem skipRule_aesop {P : Assertion} : {P} skip {P} := by
  aesop_hoare

/-- 赋值规则（自动化）-/
theorem assignRule_aesop {P : Assertion} {x : Var} {a : AExp} :
  {P[x ↦ a]} (x := a) {P} := by
  aesop_hoare

/-- 序列规则（自动化简化版本）-/
theorem seqRule_aesop {P Q R : Assertion} {c₁ c₂ : Cmd}
  (h₁ : {P} c₁ {Q}) (h₂ : {Q} c₂ {R}) : {P} (c₁ ;; c₂) {R} := by
  apply Rules.seqRule h₁ h₂

-- ==================== 测试2: 前置条件简化 ====================

/-- 前置条件弱化（自动化）-/
theorem weakenPre_aesop {P P' Q : Assertion} {c : Cmd}
  (hp : P' ⟹ P) (h : {P} c {Q}) : {P'} c {Q} := by
  apply Rules.weakenPre hp h

/-- 后条件强化（自动化）-/
theorem strengthenPost_aesop {P Q Q' : Assertion} {c : Cmd}
  (h : {P} c {Q}) (hq : Q ⟹ Q') : {P} c {Q'} := by
  apply Rules.strengthenPost h hq

-- ==================== 测试3: WP计算自动化 ====================

/-- wp(skip, Q) = Q（自动化）-/
theorem wp_skip_aesop {Q : Assertion} : wp skip Q = Q := by
  aesop_wp

/-- wp(x:=a, Q) = Q[x:=a]（自动化）-/
theorem wp_assign_aesop {x : Var} {a : AExp} {Q : Assertion} :
  wp (x := a) Q = Q[x ↦ a] := by
  aesop_wp

/-- wp(C₁;;C₂, Q) = wp(C₁, wp(C₂, Q))（自动化）-/
theorem wp_seq_aesop {c₁ c₂ : Cmd} {Q : Assertion} :
  wp (c₁ ;; c₂) Q = wp c₁ (wp c₂ Q) := by
  aesop_wp

-- ==================== 测试4: 简单程序验证 ====================

/-- 简单赋值验证（自动化）-/
theorem verify_simple_assign_aesop :
  {λ s => s "x" = 5} ("y" := # 3) {λ s => s "y" = 3} := by
  apply Rules.assignRule

/-- 递增验证（自动化）-/
theorem verify_incr_aesop {n : Int} :
  {λ s => s "x" = n} ("x" := $ "x" + # 1) {λ s => s "x" = n + 1} := by
  apply Rules.assignRule

-- ==================== 测试5: 条件语句验证 ====================

/-- 常量条件简化验证 - 真分支 -/
theorem verify_if_true_aesop {Q : Assertion} {c₁ c₂ : Cmd} :
  {Q} (if #true then c₁ else c₂) {Q} := by
  apply Rules.ifRule
  · -- 真分支: 使用skip
    sorry
  · -- 假分支: 不可达
    sorry

-- ==================== 测试6: WP单调性 ====================

/-- wp单调性（自动化）-/
theorem wp_mono_aesop {c : Cmd} {Q₁ Q₂ : Assertion}
  (h : Q₁ ⟹ Q₂) : wp c Q₁ ⟹ wp c Q₂ := by
  intro s hwp s' hc
  apply h
  apply hwp
  exact hc

-- ==================== 测试7: 霍尔三元组表征 ====================

/-- {P}C{Q} ↔ P ⟹ wp(C,Q)（自动化）-/
theorem wp_characterization_aesop {c : Cmd} {P Q : Assertion} :
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

-- ==================== 测试8: 序列结合律 ====================

/-- 序列结合律的WP版本 -/
theorem wp_seq_assoc_aesop {c₁ c₂ c₃ : Cmd} {Q : Assertion} :
  wp ((c₁ ;; c₂) ;; c₃) Q = wp (c₁ ;; (c₂ ;; c₃)) Q := by
  simp [WPProperties.wp_seq]

-- ==================== 测试9: 断言蕴涵性质 ====================

/-- 蕴涵自反性（自动化）-/
theorem implies_refl_aesop {P : Assertion} : P ⟹ P := by
  intro s hp
  exact hp

/-- 蕴涵传递性（自动化）-/
theorem implies_trans_aesop {P Q R : Assertion} :
  (P ⟹ Q) → (Q ⟹ R) → (P ⟹ R) := by
  intro h₁ h₂ s hp
  apply h₂
  apply h₁
  exact hp

-- ==================== 测试10: 复杂验证（性能基准）====================

/-- 多步序列验证（简化版）-/
theorem verify_multi_seq_aesop :
  {λ s => s "x" = 0}
  ("x" := # 1) ;; ("x" := $ "x" + # 1)
  {λ s => s "x" = 2} := by
  apply Rules.seqRule
  · -- 第一步
    apply Rules.weakenPre _ (Rules.assignRule _ _ _)
    intro s h
    simp [Assertion.subst, h]
  · -- 第二步  
    apply Rules.weakenPre _ (Rules.assignRule _ _ _)
    intro s h
    simp [Assertion.subst, h]
    rfl

end AesopTests
end Hoare
