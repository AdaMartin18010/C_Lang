/-
# 求值器实现

本模块实现了完整的STLC求值器，包括：
- 类型检查器 (Type Checker)
- 小步求值器 (Small-step Evaluator)
- 大步求值器 (Big-step Evaluator)
- 配置和状态管理
-/  

import Operational.Syntax
import Operational.Semantics

namespace Operational

-- ==================== 类型检查器 ====================

/-- 类型检查错误 -/
inductive TypeError where
  | unboundVar     : Name → TypeError
  | typeMismatch   : Ty → Ty → TypeError
  | notFunction    : Ty → TypeError
  | notBool        : Ty → TypeError
  | notNat         : Ty → TypeError
  | invalidBranches : Ty → Ty → TypeError
  deriving BEq, Repr

/-- 类型检查结果 -/
def TypeResult (α : Type) := Except TypeError α

/-- 类型检查主函数 -/
def typeCheck (Γ : Context) : Tm → TypeResult Ty
  | Tm.var x => 
    match Γ.lookup x with
    | some τ => pure τ
    | none => throw (TypeError.unboundVar x)
  
  | Tm.abs x τ t => do
    let τ' ← typeCheck (Γ.extend x τ) t
    return τ ⇒ τ'
  
  | Tm.app t₁ t₂ => do
    let τ₁ ← typeCheck Γ t₁
    let τ₂ ← typeCheck Γ t₂
    match τ₁ with
    | Ty.arrow τarg τres => 
      if τarg = τ₂ then return τres
      else throw (TypeError.typeMismatch τarg τ₂)
    | _ => throw (TypeError.notFunction τ₁)
  
  | Tm.const (Sum.inl _) => return Ty.nat
  | Tm.const (Sum.inr _) => return Ty.bool
  
  | Tm.binop op t₁ t₂ => do
    let τ₁ ← typeCheck Γ t₁
    let τ₂ ← typeCheck Γ t₂
    match op with
    | .add | .sub | .mul => 
      if τ₁ = Ty.nat && τ₂ = Ty.nat then return Ty.nat
      else throw (TypeError.notNat τ₁)
    | .eq | .lt => 
      if τ₁ = τ₂ then return Ty.bool
      else throw (TypeError.typeMismatch τ₁ τ₂)
    | .and | .or => 
      if τ₁ = Ty.bool && τ₂ = Ty.bool then return Ty.bool
      else throw (TypeError.notBool τ₁)
  
  | Tm.ifte c t e => do
    let τc ← typeCheck Γ c
    if τc ≠ Ty.bool then throw (TypeError.notBool τc)
    let τt ← typeCheck Γ t
    let τe ← typeCheck Γ e
    if τt = τe then return τt
    else throw (TypeError.invalidBranches τt τe)
  
  | Tm.letb x t₁ t₂ => do
    let τ₁ ← typeCheck Γ t₁
    typeCheck (Γ.extend x τ₁) t₂

-- 表示法: Γ ⊢ t : τ
notation Γ " ⊢ " t " : " τ => typeCheck Γ t = pure τ

-- ==================== 小步求值器 ====================

/-- 求值结果 -/
inductive EvalResult where
  | value   : Tm → EvalResult      -- 求值到值
  | stuck   : String → EvalResult  -- 卡住
  | timeout : EvalResult           -- 超时（防止无限循环）
  deriving BEq, Repr

/-- 单步求值 -/
def evalStep (t : Tm) : EvalResult :=
  match t with
  | Tm.const _ => .value t
  | Tm.abs _ _ _ => .value t
  
  -- β规约: (λx.t) v → t[x:=v]
  | Tm.app (Tm.abs x τ t) v =>
    if v.isValue then .value (t.subst x v)
    else match evalStep v with
         | .value v' => .value (Tm.app (Tm.abs x τ t) v')
         | r => r
  
  -- E-App1: 先归约函数
  | Tm.app t₁ t₂ =>
    if t₁.isValue then
      if t₂.isValue then .stuck "Cannot apply non-function"
      else match evalStep t₂ with
           | .value t₂' => .value (Tm.app t₁ t₂')
           | r => r
    else match evalStep t₁ with
         | .value t₁' => .value (Tm.app t₁' t₂)
         | r => r
  
  -- 二元运算
  | Tm.binop op t₁ t₂ =>
    match t₁, t₂ with
    | Tm.const (Sum.inl n₁), Tm.const (Sum.inl n₂) =>
      match op with
      | .add => .value (Tm.num (n₁ + n₂))
      | .sub => .value (Tm.num (n₁ - n₂))
      | .mul => .value (Tm.num (n₁ * n₂))
      | .eq  => .value (Tm.bool (n₁ = n₂))
      | .lt  => .value (Tm.bool (n₁ < n₂))
      | _    => .stuck "Invalid operation for naturals"
    | Tm.const (Sum.inr b₁), Tm.const (Sum.inr b₂) =>
      match op with
      | .and => .value (Tm.bool (b₁ && b₂))
      | .or  => .value (Tm.bool (b₁ || b₂))
      | .eq  => .value (Tm.bool (b₁ = b₂))
      | _    => .stuck "Invalid operation for booleans"
    | _, _ =>
      if t₁.isValue then
        match evalStep t₂ with
        | .value t₂' => .value (Tm.binop op t₁ t₂')
        | r => r
      else
        match evalStep t₁ with
        | .value t₁' => .value (Tm.binop op t₁' t₂)
        | r => r
  
  -- 条件表达式
  | Tm.ifte c t e =>
    match c with
    | Tm.const (Sum.inr true) => .value t
    | Tm.const (Sum.inr false) => .value e
    | _ => match evalStep c with
           | .value c' => .value (Tm.ifte c' t e)
           | r => r
  
  -- let绑定
  | Tm.letb x t₁ t₂ =>
    if t₁.isValue then .value (t₂.subst x t₁)
    else match evalStep t₁ with
         | .value t₁' => .value (Tm.letb x t₁' t₂)
         | r => r

