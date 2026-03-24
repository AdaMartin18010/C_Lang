/-
# 指称语义求值器 - 主程序

本程序演示STLC的指称语义实现，
包括域论、语义函数、连续性证明和不动点理论。

## 运行方式:
```bash
lake build
lake exe denotational
```
-/  

import Denotational.Domain
import Denotational.Meaning
import Denotational.Continuity
import Denotational.FixedPoint
import Tests.MeaningTest

namespace Denotational.Main

-- ==================== 域论演示 ====================

def demoDomains : IO Unit := do
  IO.println ""
  IO.println "=== Domain Theory Examples ==="
  
  -- 提升域示例
  let v1 : ℕ⊥ := Lift.lift 42
  let v2 : ℕ⊥ := ⊥
  
  IO.println s!"Lifted value: {repr v1}"
  IO.println s!"Bottom: {repr v2}"
  IO.println s!"v2 ⊑ v1: true (by definition of Lift.le)"

-- ==================== 语义函数演示 ====================

def demoSemantics : IO Unit := do
  IO.println ""
  IO.println "=== Denotational Semantics Examples ==="
  
  -- 示例1: 常量
  let e1 := # 42
  IO.println s!"\n⟦{repr e1}⟧ = {repr (denote Env.empty e1 .nat)}"
  
  -- 示例2: 加法
  let e2 := # 2 + # 3
  IO.println s!"⟦{repr e2}⟧ = {repr (denote Env.empty e2 .nat)}"
  
  -- 示例3: 函数
  let e3 := λ "x" : .nat => Tm.var "x" + # 1
  IO.println s!"\n⟦{repr e3}⟧ = <continuous function>"
  
  -- 示例4: 应用
  let e4 := (λ "x" : .nat => Tm.var "x" + # 1) □ # 41
  IO.println s!"⟦{repr e4}⟧ = {repr (denote Env.empty e4 .nat)}"
  
  -- 示例5: 条件
  let e5 := if !true then # 1 else # 2
  IO.println s!"\n⟦{repr e5}⟧ = {repr (denote Env.empty e5 .nat)}"

-- ==================== 连续性演示 ====================

def demoContinuity : IO Unit := do
  IO.println ""
  IO.println "=== Continuity Examples ==="
  
  IO.println "Proven continuous functions:"
  IO.println "  1. Identity: λx. x"
  IO.println "  2. Constant: λx. c"
  IO.println "  3. Projection: fst, snd"
  IO.println "  4. Application: apply"
  IO.println "  5. Composition: g ∘ f (when both continuous)"
  IO.println "  6. Pairing: λx. (f x, g x)"
  IO.println "  7. Currying and uncurrying"
  
  -- 演示连续性
  let c : Nat → ℕ⊥ := λ n => Lift.lift n
  IO.println s!"\nChain: c(n) = ↑n"
  IO.println "This is a chain: c(0) ⊑ c(1) ⊑ c(2) ⊑ ..."

-- ==================== 不动点演示 ====================

def demoFixedPoint : IO Unit := do
  IO.println ""
  IO.println "=== Fixed Point Theory ==="
  
  IO.println "Key theorems:"
  IO.println "  1. Knaster-Tarski: fix f = ⨆{fⁿ(⊥) | n ∈ ℕ}"
  IO.println "  2. fix f is a fixed point: f(fix f) = fix f"
  IO.println "  3. fix f is the least fixed point"
  IO.println "  4. Scott Induction principle"
  
  -- 阶乘示例
  IO.println "\nFactorial defined as fixed point:"
  IO.println "  fact = fix (λf. λn. if n=0 then 1 else n * f(n-1))"
  
  -- 展示迭代
  IO.println "\nIterative approximation:"
  let f : ℕ⊥ → ℕ⊥ := λ x => 
    match x with
    | .bot => ⊥
    | .lift n => if n = 0 then .lift 1 else .lift (n + 1)
  
  IO.println "  f⁰(⊥) = ⊥"
  IO.println "  f¹(⊥) = f(⊥) = ⊥"
  IO.println "  f²(⊥) = f(f(⊥)) = ..."
  IO.println "  ..."
  IO.println "  fix f = ⨆{fⁿ(⊥)}"

-- ==================== 计算示例 ====================

def demoCalculations : IO Unit := do
  IO.println ""
  IO.println "=== Computation Examples ==="
  
  -- 阶乘计算
  IO.println "\nFactorial computation:"
  IO.println "  0! = 1"
  IO.println "  1! = 1"
  IO.println "  2! = 2"
  IO.println "  3! = 6"
  IO.println "  5! = 120"
  
  -- 验证
  let fact3 := # 1 * # 2 * # 3
  IO.println s!"\nVerifying 3! using direct computation:"
  IO.println s!"  ⟦1 * 2 * 3⟧ = {repr (denote Env.empty fact3 .nat)}"

-- ==================== 主函数 ====================

def main : IO Unit := do
  IO.println "╔══════════════════════════════════════════════════════════════╗"
  IO.println "║     STLC Denotational Semantics in Lean 4                   ║"
  IO.println "╚══════════════════════════════════════════════════════════════╝"
  
  demoDomains
  demoSemantics
  demoContinuity
  demoFixedPoint
  demoCalculations
  
  -- 运行测试
  IO.println ""
  Tests.runAllTests
  
  IO.println ""
  IO.println "╔══════════════════════════════════════════════════════════════╗"
  IO.println "║                    Demo Complete                            ║"
  IO.println "╚══════════════════════════════════════════════════════════════╝"

end Denotational.Main
