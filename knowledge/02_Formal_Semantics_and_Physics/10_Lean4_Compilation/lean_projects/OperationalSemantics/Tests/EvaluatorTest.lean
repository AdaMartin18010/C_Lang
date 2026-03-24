/-
# 求值器测试

本模块包含10+个完整的测试用例，验证求值器的正确性。
测试涵盖:
- 常量求值
- 变量查找
- 函数应用
- 二元运算
- 条件表达式
- let绑定
- 高阶函数
-/  

import Operational.Syntax
import Operational.Semantics
import Operational.Evaluator
import Operational.Properties

namespace Operational.Tests

-- ==================== 测试辅助函数 ====================

def testCase (name : String) (t : Tm) (expected : Tm) : IO Unit := do
  let result := eval t
  match result with
  | .value v =>
    if v = expected then
      IO.println s!"✓ {name}: PASS"
    else
      IO.println s!"✗ {name}: FAIL - Expected {repr expected}, got {repr v}"
  | .stuck msg =>
    IO.println s!"✗ {name}: STUCK - {msg}"
  | .timeout =>
    IO.println s!"✗ {name}: TIMEOUT"

def testType (name : String) (t : Tm) (expected : Ty) : IO Unit := do
  let result := typeCheck [] t
  match result with
  | .ok τ =>
    if τ = expected then
      IO.println s!"✓ {name} (type): PASS - {repr τ}"
    else
      IO.println s!"✗ {name} (type): FAIL - Expected {repr expected}, got {repr τ}"
  | .error e =>
    IO.println s!"✗ {name} (type): TYPE ERROR - {repr e}"

-- ==================== 测试用例 ====================

/-- 测试1: 常量求值 -/
def test1 : IO Unit :=
  testCase "Constant Evaluation" (# 42) (# 42)

/-- 测试2: 简单加法 -/
def test2 : IO Unit :=
  testCase "Simple Addition" (# 3 + # 4) (# 7)

/-- 测试3: 简单乘法 -/
def test3 : IO Unit :=
  testCase "Simple Multiplication" (# 6 * # 7) (# 42)

/-- 测试4: 复合算术表达式 -/
def test4 : IO Unit :=
  testCase "Complex Arithmetic" ((# 1 + # 2) * (# 3 + # 4)) (# 21)

/-- 测试5: 恒等函数应用 -/
def test5 : IO Unit :=
  testCase "Identity Function" 
    (Tm.app (λ "x" : Ty.nat => Tm.var "x") (# 42))
    (# 42)

/-- 测试6: 常量函数 -/
def test6 : IO Unit :=
  testCase "Constant Function"
    (Tm.app (λ "x" : Ty.nat => # 99) (# 42))
    (# 99)

/-- 测试7: 条件表达式 (真分支) -/
def test7 : IO Unit :=
  testCase "If-Then-Else (True)"
    (Tm.ifte (Tm.bool true) (# 1) (# 2))
    (# 1)

/-- 测试8: 条件表达式 (假分支) -/
def test8 : IO Unit :=
  testCase "If-Then-Else (False)"
    (Tm.ifte (Tm.bool false) (# 1) (# 2))
    (# 2)

/-- 测试9: 嵌套条件 -/
def test9 : IO Unit :=
  testCase "Nested If"
    (Tm.ifte (Tm.bool true) 
      (Tm.ifte (Tm.bool false) (# 1) (# 2))
      (# 3))
    (# 2)

/-- 测试10: let绑定 -/
def test10 : IO Unit :=
  testCase "Let Binding"
    (Tm.letb "x" (# 5) (Tm.var "x" + # 3))
    (# 8)

/-- 测试11: 嵌套let -/
def test11 : IO Unit :=
  testCase "Nested Let"
    (Tm.letb "x" (# 2) 
      (Tm.letb "y" (# 3) 
        (Tm.var "x" + Tm.var "y")))
    (# 5)

/-- 测试12: let中的函数 -/
def test12 : IO Unit :=
  testCase "Let with Function"
    (Tm.letb "f" (λ "x" : Ty.nat => Tm.var "x" + # 1)
      (Tm.var "f" □ # 41))
    (# 42)

/-- 测试13: 布尔运算 -/
def test13 : IO Unit :=
  testCase "Boolean Operations"
    (Tm.bool true && Tm.bool false)
    (Tm.bool false)

/-- 测试14: 比较运算 -/
def test14 : IO Unit :=
  testCase "Comparison"
    (# 3 < # 5)
    (Tm.bool true)

/-- 测试15: 高阶函数 -/
def test15 : IO Unit :=
  testCase "Higher-Order Function"
    (Tm.letb "apply" 
      (λ "f" : (Ty.nat ⇒ Ty.nat) => 
        λ "x" : Ty.nat => 
          Tm.var "f" □ Tm.var "x")
      ((Tm.var "apply" □ (λ "y" : Ty.nat => Tm.var "y" + # 1)) □ # 41))
    (# 42)

/-- 测试16: 类型检查 - 恒等函数 -/
def test16 : IO Unit :=
  testType "Identity Function Type"
    (λ "x" : Ty.nat => Tm.var "x")
    (Ty.nat ⇒ Ty.nat)

/-- 测试17: 类型检查 - 应用 -/
def test17 : IO Unit :=
  testType "Application Type"
    (Tm.app (λ "x" : Ty.nat => Tm.var "x") (# 42))
    Ty.nat

/-- 测试18: 类型检查 - 加法 -/
def test18 : IO Unit :=
  testType "Addition Type"
    (# 1 + # 2)
    Ty.nat

/-- 测试19: 类型检查 - 条件 -/
def test19 : IO Unit :=
  testType "If-Then-Else Type"
    (Tm.ifte (Tm.bool true) (# 1) (# 2))
    Ty.nat

/-- 测试20: 阶乘计算 (迭代展开) -/
def test20 : IO Unit :=
  testCase "Factorial (3!)"
    (# 1 * # 2 * # 3 * # 4 * # 5)
    (# 120)

-- ==================== 运行所有测试 ====================

def runAllTests : IO Unit := do
  IO.println "====================================="
  IO.println "Operational Semantics Test Suite"
  IO.println "====================================="
  IO.println ""
  
  IO.println "--- Evaluation Tests ---"
  test1; test2; test3; test4; test5
  test6; test7; test8; test9; test10
  test11; test12; test13; test14; test15
  test20
  
  IO.println ""
  IO.println "--- Type Checking Tests ---"
  test16; test17; test18; test19
  
  IO.println ""
  IO.println "====================================="
  IO.println "Test Suite Complete"
  IO.println "====================================="

end Operational.Tests
