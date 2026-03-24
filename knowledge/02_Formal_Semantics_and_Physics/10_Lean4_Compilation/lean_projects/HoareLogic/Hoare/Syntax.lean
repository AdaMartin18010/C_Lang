/-
# 霍尔逻辑 - 命令式语言语法

本模块定义了命令式编程语言的抽象语法，包括：
- 算术表达式
- 布尔表达式
- 命令（赋值、序列、条件、循环）
- 程序状态
-/  

namespace Hoare

-- ==================== 变量和状态 ====================

/-- 变量名 -/
abbrev Var := String

/-- 整数值 -/
abbrev Val := Int

/-- 程序状态: 变量到值的映射 -/
def State := Var → Val

def State.empty : State := λ _ => 0

def State.get (s : State) (x : Var) : Val := s x

def State.set (s : State) (x : Var) (v : Val) : State :=
  λ y => if x = y then v else s y

-- 表示法: s[x ↦ v]
notation s "[" x " ↦ " v "]" => State.set s x v

-- ==================== 算术表达式 ====================

/-- 算术表达式 -/
inductive AExp : Type where
  | num   : Int → AExp
  | var   : Var → AExp
  | add   : AExp → AExp → AExp
  | sub   : AExp → AExp → AExp
  | mul   : AExp → AExp → AExp
  | div   : AExp → AExp → AExp
  deriving BEq, Repr

-- 表示法
notation "# " n => AExp.num n
notation "$ " x => AExp.var x
notation a₁ " + " a₂ => AExp.add a₁ a₂
notation a₁ " - " a₂ => AExp.sub a₁ a₂
notation a₁ " * " a₂ => AExp.mul a₁ a₂
notation a₁ " / " a₂ => AExp.div a₁ a₂

/-- 算术表达式求值 -/
def AExp.eval (a : AExp) (s : State) : Val :=
  match a with
  | num n => n
  | var x => s x
  | add a₁ a₂ => a₁.eval s + a₂.eval s
  | sub a₁ a₂ => a₁.eval s - a₂.eval s
  | mul a₁ a₂ => a₁.eval s * a₂.eval s
  | div a₁ a₂ => 
    let v₂ := a₂.eval s
    if v₂ = 0 then 0 else a₁.eval s / v₂

-- 表示法: ⟦a⟧s
notation "⟦" a "⟧" s => AExp.eval a s

-- ==================== 布尔表达式 ====================

/-- 布尔表达式 -/
inductive BExp : Type where
  | bool  : Bool → BExp
  | not   : BExp → BExp
  | and   : BExp → BExp → BExp
  | or    : BExp → BExp → BExp
  | eq    : AExp → AExp → BExp
  | lt    : AExp → AExp → BExp
  | le    : AExp → AExp → BExp
  deriving BEq, Repr

-- 表示法
notation "! " b => BExp.bool b
notation "¬ " b => BExp.not b
notation b₁ " ∧ " b₂ => BExp.and b₁ b₂
notation b₁ " ∨ " b₂ => BExp.or b₁ b₂
notation a₁ " == " a₂ => BExp.eq a₁ a₂
notation a₁ " < " a₂ => BExp.lt a₁ a₂
notation a₁ " ≤ " a₂ => BExp.le a₁ a₂

/-- 布尔表达式求值 -/
def BExp.eval (b : BExp) (s : State) : Bool :=
  match b with
  | bool bv => bv
  | not b => ¬(b.eval s)
  | and b₁ b₂ => b₁.eval s && b₂.eval s
  | or b₁ b₂ => b₁.eval s || b₂.eval s
  | eq a₁ a₂ => a₁.eval s = a₂.eval s
  | lt a₁ a₂ => a₁.eval s < a₂.eval s
  | le a₁ a₂ => a₁.eval s ≤ a₂.eval s

-- 表示法: ⟦b⟧s
notation "⟦" b "⟧" s => BExp.eval b s

-- ==================== 命令 ====================

/-- 命令式语言命令 -/
inductive Cmd : Type where
  | skip    : Cmd
  | assign  : Var → AExp → Cmd
  | seq     : Cmd → Cmd → Cmd
  | ifthen  : BExp → Cmd → Cmd → Cmd
  | while   : BExp → Cmd → Cmd
  deriving BEq, Repr

