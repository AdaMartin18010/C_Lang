---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# Lean 4证明策略与技巧

## 1. 证明策略概述

### 1.1 什么是Tactics？

在Lean 4中，证明是通过一系列**策略（Tactics）**来交互式构造的。每个策略将当前目标转换为零个或多个更简单的子目标。

```lean
-- 证明: 对于所有自然数n, n + 0 = n
example (n : Nat) : n + 0 = n := by
  -- 初始目标: n + 0 = n
  induction n with
  | zero =>       -- 子目标1: 0 + 0 = 0
    rfl           -- 完成
  | succ n ih =>  -- 子目标2: (n+1) + 0 = n+1, 假设 ih: n + 0 = n
    simp [Nat.add_succ, ih]  -- 完成
```

### 1.2 策略状态

在证明的任意时刻，都有一个**策略状态**包含：

- **目标**: 需要证明的命题
- **上下文**: 可用的假设（变量和定理）

```lean
-- 策略状态示例
n : Nat          -- 上下文中: 变量n
ih : n + 0 = n   -- 上下文中: 归纳假设
⊢ (n+1) + 0 = n+1 -- 当前目标（⊢ 符号表示"需要证明"）
```

---

## 2. 基础策略

### 2.1 精确匹配 (exact)

```lean
-- 当有一个假设恰好等于目标时使用
example (P : Prop) (hp : P) : P := by
  exact hp  -- 用hp精确匹配目标P
```

### 2.2 应用定理 (apply)

```lean
-- 目标: Q
-- 定理: P → Q
-- 应用后新目标: P

example (P Q : Prop) (h : P → Q) (hp : P) : Q := by
  apply h   -- 应用h，新目标是P
  exact hp  -- 证明P
```

### 2.3 引入假设 (intro)

```lean
-- 目标: P → Q
-- intro后: 假设h: P, 目标变为Q

example (P Q : Prop) : P → Q → P ∧ Q := by
  intro hp  -- 假设hp: P
  intro hq  -- 假设hq: Q
  exact And.intro hp hq  -- 证明P ∧ Q
```

多目标同时引入：

```lean
example : ∀ (n m : Nat), n + m = m + n := by
  intro n m  -- 同时引入n和m
  -- 证明 n + m = m + n
  rw [Nat.add_comm]
```

### 2.4 分解合取 (constructor)

```lean
-- 目标: P ∧ Q
-- constructor后: 两个子目标 P 和 Q

example (P Q : Prop) (hp : P) (hq : Q) : P ∧ Q := by
  constructor  -- 分成两个目标
  · exact hp   -- 证明P
  · exact hq   -- 证明Q
```

### 2.5 选择析取方向 (left / right)

```lean
-- 目标: P ∨ Q
-- left后: 目标变为P
-- right后: 目标变为Q

example (P Q : Prop) (hp : P) : P ∨ Q := by
  left   -- 选择左边
  exact hp

example (P Q : Prop) (hq : Q) : P ∨ Q := by
  right  -- 选择右边
  exact hq
```

---

## 3. 假设操作策略

### 3.1 解构假设 (cases)

```lean
-- 假设h: P ∧ Q
-- cases h with
-- | intro hp hq => 得到hp: P和hq: Q

example (P Q R : Prop) (h : P ∧ Q) (f : P → Q → R) : R := by
  cases h with
  | intro hp hq =>  -- 解构h
    apply f hp hq

-- 解构存在量词
example (P : Nat → Prop) (h : ∃ n, P n) : ∃ n, P n := by
  cases h with
  | intro w hw =>  -- 得到见证w和证明hw: P w
    exact Exists.intro w hw
```

### 3.2 重写 (rw)

```lean
-- 使用等式重写目标

example (a b c : Nat) (h1 : a = b) (h2 : b = c) : a = c := by
  rw [h1]  -- 将a重写为b，目标变为b = c
  rw [h2]  -- 将b重写为c，目标变为c = c
  rfl      -- 完成

-- 反向重写
example (a b : Nat) (h : b = a) : a = b := by
  rw [←h]  -- 使用h的反向

-- 在假设中重写
example (a b : Nat) (h1 : a = b) (h2 : a + a = 2 * a) : b + b = 2 * b := by
  rw [h1] at h2  -- 在h2中重写
  exact h2
```

### 3.3 简化 (simp)

