/-
示例4: 元编程
展示宏、精化和策略编写
-/

import Lean

namespace MetaProgrammingExamples

open Lean Lean.Meta Lean.Elab Lean.Elab.Tactic

/- ========== 宏示例 ========== -/

-- 定义unless语法
macro "unless" cond:term "do" body:term : term =>
  `(if ¬ $cond then $body else ())

-- 使用unless
#eval unless (1 > 2) do
  IO.println "条件为假，执行此代码"

-- 重复表达式宏
syntax "sum!" term,+ : term

macro_rules
  | `(sum! $x) => return x
  | `(sum! $x, $xs,*) => `($x + sum! $xs,*)

#eval sum! 1, 2, 3, 4, 5  -- 15

/- ========== 自定义策略 ========== -/

def myTac : TacticM Unit := do
  -- 获取当前目标
  let goals ← getGoals
  match goals with
  | [] => return
  | g :: _ =>
      withMVarContext g do
        let target ← getType'
        logInfo s!"当前目标: {target}"
        -- 尝试不同的策略
        evalTactic (← `(tactic| try simp))

elab "my_tac" : tactic => myTac

-- 测试策略
example (p q : Prop) (hp : p) (hq : q) : p ∧ q := by
  constructor
  my_tac  -- 记录目标并尝试simp
  assumption

/- ========== 简单自动化策略 ========== -/

def autoSolve : TacticM Unit := do
  let tactics := #[
    (← `(tactic| rfl)),
    (← `(tactic| simp)),
    (← `(tactic| assumption)),
    (← `(tactic| tauto))
  ]
  
  for tac in tactics do
    try
      evalTactic tac
      return
    catch _ =>
      continue
  
  throwError "autoSolve无法解决此目标"

elab "auto_solve" : tactic => autoSolve

-- 测试
example : 1 + 1 = 2 := by auto_solve
example (p : Prop) (h : p) : p := by auto_solve
example (p q : Prop) (hp : p) (hq : q) : p ∧ q := by
  constructor
  auto_solve
  auto_solve

/- ========== 使用MetaM ========== -/

def getGoalVars : MetaM (List Name) := do
  let lctx ← getLCtx
  return lctx.decls.toList.filterMap fun decl? =>
    match decl? with
    | some decl => some decl.userName
    | none => none

-- 在策略中使用
def printVars : TacticM Unit := do
  let vars ← liftMetaM getGoalVars
  logInfo s!"局部变量: {vars}"

elab "print_vars" : tactic => printVars

example (n m : Nat) (h : n < m) : True := by
  print_vars  -- 显示n, m, h
  trivial

end MetaProgrammingExamples