/-- 多步求值（带步数限制） -/
def evalN (fuel : Nat) (t : Tm) : EvalResult :=
  match fuel with
  | 0 => .timeout
  | fuel' + 1 =>
    if t.isValue then .value t
    else match evalStep t with
         | .value t' => evalN fuel' t'
         | r => r

/-- 默认燃料限制 -/
def eval (t : Tm) : EvalResult := evalN 1000 t

-- ==================== 大步求值器 ====================

/-- 大步求值（递归实现） -/
def evalBig (fuel : Nat) (t : Tm) : EvalResult :=
  if fuel = 0 then .timeout else
  match t with
  | Tm.const _ => .value t
  | Tm.abs _ _ _ => .value t
  
  | Tm.app t₁ t₂ => do
    let v₁ ← evalBig (fuel - 1) t₁
    let v₂ ← evalBig (fuel - 1) t₂
    match v₁ with
    | Tm.abs x _ t => evalBig (fuel - 1) (t.subst x v₂)
    | _ => .stuck "Cannot apply non-function"
  
  | Tm.binop op t₁ t₂ => do
    let v₁ ← evalBig (fuel - 1) t₁
    let v₂ ← evalBig (fuel - 1) t₂
    match v₁, v₂ with
    | Tm.const (Sum.inl n₁), Tm.const (Sum.inl n₂) =>
      match op with
      | .add => .value (Tm.num (n₁ + n₂))
      | .sub => .value (Tm.num (n₁ - n₂))
      | .mul => .value (Tm.num (n₁ * n₂))
      | .eq  => .value (Tm.bool (n₁ = n₂))
      | .lt  => .value (Tm.bool (n₁ < n₂))
      | _    => .stuck "Invalid operation"
    | Tm.const (Sum.inr b₁), Tm.const (Sum.inr b₂) =>
      match op with
      | .and => .value (Tm.bool (b₁ && b₂))
      | .or  => .value (Tm.bool (b₁ || b₂))
      | .eq  => .value (Tm.bool (b₁ = b₂))
      | _    => .stuck "Invalid operation"
    | _, _ => .stuck "Invalid operands"
  
  | Tm.ifte c t e => do
    let vc ← evalBig (fuel - 1) c
    match vc with
    | Tm.const (Sum.inr true) => evalBig (fuel - 1) t
    | Tm.const (Sum.inr false) => evalBig (fuel - 1) e
    | _ => .stuck "Condition not a boolean"
  
  | Tm.letb x t₁ t₂ => do
    let v₁ ← evalBig (fuel - 1) t₁
    evalBig (fuel - 1) (t₂.subst x v₁)

def evalBigStep (t : Tm) : EvalResult := evalBig 1000 t

-- ==================== 求值跟踪 ====================

/-- 生成求值步骤序列 -/
def traceEval (fuel : Nat) (t : Tm) : List Tm × Option Tm :=
  let rec loop (steps : List Tm) (fuel : Nat) (t : Tm) :=
    match fuel with
    | 0 => (steps, none)
    | fuel' + 1 =>
      if t.isValue then (steps, some t)
      else match evalStep t with
           | .value t' => loop (steps ++ [t']) fuel' t'
           | _ => (steps, none)
  loop [t] fuel t

def getEvalTrace (t : Tm) : List Tm × Option Tm := traceEval 100 t

-- ==================== 辅助函数 ====================

/-- 显示求值结果 -/
def showEvalResult (r : EvalResult) : String :=
  match r with
  | .value v => s!"Value: {repr v}"
  | .stuck msg => s!"Stuck: {msg}"
  | .timeout => "Timeout"

/-- 从结果中提取值 -/
def EvalResult.getValue? (r : EvalResult) : Option Tm :=
  match r with
  | .value v => some v
  | _ => none

/-- 检查求值是否成功 -/
def EvalResult.isOk (r : EvalResult) : Bool :=
  match r with
  | .value _ => true
  | _ => false

end Operational