```lean
-- 使用简化规则集自动简化

example (n : Nat) : n + 0 = n := by
  simp  -- 使用Nat.add_zero规则自动简化

-- 指定简化规则
example (n : Nat) : (n + 1) * 2 = n * 2 + 2 := by
  simp [Nat.mul_add, Nat.add_mul]

-- 在假设中简化
example (n : Nat) (h : n + 0 = 5) : n = 5 := by
  simp at h  -- 简化h为n = 5
  exact h

-- 扩展简化规则集
example (l : List Nat) : [] ++ l = l := by
  simp [List.nil_append]
```

---

## 4. 归纳证明

### 4.1 自然数归纳 (induction)

```lean
-- 数学归纳法原理
-- 证明P(0)且P(n) → P(n+1)，则对所有n, P(n)成立

-- 求和公式: 0 + 1 + ... + n = n(n+1)/2
theorem sum_formula (n : Nat) : 2 * ∑ i in range (n+1), i = n * (n + 1) := by
  induction n with
  | zero =>  -- 基础情况: n = 0
    simp [sum_range_zero]
  | succ n ih =>  -- 归纳步骤
    rw [sum_range_succ, Nat.mul_add, ih]
    ring  -- 代数简化
```

### 4.2 列表归纳

```lean
-- 列表反转的involution性质: reverse (reverse l) = l
theorem reverse_involutive {α : Type} (l : List α) :
  List.reverse (List.reverse l) = l := by
  induction l with
  | nil =>  -- 基础: l = []
    simp [List.reverse]
  | cons x xs ih =>  -- 归纳: l = x::xs
    simp [List.reverse, List.reverse_append, ih]
```

### 4.3 结构归纳

```lean
-- 二叉树定义
inductive Tree (α : Type) where
  | empty : Tree α
  | node : α → Tree α → Tree α → Tree α

-- 树的大小
def Tree.size {α : Type} : Tree α → Nat
  | empty => 0
  | node _ l r => 1 + size l + size r

-- 证明size非负
theorem Tree.size_nonneg {α : Type} (t : Tree α) : size t ≥ 0 := by
  induction t with
  | empty => simp [size]; omega
  | node x l r ihl ihr =>
    simp [size]
    omega
```

---

## 5. 自动化策略

### 5.1 通用自动证明 (aesop)

```lean
-- aesop: 通用自动证明策略，使用上下文中的假设

example (P Q R : Prop) (h1 : P → Q) (h2 : Q → R) (hp : P) : R := by
  aesop  -- 自动链式应用h1和h2

-- 列表成员性质
example {α : Type} (x : α) (l1 l2 : List α) (h : x ∈ l1) : x ∈ l1 ++ l2 := by
  aesop  -- 自动应用List.mem_append_left
```

### 5.2 线性算术 (linarith / omega)

```lean
-- linarith: 自动解决线性算术问题

example (a b c : Int) (h1 : a < b) (h2 : b < c) : a < c := by
  linarith  -- 传递性自动推导

example (x y : Nat) (h : x + y > 10) : x > 10 - y := by
  omega  -- 处理自然数/整数算术

-- 带约束的算术
example (n m : Nat) (h1 : n ≤ 5) (h2 : m ≤ 3) : n + m ≤ 8 := by
  linarith
```

### 5.3 等式推理 (ring / abel / field)

```lean
-- ring: 自动证明环等式

example (a b : Int) : (a + b) ^ 2 = a ^ 2 + 2 * a * b + b ^ 2 := by
  ring  -- 自动展开和简化

-- field: 处理分式
example (a b c : Rat) (hb : b ≠ 0) (hc : c ≠ 0) :
  a / b + a / c = a * (c + b) / (b * c) := by
  field_simp [hb, hc]
  ring
```

---

## 6. 高级策略

### 6.1 分情况讨论 (by_cases / by_contra)

```lean
-- 排中律: P ∨ ¬P
example (P : Prop) : P ∨ ¬P := by
  by_cases h : P  -- 情况1: h: P, 情况2: h: ¬P
  · left; exact h
  · right; exact h

-- 反证法
example (P : Prop) (h : ¬¬P) : P := by
  by_contra h'  -- 假设¬P
  exact h h'    -- 与h: ¬¬P矛盾
```

### 6.2 存在量词 (use / exists)

```lean
-- 构造存在证明
example : ∃ n : Nat, n > 10 := by
  use 11  -- 提供见证值
  norm_num  -- 证明11 > 10

-- 带条件的存在
def is_even (n : Nat) := ∃ k, n = 2 * k

example : is_even 100 := by
  use 50
  rfl
```

