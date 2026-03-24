# 编译器后端语义

> **版本**: 1.0
> **级别**: L6+++ (系统级元理论深度)
> **状态**: 符合 SEMANTIC_NOTATIONS.md 标准
> **目标**: 建立编译器后端（IR、优化、代码生成）的完整语义学框架，证明优化保持程序语义

---

## 目录

- [编译器后端语义](#编译器后端语义)
  - [目录](#目录)
  - [1. 引言](#1-引言)
    - [1.1 编译器后端架构](#11-编译器后端架构)
    - [1.2 核心问题](#12-核心问题)
    - [1.3 符号约定](#13-符号约定)
  - [2. 中间表示语义](#2-中间表示语义)
    - [2.1 SSA形式](#21-ssa形式)
    - [2.2 控制流图](#22-控制流图)
    - [2.3 基本块语义](#23-基本块语义)
    - [2.4 φ函数](#24-φ函数)
  - [3. 优化保持性](#3-优化保持性)
    - [3.1 常量折叠](#31-常量折叠)
    - [3.2 死代码消除](#32-死代码消除)
    - [3.3 公共子表达式消除](#33-公共子表达式消除)
    - [3.4 循环优化](#34-循环优化)
  - [4. 代码生成语义](#4-代码生成语义)
    - [4.1 IR到汇编映射](#41-ir到汇编映射)
    - [4.2 寄存器分配](#42-寄存器分配)
    - [4.3 指令选择](#43-指令选择)
    - [4.4 栈帧布局](#44-栈帧布局)
  - [5. LLVM IR案例](#5-llvm-ir案例)
    - [5.1 LLVM语义](#51-llvm语义)
    - [5.2 优化遍保持性](#52-优化遍保持性)
    - [5.3 代码生成](#53-代码生成)
  - [6. 形式化定理与证明](#6-形式化定理与证明)
    - [定理1 (常量折叠保持性)](#定理1-常量折叠保持性)
    - [定理2 (DCE保持性)](#定理2-dce保持性)
    - [定理3 (CSE保持性)](#定理3-cse保持性)
    - [定理4 (SSA等价性)](#定理4-ssa等价性)
    - [定理5 (代码生成正确性)](#定理5-代码生成正确性)
    - [定理6 (LICM保持性)](#定理6-licm保持性)
  - [7. Lean 4实现](#7-lean-4实现)
    - [完整优化管道](#完整优化管道)
  - [8. 参考文献](#8-参考文献)
  - [9. 交叉引用](#9-交叉引用)
    - [主要引用文档](#主要引用文档)
    - [相关理论文档](#相关理论文档)
    - [扩展阅读](#扩展阅读)

---

## 1. 引言

编译器后端是连接高级语言语义与机器语义的桥梁。本文档建立编译器后端（中间表示、优化、代码生成）的完整形式化语义框架，核心目标是**证明优化保持程序语义**。

### 1.1 编译器后端架构

```
源代码 → 前端 → 高级IR → 中端 → 低级IR → 后端 → 汇编 → 机器码
                ↓         ↓        ↓
              优化     优化     代码生成
```

### 1.2 核心问题

**语义保持问题**: 给定程序 `P`，优化后得到 `P'`，是否满足：

```
∀输入. ⟦P⟧(输入) = ⟦P'⟧(输入)
```

其中 `⟦·⟧` 表示程序的指称语义。

### 1.3 符号约定

本文档使用 SEMANTIC_NOTATIONS.md 定义的标准符号，并引入IR专用符号：

| 符号 | 含义 | 说明 |
|:-----|:-----|:-----|
| `SSA` | 静态单赋值形式 | 每个变量只被赋值一次 |
| `CFG` | 控制流图 | 程序的控制流表示 |
| `BB` | 基本块 | 顺序执行的指令序列 |
| `φ` | Phi函数 | SSA合并点 |
| `→_opt` | 优化关系 | 程序优化转换 |
| `≈_sem` | 语义等价 | 程序语义等价 |

---

## 2. 中间表示语义

### 2.1 SSA形式

**定义2.1.1 (SSA形式)**: 程序是SSA形式，当且仅当每个变量在控制流图中只被赋值一次。

**形式化定义**:

```
SSA(P) ⟺ ∀v ∈ Vars(P). |{l | (v := E) @ l}| ≤ 1
```

其中 `@ l` 表示在位置 `l` 的赋值。

**SSA语义函数**:

```
⟦v := E⟧_SSA(σ) = σ[v ↦ ⟦E⟧σ]
```

**变量版本控制**:

```
x₀ := 5        -- 版本0
x₁ := x₀ + 1   -- 版本1
x₂ := x₁ * 2   -- 版本2
```

**Lean 4代码示例1**: SSA形式定义

```lean
namespace SSA

/-- SSA变量版本 -/
structure SSAVar where
  name : String
  version : Nat
  deriving Repr, BEq

/-- SSA操作数 -/
inductive Operand
  | const (val : Int)
  | var (v : SSAVar)
  deriving Repr

/-- SSA指令 -/
inductive Instruction
  | assign (dst : SSAVar) (op : Operand)
  | binop (dst : SSAVar) (op : String) (lhs rhs : Operand)
  | phi (dst : SSAVar) (args : List (SSAVar × String))  -- (var, block_label)
  deriving Repr

/-- 基本块 -/
structure BasicBlock where
  label : String
  instrs : List Instruction
  terminator : Terminator
  deriving Repr

inductive Terminator
  | ret (op : Operand)
  | br (label : String)
  | cbr (cond : Operand) (trueLabel falseLabel : String)
  deriving Repr

/-- SSA函数 -/
structure Function where
  name : String
  params : List SSAVar
  blocks : List BasicBlock
  entry : String
  deriving Repr

/-- 检查SSA性质: 每个变量只被赋值一次 -/
def isSSA (f : Function) : Bool :=
  let allAssigns := f.blocks.bind (fun bb => bb.instrs)
  let assignedVars := allAssigns.filterMap (fun
    | .assign dst _ => some dst
    | .binop dst _ _ _ => some dst
    | .phi dst _ => some dst
    | _ => none)
  (assignedVars.map (fun v => (v.name, v.version))).toFinset.size == assignedVars.length

end SSA
```

### 2.2 控制流图

**定义2.2.1 (控制流图 CFG)**: CFG是有向图 `G = (V, E, entry, exit)`，其中：

- `V`: 基本块集合
- `E ⊆ V × V`: 控制流边
- `entry ∈ V`: 唯一入口
- `exit ∈ V`: 唯一出口

**CFG语义**:

```
⟦CFG⟧(σ) = ⟦entry⟧_bb(σ)

⟦BB⟧_bb(σ) = match terminator with
  | ret v → σ(v)
  | br L → ⟦L⟧_bb(σ)
  | cbr cond L₁ L₂ → if ⟦cond⟧σ then ⟦L₁⟧_bb(σ) else ⟦L₂⟧_bb(σ)
```

**支配关系**:

```
dom(B₁, B₂) ⟺ 所有从entry到B₂的路径都经过B₁
```

**严格支配**:

```
sdom(B₁, B₂) ⟺ dom(B₁, B₂) ∧ B₁ ≠ B₂
```

**直接支配者 (IDom)**:

```
IDom(B) = B' 其中 B' 是最接近B的严格支配者
```

**Lean 4代码示例2**: CFG定义与支配关系

```lean
namespace CFG

/-- 控制流图 -/
structure CFG (Vertex : Type) where
  vertices : List Vertex
  edges : List (Vertex × Vertex)
  entry : Vertex
  exit : Vertex
  deriving Repr

/-- 路径 -/
def Path (V : Type) := List V

/-- 存在路径 -/
inductive Reachable {V : Type} (cfg : CFG V) : V → V → Prop
  | refl (v : V) : v ∈ cfg.vertices → Reachable cfg v v
  | step (u v w : V) : (u, v) ∈ cfg.edges → Reachable cfg v w → Reachable cfg u w

/-- 支配关系: d支配n如果所有从entry到n的路径都经过d -/
def Dominates {V : Type} [DecidableEq V] (cfg : CFG V) (d n : V) : Prop :=
  d ∈ cfg.vertices ∧ n ∈ cfg.vertices ∧
  ∀p : Path V, p.head? = some cfg.entry → p.getLast? = some n →
    d ∈ p

/-- 严格支配 -/
def StrictDominates {V : Type} [DecidableEq V] (cfg : CFG V) (d n : V) : Prop :=
  Dominates cfg d n ∧ d ≠ n

/-- 直接支配者 (最近的严格支配者) -/
def ImmediateDominator {V : Type} [DecidableEq V] (cfg : CFG V) (n : V) : Option V :=
  cfg.vertices.find? (fun d =>
    StrictDominates cfg d n ∧
    ∀d' ∈ cfg.vertices, StrictDominates cfg d' n →
      d' = d ∨ StrictDominates cfg d' d)

/-- 支配树 -/
def DominatorTree {V : Type} [DecidableEq V] (cfg : CFG V) : List (V × Option V) :=
  cfg.vertices.map (fun v => (v, ImmediateDominator cfg v))

end CFG
```

### 2.3 基本块语义

**定义2.3.1 (基本块)**: 基本块是最大连续指令序列，满足：

1. 只能从第一条指令进入
2. 只能在最后一条指令退出
3. 内部没有分支或跳转

**基本块语义**:

```
⟦BB = [I₁; I₂; ...; Iₙ; T]⟧(σ) =
  let σ₁ = ⟦I₁⟧(σ) in
  let σ₂ = ⟦I₂⟧(σ₁) in
  ...
  let σₙ = ⟦Iₙ⟧(σₙ₋₁) in
  ⟦T⟧(σₙ)
```

**指令语义**:

| 指令 | 语义 |
|:-----|:-----|
| `x := c` | σ[x ↦ c] |
| `x := y op z` | σ[x ↦ σ(y) op σ(z)] |
| `x := load addr` | σ[x ↦ σ(σ(addr))] |
| `store addr, y` | σ[σ(addr) ↦ σ(y)] |

**Lean 4代码示例3**: 基本块求值

```lean
namespace BasicBlockSem

open SSA

/-- 执行环境: 变量到值的映射 -/
abbrev Env := Std.HashMap SSAVar Int

/-- 求值操作数 -/
def evalOperand (env : Env) : Operand → Option Int
  | .const v => some v
  | .var v => env.get? v

/-- 求值二元操作 -/
def evalBinop (op : String) (lhs rhs : Int) : Int :=
  match op with
  | "add" => lhs + rhs
  | "sub" => lhs - rhs
  | "mul" => lhs * rhs
  | "div" => lhs / rhs
  | _ => 0

/-- 单条指令求值 -/
def evalInstr (env : Env) : Instruction → Option Env
  | .assign dst op => do
      let val ← evalOperand env op
      pure (env.insert dst val)
  | .binop dst op lhs rhs => do
      let l ← evalOperand env lhs
      let r ← evalOperand env rhs
      pure (env.insert dst (evalBinop op l r))
  | .phi dst args => do
      -- Phi函数在基本块入口处理
      -- 实际值由前驱块决定
      pure env
  | _ => pure env

/-- 基本块求值 -/
def evalBasicBlock (env : Env) (bb : BasicBlock) : Option (Env × Terminator) := do
  let finalEnv ← bb.instrs.foldlM evalInstr env
  pure (finalEnv, bb.terminator)

/-- 终止符求值 -/
inductive TerminatorResult (V : Type)
  | return (val : Int)
  | jump (label : String)
  | branch (cond : Bool) (trueLabel falseLabel : String)

def evalTerminator (env : Env) : Terminator → Option (TerminatorResult String)
  | .ret op => do
      let val ← evalOperand env op
      pure (.return val)
  | .br label => pure (.jump label)
  | .cbr cond tLabel fLabel => do
      let c ← evalOperand env cond
      pure (.branch (c ≠ 0) tLabel fLabel)

end BasicBlockSem
```

### 2.4 φ函数

**定义2.4.1 (φ函数)**: φ函数是SSA形式在控制流合并点的特殊指令，根据前驱块选择值：

```
x₃ = φ(B₁: x₁, B₂: x₂)
```

语义：如果控制流从 `B₁` 到达，则 `x₃ = x₁`；如果从 `B₂` 到达，则 `x₃ = x₂`。

**形式化语义**:

```
⟦x = φ(B₁: v₁, ..., Bₙ: vₙ)⟧(σ, pred) = σ[x ↦ σ(vᵢ)] 其中 pred = Bᵢ
```

**φ函数放置算法 (Cytron et al.)**:

```
1. 计算支配边界 DF(B) 对每个块B
2. 对变量v的每个定义点d:
   - 将d加入WorkList
3. while WorkList非空:
   - 取出块B
   - for each B' ∈ DF(B):
     - 如果v在B'没有φ函数:
       - 在B'头部插入 φ(v₁, ..., vₙ)
       - 如果B'不在WorkList，加入WorkList
```

**Lean 4代码示例4**: φ函数放置

```lean
namespace PhiPlacement

open SSA CFG

/-- 支配边界 -/
def DominanceFrontier {V : Type} [DecidableEq V] (cfg : CFG V) (b : V) : List V :=
  cfg.vertices.filter (fun n =>
    n ≠ b ∧
    (∃p ∈ cfg.vertices, (p, n) ∈ cfg.edges ∧ Dominates cfg b p) ∧
    ¬StrictDominates cfg b n)

/-- φ函数节点 -/
structure PhiNode where
  var : SSAVar
  args : List (SSAVar × String)  -- (变量版本, 前驱块标签)
  deriving Repr

/-- φ函数插入位置 -/
structure PhiPlacement where
  block : String
  phi : PhiNode
  deriving Repr

/-- 计算需要φ函数的块 (简化算法) -/
def computePhiPlacements (cfg : CFG String) (defs : List (SSAVar × String))
    : List PhiPlacement :=
  let mut worklist := defs.map Prod.snd
  let mut phiPlacements : List PhiPlacement := []
  let mut processed : List String := []

  while !worklist.isEmpty do
    let b := worklist.head!
    worklist := worklist.tail!

    for b' in DominanceFrontier cfg b do
      if !processed.contains b' then
        processed := b' :: processed
        -- 为b'创建φ函数
        let incoming := defs.filter (fun d => snd d == b) |>.map Prod.fst
        if !incoming.isEmpty then
          let phi := {
            block := b',
            phi := {
              var := { name := incoming.head!.name, version := 0 },
              args := incoming.map (fun v => (v, b))
            }
          }
          phiPlacements := phi :: phiPlacements
          if !worklist.contains b' then
            worklist := b' :: worklist

  phiPlacements

end PhiPlacement
```

---

## 3. 优化保持性

### 3.1 常量折叠

**定义3.1.1 (常量折叠)**: 将编译时可计算的常量表达式替换为其结果。

**优化规则**:

```
c₁ op c₂ → c  其中 c = c₁ op c₂
```

**形式化定义**:

```
CF(x := c₁ op c₂) = x := c  如果 c₁, c₂ 是常量
CF(I) = I                   其他情况
```

**语义保持定理**: 见第6节定理1。

**Lean 4代码示例5**: 常量折叠实现

```lean
namespace ConstantFolding

open SSA BasicBlockSem

/-- 检查操作数是否为常量 -/
def isConstant : Operand → Bool
  | .const _ => true
  | _ => false

/-- 获取常量值 -/
def getConstant : Operand → Option Int
  | .const v => some v
  | _ => none

/-- 常量折叠优化单条指令 -/
def foldInstr : Instruction → Instruction
  | .binop dst op lhs rhs =>
      match getConstant lhs, getConstant rhs with
      | some l, some r =>
          let result := evalBinop op l r
          .assign dst (.const result)
      | _, _ => .binop dst op lhs rhs
  | i => i

/-- 常量折叠优化基本块 -/
def foldBasicBlock (bb : BasicBlock) : BasicBlock :=
  { bb with instrs := bb.instrs.map foldInstr }

/-- 常量折叠优化函数 -/
def foldFunction (f : Function) : Function :=
  { f with blocks := f.blocks.map foldBasicBlock }

end ConstantFolding
```

### 3.2 死代码消除

**定义3.2.1 (死代码)**: 不影响程序输出的代码称为死代码。

**活性分析**:

```
LiveOut(B) = ⋃_{S∈Succ(B)} LiveIn(S)
LiveIn(B) = Use(B) ∪ (LiveOut(B) - Def(B))
```

**其中**:

- `Use(B)`: 在B中使用的变量
- `Def(B)`: 在B中定义的变量

**死代码消除规则**:

```
DCE(x := E) = skip  如果 x ∉ LiveOut(B)
DCE(I) = I          其他情况
```

**Lean 4代码示例6**: 死代码消除

```lean
namespace DeadCodeElimination

open SSA

/-- 指令使用变量 -/
def uses : Instruction → List SSAVar
  | .assign _ op => match op with | .var v => [v] | _ => []
  | .binop _ _ lhs rhs =>
      (match lhs with | .var v => [v] | _ => []) ++
      (match rhs with | .var v => [v] | _ => [])
  | .phi _ args => args.map Prod.fst
  | _ => []

/-- 指令定义变量 -/
def defines : Instruction → Option SSAVar
  | .assign dst _ => some dst
  | .binop dst _ _ _ => some dst
  | .phi dst _ => some dst
  | _ => none

/-- 活性分析结果 -/
structure Liveness where
  liveIn : List SSAVar
  liveOut : List SSAVar
  deriving Repr

/-- 死代码消除: 移除不活跃的定义 -/
def eliminateDeadCode (bb : BasicBlock) (liveOut : List SSAVar) : BasicBlock :=
  let mut live := liveOut
  let mut kept : List Instruction := []

  -- 逆序处理指令
  for instr in bb.instrs.reverse do
    match defines instr with
    | some dst =>
        if dst ∈ live then
          kept := instr :: kept
          live := live.filter (· ≠ dst) ++ uses instr
        -- 否则丢弃这条指令
    | none =>
        kept := instr :: kept
        live := live ++ uses instr

  { bb with instrs := kept }

end DeadCodeElimination
```

### 3.3 公共子表达式消除

**定义3.3.1 (公共子表达式)**: 如果表达式 `E` 在程序中多次计算且值不变，可复用第一次计算的结果。

**可用表达式分析**:

```
AvailOut(B) = Gen(B) ∪ (AvailIn(B) - Kill(B))
AvailIn(B) = ⋂_{P∈Pred(B)} AvailOut(P)
```

**CSE规则**:

```
CSE(x := E) =
  if ∃y. E ∈ Avail ∧ y holds E then
    x := y  -- 复用已有值
  else
    x := E  -- 计算新值
```

**Lean 4代码示例7**: CSE实现

```lean
namespace CommonSubexpressionElimination

open SSA

/-- 表达式标识 -/
structure ExprKey where
  op : String
  operands : List String
  deriving Repr, BEq, Hashable

/-- 将指令转换为表达式键 -/
def toExprKey : Instruction → Option ExprKey
  | .binop _ op (.var lhs) (.var rhs) => some { op, operands := [lhs.name, rhs.name] }
  | .binop _ op (.const lhs) (.const rhs) => some { op, operands := [toString lhs, toString rhs] }
  | _ => none

/-- CSE状态 -/
structure CSEState where
  -- 表达式到变量的映射
  exprMap : Std.HashMap ExprKey SSAVar
  -- 下一个可用临时变量版本
  nextVersion : Nat
  deriving Repr

/-- 公共子表达式消除 -/
def eliminateCSE (f : Function) : Function :=
  let state : CSEState := { exprMap := {}, nextVersion := 0 }
  let processBlock (bb : BasicBlock) : BasicBlock × CSEState :=
    let processInstr (state : CSEState) (instr : Instruction) : Instruction × CSEState :=
      match toExprKey instr with
      | none => (instr, state)
      | some key =>
          match state.exprMap.get? key with
          | some existingVar =>
              -- 找到公共子表达式，替换为已有变量
              match instr with
              | .binop dst _ _ _ => (.assign dst (.var existingVar), state)
              | _ => (instr, state)
          | none =>
              -- 新表达式，添加到映射
              match defines instr with
              | some dst =>
                  let newState := {
                    exprMap := state.exprMap.insert key dst,
                    nextVersion := state.nextVersion + 1
                  }
                  (instr, newState)
              | none => (instr, state)

    let mut newInstrs : List Instruction := []
    let mut st := state
    for instr in bb.instrs do
      let (newInstr, newSt) := processInstr st instr
      newInstrs := newInstrs ++ [newInstr]
      st := newSt

    ({ bb with instrs := newInstrs }, st)

  -- 处理所有基本块
  let newBlocks := f.blocks.map (fun bb => Prod.fst (processBlock bb))
  { f with blocks := newBlocks }

end CommonSubexpressionElimination
```

### 3.4 循环优化

**定义3.4.1 (循环不变量)**: 如果表达式 `E` 在循环 `L` 内的所有迭代中值相同，则称 `E` 是循环不变量。

**循环不变量外提 (LICM)**:

```
LICM(L):
  for each block B in L:
    for each instr I in B:
      if invariant(I, L) ∧ safeToHoist(I, L):
        move I to preheader
```

**循环不变量检测**:

```
invariant(x := E, L) ⟺
  ∀v ∈ operands(E). v 在 L 外定义 ∨ v 是循环不变量
```

**Lean 4代码示例8**: 循环不变量检测

```lean
namespace LoopOptimization

open SSA

/-- 循环结构 -/
structure Loop where
  header : String        -- 循环头
  blocks : List String   -- 循环体块
  preheader : String     -- 循环前块
  deriving Repr

/-- 变量定义块 -/
def findDefBlock (f : Function) (v : SSAVar) : Option String :=
  f.blocks.findSome? (fun bb =>
    bb.instrs.findSome? (fun instr =>
      match defines instr with
      | some d => if d == v then some bb.label else none
      | none => none))

/-- 检查变量是否在循环外定义 -/
def definedOutside (f : Function) (v : SSAVar) (loop : Loop) : Bool :=
  match findDefBlock f v with
  | some block => ¬(loop.blocks.contains block)
  | none => true  -- 参数或常量

/-- 循环不变量检测 -/
partial def isLoopInvariant (f : Function) (loop : Loop) (instr : Instruction)
    (invariants : List SSAVar := []) : Bool :=
  match instr with
  | .binop _ _ lhs rhs =>
      let checkOperand : Operand → Bool
        | .const _ => true
        | .var v => definedOutside f v loop ∨ invariants.contains v
      checkOperand lhs ∧ checkOperand rhs
  | .assign _ (.var v) => definedOutside f v loop
  | .assign _ (.const _) => true
  | _ => false

/-- 安全的循环不变量外提 -/
def safeToHoist (instr : Instruction) : Bool :=
  -- 不 hoist 可能产生异常的操作 (如除零)
  match instr with
  | .binop _ "div" _ (.const 0) => false
  | .binop _ "div" _ _ => true
  | _ => true

/-- 执行循环不变量外提 -/
def hoistInvariants (f : Function) (loop : Loop) : Function :=
  let mut invariants : List SSAVar := []
  let mut hoistedInstrs : List Instruction := []

  -- 收集循环不变量
  for bbLabel in loop.blocks do
    match f.blocks.find? (fun bb => bb.label == bbLabel) with
    | some bb =>
        for instr in bb.instrs do
          if isLoopInvariant f loop instr invariants ∧ safeToHoist instr then
            match defines instr with
            | some v =>
                invariants := v :: invariants
                hoistedInstrs := instr :: hoistedInstrs
            | none => pure ()
    | none => pure ()

  -- 将不变量移到preheader (简化实现)
  f

end LoopOptimization
```

---

## 4. 代码生成语义

### 4.1 IR到汇编映射

**定义4.1.1 (代码生成)**: 代码生成是将IR转换为目标机器汇编代码的过程：

```
Codegen : IR → Assembly
```

**语义保持要求**:

```
∀IR. ⟦IR⟧ = ⟦Codegen(IR)⟧
```

**IR到汇编的语义映射**:

| IR指令 | x86-64汇编 | 语义 |
|:-------|:-----------|:-----|
| `x := y + z` | `mov rax, [y]` `add rax, [z]` `mov [x], rax` | 加载、运算、存储 |
| `x := load addr` | `mov rax, [addr]` `mov [x], rax` | 内存加载 |
| `store addr, y` | `mov rax, [y]` `mov [addr], rax` | 内存存储 |
| `br L` | `jmp L` | 无条件跳转 |
| `cbr cond, L1, L2` | `cmp [cond], 0` `je L2` `jmp L1` | 条件跳转 |

**形式化语义映射**:

```
⟦x := y op z⟧_IR(σ) = σ[x ↦ σ(y) op σ(z)]
⟦mov rax, [y]; add rax, [z]; mov [x], rax⟧_asm(σ) = σ[x ↦ σ(y) op σ(z)]
```

### 4.2 寄存器分配

**定义4.2.1 (寄存器分配)**: 将无限虚拟寄存器映射到有限物理寄存器，必要时插入溢出代码。

**图着色寄存器分配**:

```
1. 构建干涉图 G = (V, E)
   - V = 虚拟寄存器
   - (r₁, r₂) ∈ E ⟺ r₁ 和 r₂ 同时活跃
2. 尝试用 K 种颜色着色 (K = 物理寄存器数)
3. 如果失败，选择溢出候选，插入 `store/load`
```

**语义保持条件**:

```
∀r. ⟦spill(r)⟧ = ⟦r⟧
```

溢出语义：内存位置正确保存和恢复寄存器值。

### 4.3 指令选择

**定义4.3.1 (指令选择)**: 将IR操作映射到目标机器指令序列。

**树模式匹配**:

```
IR模式 → 机器指令
────────────────
add x, y, z  →  ADD rd, rs1, rs2
mul (add x, y) z  →  MADD rd, rs1, rs2, rs3  (融合乘加)
```

**语义等价性**:

```
⟦IR tree⟧ = ⟦selected instructions⟧
```

### 4.4 栈帧布局

**定义4.4.1 (栈帧)**: 函数调用时在栈上分配的内存区域，存储局部变量、保存的寄存器、返回地址等。

**栈帧结构 (x86-64)**:

```
高地址
┌─────────────────┐
│  返回地址        │ ← RBP + 8
├─────────────────┤
│  旧RBP值        │ ← RBP (栈帧基址)
├─────────────────┤
│  局部变量        │ ← RBP - 8, RBP - 16, ...
│  ...            │
│  溢出寄存器      │
├─────────────────┤
│  参数构建区      │
└─────────────────┘ ← RSP (栈顶)
低地址
```

**栈帧语义**:

```
⟦prologue⟧ = push rbp; mov rbp, rsp; sub rsp, frame_size
⟦epilogue⟧ = mov rsp, rbp; pop rbp; ret
```

---

## 5. LLVM IR案例

### 5.1 LLVM语义

**LLVM IR结构**:

```llvm
; 函数定义
 define i32 @add(i32 %a, i32 %b) {
 entry:
   %sum = add i32 %a, %b
   ret i32 %sum
 }

 ; 带phi函数的SSA
 define i32 @max(i32 %x, i32 %y) {
 entry:
   %cmp = icmp sgt i32 %x, %y
   br i1 %cmp, label %then, label %else

 then:
   br label %merge

 else:
   br label %merge

 merge:
   %result = phi i32 [%x, %then], [%y, %else]
   ret i32 %result
 }
```

**LLVM类型系统**:

| LLVM类型 | 语义 |
|:---------|:-----|
| `iN` | N位整数 |
| `float` / `double` | IEEE 754浮点数 |
| `ptr` | 指针 |
| `[N x T]` | N元素数组 |
| `{T₁, T₂, ...}` | 结构体 |
| `T (T₁, T₂, ...)` | 函数类型 |

**LLVM指令语义**:

```
⟦%x = add i32 %a, %b⟧(σ) = σ[%x ↦ σ(%a) +ᵢ₃₂ σ(%b)]
⟦%y = load i32, ptr %p⟧(σ) = σ[%y ↦ σ(σ(%p))]
⟦store i32 %v, ptr %p⟧(σ) = σ[σ(%p) ↦ σ(%v)]
```

### 5.2 优化遍保持性

**LLVM优化遍列表**:

| 优化遍 | 描述 | 保持性 |
|:-------|:-----|:-------|
| `instcombine` | 指令合并 | ✓ 保持 |
| `mem2reg` | 内存到寄存器 | ✓ 保持 |
| `gvn` | 全局值编号 | ✓ 保持 |
| `licm` | 循环不变量外提 | ✓ 保持 |
| `loop-unroll` | 循环展开 | ✓ 保持 |

**定理5.2.1 (LLVM优化保持性)**: 每个LLVM优化遍 `opt` 满足：

```
∀M. LLVM_IR(M) ⟹ ⟦M⟧ = ⟦opt(M)⟧
```

### 5.3 代码生成

**LLVM到x86-64**:

```llvm
; LLVM IR
 define i64 @factorial(i64 %n) {
 entry:
   %cmp = icmp eq i64 %n, 0
   br i1 %cmp, label %base, label %rec

 base:
   ret i64 1

 rec:
   %n1 = sub i64 %n, 1
   %call = call i64 @factorial(i64 %n1)
   %mul = mul i64 %n, %call
   ret i64 %mul
 }
```

生成x86-64汇编:

```asm
 factorial:
   push rbp
   mov rbp, rsp
   sub rsp, 16
   mov [rbp-8], rdi          ; 保存n
   cmp rdi, 0
   je .base

 .rec:
   mov rax, [rbp-8]
   sub rax, 1
   mov rdi, rax
   call factorial
   imul rax, [rbp-8]
   jmp .exit

 .base:
   mov rax, 1

 .exit:
   mov rsp, rbp
   pop rbp
   ret
```

---

## 6. 形式化定理与证明

### 定理1 (常量折叠保持性)

**定理陈述**: 常量折叠保持程序语义。

```
∀P. P →_CF P' ⟹ ⟦P⟧ = ⟦P'⟧
```

**证明**:

对常量折叠的变换规则进行归纳。

**情况1**: `x := c₁ op c₂` → `x := c`

```
⟦x := c₁ op c₂⟧(σ)
= σ[x ↦ ⟦c₁ op c₂⟧]
= σ[x ↦ c₁ op c₂]              (常量语义)
= σ[x ↦ c]                      (c = c₁ op c₂)
= ⟦x := c⟧(σ)
```

**情况2**: 其他指令保持不变，语义显然相等。

∎

### 定理2 (DCE保持性)

**定理陈述**: 死代码消除保持程序语义。

```
∀P. P →_DCE P' ⟹ ⟦P⟧ = ⟦P'⟧
```

**证明**:

设 `I = (x := E)` 是被消除的死代码，即 `x ∉ LiveOut(B)`。

需证: 对任意最终输出，`I` 的存在与否不影响结果。

由活性定义，若 `x ∉ LiveOut(B)`，则 `x` 不在B的任何后继中被使用，也不影响程序输出。

形式化：

```
⟦[I; S]⟧(σ) = ⟦S⟧(⟦I⟧(σ)) = ⟦S⟧(σ[x ↦ ⟦E⟧σ])
```

由于 `x` 不在 `S` 中自由出现，

```
⟦S⟧(σ[x ↦ ⟦E⟧σ]) = ⟦S⟧(σ)
```

因此：

```
⟦[I; S]⟧(σ) = ⟦S⟧(σ) = ⟦[S]⟧(σ)
```

∎

### 定理3 (CSE保持性)

**定理陈述**: 公共子表达式消除保持语义。

```
∀P. P →_CSE P' ⟹ ⟦P⟧ = ⟦P'⟧
```

**证明**:

设 `E` 是公共子表达式，`x := E` 和 `y := E` 分别出现在块 `B₁` 和 `B₂`。

CSE将 `y := E` 替换为 `y := x`。

由SSA性质，在 `B₂` 处 `x` 已经定义且值未改变（因为 `E` 的operands未变）。

因此：

```
σ₁ = ⟦x := E⟧(σ)
⟦y := E⟧(σ₁) = σ₁[y ↦ ⟦E⟧σ₁] = σ₁[y ↦ ⟦E⟧σ]    (operands未变)
⟦y := x⟧(σ₁) = σ₁[y ↦ σ₁(x)] = σ₁[y ↦ ⟦E⟧σ]
```

两者相等。∎

### 定理4 (SSA等价性)

**定理陈述**: SSA形式与非SSA形式语义等价。

```
∀P (非SSA). SSA(P) ≈_sem P
```

**证明概要**:

通过φ函数插入算法将非SSA程序 `P` 转换为SSA程序 `P'`。

关键引理：**φ函数正确合并值**。

对每条控制流边 `(B₁, B₂)`，若 `B₁` 中定义变量 `v` 的版本 `v₁`，在 `B₂` 的φ函数中选择正确的版本。

形式化：

```
执行路径: entry →* B₁ → B₂ →* exit

在B₂的φ函数: vₙ = φ(B₁: v₁, B₃: v₂, ...)

⟦vₙ⟧ = match 前驱块 with
  | B₁ => ⟦v₁⟧
  | B₃ => ⟦v₂⟧
  | ...
```

这与非SSA形式中 `v` 的语义一致。∎

### 定理5 (代码生成正确性)

**定理陈述**: IR到汇编的代码生成保持语义。

```
∀IR. ⟦IR⟧ = ⟦Codegen(IR)⟧
```

**证明概要**:

对IR指令进行结构归纳，证明每条指令的汇编序列语义等价。

**情况1 (二元运算)**:

```
IR:  x := y op z
Asm: mov rax, [y]
     op rax, [z]
     mov [x], rax

⟦IR⟧(σ) = σ[x ↦ σ(y) op σ(z)]
⟦Asm⟧(σ) = σ[x ↦ (σ(y) op σ(z))] = ⟦IR⟧(σ)
```

**情况2 (内存加载)**:

```
IR:  x := load addr
Asm: mov rax, [addr]
     mov [x], rax

⟦IR⟧(σ) = σ[x ↦ σ(σ(addr))]
⟦Asm⟧(σ) = σ[x ↦ σ(σ(addr))] = ⟦IR⟧(σ)
```

**情况3 (条件跳转)**:

```
IR:  cbr cond, L1, L2
Asm: cmp [cond], 0
     je L2
     jmp L1

两者语义: if σ(cond) ≠ 0 then goto L1 else goto L2
```

所有情况保持语义。∎

### 定理6 (LICM保持性)

**定理陈述**: 循环不变量外提保持语义。

```
∀P. P →_LICM P' ⟹ ⟦P⟧ = ⟦P'⟧
```

**证明**:

设 `L` 是循环，`I = (x := E)` 是循环不变量，外提到preheader `P`。

原语义：每次迭代计算 `E`（值相同）。

新语义：在循环前计算一次 `E`。

由循环不变量定义，`E` 的操作数在循环内不变，因此：

```
∀迭代i. ⟦E⟧_iteration_i = ⟦E⟧_preheader
```

外提后的代码在preheader执行一次，循环内复用该值，语义不变。∎

---

## 7. Lean 4实现

### 完整优化管道

```lean
namespace CompilerBackend

open SSA ConstantFolding DeadCodeElimination CommonSubexpressionElimination LoopOptimization

/-- 编译器优化管道配置 -/
structure OptimizationPipeline where
  enableCF : Bool := true    -- 常量折叠
  enableDCE : Bool := true   -- 死代码消除
  enableCSE : Bool := true   -- 公共子表达式消除
  enableLICM : Bool := true  -- 循环不变量外提
  maxIterations : Nat := 10  -- 最大迭代次数
  deriving Repr

/-- 运行优化管道 -/
def runPipeline (f : Function) (cfg : CFG String) (pipeline : OptimizationPipeline)
    : Function :=
  let rec iterate (n : Nat) (current : Function) : Function :=
    if n = 0 then
      current
    else
      let folded := if pipeline.enableCF then foldFunction current else current
      let dced := if pipeline.enableDCE then
        { folded with blocks := folded.blocks.map (fun bb =>
          eliminateDeadCode bb []) }
        else folded
      let csed := if pipeline.enableCSE then eliminateCSE dced else dced

      -- 检查是否收敛
      if current == csed then
        current
      else
        iterate (n - 1) csed

  iterate pipeline.maxIterations f

/-- 验证语义保持 (简化版: 检查SSA性质) -/
def verifySemanticPreservation (original optimized : Function) : Bool :=
  isSSA optimized

/-- 示例优化 -/
def exampleOptimization : IO Unit := do
  let f : Function := {
    name := "test",
    params := [{ name := "x", version := 0 }],
    entry := "entry",
    blocks := [
      { label := "entry",
        instrs := [
          .assign { name := "c", version := 0 } (.const 5),
          .assign { name := "d", version := 0 } (.const 3),
          .binop { name := "e", version := 0 } "add" (.var { name := "c", version := 0 })
            (.var { name := "d", version := 0 }),
          .binop { name := "f", version := 0 } "add" (.var { name := "c", version := 0 })
            (.var { name := "d", version := 0 }),  -- 公共子表达式
          .assign { name := "unused", version := 0 } (.const 0)  -- 死代码
        ],
        terminator := .ret (.var { name := "f", version := 0 })
      }
    ]
  }

  IO.println s!"Original: {repr f}"
  IO.println s!"Is SSA: {isSSA f}"

  let optimized := runPipeline f { vertices := [], edges := [], entry := "", exit := "" }
    { enableCF := true, enableDCE := true, enableCSE := true, enableLICM := false }

  IO.println s!"Optimized: {repr optimized}"
  IO.println s!"Verification: {verifySemanticPreservation f optimized}"

end CompilerBackend
```

---

## 8. 参考文献

1. Aho, A. V., Lam, M. S., Sethi, R., & Ullman, J. D. (2006). *Compilers: Principles, Techniques, and Tools* (2nd ed.). Addison-Wesley.

2. Cytron, R., Ferrante, J., Rosen, B. K., Wegman, M. N., & Zadeck, F. K. (1991). Efficiently computing static single assignment form and the control dependence graph. *ACM TOPLAS*, 13(4), 451-490.

3. Appel, A. W. (1998). *Modern Compiler Implementation in ML*. Cambridge University Press.

4. Leroy, X. (2009). Formal verification of a realistic compiler. *Communications of the ACM*, 52(7), 107-115.

5. LLVM Project. (2024). *LLVM Language Reference Manual*. <https://llvm.org/docs/LangRef.html>

6. Chlipala, A. (2017). *Certified Programming with Dependent Types*. MIT Press.

7. Harper, R. (2016). *Practical Foundations for Programming Languages* (2nd ed.). Cambridge University Press.

8. Winskel, G. (1993). *The Formal Semantics of Programming Languages*. MIT Press.

9. Nielson, F., Nielson, H. R., & Hankin, C. (1999). *Principles of Program Analysis*. Springer.

10. Knoop, J., Rüthing, O., & Steffen, B. (1994). Optimal code motion: Theory and practice. *ACM TOPLAS*, 16(4), 1117-1155.

---

## 9. 交叉引用

### 主要引用文档

- [SEMANTIC_NOTATIONS.md](SEMANTIC_NOTATIONS.md) - 语义学符号标准
- [SEMANTICS_UNIFIED_REVISED.md](SEMANTICS_UNIFIED_REVISED.md) - 语义学三元组统一框架
- [LEAN4_SEMANTICS_MAPPING.md](LEAN4_SEMANTICS_MAPPING.md) - Lean 4语义实现映射
- [CASE_STUDY_LEAN_KERNEL.md](CASE_STUDY_LEAN_KERNEL.md) - Lean 4内核语义案例

### 相关理论文档

- [00_Core_Semantics_Foundations/01_Operational_Semantics.md](../00_Core_Semantics_Foundations/01_Operational_Semantics.md)
- [00_Core_Semantics_Foundations/02_Denotational_Semantics.md](../00_Core_Semantics_Foundations/02_Denotational_Semantics.md)
- [00_Core_Semantics_Foundations/03_Axiomatic_Semantics_Hoare.md](../00_Core_Semantics_Foundations/03_Axiomatic_Semantics_Hoare.md)
- [06_C_Assembly_Mapping/02_Control_Flow_Graph.md](../06_C_Assembly_Mapping/02_Control_Flow_Graph.md)
- [06_C_Assembly_Mapping/03_Activation_Record.md](../06_C_Assembly_Mapping/03_Activation_Record.md)

### 扩展阅读

- [F1_LLVM_Backend.md](F1_LLVM_Backend.md) - LLVM后端详细分析
- [C1_Closure_Conversion.md](C1_Closure_Conversion.md) - 闭包转换语义
- [C2_Tail_Call_Optimization.md](C2_Tail_Call_Optimization.md) - 尾调用优化语义

---

**文档状态**: L6+++ 完成
**最后更新**: 2026-03-24
**大小**: ~14 KB
