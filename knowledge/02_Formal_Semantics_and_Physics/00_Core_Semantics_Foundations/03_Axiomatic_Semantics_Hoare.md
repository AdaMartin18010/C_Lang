# 公理语义与Hoare逻辑 (Axiomatic Semantics)

> **难度**: L5 | **预估学习时间**: 8-10小时
> **参考**: Hoare (1969), Dijkstra (1975), Apt (1981), Winskel (1993)

---

## 1. 公理语义的核心思想

公理语义通过**逻辑断言**描述程序行为：程序执行前状态满足前置条件，执行后状态满足后置条件。

### 1.1 Hoare三元组

$$
\{P\} \, S \, \{Q\}
$$

- **P**: 前置条件 (Precondition) - 执行前必须为真的断言
- **S**: 程序语句
- **Q**: 后置条件 (Postcondition) - 执行后必须为真的断言

### 1.2 部分正确性与完全正确性

| 符号 | 含义 | 说明 |
|:-----|:-----|:-----|
| $\{P\} \, S \, \{Q\}$ | **部分正确性** | 如果P成立且S终止，则Q成立 |
| $[P] \, S \, [Q]$ | **完全正确性** | P成立蕴含S终止且Q成立 |

**关系**: 完全正确性 = 部分正确性 + 终止性

---

## 2. Hoare逻辑推理规则

### 2.1 基本规则

#### 空语句 (Skip)

$$
\frac{}{\{P\} \, \text{skip} \, \{P\}}
$$

#### 赋值语句 (Assignment)

$$
\frac{}{\{Q[e/x]\} \, x := e \, \{Q\}}
$$

**解释**: 后置条件Q中的x被替换为表达式e，得到前置条件。

**示例**:

```c
{x + 1 > 0} x := x + 1; {x > 0}
```

#### 顺序组合 (Sequence)

$$
\frac{\{P\} \, S_1 \, \{R\} \quad \{R\} \, S_2 \, \{Q\}}
     {\{P\} \, S_1; S_2 \, \{Q\}}
$$

#### 条件语句 (Conditional)

$$
\frac{\{P \land B\} \, S_1 \, \{Q\} \quad \{P \land \neg B\} \, S_2 \, \{Q\}}
     {\{P\} \, \text{if } B \text{ then } S_1 \text{ else } S_2 \, \{Q\}}
$$

### 2.2 While循环规则（部分正确性）

$$
\frac{\{I \land B\} \, S \, \{I\}}
     {\{I\} \, \text{while } B \text{ do } S \, \{I \land \neg B\}}
$$

**I** 称为**循环不变式 (Loop Invariant)**：

- 进入循环前成立
- 每次迭代后保持
- 循环终止时，结合$\neg B$可推出后置条件

### 2.3 后果规则 (Consequence)

$$
\frac{P \Rightarrow P' \quad \{P'\} \, S \, \{Q'\} \quad Q' \Rightarrow Q}
     {\{P\} \, S \, \{Q\}}
$$

---

## 3. 实例证明

### 3.1 阶乘计算

```c
{ n >= 0 }
i = 0;
f = 1;
while (i < n) {
    i = i + 1;
    f = f * i;
}
{ f == n! }
```

**循环不变式**: $I \equiv (f == i! \land i \leq n)$

**证明步骤**:

```text
1. { n >= 0 }
   i = 0;
   { n >= 0 ∧ i == 0 }

2. f = 1;
   { n >= 0 ∧ i == 0 ∧ f == 1 }
   ⟹ { f == i! ∧ i <= n }      (因为 1 == 0!)

3. while (i < n) {
       { f == i! ∧ i <= n ∧ i < n }
       i = i + 1;
       { f == (i-1)! ∧ i-1 < n }
       f = f * i;
       { f == i! ∧ i <= n }      (因为 f*i == (i-1)!*i == i!)
   }

4. { f == i! ∧ i <= n ∧ !(i < n) }
   ⟹ { f == i! ∧ i == n }
   ⟹ { f == n! }
```