### 6.3 泛函扩展 (funext / ext)

```lean
-- 证明函数相等
example (f g : Nat → Nat) (h : ∀ x, f x = g x) : f = g := by
  funext x  -- 对每个x证明f x = g x
  exact h x

-- 集合/类型扩展
example (A B : Set Nat) (h : ∀ x, x ∈ A ↔ x ∈ B) : A = B := by
  ext x  -- 扩展到元素级别
  exact h x
```

### 6.4 选择 (rcases / obtain)

```lean
-- 复杂模式匹配
example (P Q R : Prop) (h : P ∧ (Q ∨ R)) : (P ∧ Q) ∨ (P ∧ R) := by
  rcases h with ⟨hp, hq | hr⟩  -- 同时解构∧和∨
  · left; constructor; exact hp; exact hq
  · right; constructor; exact hp; exact hr

-- 从存在假设中提取
example (P : Nat → Prop) (h : ∃ n, P n ∧ n > 0) : ∃ n, P n := by
  obtain ⟨n, hn, _⟩ := h  -- 解构存在量词
  exact ⟨n, hn⟩
```

---

## 7. 调试与探索策略

### 7.1 查看当前状态

```lean
example (n : Nat) : n + 0 = n := by
  have h : n + 0 = n := by simp  -- 引入中间步骤验证
  exact h
```

### 7.2 占位符证明 (sorry / admit)

```lean
-- 在开发中跳过子目标
example (P : Prop) : P → P := by
  intro hp
  sorry  -- 标记为待完成

-- sorry允许整个证明通过（带警告）
```

### 7.3 跟踪简化过程

```lean
-- 查看simp应用了哪些规则
set_option trace.Meta.Tactic.simp true

example (n : Nat) : n + 0 = n := by
  simp  -- 输出simp使用的规则
```

---

## 8. 常见证明模式

### 模式1: 传递等式链

```lean
example (a b c d : Nat) (h1 : a = b) (h2 : b = c) (h3 : c = d) : a = d := by
  calc
    a = b := by rw [h1]
    _ = c := by rw [h2]
    _ = d := by rw [h3]
```

### 模式2: 双向证明（当且仅当）

```lean
example (P Q : Prop) (h1 : P → Q) (h2 : Q → P) : P ↔ Q := by
  constructor
  · exact h1
  · exact h2
```

### 模式3: 归纳-简化模式

```lean
-- 列表长度性质证明的标准模式
theorem length_append {α : Type} (l1 l2 : List α) :
  (l1 ++ l2).length = l1.length + l2.length := by
  induction l1 with
  | nil => simp  -- 基础情况: [] ++ l2 = l2
  | cons x xs ih =>
    simp [Nat.add_assoc, ih]  -- 归纳步骤: 使用归纳假设
```

---

## 9. 策略速查表

| 策略 | 用途 | 示例 |
|------|------|------|
| `exact` | 精确匹配 | `exact h` |
| `apply` | 应用蕴含 | `apply h` |
| `intro` | 引入假设 | `intro x` |
| `constructor` | 分解合取 | `constructor` |
| `left/right` | 选择析取 | `left` |
| `cases` | 解构 | `cases h` |
| `rw` | 重写 | `rw [h]` |
| `simp` | 简化 | `simp [rules]` |
| `induction` | 归纳 | `induction n` |
| `linarith` | 线性算术 | `linarith` |
| `ring` | 环等式 | `ring` |
| `aesop` | 自动证明 | `aesop` |
| `use` | 存在引入 | `use witness` |
| `funext` | 函数扩展 | `funext x` |
| `by_cases` | 分情况 | `by_cases h : P` |
| `calc` | 计算链 | `calc a = b := ...` |

---

## 10. 练习

尝试证明以下定理：

```lean
-- 练习1: 自反性
example (P : Prop) : P ↔ P := by
  sorry  -- 替换为实际证明

-- 练习2: 结合律
theorem or_assoc (P Q R : Prop) : (P ∨ Q) ∨ R ↔ P ∨ (Q ∨ R) := by
  sorry

-- 练习3: 列表长度
theorem length_reverse {α : Type} (l : List α) :
  l.reverse.length = l.length := by
  sorry
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**维护者**: C_Lang Knowledge Base Team


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
