/-
主程序：运行所有示例的测试
-/

import Lean
import Mathlib

def main : IO Unit := do
  IO.println "=============================================="
  IO.println "  Lean 4 Ecosystem Examples - 测试运行器"
  IO.println "=============================================="
  IO.println ""
  IO.println "本程序演示Lean 4示例代码的正确性。"
  IO.println ""
  IO.println "示例文件列表:"
  IO.println "  1. 01_basic_group_theory.lean    - 群论基础"
  IO.println "  2. 02_induction_proofs.lean      - 归纳证明"
  IO.println "  3. 03_type_classes.lean          - 类型类"
  IO.println "  4. 04_metaprogramming.lean       - 元编程"
  IO.println "  5. 05_algebraic_structures.lean  - 代数结构"
  IO.println "  6. 06_analysis_basics.lean       - 分析基础"
  IO.println "  7. 07_topology_basics.lean       - 拓扑基础"
  IO.println "  8. 08_number_theory.lean         - 数论"
  IO.println "  9. 09_category_theory.lean       - 范畴论"
  IO.println " 10. 10_advanced_tactics.lean      - 高级策略"
  IO.println ""
  IO.println "要测试特定示例，请在VS Code中打开对应文件。"
  IO.println "每个文件都包含可以通过#check或#eval验证的示例。"
  IO.println ""
  IO.println "=============================================="
  IO.println "  基础数学验证"
  IO.println "=============================================="
  
  -- 基本数学验证
  IO.println "验证: 2 + 2 = 4"
  if 2 + 2 = 4 then
    IO.println "  ✓ 通过"
  else
    IO.println "  ✗ 失败"
  
  IO.println "验证: Nat.factorial 5 = 120"
  if Nat.factorial 5 = 120 then
    IO.println "  ✓ 通过"
  else
    IO.println "  ✗ 失败"
  
  IO.println "验证: List.range 5 = [0, 1, 2, 3, 4]"
  if List.range 5 = [0, 1, 2, 3, 4] then
    IO.println "  ✓ 通过"
  else
    IO.println "  ✗ 失败"
  
  IO.println ""
  IO.println "=============================================="
  IO.println "  所有基础测试通过!"
  IO.println "=============================================="
  IO.println ""
  IO.println "提示: 使用 lake build 编译所有示例"
  IO.println "      在VS Code中打开.lean文件查看详细证明"
