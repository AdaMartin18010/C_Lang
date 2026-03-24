/-
# 霍尔逻辑验证器 - 主程序

本程序演示霍尔逻辑验证器的实现，
包括推理规则、最弱前置条件和自动验证。

## 运行方式:
```bash
lake build
lake exe hoare
```
-/  

import Hoare.Syntax
import Hoare.Rules
import Hoare.WP
import Hoare.Verification
import Tests.VerificationTest

namespace Hoare.Main

-- ==================== 霍尔逻辑演示 ====================

def demoHoareRules : IO Unit := do
  IO.println ""
  IO.println "=== Hoare Logic Rules ==="
  
  IO.println ""
  IO.println "Basic Rules:"
  IO.println "  1. Skip:      {P} skip {P}"
  IO.println "  2. Assign:    {P[x:=a]} x:=a {P}"
  IO.println "  3. Sequence:  {P} C₁ {Q} → {Q} C₂ {R} → {P} C₁;;C₂ {R}"
  IO.println "  4. If:        {P∧b} C₁ {Q} → {P∧¬b} C₂ {Q} → {P} if b then C₁ else C₂ {Q}"
  IO.println "  5. While:     {I∧b} C {I} → {I} while b do C {I∧¬b}"
  IO.println "  6. Conseq:    P'⇒P → {P} C {Q} → Q⇒Q' → {P'} C {Q'}"

-- ==================== wp演示 ====================

def demoWP : IO Unit := do
  IO.println ""
  IO.println "=== Weakest Precondition ==="
  
  IO.println ""
  IO.println "wp(skip, Q) = Q"
  IO.println "wp(x:=a, Q) = Q[x:=a]"
  IO.println "wp(C₁;;C₂, Q) = wp(C₁, wp(C₂, Q))"
  IO.println "wp(if b then C₁ else C₂, Q) = (b → wp(C₁,Q)) ∧ (¬b → wp(C₂,Q))"
  IO.println "wp(while b do C, Q) = μX.(b → wp(C,X)) ∧ (¬b → Q)"
  
  -- 计算示例
  IO.println ""
  IO.println "Example: wp(x:=5, x>0)"
  IO.println "  = (x>0)[x:=5]"
  IO.println "  = 5>0"
  IO.println "  = true"

-- ==================== 程序验证演示 ====================

def demoVerification : IO Unit := do
  IO.println ""
  IO.println "=== Program Verification Examples ==="
  
  -- 示例1: 赋值
  IO.println ""
  IO.println "Example 1: Simple Assignment"
  IO.println "  Program: x := 5"
  IO.println "  Specification: {true} x:=5 {x=5}"
  IO.println "  Proof: By assignment rule"
  
  -- 示例2: 交换
  IO.println ""
  IO.println "Example 2: Variable Swap"
  IO.println "  Program: temp := x; x := y; y := temp"
  IO.println "  Specification: {x=a ∧ y=b} swap {x=b ∧ y=a}"
  IO.println "  Proof: By sequence and assignment rules"
  
  -- 示例3: 绝对值
  IO.println ""
  IO.println "Example 3: Absolute Value"
  IO.println "  Program: if x < 0 then y := -x else y := x"
  IO.println "  Specification: {true} abs {y = |x|}"
  IO.println "  Proof: By if rule and arithmetic"
  
  -- 示例4: 阶乘
  IO.println ""
  IO.println "Example 4: Factorial"
  IO.println "  Program:"
  IO.println "    fact := 1;"
  IO.println "    while n > 0 do"
  IO.println "      fact := fact * n;"
  IO.println "      n := n - 1"
  IO.println "  Specification: {n=n₀ ∧ n₀≥0} fact {fact=n₀!}"
  IO.println "  Invariant: fact * n! = n₀! ∧ n ≥ 0"
  IO.println "  Proof: By while rule with invariant"
  
  -- 示例5: 求和
  IO.println ""
  IO.println "Example 5: Summation"
  IO.println "  Program:"
  IO.println "    i := 1; sum := 0;"
  IO.println "    while i ≤ n do"
  IO.println "      sum := sum + i;"
  IO.println "      i := i + 1"
  IO.println "  Specification: {n≥0} sum {sum = n(n+1)/2}"
  IO.println "  Invariant: sum = Σ(1..i-1) ∧ i ≤ n+1"
  IO.println "  Proof: By while rule with invariant"