-- 表示法
notation "skip" => Cmd.skip
notation x " := " a => Cmd.assign x a
notation c₁ " ;; " c₂ => Cmd.seq c₁ c₂
notation "if " b " then " c₁ " else " c₂ => Cmd.ifthen b c₁ c₂
notation "while " b " do " c => Cmd.while b c

-- ==================== 命令的操作语义 (大步) ====================

/-- 命令求值关系 ⟨c, s⟩ ⇓ s' -/
inductive BigStep : Cmd → State → State → Prop where
  | skip    : ∀ {s}, BigStep skip s s
  | assign  : ∀ {x a s}, 
              BigStep (x := a) s (s[x ↦ ⟦a⟧s])
  | seq     : ∀ {c₁ c₂ s₁ s₂ s₃}, 
              BigStep c₁ s₁ s₂ → 
              BigStep c₂ s₂ s₃ → 
              BigStep (c₁ ;; c₂) s₁ s₃
  | ifTrue  : ∀ {b c₁ c₂ s s'}, 
              ⟦b⟧s = true → 
              BigStep c₁ s s' → 
              BigStep (if b then c₁ else c₂) s s'
  | ifFalse : ∀ {b c₁ c₂ s s'}, 
              ⟦b⟧s = false → 
              BigStep c₂ s s' → 
              BigStep (if b then c₁ else c₂) s s'
  | whileFalse : ∀ {b c s}, 
                 ⟦b⟧s = false → 
                 BigStep (while b do c) s s
  | whileTrue  : ∀ {b c s₁ s₂ s₃}, 
                 ⟦b⟧s₁ = true → 
                 BigStep c s₁ s₂ → 
                 BigStep (while b do c) s₂ s₃ → 
                 BigStep (while b do c) s₁ s₃

-- 表示法: ⟨c, s⟩ ⇓ s'
notation "⟨" c "," s "⟩ ⇓ " s' => BigStep c s s'

-- ==================== 命令求值函数 ====================

def Cmd.eval (fuel : Nat) (c : Cmd) (s : State) : Option State :=
  match fuel with
  | 0 => none
  | fuel' + 1 =>
    match c with
    | skip => some s
    | assign x a => some (s[x ↦ a.eval s])
    | seq c₁ c₂ =>
      match Cmd.eval fuel' c₁ s with
      | some s' => Cmd.eval fuel' c₂ s'
      | none => none
    | ifthen b c₁ c₂ =>
      if b.eval s then Cmd.eval fuel' c₁ s
      else Cmd.eval fuel' c₂ s
    | while b c =>
      if b.eval s then
        match Cmd.eval fuel' c s with
        | some s' => Cmd.eval fuel' (while b do c) s'
        | none => none
      else some s

-- ==================== 示例程序 ====================

namespace Examples

/-- 示例1: 简单赋值 -/
def ex1 : Cmd := "x" := # 5

/-- 示例2: 序列 -/
def ex2 : Cmd := 
  ("x" := # 5) ;; 
  ("y" := $ "x" + # 3)

/-- 示例3: 条件 -/
def ex3 : Cmd :=
  if $ "x" < # 0 then
    "y" := # 0 - $ "x"
  else
    "y" := $ "x"

/-- 示例4: While循环 - 递减 -/
def ex4 : Cmd :=
  while # 0 < $ "x" do
    "x" := $ "x" - # 1

/-- 示例5: 阶乘计算 -/
def factorial (n : Int) : Cmd :=
  ("n" := # n) ;;
  ("fact" := # 1) ;;
  (while # 0 < $ "n" do
    ("fact" := $ "fact" * $ "n") ;;
    ("n" := $ "n" - # 1))

/-- 示例6: 最大公约数 (简化版) -/
def gcd : Cmd :=
  while ¬($ "x" == $ "y") do
    if $ "x" < $ "y" then
      "y" := $ "y" - $ "x"
    else
      "x" := $ "x" - $ "y"

/-- 示例7: 求和 (1到n) -/
def sumToN : Cmd :=
  ("i" := # 1) ;;
  ("sum" := # 0) ;;
  (while $ "i" ≤ $ "n" do
    ("sum" := $ "sum" + $ "i") ;;
    ("i" := $ "i" + # 1))

end Examples

end Hoare
