/-
# 操作语义解释器 - 主程序

本程序演示STLC(简单类型lambda演算)的操作语义实现，
包括小步/大步语义、类型检查和求值器。

## 运行方式:
```bash
lake build
lake exe operational
```
-/  

import Operational.Syntax
import Operational.Semantics
import Operational.Evaluator
import Operational.Properties
import Tests.EvaluatorTest

namespace Operational.Main

-- ==================== 示例演示 ====================

def demoBasic : IO Unit := do
  IO.println ""
  IO.println "=== Basic Examples ==="
  
  -- 示例1: 简单算术
  let e1 := # 2 + # 3 * # 4  -- 2 + (3 * 4) = 14
  IO.println s!"Expression: 2 + 3 * 4"
  IO.println s!"Result: {showEvalResult (eval e1)}"
  
  -- 示例2: 恒等函数
  let e2 := (λ "x" : Ty.nat => Tm.var "x") □ # 42
  IO.println s!"\nExpression: (λx. x) 42"
  IO.println s!"Result: {showEvalResult (eval e2)}"
  
  -- 示例3: 条件表达式
  let e3 := if !true then # 1 else # 2
  IO.println s!"\nExpression: if true then 1 else 2"
  IO.println s!"Result: {showEvalResult (eval e3)}"

def demoFunctions : IO Unit := do
  IO.println ""
  IO.println "=== Function Examples ==="
  
  -- 高阶函数示例
  let applyTwice := 
    λ "f" : (Ty.nat ⇒ Ty.nat) =>
      λ "x" : Ty.nat =>
        Tm.var "f" □ (Tm.var "f" □ Tm.var "x")
  
  let addOne := λ "x" : Ty.nat => Tm.var "x" + # 1
  
  let program := (Tm.app (Tm.app applyTwice addOne) (# 0))
  
  IO.println "Expression: applyTwice (λx. x + 1) 0"
  IO.println "Expected: 2"
  IO.println s!"Result: {showEvalResult (eval program)}"

def demoLetBindings : IO Unit := do
  IO.println ""
  IO.println "=== Let Binding Examples ==="
  
  -- let x = 5 in let y = 3 in x + y
  let e := 
    let "x" := # 5 in
    let "y" := # 3 in
    Tm.var "x" + Tm.var "y"
  
  IO.println "Expression: let x = 5 in let y = 3 in x + y"
  IO.println s!"Result: {showEvalResult (eval e)}"

def demoTypeChecking : IO Unit := do
  IO.println ""
  IO.println "=== Type Checking Examples ==="
  
  -- 正确的类型
  let e1 := λ "x" : Ty.nat => Tm.var "x" + # 1
  match typeCheck [] e1 with
  | .ok τ => IO.println s!"Type of (λx:nat. x + 1): {repr τ}"
  | .error e => IO.println s!"Type error: {repr e}"
  
  -- 错误的类型（应用非函数）
  let e2 := # 42 □ # 1
  match typeCheck [] e2 with
  | .ok τ => IO.println s!"Type of (42 1): {repr τ}"
  | .error e => IO.println s!"Type error (expected): {repr e}"

def demoEvaluationTrace : IO Unit := do
  IO.println ""
  IO.println "=== Evaluation Trace ==="
  
  let e := (# 1 + # 2) * (# 3 + # 4)
  let (trace, result) := getEvalTrace e
  
  IO.println s!"Expression: (1 + 2) * (3 + 4)"
  IO.println "Evaluation steps:"
  for step in trace do
    IO.println s!"  → {repr step}"
  match result with
  | some v => IO.println s!"Final value: {repr v}"
  | none => IO.println "Evaluation did not complete"

-- ==================== 主函数 ====================

def main : IO Unit := do
  IO.println "╔══════════════════════════════════════════════════════════════╗"
  IO.println "║     STLC Operational Semantics Interpreter in Lean 4        ║"
  IO.println "╚══════════════════════════════════════════════════════════════╝"
  
  demoBasic
  demoFunctions
  demoLetBindings
  demoTypeChecking
  demoEvaluationTrace
  
  -- 运行完整测试套件
  IO.println ""
  Tests.runAllTests
  
  IO.println ""
  IO.println "╔══════════════════════════════════════════════════════════════╗"
  IO.println "║                    Demo Complete                            ║"
  IO.println "╚══════════════════════════════════════════════════════════════╝"

end Operational.Main
