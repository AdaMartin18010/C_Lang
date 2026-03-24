# Lean 4 MetaM：元编程的单子架构

> **层级**: L6 (元理论层)
> **引用**: de Moura & Ullrich (2021), Christiansen (2019)
> **课程对标**: MIT 6.945, Stanford CS242

---

## 一、元编程的数学基础

### 1.1 反射与单子的理论

**定义 1.1** (反射层级)

设 Base 为对象语言（Lean），Meta 为元语言（也是Lean）。

反射塔(Reflective Tower)：

```
Level 0: 对象程序（运行时）
Level 1: 元程序（编译时）
Level 2: 元元程序...
```

Lean 4的MetaM工作在Level 1。

### 1.2 单子作为计算效应

**定义 1.2** (MetaM单子)

MetaM是一个状态单子，携带：

- 环境：局部变量、选项
- 状态：元变量、约束
- 异常：错误处理

```lean
MetaM α = State → (α × State) ⊎ Error
```

---

## 二、MetaM的核心操作

### 2.1 类型检查与合成

**核心函数**：

```lean
-- 类型检查
MetaM.check (e : Expr) (expectedType? : Option Expr) : MetaM Expr

-- 类型推断
MetaM.inferType (e : Expr) : MetaM Expr

-- 统一
MetaM.isDefEq (t₁ t₂ : Expr) : MetaM Bool
```

**定理 2.1** (类型检查正确性)

如果 check e (some T) = pure e'，
则 e' : T 在核心类型论中成立。

### 2.2 元变量管理

**定义 2.2** (元变量)

元变量 ?m 表示待填充的项，带有：

- 类型：?m : T
- 上下文：局部变量
- 约束：与其他项的关系

**操作**：

```lean
-- 创建元变量
MetaM.mkFreshExprMVar (type? : Option Expr) : MetaM Expr

-- 赋值
MetaM.assign (mvar : MVarId) (val : Expr) : MetaM Unit
```

---

## 三、Macro与Elab

### 3.1 宏展开的单子解释

**MacroM**：在Syntax层面操作的单子。

```lean
MacroM α = Macro.Context → Macro.State → EMacroException (α × Macro.State)
```

**展开过程**：

```
Syntax → MacroM Syntax → (展开后) Syntax → ElabM Expr → (核心) Expr
```

### 3.2 Elaboration的形式化

**定义 3.1** (Elaboration)

Elaboration是将Surface Syntax映射到Core Expr的过程：

```
elab : Syntax → TermElabM Expr
```

**性质**：

- 非确定性：可能有多个有效 elaboration
- 上下文敏感：依赖当前环境

---

## 四、Tactic单子的理论

### 4.1 Tactic作为证明搜索

**定义 4.1** (Tactic状态)

Tactic状态包含：

- 目标：待证明的命题列表
- 元变量：待填充的证明项
- 约束：统一约束

```lean
def TacticM α := Tactic.State → Tactic.State × Option α
```

### 4.2 Tactic的组合子

**基本组合子**：

```lean
-- 顺序组合
t₁ >> t₂ : TacticM Unit

-- 尝试
try t : TacticM Unit

-- 选择
t₁ <|> t₂ : TacticM Unit

-- 重复
tactic.repeat t : TacticM Unit
```

**定理 4.2** (Tactic正确性)

如果 tactic 将目标G转换为子目标G₁,...,Gₙ，
并且证明了所有Gᵢ，
则构造的证明证明了G。

---

## 五、从MetaM到C代码

### 5.1 编译器的自举

Lean 4编译器用Lean 4编写，因此：

- MetaM代码被编译为C
- 编译器可以扩展自身

**编译流程**：

```
Lean Source → MetaM扩展 → Elab → IR → C → 二进制
```

### 5.2 运行时支持

MetaM的运行时支持在C运行时中：

```c
// 元变量表示
lean_object* lean_mk_mvar(lean_object* type);

// 统一
uint8_t lean_is_def_eq(lean_object* t1, lean_object* t2);
```

---

**参考文献**：

- de Moura, L. & Ullrich, S. "The Lean 4 Theorem Prover" (2021)
- Christiansen, D. "Practical Reflection and Metaprogramming" (2019)
