# MetaM单子架构：元编程的数学基础与实现

> **层级**: L6+
> **引用**: de Moura & Ullrich (2021), Christiansen (2019), Moggi (1991)

---

## 目录

- [MetaM单子架构：元编程的数学基础与实现](#metam单子架构元编程的数学基础与实现)
  - [目录](#目录)
  - [【思维导图·MetaM架构】](#思维导图metam架构)
  - [【科学·MetaM的数学结构】](#科学metam的数学结构)
    - [1. 单子定义](#1-单子定义)
    - [2. 单子律](#2-单子律)
    - [3. 单子变换子](#3-单子变换子)
  - [【工程·MetaM实现】](#工程metam实现)
    - [1. 状态管理](#1-状态管理)
    - [2. 元变量管理](#2-元变量管理)
    - [3. 类型检查与统一](#3-类型检查与统一)
  - [【定理·MetaM正确性】](#定理metam正确性)
    - [类型安全定理](#类型安全定理)
  - [【可视化·单子栈】](#可视化单子栈)
  - [参考](#参考)

## 【思维导图·MetaM架构】

```
MetaM
├── 单子结构
│   ├── State (状态)
│   ├── Reader (环境)
│   ├── Except (异常)
│   └── IO (副作用)
├── 核心功能
│   ├── 类型检查
│   ├── 统一
│   └── 元变量管理
└── 扩展功能
    ├── 战术(Tactic)
    ├── 宏(Macro)
    └── 属性(Attribute)
```

---

## 【科学·MetaM的数学结构】

### 1. 单子定义

**定义**: MetaM是一个单子栈

```
MetaM α = StateT Meta.State (
          ReaderT Meta.Context (
          ExceptT Exception IO)) α
```

**展开**:

```
MetaM α = Meta.State → Meta.Context →
          IO (Either Exception (α × Meta.State))
```

### 2. 单子律

**左单位元**:

```
return a >>= f ≡ f a
```

**右单位元**:

```
m >>= return ≡ m
```

**结合律**:

```
(m >>= f) >>= g ≡ m >>= (λx. f x >>= g)
```

**定理**: MetaM满足单子律。

### 3. 单子变换子

**StateT**:

```
StateT S M A = S → M (A × S)
```

**ReaderT**:

```
ReaderT R M A = R → M A
```

**ExceptT**:

```
ExceptT E M A = M (E ⊕ A)
```

---

## 【工程·MetaM实现】

### 1. 状态管理

**Meta.State**:

```lean
structure State where
  mctx   : MetavarContext   -- 元变量上下文
  cache  : Cache            -- 类型检查缓存
  levelAssign? : Bool       -- 级别分配模式
```

**操作**:

```lean
def getMCtx : MetaM MetavarContext := ...
def setMCtx (mctx : MetavarContext) : MetaM Unit := ...
def modifyMCtx (f : MetavarContext → MetavarContext) : MetaM Unit := ...
```

### 2. 元变量管理

**创建元变量**:

```lean
def mkFreshExprMVar (type? : Option Expr)
                    (kind : MetavarKind := .natural)
                    (userName : Name := Name.anonymous)
                    : MetaM Expr := do
  let mvarId ← mkFreshMVarId
  let type ← match type? with
    | some type => pure type
    | none => mkFreshTypeMVar
  modifyMCtx fun mctx => mctx.addExprMVar mvarId type kind userName
  return mkMVar mvarId
```

**元变量赋值**:

```lean
def assign (mvarId : MVarId) (val : Expr) : MetaM Bool := do
  checkAssignment mvarId val
  modifyMCtx fun mctx => mctx.assignExpr mvarId val
  return true
```

### 3. 类型检查与统一

**类型推断**:

```lean
def inferType (e : Expr) : MetaM Expr := do
  match e with
  | Expr.const declName _ =>
    return (← getConstInfo declName).type
  | Expr.fvar fvarId =>
    return (← getLocalDecl fvarId).type
  | Expr.app f a =>
    let fType ← inferType f
    let (paramType, resultType) ← forallMetaTelescope fType
    check a paramType
    return resultType
  | ...
```

**统一**:

```lean
def isDefEq (t₁ t₂ : Expr) : MetaM Bool := do
  if t₁ == t₂ then return true
  let t₁ ← whnf t₁
  let t₂ ← whnf t₂
  match t₁, t₂ with
  | Expr.mvar mvarId₁ _, _ => isDefEqMVar mvarId₁ t₂
  | _, Expr.mvar mvarId₂ _ => isDefEqMVar mvarId₂ t₁
  | Expr.const n₁ _, Expr.const n₂ _ => return n₁ == n₂
  | Expr.app f₁ a₁, Expr.app f₂ a₂ =>
    isDefEq f₁ f₂ <&&> isDefEq a₁ a₂
  | ...
```

---

## 【定理·MetaM正确性】

### 类型安全定理

**定理**: MetaM操作保持类型正确性。

**证明**:

```
对MetaM操作归纳:

基本情况:
- inferType: 根据Expr结构返回正确类型
- whnf: 归约保持类型（由β规约定理）

归纳步骤:
- isDefEq: 统一保持等价性
- assign: 赋值保持一致性

结论: MetaM操作类型安全 ∎
```

---

## 【可视化·单子栈】

```
MetaM α
  = StateT Meta.State (
    ReaderT Meta.Context (
      ExceptT Exception (
        IO α)))

展开:
  IO {
    handle: Exception {
      read: Meta.Context {
        state: Meta.State {
          result: α
        }
      }
    }
  }
```

---

## 参考

- Moggi, E. "Notions of Computation and Monads" (1991)
- de Moura, L. & Ullrich, S. "The Lean 4 Theorem Prover" (2021)
- Christiansen, D. "Practical Reflection and Metaprogramming" (2019)
