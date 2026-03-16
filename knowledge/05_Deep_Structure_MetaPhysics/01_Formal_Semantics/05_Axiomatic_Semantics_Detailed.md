# 公理语义详解 (Axiomatic Semantics Detailed)

## 目录

- [公理语义详解 (Axiomatic Semantics Detailed)](#公理语义详解-axiomatic-semantics-detailed)
  - [目录](#目录)
  - [1. Hoare 逻辑基础](#1-hoare-逻辑基础)
    - [1.1 历史背景](#11-历史背景)
    - [1.2 核心思想](#12-核心思想)
  - [2. 霍尔三元组 (Hoare Triple)](#2-霍尔三元组-hoare-triple)
    - [2.1 语法定义](#21-语法定义)
    - [2.2 语义解释](#22-语义解释)
  - [3. 推理规则系统](#3-推理规则系统)
    - [3.1 公理模式](#31-公理模式)
      - [3.1.1 赋值公理 (Assignment Axiom)](#311-赋值公理-assignment-axiom)
      - [3.1.2 空语句公理 (Skip Axiom)](#312-空语句公理-skip-axiom)
    - [3.2 推理规则](#32-推理规则)
      - [3.2.1 顺序组合 (Sequential Composition)](#321-顺序组合-sequential-composition)
      - [3.2.2 条件语句 (Conditional)](#322-条件语句-conditional)
      - [3.2.3 While 循环 (Partial Correctness)](#323-while-循环-partial-correctness)
      - [3.2.4 While 循环 (Total Correctness)](#324-while-循环-total-correctness)
    - [3.3 辅助规则](#33-辅助规则)
      - [3.3.1 强化前置条件 (Strengthening Precondition)](#331-强化前置条件-strengthening-precondition)
      - [3.3.2 弱化后置条件 (Weakening Postcondition)](#332-弱化后置条件-weakening-postcondition)
      - [3.3.3 合取与析取规则](#333-合取与析取规则)
  - [4. 循环不变式 (Loop Invariants)](#4-循环不变式-loop-invariants)
    - [4.1 定义与性质](#41-定义与性质)
    - [4.2 不变式推导方法](#42-不变式推导方法)
      - [方法 1: 从后向前推导](#方法-1-从后向前推导)
      - [方法 2: 归纳构造](#方法-2-归纳构造)
      - [方法 3: 候选不变式细化](#方法-3-候选不变式细化)
  - [5. 最弱前置条件 (Weakest Precondition)](#5-最弱前置条件-weakest-precondition)
    - [5.1 定义](#51-定义)
    - [5.2 计算规则](#52-计算规则)
    - [5.3 最弱自由前置条件](#53-最弱自由前置条件)
    - [5.4 最强后置条件](#54-最强后置条件)
  - [6. 程序验证实例](#6-程序验证实例)
    - [实例 1: 简单赋值序列](#实例-1-简单赋值序列)
    - [实例 2: 条件语句验证](#实例-2-条件语句验证)
    - [实例 3: 阶乘计算 (循环)](#实例-3-阶乘计算-循环)
    - [实例 4: 数组求和](#实例-4-数组求和)
    - [实例 5: 二分查找 (复杂循环)](#实例-5-二分查找-复杂循环)
  - [附录：Hoare 逻辑规则总表](#附录hoare-逻辑规则总表)

---

## 1. Hoare 逻辑基础

### 1.1 历史背景

Hoare 逻辑由 C.A.R. Hoare 于 1969 年提出，是形式化验证命令式程序正确性的公理系统。

### 1.2 核心思想

```
通过前置条件和后置条件描述程序行为：
{P} C {Q}

含义: 如果前置条件 P 在执行 C 前成立，
      且 C 终止，则后置条件 Q 成立
```

---

## 2. 霍尔三元组 (Hoare Triple)

### 2.1 语法定义

**定义 2.1.1** (霍尔三元组)：

```
{P} C {Q}

其中:
- P: 前置条件 (Precondition), 断言
- C: 程序/命令 (Command)
- Q: 后置条件 (Postcondition), 断言
```

### 2.2 语义解释

**定义 2.2.1** (部分正确性)：

```
⊨_{par} {P} C {Q}  ⇔
∀σ. P(σ) ∧ (C,σ) ↓ σ' → Q(σ')

含义: 若 P 成立且 C 终止于 σ'，则 Q 成立
```

**定义 2.2.2** (完全正确性)：

```
⊨_{tot} {P} C {Q}  ⇔
∀σ. P(σ) → (∃σ'. (C,σ) ↓ σ' ∧ Q(σ'))

含义: P 成立保证 C 终止且 Q 成立
```

**定理 2.2.1** (正确性关系)：

```
⊨_{tot} {P} C {Q} → ⊨_{par} {P} C {Q}
```

---

## 3. 推理规则系统

### 3.1 公理模式

#### 3.1.1 赋值公理 (Assignment Axiom)

```
──────────────────────────── (Assign)
{Q[e/x]} x := e {Q}

或等价地:
{Q[E/x]} x := E {Q}

含义: 执行 x:=E 后 Q 成立，当且仅当 Q[E/x] 在执行前成立
```

**示例**：

```
{x + 1 > 0} x := x + 1 {x > 0}
{y = 5} x := 3 {y = 5}
```

#### 3.1.2 空语句公理 (Skip Axiom)

```
────────────── (Skip)
{P} skip {P}
```

### 3.2 推理规则

#### 3.2.1 顺序组合 (Sequential Composition)

```
{P} C₁ {R}    {R} C₂ {Q}
─────────────────────────── (Seq)
      {P} C₁; C₂ {Q}
```

#### 3.2.2 条件语句 (Conditional)

```
{P ∧ B} C₁ {Q}    {P ∧ ¬B} C₂ {Q}
────────────────────────────────── (If)
     {P} if B then C₁ else C₂ {Q}
```

#### 3.2.3 While 循环 (Partial Correctness)

```
      {P ∧ B} C {P}
──────────────────────────────── (While)
{P} while B do C {P ∧ ¬B}

P 称为循环不变式 (Loop Invariant)
```

#### 3.2.4 While 循环 (Total Correctness)

```
      {P ∧ B ∧ E = n} C {P ∧ E < n}
      P ∧ B → E ≥ 0
──────────────────────────────────── (While-Tot)
      {P} while B do C {P ∧ ¬B}

E 称为变体函数 (Variant Function), 保证终止
```

### 3.3 辅助规则

#### 3.3.1 强化前置条件 (Strengthening Precondition)

```
P' → P    {P} C {Q}
─────────────────── (Pre)
     {P'} C {Q}
```

#### 3.3.2 弱化后置条件 (Weakening Postcondition)

```
{P} C {Q}    Q → Q'
─────────────────── (Post)
     {P} C {Q'}
```

#### 3.3.3 合取与析取规则

```
{P₁} C {Q₁}    {P₂} C {Q₂}
─────────────────────────── (Conj)
  {P₁ ∧ P₂} C {Q₁ ∧ Q₂}

{P₁} C {Q₁}    {P₂} C {Q₂}
─────────────────────────── (Disj)
  {P₁ ∨ P₂} C {Q₁ ∨ Q₂}
```

---

## 4. 循环不变式 (Loop Invariants)

### 4.1 定义与性质

**定义 4.1.1** (循环不变式)：

```
I 是 while B do C 的循环不变式，如果：

(1) 初始化: 前置条件 → I
(2) 保持: {I ∧ B} C {I}
(3) 终止: I ∧ ¬B → 后置条件
```

### 4.2 不变式推导方法

#### 方法 1: 从后向前推导

```
目标: 循环结束后 R 成立
尝试: I = R 或 R 的弱化形式
验证: I ∧ B → 循环体保持 I
```

#### 方法 2: 归纳构造

```
基础: I₀ = 前置条件
归纳: I_{n+1} = I_n ∨ wp(C, I_n)  (最弱前置条件)
极限: I = ∪ I_n
```

#### 方法 3: 候选不变式细化

```
初始候选: 变量范围、数据结构的结构性质
迭代验证: 检查保持性，必要时添加合取项
```

---

## 5. 最弱前置条件 (Weakest Precondition)

### 5.1 定义

**定义 5.1.1** (最弱前置条件)：

```
wp(C, Q) = 最弱断言 P 使得 {P} C {Q}

"最弱" = 最一般的前置条件
性质: {P} C {Q} ⇔ P → wp(C, Q)
```

### 5.2 计算规则

```
wp(skip, Q) = Q

wp(x := E, Q) = Q[E/x]

wp(C₁; C₂, Q) = wp(C₁, wp(C₂, Q))

wp(if B then C₁ else C₂, Q) =
  (B → wp(C₁, Q)) ∧ (¬B → wp(C₂, Q))

wp(while B do C, Q) =
  ∃k. I_k  其中 I₀ = Q ∧ ¬B, I_{k+1} = I_k ∨ (B ∧ wp(C, I_k))
```

### 5.3 最弱自由前置条件

```
wlp(skip, Q) = Q

wlp(x := E, Q) = Q[E/x]

wlp(C₁; C₂, Q) = wlp(C₁, wlp(C₂, Q))

wlp(if B then C₁ else C₂, Q) =
  (B → wlp(C₁, Q)) ∧ (¬B → wlp(C₂, Q))

wlp(while B do C, Q) =
  νZ. (¬B → Q) ∧ (B → wlp(C, Z))   (最大不动点)
```

### 5.4 最强后置条件

```
sp(P, skip) = P

sp(P, x := E) = ∃v. P[v/x] ∧ x = E[v/x]

sp(P, C₁; C₂) = sp(sp(P, C₁), C₂)

sp(P, if B then C₁ else C₂) =
  sp(P ∧ B, C₁) ∨ sp(P ∧ ¬B, C₂)
```

---

## 6. 程序验证实例

### 实例 1: 简单赋值序列

**程序**：

```c
{x = a ∧ y = b}
t := x;
x := y;
y := t;
{x = b ∧ y = a}
```

**证明**：

```
1. {x = a ∧ y = b}                    (前置条件)
2. {y = b ∧ x = a}                    (等价变形)
3. {y = b ∧ t = a}[t/x]               (准备赋值)
4. t := x;                            (赋值)
5. {y = b ∧ t = a}
6. {x = b ∧ t = a}[y/x]               (准备赋值)
7. x := y;                            (赋值)
8. {x = b ∧ t = a}
9. {x = b ∧ y = a}[t/y]               (准备赋值)
10. y := t;                           (赋值)
11. {x = b ∧ y = a}                   (后置条件) ✓
```

---

### 实例 2: 条件语句验证

**程序**：求绝对值

```c
{x = a}
if x < 0 then
    x := -x
else
    skip
{x = |a|}
```

**证明**：

```
左分支:
  {x = a ∧ x < 0}
  {-x = |a|}[ -x / x ]  因为 x<0 时 -x = |x| = |a|
  x := -x
  {x = |a|}

右分支:
  {x = a ∧ x ≥ 0}
  {x = |a|}             因为 x≥0 时 x = |x| = |a|
  skip
  {x = |a|}

由 (If) 规则:
  {x = a} if x < 0 then x := -x else skip {x = |a|} ✓
```

---

### 实例 3: 阶乘计算 (循环)

**程序**：

```c
{n ≥ 0}
i := 0;
f := 1;
while i < n do
    i := i + 1;
    f := f * i
{f = n!}
```

**循环不变式**：`I ≡ f = i! ∧ i ≤ n`

**证明**：

**初始化**：

```
{n ≥ 0}
{f = 0! ∧ 0 ≤ n}[1/f, 0/i]
i := 0;
{f = i! ∧ i ≤ n}[1/f]
f := 1;
{I}
```

**保持**（设 `{I ∧ i < n}`）：

```
{f = i! ∧ i ≤ n ∧ i < n}
{f = (i+1-1)! ∧ i+1 ≤ n}[i+1/i]
i := i + 1;
{f = (i-1)! ∧ i ≤ n}
{f * i = i! ∧ i ≤ n}[f*i/f]
f := f * i;
{f = i! ∧ i ≤ n} = {I}
```

**终止**：

```
I ∧ ¬(i < n)
= f = i! ∧ i ≤ n ∧ i ≥ n
= f = i! ∧ i = n
= f = n!
```

**完全正确性** (使用变体 `E = n - i`)：

```
{n - i ≥ 0} 由 I ∧ i < n 得 i < n, 故 n - i > 0
循环体执行后 i 增加，故 E 减小
```

---

### 实例 4: 数组求和

**程序**：

```c
{n ≥ 0}
i := 0;
s := 0;
while i < n do
    s := s + a[i];
    i := i + 1
{s = Σ_{k=0}^{n-1} a[k]}
```

**循环不变式**：`I ≡ s = Σ_{k=0}^{i-1} a[k] ∧ i ≤ n`

**证明**：

**初始化**：

```
{n ≥ 0}
{0 = Σ_{k=0}^{-1} a[k] ∧ 0 ≤ n}[0/i, 0/s]
i := 0; s := 0;
{I}
```

**保持**：

```
{I ∧ i < n}
= {s = Σ_{k=0}^{i-1} a[k] ∧ i ≤ n ∧ i < n}
{s + a[i] = Σ_{k=0}^{i} a[k] ∧ i+1 ≤ n}[s+a[i]/s]
s := s + a[i];
{s = Σ_{k=0}^{i} a[k] ∧ i+1 ≤ n}[i+1/i]
i := i + 1;
{s = Σ_{k=0}^{i-1} a[k] ∧ i ≤ n} = {I}
```

**终止**：

```
I ∧ ¬(i < n) = s = Σ_{k=0}^{i-1} a[k] ∧ i = n = s = Σ_{k=0}^{n-1} a[k] ✓
```

---

### 实例 5: 二分查找 (复杂循环)

**程序**：

```c
{sorted(a) ∧ n ≥ 0 ∧ x ∈ a[0..n-1]}
low := 0;
high := n - 1;
found := false;
while low ≤ high ∧ ¬found do
    mid := (low + high) / 2;
    if a[mid] = x then
        found := true
    else if a[mid] < x then
        low := mid + 1
    else
        high := mid - 1
{found → a[mid] = x}
```

**循环不变式**：

```
I ≡ 0 ≤ low ≤ high+1 ≤ n
    ∧ (x ∈ a[0..n-1] → x ∈ a[low..high])
    ∧ sorted(a)
```

**证明概要**：

**初始化**：

```
初始: low=0, high=n-1
I[0/low, n-1/high] = 0 ≤ 0 ≤ n ≤ n ∧ (x∈a → x∈a[0..n-1]) ✓
```

**保持**（三种情况）：

情况 1: `a[mid] = x`

```
found := true 后循环条件失效，不变式保持
```

情况 2: `a[mid] < x`

```
由 sorted(a), x 在 mid 右侧
low := mid+1 保持 x ∈ a[low..high]
```

情况 3: `a[mid] > x`

```
由 sorted(a), x 在 mid 左侧
high := mid-1 保持 x ∈ a[low..high]
```

**终止**：

```
若 found: 显然成立
若 ¬found: low > high，但 I 保证 x 应在范围内，矛盾
故必 found
```

**变体函数**（保证终止）：

```
E = high - low + 1
每次迭代至少减少 1（因 low 增或 high 减）
```

---

## 附录：Hoare 逻辑规则总表

| 规则名 | 形式 |
|--------|------|
| Skip | `{P} skip {P}` |
| Assign | `{Q[E/x]} x:=E {Q}` |
| Seq | `{P}C₁{R}, {R}C₂{Q} ⊢ {P}C₁;C₂{Q}` |
| If | `{P∧B}C₁{Q}, {P∧¬B}C₂{Q} ⊢ {P}if B then C₁ else C₂{Q}` |
| While | `{P∧B}C{P} ⊢ {P}while B do C{P∧¬B}` |
| Pre | `P'→P, {P}C{Q} ⊢ {P'}C{Q}` |
| Post | `{P}C{Q}, Q→Q' ⊢ {P}C{Q'}` |
| Consequence | `P'→P, {P}C{Q}, Q→Q' ⊢ {P'}C{Q'}` |

---

*参考文献*:

1. Hoare, C.A.R. (1969). An Axiomatic Basis for Computer Programming
2. Apt, K.R. (1981). Ten Years of Hoare's Logic: A Survey
3. Winskel, G. (1993). The Formal Semantics of Programming Languages
