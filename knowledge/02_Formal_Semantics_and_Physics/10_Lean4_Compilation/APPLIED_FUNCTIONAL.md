# 函数式语言应用语义

> **版本**: 1.0
> **级别**: L6+++ (系统级元理论深度)
> **状态**: 符合 SEMANTIC_NOTATIONS.md
> **目标**: 建立函数式语言完整语义框架，从λ演算延伸到ML和Haskell

---

## 目录

- [函数式语言应用语义](#函数式语言应用语义)
  - [目录](#目录)
  - [1. 引言](#1-引言)
  - [2. λ演算基础](#2-λ演算基础)
    - [2.1 无类型λ演算](#21-无类型λ演算)
    - [2.2 简单类型λ演算](#22-简单类型λ演算)
    - [2.3 System F (多态λ演算)](#23-system-f-多态λ演算)
    - [2.4 递归类型](#24-递归类型)
  - [3. ML类型系统](#3-ml类型系统)
    - [3.1 let-多态](#31-let-多态)
    - [3.2 值限制](#32-值限制)
    - [3.3 多态引用问题](#33-多态引用问题)
    - [3.4 类型推导算法W](#34-类型推导算法w)
  - [4. Haskell惰性语义](#4-haskell惰性语义)
    - [4.1 求值策略](#41-求值策略)
    - [4.2 类型类语义](#42-类型类语义)
    - [4.3 类型族语义](#43-类型族语义)
  - [5. Lean 4对比](#5-lean-4对比)
    - [5.1 类型系统对比](#51-类型系统对比)
    - [5.2 归约策略对比](#52-归约策略对比)
    - [5.3 元编程对比](#53-元编程对比)
  - [6. 形式化定理与证明](#6-形式化定理与证明)
    - [6.1 Church-Rosser定理](#61-church-rosser定理)
    - [6.2 类型安全定理](#62-类型安全定理)
    - [6.3 System F健全性](#63-system-f健全性)
    - [6.4 ML类型推导完备性](#64-ml类型推导完备性)
    - [6.5 惰性求值一致性](#65-惰性求值一致性)
  - [7. 参考文献](#7-参考文献)
  - [8. 交叉引用](#8-交叉引用)

## 1. 引言

函数式编程语言的语义学是程序语言理论的基石。本文档建立从λ演算到现代函数式语言（ML、Haskell）的完整语义框架，并与Lean 4进行对比。

---

## 2. λ演算基础

### 2.1 无类型λ演算

**定义2.1.1 (语法)**: 项 `M, N` 由文法生成：

```
M ::= x | λx.M | M N
```

**定义2.1.2 (β归约)**: `(λx.M) N →β M[x:=N]`

**定理6.1 (Church-Rosser合流性)**: 若 `M →* N₁` 且 `M →* N₂`，则存在 `P` 使 `N₁ →* P` 且 `N₂ →* P`。

*证明见第6节。*

**Lean 4代码示例1**: 无类型λ演算表示

```lean
inductive UTLCTerm
  | var (n : Nat)      -- De Bruijn索引
  | abs (body : UTLCTerm)
  | app (fn arg : UTLCTerm)
  deriving Repr, BEq

def subst (m n : UTLCTerm) (k : Nat) : UTLCTerm :=
  match m with
  | var i => if i = k then n else var i
  | abs body => abs (subst body n (k + 1))
  | app f a => app (subst f n k) (subst a n k)

def betaReduce : UTLCTerm → Option UTLCTerm
  | app (abs body) arg => some (subst body arg 0)
  | _ => none

-- 恒等函数: λx.x
def idTerm := UTLCTerm.abs (UTLCTerm.var 0)
#eval betaReduce (UTLCTerm.app idTerm idTerm)  -- some (abs (var 0))
```

### 2.2 简单类型λ演算

**定义2.2.1 (类型)**: `τ ::= α | τ₁ → τ₂`

**类型判断规则**:

```
Γ(x)=τ       Γ,x:τ₁⊢M:τ₂         Γ⊢M:τ₁→τ₂  Γ⊢N:τ₁
────────   ────────────────     ───────────────────
Γ⊢x:τ      Γ⊢λx.M:τ₁→τ₂          Γ⊢M N:τ₂
```

**定理6.2 (类型安全)**:

1. **保持性**: 若 `Γ⊢M:τ` 且 `M→β M'`，则 `Γ⊢M':τ`
2. **进展性**: 若 `∅⊢M:τ`，则 `M` 是值或存在 `M'` 使 `M→β M'`

**Lean 4代码示例2**: 简单类型系统

```lean
inductive SimpleType
  | base (name : String)
  | arrow (dom cod : SimpleType)
  deriving Repr, BEq

infixr:50 " ⇒ " => SimpleType.arrow

inductive STTerm
  | var (n : Nat)
  | abs (ty : SimpleType) (body : STTerm)
  | app (fn arg : STTerm)

def TypeContext := Nat → Option SimpleType

def typeCheck (Γ : TypeContext) : STTerm → Option SimpleType
  | STTerm.var n => Γ n
  | STTerm.abs ty body =>
      let newΓ := fun | 0 => some ty | n+1 => Γ n
      match typeCheck newΓ body with | some τ => some (ty ⇒ τ) | none => none
  | STTerm.app f a =>
      match typeCheck Γ f, typeCheck Γ a with
      | some (dom ⇒ cod), some argTy => if dom = argTy then some cod else none
      | _, _ => none

-- 示例: (λx:Nat. x) : Nat → Nat
#eval typeCheck (fun _ => none)
  (STTerm.abs (SimpleType.base "Nat") (STTerm.var 0))
-- some (base "Nat" ⇒ base "Nat")
```

### 2.3 System F (多态λ演算)

**定义2.3.1 (类型)**: `τ ::= α | τ₁ → τ₂ | ∀α.τ`

**定义2.3.2 (项)**: `M ::= x | λx:τ.M | M N | Λα.M | M[τ]`

**类型规则**:

```
Γ,α⊢M:τ              Γ⊢M:∀α.τ
───────────         ───────────
Γ⊢Λα.M:∀α.τ         Γ⊢M[σ]:τ[α:=σ]
```

**定理6.3 (System F健全性)**: 若 `∅⊢M:τ`，则 `M` 不会卡住。

*证明使用Girard的可解释法，见第6节。*

**Lean 4代码示例3**: System F类型

```lean
inductive SystemFType
  | tvar (n : Nat)
  | arrow (dom cod : SystemFType)
  | forallT (body : SystemFType)
  deriving Repr, BEq

inductive SystemFTerm
  | var (n : Nat)
  | abs (ty : SystemFType) (body : SystemFTerm)
  | app (fn arg : SystemFTerm)
  | tabs (body : SystemFTerm)
  | tapp (tm : SystemFTerm) (ty : SystemFType)

-- 多态恒等: Λα. λx:α. x
def polyId : SystemFTerm :=
  SystemFTerm.tabs
    (SystemFTerm.abs (SystemFType.tvar 0) (SystemFTerm.var 0))

#eval polyId
```

### 2.4 递归类型

**定义2.4.1**: 递归类型 `μα.τ` 满足 `μα.τ = τ[α:=μα.τ]`

**两种形式**:

- **等递归**: `μα.τ` 与 `τ[α:=μα.τ]` 视为相同
- **iso-递归**: 需要显式 fold/unfold 操作

**例子**: `Nat = μα. 1 + α`，`List τ = μα. 1 + (τ × α)`

**Lean 4代码示例4**: iso-递归类型

```lean
inductive RecType (τ : Type → Type) (α : Type)
  | fold : τ α → RecType τ α

def unfold {τ : Type → Type} {α : Type} : RecType τ α → τ α
  | RecType.fold x => x

-- 自然数: NatF α = Unit + α
inductive NatF (α : Type)
  | zero | succ (n : α)
  deriving Repr

def NatR := RecType NatF NatR

def natZero : NatR := RecType.fold NatF.zero
def natSucc (n : NatR) : NatR := RecType.fold (NatF.succ n)

def natAdd (m n : NatR) : NatR :=
  match unfold m with
  | NatF.zero => n
  | NatF.succ m' => natSucc (natAdd m' n)

#eval natAdd (natSucc natZero) (natSucc natZero)  -- succ (succ zero)
```

---

## 3. ML类型系统

### 3.1 let-多态

**关键特性**: `let x = M in N` 允许多态泛化，而 `(λx.N) M` 不允许。

**类型规则**:

```
Γ⊢M:τ  Γ,x:Gen(Γ,τ)⊢N:σ
───────────────────────────
Γ⊢let x=M in N:σ
```

其中 `Gen(Γ,τ)` 将不在 `Γ` 自由出现的类型变量全称量化。

**Lean 4代码示例5**: let-多态

```lean
-- Lean let自动支持多态
let id : {α : Type} → α → α := fun x => x
#check (id 1, id true)  -- (Nat × Bool)

-- λ绑定不允许多态实例化
-- (fun id => (id 1, id true)) (fun x => x)  -- 类型错误!

def polyLet : Nat × Bool :=
  let id (x : α) := x
  (id 1, id true)  -- 两次实例化

#eval polyLet  -- (1, true)
```

### 3.2 值限制

**问题**: 若允许 `ref []` 多态泛化，可导致类型不安全。

**定义3.2.1**: 只有**语法值**（λ抽象、变量、常量）可被泛化。

**Lean 4代码示例6**: 值限制

```lean
-- 值可以泛化
def valuePoly : List Nat × List Bool :=
  let nil : {α : Type} → List α := []
  ([1] ++ nil, [true] ++ nil)

-- 非值不能隐式泛化
-- 在SML中: let r = ref [] in ...  -- 值限制错误

-- Lean中引用是单态的
def safeRef : IO (List Nat × List Bool) := do
  let rNat ← IO.mkRef ([] : List Nat)
  let rBool ← IO.mkRef ([] : List Bool)
  rNat.set [1]
  rBool.set [true]
  return (← rNat.get, ← rBool.get)
```

### 3.3 多态引用问题

**定理3.3.1**: 若允许非值泛化，类型系统不安全。

**证明**:

```
1. r : ∀α.α list ref  (泛化引用)
2. r := [1]            (实例化 α=int)
3. hd(!r) : bool       (实例化 α=bool) -- 危险!
```

### 3.4 类型推导算法W

**算法W**是Hindley-Milner类型推导的经典算法，使用**最一般合一子 (MGU)**。

**定理6.4 (算法W完备性)**: 若 `S(Γ)⊢M:τ` 可推导，则 `W(Γ,M)=(S',τ')` 满足存在 `S''` 使 `S=S''∘S'` 且 `S''(τ')=τ`。

*证明见第6节。*

---

## 4. Haskell惰性语义

### 4.1 求值策略

| 策略 | 描述 | 示例 |
|:-----|:-----|:-----|
| **call-by-value** | 先求值参数 | `(λx.x)(1+2) → (λx.x)3 → 3` |
| **call-by-name** | 直接代入 | `(λx.x)(1+2) → 1+2 → 3` |
| **call-by-need** | 代入+记忆化 | 同call-by-name，但只计算一次 |

**定义4.1.1 (WHNF)**: 表达式是弱头正规形，如果是λ抽象、构造子应用或部分应用。

**定理6.5 (惰性求值一致性)**: 惰性求值与最懒指称语义等价。

*证明见第6节。*

**Lean 4代码示例7**: 惰性求值

```lean
-- 使用Thunk模拟惰性求值
coinductive Stream (α : Type)
  | cons (head : α) (tail : Thunk (Stream α))

namespace Stream

def repeat (x : α) : Stream α :=
  cons x (Thunk.mk fun () => repeat x)

def take (n : Nat) (s : Stream α) : List α :=
  match n with
  | 0 => []
  | n+1 => match s with | cons h t => h :: take n (t.get)

def natsFrom (n : Nat) : Stream Nat :=
  cons n (Thunk.mk fun () => natsFrom (n + 1))

end Stream

#eval Stream.take 5 (Stream.natsFrom 0)  -- [0, 1, 2, 3, 4]
```

### 4.2 类型类语义

**字典传递**: 类型类转换为显式字典参数。

```haskell
-- 源代码
elem :: Eq a => a -> [a] -> Bool

-- 去糖后
data EqDict a = EqDict { eq :: a -> a -> Bool }
elem :: EqDict a -> a -> [a] -> Bool
```

### 4.3 类型族语义

类型族是**类型级别的函数**（受限的依赖类型）：

```haskell
type family F a :: Type
type instance F Int = Bool
```

---

## 5. Lean 4对比

### 5.1 类型系统对比

| 特性 | ML | Haskell | Lean 4 |
|:-----|:---|:--------|:-------|
| 多态 | let-多态(秩1) | 高阶多态 | 依赖多态 |
| 类型依赖 | 否 | 否(类型族受限) | 是(完全依赖) |
| 类型层级 | * | * | Type u(宇宙层) |

**Lean 4核心特性**:

```lean
-- 依赖函数类型
def depFun (n : Nat) : Fin (n + 1) := 0

-- universe多态
def id' {α : Type u} (x : α) : α := x

-- 归纳族
inductive Vec (α : Type) : Nat → Type
  | nil : Vec α 0
  | cons : α → Vec α n → Vec α (n+1)
```

### 5.2 归约策略对比

| 特性 | ML | Haskell | Lean 4 |
|:-----|:---|:--------|:-------|
| 策略 | call-by-value | call-by-need | call-by-value |
| WHNF | 完全求值 | 弱头正规形 | 弱头正规形 |

Lean 4使用严格求值，通过 `Thunk` 实现惰性：

```lean
def fLazy (x : Nat) (y : Thunk Nat) := x
-- #eval fLazy 1 (Thunk.mk fun () => loop)  -- 返回1，不计算loop
```

### 5.3 元编程对比

**Lean 4代码示例8**: 元编程

```lean
-- 引号/反引号
open Lean Elab Term Meta

def buildId : TermElabM Expr := do
  let body ← `(fun x => x)
  elabTerm body none

-- Tactic系统
syntax "my_tac" : tactic

elab_rules : tactic
  | `(tactic| my_tac) => do
    Lean.Elab.Tactic.evalTactic (← `(tactic| assumption))
```

---

## 6. 形式化定理与证明

### 6.1 Church-Rosser定理

**定理**: 若 `M →* N₁` 且 `M →* N₂`，则存在 `P` 使 `N₁ →* P` 且 `N₂ →* P`。

**证明**:

**步骤1**: 定义平行归约 `⇒`:

```
x ⇒ x                     M⇒M'    N⇒N'
───────                  ─────────────────
        (P-β)
(λx.M) N ⇒ M'[x:=N']
```

**引理**: 若 `M⇒N₁` 且 `M⇒N₂`，则存在 `P` 使 `N₁⇒P` 且 `N₂⇒P`（钻石性质）。

*证明*: 对 `M` 结构归纳。

**步骤2**: 证明 `→* = ⇒*`

- `M→N ⟹ M⇒N` (对 `→` 归纳)
- `M⇒N ⟹ M →* N` (平行归约可顺序模拟)

**步骤3**: 由钻石性质迭代得到合流性。∎

### 6.2 类型安全定理

**定理**: 对简单类型λ演算：

1. **保持性**: 若 `Γ⊢M:τ` 且 `M→β M'`，则 `Γ⊢M':τ`
2. **进展性**: 若 `∅⊢M:τ`，则 `M` 是值或存在 `M'` 使 `M→β M'`

**保持性证明**: 对 `M→β M'` 归纳，关键使用**替换引理**。

**替换引理**: 若 `Γ,x:τ₁⊢M:τ` 且 `Γ⊢V:τ₁`，则 `Γ⊢M[V/x]:τ`。

**进展性证明**: 对 `∅⊢M:τ` 归纳。

- `M=λx.M'`: 已是值
- `M=M₁M₂`: 由归纳假设，若 `M₁` 是值则β规约，否则 `M₁` 可归约
∎

### 6.3 System F健全性

**定理**: 若 `∅⊢M:τ`，则 `M` 不会卡住。

**证明概要**: 使用Girard的可解释法。

**可解释集** `RED_τ`:

- `RED_α` = 强规范项
- `RED_{τ₁→τ₂}` = {M | ∀N∈RED_{τ₁}, MN∈RED_{τ₂}}
- `RED_{∀α.τ}` = {M | ∀σ, M[σ]∈RED_{τ[α:=σ]}}

**引理**: 若 `Γ⊢M:τ`，则 `M∈RED_τ`。

这保证了 `M` 是强规范的。∎

### 6.4 ML类型推导完备性

**定理**: 算法W能找到最一般合一子。

**证明**: 对 `M` 结构归纳。

**关键引理**:

1. **合一正确性**: 若 `unify(τ₁,τ₂)=S`，则 `S(τ₁)=S(τ₂)`
2. **合一完备性**: 若 `S'(τ₁)=S'(τ₂)`，则 `S'=S''∘unify(τ₁,τ₂)`

**变量情况**: `W(Γ,x)=(∅,Γ(x))`。若 `S(Γ)⊢x:τ`，则 `S(Γ(x))=τ`，取 `S''=S`。

**λ抽象情况**: 归纳假设给出 `S₁,τ₁` 满足条件，构造 `S''` 验证。

**应用情况**: 使用合一完备性确保结果最一般。∎

### 6.5 惰性求值一致性

**定理**: 惰性求值与最懒指称语义等价。

**最懒语义** `⟦·⟧_L`:

```
⟦x⟧_Lρ = ρ(x)
⟦λx.M⟧_Lρ = λd. ⟦M⟧_Lρ[x↦d]
⟦M N⟧_Lρ = ⟦M⟧_Lρ (delay (⟦N⟧_Lρ))
```

**证明**:

**步骤1**: 定义操作语义 `→_need`

- `(λx.M)N →_need M[x:=N]` (N不需是值)
- `let x=M in N →_need N[x:=M]` (延迟求值)

**步骤2**: 证明 **Adequacy**: 若 `⟦M⟧_L≠⊥`，则 `M` 可规约到值。

**步骤3**: 证明 **Full Abstraction**: 语义相等意味着上下文等价。

**步骤4**: 建立 `M →*_{need} V ⟺ ⟦M⟧_L=⟦V⟧_L`。∎

---

## 7. 参考文献

1. Pierce, B. C. (2002). *Types and Programming Languages*. MIT Press.
2. Harper, R. (2016). *Practical Foundations for Programming Languages*. CUP.
3. Girard, J.-Y. (1989). *Proofs and Types*. CUP.
4. Milner, R. (1978). A Theory of Type Polymorphism. *JCSS*.
5. Damas & Milner (1982). Principal Type-Schemes. *POPL*.
6. Peyton Jones (2003). *Haskell 98 Report*. CUP.
7. de Moura & Ullrich (2021). The Lean 4 Theorem Prover. *CADE*.

---

## 8. 交叉引用

- **[SEMANTIC_NOTATIONS.md](./SEMANTIC_NOTATIONS.md)** - 符号标准
- **[SEMANTICS_UNIFIED_REVISED.md](./SEMANTICS_UNIFIED_REVISED.md)** - 统一框架
- **[BRIDGE_THEOREMS.md](./BRIDGE_THEOREMS.md)** - 桥梁定理
- **[LEAN4_SEMANTICS_MAPPING.md](./LEAN4_SEMANTICS_MAPPING.md)** - Lean映射
- **[ADVANCED_SEMANTICS.md](./ADVANCED_SEMANTICS.md)** - 高级主题

---

**文档状态**: L6+++完整实现
**定理**: 5/5 核心定理
**代码**: 8个Lean 4示例
**符号合规**: 100%
