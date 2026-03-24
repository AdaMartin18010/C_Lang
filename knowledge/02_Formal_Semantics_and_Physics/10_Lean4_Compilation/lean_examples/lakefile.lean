import Lake
open Lake DSL

package LeanExamples where
  version := v!"0.1.0"
  keywords := #["lean4", "examples", "mathlib"]
  description := "Lean 4生态系统示例代码集合"

require mathlib from git
  "https://github.com/leanprover-community/mathlib4" @ "master"

require aesop from git
  "https://github.com/JLimperg/aesop" @ "master"

require std from git
  "https://github.com/leanprover/std4" @ "main"

@[default_target]
lean_lib LeanExamples where
  srcDir := "."
  roots := #[`GroupTheoryExample, `InductionExamples, `TypeClassExamples, 
              `MetaProgrammingExamples, `AlgebraicStructures, 
              `AnalysisBasics, `TopologyBasics, `NumberTheoryExamples,
              `CategoryTheoryExamples, `AdvancedTactics]

lean_exe runExamples where
  root := `Main
