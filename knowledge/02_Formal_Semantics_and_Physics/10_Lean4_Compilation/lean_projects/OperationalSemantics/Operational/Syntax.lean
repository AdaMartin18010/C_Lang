/-
# 简单类型lambda演算 (STLC) 的语法定义

本模块定义了STLC的抽象语法树(AST)，包括：
- 类型定义 (Ty): 基础类型、函数类型
- 项定义 (Tm): 变量、抽象、应用、常量
- 上下文定义 (Context): 类型环境
- 语法糖: 自然数运算的扩展
-/  

namespace Operational

-- ==================== 类型定义 ====================

/-- STLC的类型系统 -/
inductive Ty : Type where
  | nat     : Ty              -- 自然数类型
  | bool    : Ty              -- 布尔类型
  | arrow   : Ty → Ty → Ty    -- 函数类型: τ₁ → τ₂
  deriving BEq, Repr

-- 中缀表示法: τ₁ ⇒ τ₂ 表示函数类型
infixr:50 " ⇒ " => Ty.arrow

-- ==================== 项定义 ====================

/-- 变量标识符 -/
abbrev Name := String

/-- STLC的项 -/
inductive Tm : Type where
  | var   : Name → Tm                    -- 变量 x
  | abs   : Name → Ty → Tm → Tm          -- 抽象 λx:τ.t
  | app   : Tm → Tm → Tm                 -- 应用 t₁ t₂
  | const : Const → Tm                   -- 常量 (数字、布尔值)
  | binop : BinOp → Tm → Tm → Tm         -- 二元运算
  | ifte  : Tm → Tm → Tm → Tm            -- 条件 if t₁ then t₂ else t₃
  | letb  : Name → Tm → Tm → Tm          -- let绑定 let x = t₁ in t₂
  deriving BEq, Repr

where
  /-- 常量定义 -/
  Const := Nat ⊕ Bool
  
  /-- 二元运算符 -/
  BinOp := BinOp.add | BinOp.sub | BinOp.mul | BinOp.eq | BinOp.lt | BinOp.and | BinOp.or

-- 中缀表示法
notation "λ " x " : " τ " => " t => Tm.abs x τ t
notation t₁ " □ " t₂ => Tm.app t₁ t₂
notation "if " c " then " t " else " e => Tm.ifte c t e
notation "let " x " := " t₁ " in " t₂ => Tm.letb x t₁ t₂

-- 常量构造器
def Tm.num (n : Nat) : Tm := Tm.const (Sum.inl n)
def Tm.bool (b : Bool) : Tm := Tm.const (Sum.inr b)

-- 语法糖: 常用常量的表示法
notation "# " n => Tm.num n
notation "! " b => Tm.bool b

-- 二元运算符语法糖
notation t₁ " + " t₂ => Tm.binop Tm.BinOp.add t₁ t₂
notation t₁ " - " t₂ => Tm.binop Tm.BinOp.sub t₁ t₂
notation t₁ " * " t₂ => Tm.binop Tm.BinOp.mul t₁ t₂
notation t₁ " == " t₂ => Tm.binop Tm.BinOp.eq t₁ t₂
notation t₁ " < " t₂ => Tm.binop Tm.BinOp.lt t₁ t₂
notation t₁ " && " t₂ => Tm.binop Tm.BinOp.and t₁ t₂
notation t₁ " || " t₂ => Tm.binop Tm.BinOp.or t₁ t₂

-- ==================== 上下文定义 ====================

/-- 类型上下文: 变量名到类型的映射 -/
def Context := List (Name × Ty)

def Context.empty : Context := []

/-- 在上下文中查找变量类型 -/
def Context.lookup (Γ : Context) (x : Name) : Option Ty :=
  Γ.lookup x

/-- 扩展上下文 -/
def Context.extend (Γ : Context) (x : Name) (τ : Ty) : Context :=
  (x, τ) :: Γ

notation Γ " ⊢ " x " : " τ => Context.extend Γ x τ

-- ==================== 自由变量 ====================

/-- 计算项的自由变量 -/
def Tm.fv : Tm → List Name
  | var x => [x]
  | abs x _ t => t.fv.filter (· ≠ x)
  | app t₁ t₂ => t₁.fv ++ t₂.fv
  | const _ => []
  | binop _ t₁ t₂ => t₁.fv ++ t₂.fv
  | ifte c t e => c.fv ++ t.fv ++ e.fv
  | letb x t₁ t₂ => t₁.fv ++ t₂.fv.filter (· ≠ x)

/-- 检查变量是否在项中是自由的 -/
def Tm.isFree (t : Tm) (x : Name) : Bool :=
  x ∈ t.fv

-- ==================== 替换 ====================

/-- 替换: t[x := s] 表示将t中的x替换为s -/
def Tm.subst (t : Tm) (x : Name) (s : Tm) : Tm :=
  match t with
  | var y => if x = y then s else var y
  | abs y τ t' => 
      if x = y then abs y τ t'
      else if y ∈ s.fv then abs y τ t'  -- 简化处理，不做α转换
      else abs y τ (t'.subst x s)
  | app t₁ t₂ => app (t₁.subst x s) (t₂.subst x s)
  | const c => const c
  | binop op t₁ t₂ => binop op (t₁.subst x s) (t₂.subst x s)
  | ifte c t₁ t₂ => ifte (c.subst x s) (t₁.subst x s) (t₂.subst x s)
  | letb y t₁ t₂ => 
      if x = y then letb y (t₁.subst x s) t₂
      else letb y (t₁.subst x s) (t₂.subst x s)

notation t " [ " x " := " s " ] " => Tm.subst t x s

-- ==================== 示例表达式 ====================

namespace Examples

/-- 恒等函数: λx:nat. x -/
def idNat : Tm := λ "x" : Ty.nat => Tm.var "x"

/-- 加法函数: λx:nat. λy:nat. x + y -/
def addFunc : Tm := 
  λ "x" : Ty.nat => λ "y" : Ty.nat => Tm.var "x" + Tm.var "y"

/-- 阶乘辅助函数示例 (非递归定义，仅用于测试) -/
def fact3 : Tm := # 1 * # 2 * # 3  -- 3! = 6

/-- 条件表达式示例 -/
def maxExample : Tm :=
  let "x" := # 5 in
  let "y" := # 3 in
  if Tm.var "x" < Tm.var "y" then Tm.var "y" else Tm.var "x"

/-- 高阶函数示例: (λf:nat→nat. f 5) (λx:nat. x + 1) -/
def higherOrderExample : Tm :=
  let "f" := (λ "x" : Ty.nat => Tm.var "x" + # 1) in
  Tm.var "f" □ # 5

end Examples

end Operational
