---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# 公理定理证明推理决策树

## 1. 决策树使用指南

本决策树指导如何在形式化证明中选择合适的策略。适用于Coq、Lean 4、Isabelle等定理证明器。

---

## 2. 主决策树

```
开始证明当前目标
│
├─► 目标是否为真命题 (True)？
│   ├─► 是 → 使用 trivial / exact True.intro
│   └─► 否 → 继续
│
├─► 目标是否为假命题 (False)？
│   ├─► 是 → 查找矛盾假设 → contradiction
│   └─► 否 → 继续
│
├─► 目标是否为蕴含式 (P → Q)？
│   ├─► 是 → intro hP → 将P加入假设 → 证明Q
│   └─► 否 → 继续
│
├─► 目标是否为全称量词 (∀ x, P x)？
│   ├─► 是 → intro x → 为任意x证明P x
│   └─► 否 → 继续
│
├─► 目标是否存在量词 (∃ x, P x)？
│   ├─► 是 → 构造见证值a → exists a → 证明P a
│   └─► 否 → 继续
│
├─► 目标是否为合取 (P ∧ Q)？
│   ├─► 是 → constructor → 分别证明P和Q
│   └─► 否 → 继续
│
├─► 目标是否为析取 (P ∨ Q)？
│   ├─► 是 → 选择可证明的一侧 → left/right → 证明该侧
│   └─► 否 → 继续
│
├─► 目标是否为等价 (P ↔ Q)？
│   ├─► 是 → constructor → 分别证明P→Q和Q→P
│   └─► 否 → 继续
│
├─► 目标是否为否定 (¬P)？
│   ├─► 是 → intro hP → 从P推出矛盾
│   └─► 否 → 继续
│
├─► 目标是否为等式 (t = u)？
│   ├─► 是 → 见"等式证明子树"
│   └─► 否 → 继续
│
├─► 目标是否涉及归纳定义类型？
│   ├─► 是 → 见"归纳证明子树"
│   └─► 否 → 继续
│
└─► 尝试自动化策略
    ├─► 简单目标 → auto / aesop (Lean) / eauto (Coq)
    ├─► 算术目标 → omega / linarith / nlinarith
    ├─► 简化目标 → simp / simpl
    └─► 所有方法失败 → 手动重写(rewrite/apply)
```

---

## 3. 等式证明子树

```
目标: t = u
│
├─► t和u是否完全相同？
│   ├─► 是 → reflexivity / rfl
│   └─► 否 → 继续
│
├─► 假设中是否有直接等式 t = u？
│   ├─► 是 → assumption / exact h
│   └─► 否 → 继续
│
├─► 是否可通过重写假设中的等式证明？
│   ├─► 是 → rw [h] → 简化后使用 reflexivity
│   └─► 否 → 继续
│
├─► 是否为函数应用 (f a = f b) 且 a = b？
│   ├─► 是 → apply f_eq → 证明 a = b
│   └─► 否 → 继续
│
├─► 是否可通过计算简化？
│   ├─► 是 → simp / compute / cbv → reflexivity
│   └─► 否 → 继续
│
├─► 是否涉及交换律/结合律？
│   ├─► 是 → rw [add_comm/mul_comm] 等 → 继续简化
│   └─► 否 → 继续
│
├─► 是否可转换为对称目标？
│   ├─► 是 → symmetry → 证明 u = t
│   └─► 否 → 继续
│
└─► 使用 transitivity
    ├─► 找到中间值 v
    ├─► 证明 t = v
    └─► 证明 v = u
```

---

## 4. 归纳证明子树