### 3.2 数组求和

```c
{ n >= 0 }
sum = 0;
i = 0;
while (i < n) {
    sum = sum + a[i];
    i = i + 1;
}
{ sum == Σ(j=0 to n-1) a[j] }
```

**循环不变式**: $I \equiv (sum == \Sigma(j=0 \text{ to } i-1) a[j] \land 0 \leq i \leq n)$

---

## 4. C语言特定扩展

### 4.1 指针与内存

分离逻辑 (Separation Logic) 扩展：

$$
\{x \mapsto -\} *x = e \, \{x \mapsto e\}
$$

**分离合取** ($*$): 断言内存不相交区域。

### 4.2 函数契约

```c
/*@ requires n >= 0;
    decreases n;        // 终止度量
    assigns \nothing;   // 不修改全局状态
    ensures \result == fact(n);
*/
int factorial(int n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}
```

### 4.3 未定义行为处理

C中的未定义行为破坏任何后置条件：

```text
{ true }
x = INT_MAX;
x = x + 1;     // 未定义行为！
{ false }      // 可以推出任何结论（爆炸原理）
```

---

## 5. 完全正确性与终止性

### 5.1 循环变式 (Loop Variant)

证明终止需要**良基关系**：

$$
\frac{\{P \land B \land V = v_0\} \, S \, \{P \land V < v_0\} \quad V \geq 0}
     {[P] \, \text{while } B \text{ do } S \, [P \land \neg B]}
$$

**V** 是取值于良基集（如自然数）的表达式，每次迭代严格递减。

### 5.2 示例：带终止证明的阶乘

```c
[n >= 0]
while (n > 0) {
    // 变式: n
    // n > 0 保证循环条件满足
    // n-- 使 n 递减
    f = f * n;
    n = n - 1;
}
[n == 0]
```

---

## 6. 最弱前置条件 (Weakest Precondition)

### 6.1 Dijkstra的谓词转换器

**定义**: $wp(S, Q)$ 是使 $\{wp(S, Q)\} \, S \, \{Q\}$ 成立的最弱（最一般）条件。

**计算规则**:

| 语句 S | wp(S, Q) |
|:-------|:---------|
| skip | Q |
| x := e | Q[e/x] |
| S₁; S₂ | wp(S₁, wp(S₂, Q)) |
| if B then S₁ else S₂ | $(B \land wp(S_1, Q)) \lor (\neg B \land wp(S_2, Q))$ |
| while B do S | 最小不动点解 |

### 6.2 最弱自由前置条件

$$wlp(S, Q) \text{ - 不保证终止，仅保证部分正确性}$$

---

## 7. 自动验证工具

### 7.1 工具对比

| 工具 | 方法 | 适用语言 | 自动化程度 |
|:-----|:-----|:---------|:-----------|
| **Why3** | ML + SMT | C/OCaml | 半自动 |
| **Frama-C/WP** | Hoare逻辑 | C | 半自动 |
| **Dafny** | 谓词转换器 | Dafny | 高 |
| **VST** | 分离逻辑 | C (CompCert) | 交互式 |
| **Prusti** | 合约 | Rust | 高 |

### 7.2 Frama-C验证示例

```c
/*@ requires \valid(a + (0..n-1));
    requires n >= 0;
    assigns \nothing;
    ensures \result == (\sum(0, n-1, \lambda integer i; a[i]));
*/
int array_sum(int *a, int n) {
    int sum = 0;
    /*@ loop invariant 0 <= i <= n;
        loop invariant sum == \sum(0, i-1, \lambda integer k; a[k]);
        loop assigns i, sum;
        loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum;
}
```

---

## 8. 可靠性与完备性

### 8.1 可靠性 (Soundness)

**定理**: 如果 $\vdash \{P\} \, S \, \{Q\}$ 可推导，则 $\models \{P\} \, S \, \{Q\}$ 成立。