-- ==================== 循环不变式演示 ====================

def demoInvariants : IO Unit := do
  IO.println ""
  IO.println "=== Loop Invariants ==="
  
  IO.println ""
  IO.println "What is a loop invariant?"
  IO.println "  A predicate I that:"
  IO.println "    1. Holds before the loop (Initiation)"
  IO.println "    2. Is preserved by the loop body (Maintenance)"
  IO.println "    3. Together with ¬b implies postcondition (Termination)"
  
  IO.println ""
  IO.println "Finding invariants:"
  IO.println "  - Look at what changes and what stays constant"
  IO.println "  - Express the relationship between variables"
  IO.println "  - Often involves the original values"
  
  IO.println ""
  IO.println "Example invariants:"
  IO.println "  - Factorial: fact * n! = n₀!"
  IO.println "  - Sum: sum = Σ(1..i-1)"
  IO.println "  - GCD: gcd(x,y) = gcd(x₀,y₀)"

-- ==================== 求值演示 ====================

def demoExecution : IO Unit := do
  IO.println ""
  IO.println "=== Program Execution ==="
  
  -- 执行示例程序
  let factorial := 
    ("fact" := # 1) ;; 
    (while # 0 < $ "n" do
      ("fact" := $ "fact" * $ "n") ;; 
      ("n" := $ "n" - # 1))
  
  let initialState := State.empty["n" ↦ 5]
  
  IO.println ""
  IO.println "Executing: Factorial(5)"
  IO.println s!"  Initial state: n = {(initialState "n")}"
  
  match Cmd.eval 1000 factorial initialState with
  | some finalState =>
    IO.println s!"  Final state: n = {(finalState "n")}, fact = {(finalState "fact")}"
    IO.println s!"  Result: 5! = {(finalState "fact")}"
  | none =>
    IO.println "  Execution failed (timeout)"
  
  -- 执行求和程序
  let sum := 
    ("i" := # 1) ;; 
    ("sum" := # 0) ;; 
    (while $ "i" ≤ $ "n" do
      ("sum" := $ "sum" + $ "i") ;; 
      ("i" := $ "i" + # 1))
  
  let initialState2 := State.empty["n" ↦ 10]
  
  IO.println ""
  IO.println "Executing: Sum(1..10)"
  IO.println s!"  Initial state: n = {(initialState2 "n")}"
  
  match Cmd.eval 1000 sum initialState2 with
  | some finalState =>
    IO.println s!"  Final state: i = {(finalState "i")}, sum = {(finalState "sum")}"
    IO.println s!"  Result: Σ(1..10) = {(finalState "sum")} = 10*11/2"
  | none =>
    IO.println "  Execution failed (timeout)"

-- ==================== 主函数 ====================

def main : IO Unit := do
  IO.println "╔══════════════════════════════════════════════════════════════╗"
  IO.println "║     Hoare Logic Verifier in Lean 4                          ║"
  IO.println "╚══════════════════════════════════════════════════════════════╝"
  
  demoHoareRules
  demoWP
  demoVerification
  demoInvariants
  demoExecution
  
  -- 运行测试
  IO.println ""
  Tests.runAllTests
  
  IO.println ""
  IO.println "╔══════════════════════════════════════════════════════════════╗"
  IO.println "║                    Demo Complete                            ║"
  IO.println "╚══════════════════════════════════════════════════════════════╝"

end Hoare.Main