```
目标: ∀ n, P n (或针对特定归纳类型)
│
├─► 是否为自然数归纳？
│   ├─► 是 → induction n with
│   │        ├─► 基础情况: n = 0 → 证明 P 0
│   │        └─► 归纳步骤: n = m+1 → 假设 ih : P m → 证明 P (m+1)
│   └─► 否 → 继续
│
├─► 是否为列表归纳？
│   ├─► 是 → induction l with
│   │        ├─► 基础情况: l = [] → 证明 P []
│   │        └─► 归纳步骤: l = x::xs → 假设 ih : P xs → 证明 P (x::xs)
│   └─► 否 → 继续
│
├─► 是否为结构归纳（一般归纳类型）？
│   ├─► 是 → induction x → 对每个构造子分别证明
│   └─► 否 → 继续
│
├─► 是否为完全归纳/强归纳？
│   ├─► 是 → 使用 strong_induction / induction' with
│   └─► 否 → 继续
│
└─► 是否为互递归归纳？
    ├─► 是 → mutual_induction
    └─► 否 → 检查类型定义，选择合适的归纳原则
```

---

## 5. 假设应用子树

```
当前假设 H : P
│
├─► P是否与目标完全相同？
│   ├─► 是 → exact H
│   └─► 否 → 继续
│
├─► P是否为目标的前提？(P → Goal)
│   ├─► 是 → apply H → 证明P
│   └─► 否 → 继续
│
├─► P是否为合取 (A ∧ B)？
│   ├─► 是 → cases H with → 获得 A 和 B 两个假设
│   └─► 否 → 继续
│
├─► P是否为析取 (A ∨ B)？
│   ├─► 是 → cases H with → 分别处理A情况和B情况
│   └─► 否 → 继续
│
├─► P是否为存在量词 (∃ x, Q x)？
│   ├─► 是 → cases H with → 获得见证值a和Q a
│   └─► 否 → 继续
│
├─► P是否为等式 (t = u)？
│   ├─► 是 → rw [H] → 重写目标中的t为u
│   ├─► 或 → rw [← H] → 反向重写
│   └─► 否 → 继续
│
├─► P是否可简化？
│   ├─► 是 → simp at H → 简化H后继续
│   └─► 否 → 继续
│
└─► 保留H供后续使用 → clear H (如果确定不需要)
```

---

## 6. 矛盾证明子树

```
目标: 从当前假设推出 False
│
├─► 是否存在互补假设？(H1 : P, H2 : ¬P)
│   ├─► 是 → contradiction / exfalso → apply H2 → exact H1
│   └─► 否 → 继续
│
├─► 是否存在矛盾等式？(如 n < 0 对自然数n)
│   ├─► 是 → omega / linarith 自动发现矛盾
│   └─► 否 → 继续
│
├─► 是否可从假设推出两个不同的结果？
│   ├─► 是 → 分别推导 → 证明它们不等
│   └─► 否 → 继续
│
└─► 从特定假设推出矛盾
    ├─► 识别最可能矛盾的假设H
    ├─► apply H → 尝试证明其前提
    └─► 或使用 cases H 解构
```

---

## 7. 特定领域决策

### 7.1 算术推理

```
目标涉及算术运算
│
├─► 是否为纯线性算术？
│   ├─► 是 → omega (Coq) / linarith (Lean)
│   └─► 否 → 继续
│
├─► 是否涉及非线性乘法？
│   ├─► 是 → nlinarith / ring / field
│   └─► 否 → 继续
│
├─► 是否为整数/自然数特定性质？
│   ├─► 是 → 使用特定引理 (Nat.add_comm, Int.mul_assoc等)
│   └─► 否 → 继续
│
└─► 手动重写 → rw [算术引理] → 简化 → 手动完成证明
```

### 7.2 集合/逻辑推理

```
目标涉及集合运算
│
├─► 是否为集合等式 (A = B)？
│   ├─► 是 → ext x → 证明 x ∈ A ↔ x ∈ B
│   └─► 否 → 继续
│
├─► 是否为子集关系 (A ⊆ B)？
│   ├─► 是 → intro x → intro h → 从 x ∈ A 证明 x ∈ B
│   └─► 否 → 继续
│
├─► 是否可用集合代数简化？
│   ├─► 是 → set_calc / set_simp
│   └─► 否 → 继续
│
└─► 展开集合定义 → 使用逻辑策略
```

---

## 8. 策略速查表