**含义**: 证明系统中推导出的结论在实际执行中确实成立。

### 8.2 完备性 (Completeness)

**问题**: 是否所有有效的三元组都可证明？

**答案**:

- 相对完备（Cook完备性）：在一阶逻辑定理证明器的假设下完备
- 绝对不完备：停机问题不可判定

**Cook完备性定理**:

```text
如果 |= {P} S {Q}，则在假设可以证明所有有效的断言蕴涵P' => P下，
可以推导出 |- {P} S {Q}
```

### 8.3 哥德尔不完备性的影响

由于一阶算术的不完备性：

- 无法机械判定所有循环不变式
- 需要人工提供或启发式推断

---

## 9. 现代扩展

### 9.1 分离逻辑 (Separation Logic)

Reynolds (2002) 为指针程序引入：

**基本断言**:

- $emp$: 空堆
- $x \mapsto v$: 堆中位置x存储值v
- $P * Q$: 分离合取（P和Q描述不相交堆）

**框架规则 (Frame Rule)**:
$$
\frac{\{P\} \, S \, \{Q\}}
     {\{P * R\} \, S \, \{Q * R\}}
$$

条件：S不修改R中的变量。

### 9.2 并发分离逻辑

O'Hearn (2007) 扩展：

$$
\frac{\{P_1\} \, S_1 \, \{Q_1\} \quad \{P_2\} \, S_2 \, \{Q_2\}}
     {\{P_1 * P_2\} \, S_1 \, || \, S_2 \, \{Q_1 * Q_2\}}
$$

### 9.3 Iris：高阶并发分离逻辑

Iris框架支持：

- 高阶函数
- 并发
- 原子操作
- 模块验证

---

## 10. 与CompCert/VST的关联

### 10.1 VST中的Hoare逻辑

Verified Software Toolchain 使用**并发分离逻辑**：

```coq
(* VST中的函数规范 *)
Definition fact_spec : ident × funspec :=
  DECLARE _factorial
    WITH n: Z
    PRE [ tint ]
      PROP (0 <= n)
      PARAMS (Vint (Int.repr n))
      GLOBALS ()
      SEP ()
    POST [ tint ]
      PROP ()
      RETURN (Vint (Int.repr (Zfact n)))
      SEP ().
```

### 10.2 从C到逻辑的映射

```text
C程序
  │
  ▼
Clight (CompCert IR)
  │
  ▼
Hoare三元组 (VST)
  │
  ▼
Coq证明
  │
  ▼
机器可验证的正确性证明
```

---

## 11. 引用与延伸阅读

### 经典论文

1. **Hoare, C.A.R.** (1969). "An Axiomatic Basis for Computer Programming"
   - 提出Hoare逻辑的奠基论文

2. **Dijkstra, E.W.** (1975). "Guarded Commands, Nondeterminacy and Formal Derivation"
   - 最弱前置条件方法

3. **Apt, K.R.** (1981). "Ten Years of Hoare's Logic: A Survey"
   - 早期系统综述

4. **Reynolds, J.C.** (2002). "Separation Logic: A Logic for Shared Mutable Data Structures"
   - 分离逻辑奠基

### 现代资源

1. **VST Documentation**: <https://vst.cs.princeton.edu/>
2. **Frama-C/WP Tutorial**: <https://frama-c.com/html/acsl_tutorial.html>
3. **Software Foundations (Vol 2)**: "Programming Language Foundations"

---

> **关联文档**: [VST分离逻辑实战](../11_CompCert_Verification/02_VST_Separation_Logic_Practical.md) | [01_Operational_Semantics](./01_Operational_Semantics.md) | [02_Denotational_Semantics](./02_Denotational_Semantics.md)


---

## 深入理解

### 核心概念

本主题的核心概念包括：基础理论、实现机制、实际应用。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 学习建议

1. 先理解基础概念
2. 再进行实践练习
3. 最后深入源码

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
