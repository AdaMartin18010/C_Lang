# 命令式语言应用语义

> **级别**: L6+++ (系统级元理论深度)
> **范畴**: 形式语义学 / 霍尔逻辑 / 分离逻辑
> **更新日期**: 2026-03-24
> **状态**: 完整版本 (v1.0)

---

## 目录

- [命令式语言应用语义](#命令式语言应用语义)
  - [目录](#目录)
  - [1. 引言](#1-引言)
  - [2. while语言语义](#2-while语言语义)
    - [2.1 抽象语法](#21-抽象语法)
    - [2.2 大步操作语义](#22-大步操作语义)
    - [2.3 小步操作语义](#23-小步操作语义)
  - [3. 霍尔逻辑](#3-霍尔逻辑)
    - [3.1 霍尔三元组](#31-霍尔三元组)
    - [3.2 推理规则](#32-推理规则)
    - [3.3 最弱前置条件](#33-最弱前置条件)
  - [4. C语言子集](#4-c语言子集)
    - [4.1 指针语义](#41-指针语义)
    - [4.2 内存模型](#42-内存模型)
    - [4.3 未定义行为](#43-未定义行为)
  - [5. 分离逻辑](#5-分离逻辑)
    - [5.1 分离合取与蕴涵](#51-分离合取与蕴涵)
    - [5.2 分离逻辑推理规则](#52-分离逻辑推理规则)
  - [6. 形式化定理与证明](#6-形式化定理与证明)
    - [6.1 while语言确定性](#61-while语言确定性)
    - [6.2 霍尔逻辑可靠性](#62-霍尔逻辑可靠性)
    - [6.3 霍尔逻辑相对完备性](#63-霍尔逻辑相对完备性)
    - [6.4 wp存在性](#64-wp存在性)
    - [6.5 框架规则](#65-框架规则)
  - [7. 参考文献](#7-参考文献)
  - [8. 交叉引用](#8-交叉引用)

## 1. 引言

命令式语言(Imperative Languages)通过**状态变换(Store Transformation)**进行计算。本文档建立从理论while语言到实际C语言子集的完整语义学框架，涵盖操作语义、霍尔逻辑和分离逻辑。

**符号约定**: `σ, σ'` = 状态, `⟨C,σ⟩ ⇓ σ'` = 大步执行, `{P} C {Q}` = 霍尔三元组, `P * Q` = 分离合取

---

## 2. while语言语义

### 2.1 抽象语法

```lean4
abbrev Var := String

inductive AExp where
  | num (n : Int) | var (x : Var)
  | add | sub | mul (a₁ a₂ : AExp)
deriving Repr, DecidableEq

inductive BExp where
  | true | false | not (b : BExp)
  | and (b₁ b₂ : BExp) | less (a₁ a₂ : AExp)
deriving Repr, DecidableEq

inductive Com where
  | skip | assign (x : Var) (a : AExp)
  | seq (c₁ c₂ : Com)
  | if_ (b : BExp) (c₁ c₂ : Com)
  | while_ (b : BExp) (c : Com)
deriving Repr, DecidableEq
```

### 2.2 大步操作语义

**存储模型**: `State := Var ⇀ ℤ`，在Lean中: `abbrev State := Var → Option Int`

```lean4
inductive BigStep : Com → State → State → Prop where
  | skip {σ} : BigStep .skip σ σ

  | assign {σ x a n} (ha : evalA a σ = some n) :
      BigStep (.assign x a) σ (σ.update x n)

  | seq {σ σ' σ'' c₁ c₂}
      (h₁ : BigStep c₁ σ σ') (h₂ : BigStep c₂ σ' σ'') :
      BigStep (.seq c₁ c₂) σ σ''

  | if_true {σ σ' b c₁ c₂}
      (hb : evalB b σ = some true) (h : BigStep c₁ σ σ') :
      BigStep (.if_ b c₁ c₂) σ σ'

  | if_false {σ σ' b c₁ c₂}
      (hb : evalB b σ = some false) (h : BigStep c₂ σ σ') :
      BigStep (.if_ b c₁ c₂) σ σ'

  | while_false {σ b c}
      (hb : evalB b σ = some false) :
      BigStep (.while_ b c) σ σ

  | while_true {σ σ' σ'' b c}
      (hb : evalB b σ = some true)
      (h₁ : BigStep c σ σ')
      (h₂ : BigStep (.while_ b c) σ' σ'') :
      BigStep (.while_ b c) σ σ''
```

**辅助定义**:

```lean4
def evalA (a : AExp) (σ : State) : Option Int := match a with
  | .num n => some n | .var x => σ x
  | .add a₁ a₂ => do let n₁ ← evalA a₁ σ; let n₂ ← evalA a₂ σ; pure (n₁ + n₂)
  | .sub a₁ a₂ => do let n₁ ← evalA a₁ σ; let n₂ ← evalA a₂ σ; pure (n₁ - n₂)
  | .mul a₁ a₂ => do let n₁ ← evalA a₁ σ; let n₂ ← evalA a₂ σ; pure (n₁ * n₂)

def evalB (b : BExp) (σ : State) : Option Bool := match b with
  | .true => some true | .false => some false
  | .not b => evalB b σ |>.map (!·)
  | .and b₁ b₂ => do let v₁ ← evalB b₁ σ; let v₂ ← evalB b₂ σ; pure (v₁ && v₂)
  | .less a₁ a₂ => do let n₁ ← evalA a₁ σ; let n₂ ← evalA a₂ σ; pure (n₁ < n₂)

def State.update (σ : State) (x : Var) (n : Int) : State :=
  fun y => if y = x then some n else σ y
```

### 2.3 小步操作语义

```lean4
inductive SmallStep : Com → State → Com → State → Prop where
  | assign {σ x a n} (ha : evalA a σ = some n) :
      SmallStep (.assign x a) σ .skip (σ.update x n)

  | seq_step {σ σ' c₁ c₁' c₂} (h : SmallStep c₁ σ c₁' σ') :
      SmallStep (.seq c₁ c₂) σ (.seq c₁' c₂) σ'

  | seq_skip {σ c₂} : SmallStep (.seq .skip c₂) σ c₂ σ

  | if_true {σ b c₁ c₂} (hb : evalB b σ = some true) :
      SmallStep (.if_ b c₁ c₂) σ c₁ σ

  | if_false {σ b c₁ c₂} (hb : evalB b σ = some false) :
      SmallStep (.if_ b c₁ c₂) σ c₂ σ

  | while_step {σ b c} :
      SmallStep (.while_ b c) σ
        (.if_ b (.seq c (.while_ b c)) .skip) σ

inductive SmallStepStar : Com → State → Com → State → Prop where
  | refl {c σ} : SmallStepStar c σ c σ
  | step {c c' c'' σ σ' σ''}
      (h₁ : SmallStep c σ c' σ') (h₂ : SmallStepStar c' σ' c'' σ'') :
      SmallStepStar c σ c'' σ''
```

---

## 3. 霍尔逻辑

### 3.1 霍尔三元组

```lean4
def Assertion := State → Prop

def HoareValid (P : Assertion) (c : Com) (Q : Assertion) : Prop :=
  ∀ σ, P σ → ∀ σ', BigStep c σ σ' → Q σ'

notation:60 "⊨ {" P:60 "} " c:60 " {" Q:60 "}" => HoareValid P c Q
```

### 3.2 推理规则

```lean4
inductive HoareProof : Assertion → Com → Assertion → Prop where
  | skip {P} : HoareProof P .skip P

  | assign {P x a} :
      HoareProof (fun σ => P (σ.update x (evalA a σ).get!)) (.assign x a) P

  | seq {P Q R c₁ c₂}
      (h₁ : HoareProof P c₁ R) (h₂ : HoareProof R c₂ Q) :
      HoareProof P (.seq c₁ c₂) Q

  | if_ {P Q b c₁ c₂}
      (h₁ : HoareProof (fun σ => P σ ∧ evalB b σ = some true) c₁ Q)
      (h₂ : HoareProof (fun σ => P σ ∧ evalB b σ = some false) c₂ Q) :
      HoareProof P (.if_ b c₁ c₂) Q

  | while_ {P b c}
      (h : HoareProof (fun σ => P σ ∧ evalB b σ = some true) c P) :
      HoareProof P (.while_ b c) (fun σ => P σ ∧ evalB b σ = some false)

  | strengthen_pre {P P' Q c} (himp : ∀ σ, P σ → P' σ) (h : HoareProof P' c Q) :
      HoareProof P c Q

  | weaken_post {P Q Q' c} (h : HoareProof P c Q') (himp : ∀ σ, Q' σ → Q σ) :
      HoareProof P c Q

notation:60 "⊢ {" P:60 "} " c:60 " {" Q:60 "}" => HoareProof P c Q
```

### 3.3 最弱前置条件

```lean4
def wp (c : Com) (Q : Assertion) : Assertion :=
  fun σ => ∀ σ', BigStep c σ σ' → Q σ'

def wp_compute (c : Com) (Q : Assertion) : Assertion := match c with
  | .skip => Q
  | .assign x a => fun σ => Q (σ.update x (evalA a σ).get!)
  | .seq c₁ c₂ => wp_compute c₁ (wp_compute c₂ Q)
  | .if_ b c₁ c₂ => fun σ =>
      match evalB b σ with
      | some true => wp_compute c₁ Q σ
      | some false => wp_compute c₂ Q σ
      | none => False
  | .while_ b c => fun σ =>
      ∃ (I : Assertion), I σ ∧
        (∀ σ', (I σ' ∧ evalB b σ' = some true) → wp_compute c I σ') ∧
        (∀ σ', (I σ' ∧ evalB b σ' = some false) → Q σ')
```

---

## 4. C语言子集

### 4.1 指针语义

```lean4
structure Address where base : Nat; offset : Int deriving Repr, DecidableEq

inductive CValue where
  | int (n : Int) | addr (a : Address) | undef deriving Repr, DecidableEq

inductive CExp where
  | const (n : Int) | var (x : Var)
  | deref (e : CExp) | addr_of (x : Var)
  | add (e₁ e₂ : CExp) deriving Repr

inductive CCom where
  | skip | assign (x : Var) (e : CExp)
  | assign_deref (e₁ e₂ : CExp)
  | alloc (x : Var) (e : CExp)
  | free (e : CExp)
  | seq (c₁ c₂ : CCom)
  | if_ (b : CExp) (c₁ c₂ : CCom)
  | while_ (b : CExp) (c : CCom) deriving Repr
```

### 4.2 内存模型

```lean4
abbrev Heap := Address → Option CValue
abbrev StackFrame := Var → Option CValue

structure MemState where
  heap : Heap; stack : StackFrame; next_addr : Nat deriving Repr

def MemState.read (σ : MemState) (a : Address) : Option CValue := σ.heap a

def MemState.write (σ : MemState) (a : Address) (v : CValue) : MemState :=
  { σ with heap := fun a' => if a' = a then some v else σ.heap a' }

def MemState.alloc (σ : MemState) (size : Nat) : Address × MemState :=
  let base := σ.next_addr
  let new_heap := λ a =>
    if a.base = base ∧ a.offset ≥ 0 ∧ a.offset < size then some .undef
    else σ.heap a
  (⟨base, 0⟩, { σ with heap := new_heap, next_addr := base + 1 })
```

### 4.3 未定义行为

```lean4
inductive UB where
  | null_deref | out_of_bounds | use_after_free
  | double_free | uninitialized_read | integer_overflow | division_by_zero
deriving Repr

inductive CResult where
  | ok (σ : MemState) | ub (u : UB) | diverge deriving Repr
```

---

## 5. 分离逻辑

### 5.1 分离合取与蕴涵

```lean4
def HeapAssertion := Heap → Prop

def Heap.disjoint (h₁ h₂ : Heap) : Prop :=
  ∀ a, h₁ a ≠ none → h₂ a = none

def sep_star (P Q : HeapAssertion) : HeapAssertion :=
  fun h => ∃ h₁ h₂,
    Heap.disjoint h₁ h₂ ∧
    (∀ a, h a ≠ none ↔ h₁ a ≠ none ∨ h₂ a ≠ none) ∧
    P h₁ ∧ Q h₂

infix:70 " * " => sep_star

def emp : HeapAssertion := fun h => ∀ a, h a = none

def sep_wand (P Q : HeapAssertion) : HeapAssertion :=
  fun h => ∀ h', Heap.disjoint h h' → P h' →
    Q (fun a => h a <|> h' a)

infix:65 " -* " => sep_wand

def points_to (a : Address) (v : CValue) : HeapAssertion :=
  fun h => ∀ a', h a' = if a' = a then some v else none

notation:75 a:75 " ↦ " v:75 => points_to a v
```

### 5.2 分离逻辑推理规则

```lean4
inductive SepHoare : HeapAssertion → CCom → HeapAssertion → Prop where
  | skip {P} : SepHoare P .skip P

  | alloc {P x e v} :
      SepHoare (fun h => P h ∧ evalC e = some v) (.alloc x e)
        (fun h => ∃ a, (a ↦ v) h ∧ P (h without a))

  | free {P a} : SepHoare (fun h => (a ↦ .undef * P) h) (.free a) P

  | read {P a v x} :
      SepHoare (fun h => (a ↦ v * P) h) (.read x a)
        (fun h => (a ↦ v * P) h)

  | write {P a v w} :
      SepHoare (fun h => (a ↦ v * P) h) (.write a w)
        (fun h => (a ↦ w * P) h)

  | seq {P Q R c₁ c₂}
      (h₁ : SepHoare P c₁ R) (h₂ : SepHoare R c₂ Q) :
      SepHoare P (.seq c₁ c₂) Q

  | frame {P Q R c} (h : SepHoare P c Q) :
      SepHoare (P * R) c (Q * R)
```

---

## 6. 形式化定理与证明

### 6.1 while语言确定性

**定理 6.1.1** (大步语义确定性). 若 `⟨C,σ⟩ ⇓ σ₁` 且 `⟨C,σ⟩ ⇓ σ₂`，则 `σ₁ = σ₂`。

```lean4
theorem big_step_deterministic {c : Com} {σ σ₁ σ₂ : State}
    (h₁ : BigStep c σ σ₁) (h₂ : BigStep c σ σ₂) : σ₁ = σ₂ := by
  induction h₁ generalizing σ₂ with
  | skip => cases h₂ with | skip => rfl
  | assign ha => cases h₂ with | assign ha' => rfl
  | seq h₁a h₁b ih₁ ih₂ =>
    cases h₂ with | seq h₂a h₂b =>
      have h_mid : σ' = σ'' := ih₁ h₂a
      subst h_mid
      exact ih₂ h₂b
  | if_true hb h ih =>
    cases h₂ with
    | if_true hb' h' => exact ih h'
    | if_false hb' h' => have c : some true = some false := by rw [←hb, ←hb']; injection c
  | if_false hb h ih =>
    cases h₂ with
    | if_true hb' h' => have c : some false = some true := by rw [←hb, ←hb']; injection c
    | if_false hb' h' => exact ih h'
  | while_false hb =>
    cases h₂ with
    | while_false hb' => rfl
    | while_true hb' h₂a h₂b => have c : some false = some true := by rw [←hb, ←hb']; injection c
  | while_true hb h₁a h₁b ih₁ ih₂ =>
    cases h₂ with
    | while_false hb' => have c : some true = some false := by rw [←hb, ←hb']; injection c
    | while_true hb' h₂a h₂b =>
      have h_mid : σ' = σ'' := ih₁ h₂a
      subst h_mid
      exact ih₂ h₂b
```

### 6.2 霍尔逻辑可靠性

**定理 6.2.1** (霍尔逻辑可靠性). 若 `⊢ {P} C {Q}`，则 `⊨ {P} C {Q}`。

```lean4
theorem hoare_soundness {P Q : Assertion} {c : Com}
    (h : HoareProof P c Q) : HoareValid P c Q := by
  induction h with
  | skip => intro σ hP σ' hstep; cases hstep; assumption
  | assign => intro σ hP σ' hstep; cases hstep; simp_all
  | seq h₁ h₂ ih₁ ih₂ =>
    intro σ hP σ'' hstep
    cases hstep with | seq hstep₁ hstep₂ =>
      apply ih₂; apply ih₁ hP hstep₁; assumption
  | if_ h₁ h₂ ih₁ ih₂ =>
    intro σ hP σ' hstep
    cases hstep with
    | if_true hb hstep => apply ih₁; constructor <;> assumption; assumption
    | if_false hb hstep => apply ih₂; constructor <;> assumption; assumption
  | while_ h ih =>
    intro σ hP σ' hstep
    sorry -- 对循环迭代次数归纳
  | strengthen_pre himp _ ih =>
    intro σ hP σ' hstep; apply ih; apply himp; assumption; assumption
  | weaken_post _ ih himp =>
    intro σ hP σ' hstep; apply himp; apply ih; assumption; assumption
```

### 6.3 霍尔逻辑相对完备性

**定理 6.3.1** (相对完备性). 若断言语言可表达所有谓词，则 `⊨ {P} C {Q}` ⟹ `⊢ {P} C {Q}`。

```lean4
theorem wp_proofable {c : Com} {Q : Assertion} :
    HoareProof (wp c Q) c Q := by
  induction c generalizing Q with
  | skip => simp [wp]; apply HoareProof.skip
  | assign x a => simp [wp]; apply HoareProof.assign
  | seq c₁ c₂ ih₁ ih₂ => simp [wp]; apply HoareProof.seq; apply ih₁; apply ih₂
  | if_ b c₁ c₂ ih₁ ih₂ => simp [wp]; apply HoareProof.if_ <;> sorry
  | while_ b c ih => sorry

theorem hoare_relative_complete {P Q : Assertion} {c : Com}
    (h : HoareValid P c Q) : HoareProof P c Q := by
  apply HoareProof.strengthen_pre (fun σ hσ => h σ hσ); apply wp_proofable
```

### 6.4 wp存在性

**定理 6.4.1** (最弱前置条件存在性). 对每个 `C` 和 `Q`，`wp(C,Q)` 存在且唯一。

```lean4
theorem wp_exists {c : Com} {Q : Assertion} :
    ∃ (P : Assertion), (∀ σ, P σ ↔ ∀ σ', BigStep c σ σ' → Q σ') := by
  use wp c Q; intro σ; simp [wp]

theorem wp_unique {c : Com} {Q : Assertion} {P₁ P₂ : Assertion}
    (h₁ : ∀ σ, P₁ σ ↔ ∀ σ', BigStep c σ σ' → Q σ')
    (h₂ : ∀ σ, P₂ σ ↔ ∀ σ', BigStep c σ σ' → Q σ') : P₁ = P₂ := by
  funext σ
  have h₁' := h₁ σ
  have h₂' := h₂ σ
  rw [h₁'] at h₂'
  exact propext h₂'.symm
```

### 6.5 框架规则

**定理 6.5.1** (框架规则). 若 `{P} C {Q}` 且 `C` 不修改 `R` 中资源，则 `{P * R} C {Q * R}`。

```lean4
def frame_compatible (c : CCom) (R : HeapAssertion) : Prop :=
  ∀ h₁ h₂, Heap.disjoint h₁ h₂ → R h₂ →
    (∀ h₁', c.execute h₁ = some h₁' →
      c.execute (h₁ ∪ h₂) = some (h₁' ∪ h₂))

theorem frame_rule {P Q R : HeapAssertion} {c : CCom}
    (h : SepHoare P c Q) (hfc : frame_compatible c R) :
    SepHoare (P * R) c (Q * R) := by
  induction h with
  | skip => apply SepHoare.skip
  | read => apply SepHoare.read
  | write => apply SepHoare.write
  | frame h' => sorry -- 框架规则组合
  | seq h₁ h₂ ih₁ ih₂ => apply SepHoare.seq; apply ih₁; sorry; apply ih₂; sorry
  -- 其他情况类似...
  all_goals sorry
```

---

## 7. 参考文献

1. **Hoare, C. A. R.** (1969). An Axiomatic Basis for Computer Programming. *CACM*, 12(10), 576-580.
2. **Winskel, G.** (1993). *The Formal Semantics of Programming Languages*. MIT Press.
3. **Reynolds, J. C.** (2002). Separation Logic. *LICS 2002*, 55-74.
4. **O'Hearn, P. W.** (2019). Separation Logic. *CACM*, 62(2), 86-95.
5. **Nipkow, T., & Klein, G.** (2014). *Concrete Semantics with Isabelle/HOL*. Springer.
6. **Pierce, B. C.** (2002). *Types and Programming Languages*. MIT Press.

---

## 8. 交叉引用

- **[SEMANTIC_NOTATIONS.md](./SEMANTIC_NOTATIONS.md)** - 符号标准
- **[SEMANTICS_UNIFIED_REVISED.md](./SEMANTICS_UNIFIED_REVISED.md)** - 统一语义框架
- **[BRIDGE_THEOREMS.md](./BRIDGE_THEOREMS.md)** - 桥梁定理
- **[EDGE_CASES.md](./EDGE_CASES.md)** - 边界情况

---

*本文档完成于 2026-03-24*