| 目标形式 | Coq策略 | Lean 4策略 | 说明 |
|---------|---------|-----------|------|
| True | `trivial` | `trivial` | 真命题 |
| False | `contradiction` | `contradiction` | 假命题/矛盾 |
| P → Q | `intro` | `intro` | 蕴含引入 |
| ∀ x, P x | `intro` | `intro` | 全称引入 |
| ∃ x, P x | `exists` | `use`/`exists` | 存在引入 |
| P ∧ Q | `split` | `constructor` | 合取引入 |
| P ∨ Q | `left`/`right` | `left`/`right` | 析取引入 |
| P ↔ Q | `split` | `constructor` | 等价引入 |
| ¬P | `intro` | `intro` | 否定引入 |
| t = t | `reflexivity` | `rfl`/`reflexivity` | 自反性 |
| 归纳类型 | `induction` | `induction` | 归纳 |
| 简化 | `simpl` | `simp` | 简化表达式 |
| 重写 | `rewrite` | `rw` | 用等式重写 |
| 应用定理 | `apply` | `apply` | 应用蕴含 |
| 精确匹配 | `exact` | `exact` | 精确匹配 |
| 线性算术 | `omega` | `linarith` | 自动算术 |
| 通用自动 | `auto` | `aesop` | 通用自动证明 |

---

## 9. 常见证明模式

### 模式1: 蕴含链证明

```lean
-- 证明: P → Q → R → S
example : P → Q → R → S := by
  intro hp hq hr     -- 引入所有前提
  -- 使用假设逐步推导
  have hq' : Q' := f hp hq
  have hr' : R' := g hq' hr
  exact h hr'        -- 证明目标
```

### 模式2: 双向证明

```lean
-- 证明: P ↔ Q
example : P ↔ Q := by
  constructor        -- 分成两个方向
  · -- 证明 P → Q
    intro hp
    -- ... 证明Q
  · -- 证明 Q → P
    intro hq
    -- ... 证明P
```

### 模式3: 分类讨论

```lean
-- 基于析取假设的分类讨论
example : P ∨ Q → R := by
  intro h
  cases h with
  | inl hp => -- 情况P成立
    -- 使用hp证明R
  | inr hq => -- 情况Q成立
    -- 使用hq证明R
```

### 模式4: 归纳模式

```lean
-- 列表长度性质证明
theorem length_nonneg {α} (l : List α) : length l ≥ 0 := by
  induction l with
  | nil =>        -- 基础: l = []
    simp [length] -- length [] = 0 ≥ 0
  | cons x xs ih => -- 归纳: l = x::xs, 假设 ih: length xs ≥ 0
    simp [length] -- length (x::xs) = 1 + length xs
    omega         -- 1 + length xs ≥ 0 (由 ih)
```

---

## 10. 调试证明技巧

### 当证明卡住时

1. **检查目标状态**: 使用 `show` 或 `?_` 显式标记子目标
2. **简化假设**: 使用 `simp at *` 简化所有假设
3. **清理上下文**: 使用 `clear` 移除无关假设，减少干扰
4. **分解复杂假设**: 使用 `cases` 解构复杂结构
5. **分步验证**: 使用 `have` 引入中间步骤，验证每步正确性

### 常见错误与解决

| 错误现象 | 可能原因 | 解决方案 |
|---------|---------|---------|
| apply失败 | 目标与定理结论不匹配 | 使用 `have` 建立中间桥梁 |
| rewrite失败 | 找不到匹配子项 | 使用 `set` 命名子项，或检查等式方向 |
| induction后无法证明 | 归纳假设不够强 | 使用强归纳或修改定理陈述 |
| 循环简化 | simp规则循环 | 使用 `simp only [特定规则]` |
| 类型不匹配 | 隐含参数推断错误 | 使用 `@` 显式提供参数 |

---

> **核心原则**: 证明是目标导向的编程。始终关注当前目标，选择合适的策略将其分解为更简单的子目标，直到可用基本公理解决。

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**适用工具**: Coq, Lean 4, Isabelle/HOL
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
