# 指称语义 (Denotational Semantics)

> **难度**: L6 | **预估学习时间**: 10-12小时
> **参考**: Scott & Strachey (1971), Stoy (1977), Winskel (1993)

---

## 1. 指称语义核心思想

指称语义将程序**映射到数学对象**（通常是函数），程序的意义就是其计算函数的数学定义。

### 1.1 基本原理

```text
语法 (Syntax)              语义 (Semantics)
    │                         │
    │   指称函数 ⟦·⟧           │
    ▼                         ▼
  程序 P  ─────────────────→  数学对象 ⟦P⟧

例如：⟦x := x + 1⟧ = λσ. σ[x ↦ σ(x) + 1]
```

### 1.2 与操作语义的区别

| 维度 | 操作语义 | 指称语义 |
|:-----|:---------|:---------|
| **抽象级别** | 计算过程 | 数学函数 |
| **组合性** | 非组合（需考虑上下文） | 完全组合 |
| **证明方法** | 归纳于推导 | 等式推理 |
| **并发** | 自然支持 | 需要额外构造 |
| **实现** | 解释器 | 编译器优化 |

---

## 2. 数学基础

### 2.1 完全偏序 (Complete Partial Orders, CPO)

**定义**: 偏序集 $(D, \sqsubseteq)$ 是CPO如果：

1. 有最小元 $\bot$（底部）
2. 每个有向子集有上确界

```text
CPO示例：
         ⊤ (终止)
        / \
      ⋮   ⋮  (计算路径)
        \ /
         ⊥ (未定义/非终止)
```

### 2.2 连续函数

函数 $f: D \to E$ 是**连续的**如果：

1. 单调: $x \sqsubseteq y \Rightarrow f(x) \sqsubseteq f(y)$
2. 保持上确界: $f(\bigsqcup X) = \bigsqcup f(X)$

**符号**: $[D \to E]$ 表示从D到E的连续函数空间。

### 2.3 不动点定理 (Knaster-Tarski)

**定理**: 设 $f: D \to D$ 是CPO上的连续函数，则：

$$
\text{fix}(f) = \bigsqcup_{n \geq 0} f^n(\bot)
$$

是最小不动点：$f(\text{fix}(f)) = \text{fix}(f)$

**直观理解**：

```text
fix(f) = ⊥ ⊔ f(⊥) ⊔ f(f(⊥)) ⊔ f(f(f(⊥))) ⊔ ...

迭代过程：
  f⁰(⊥) = ⊥          （完全未定义）
  f¹(⊥) = f(⊥)       （一次迭代的信息）
  f²(⊥) = f(f(⊥))    （两次迭代的信息）
  ...
  ⨆ fⁿ(⊥) = 极限（完全定义，如果收敛）
```

---

## 3. 简单语言的指称语义

### 3.1 表达式语义

**语义域**:

- 整数值: $\mathbb{Z}$
- 布尔值: $\mathbb{B} = \{\text{true}, \text{false}\}$
- 值域: $V = \mathbb{Z} \cup \mathbb{B}$
- 存储: $\Sigma = \text{Var} \to V$

**表达式指称函数**: $\mathcal{E}: \text{Exp} \to (\Sigma \to V)$

```text
⟦n⟧σ = n                           (常量)
⟦x⟧σ = σ(x)                        (变量)
⟦e₁ + e₂⟧σ = ⟦e₁⟧σ + ⟦e₂⟧σ        (加法)
⟦e₁ < e₂⟧σ = ⟦e₁⟧σ < ⟦e₂⟧σ        (比较)
```

### 3.2 语句语义

**语义域**: $\mathcal{C}: \text{Com} \to (\Sigma \to \Sigma_\bot)$

其中 $\Sigma_\bot = \Sigma \cup \{\bot\}$，$\bot$ 表示非终止。

#### 赋值语句

$$
\mathcal{C}⟦x := e⟧ = \lambda \sigma \in \Sigma. \sigma[x \mapsto \mathcal{E}⟦e⟧\sigma]
$$

#### 顺序语句

$$
\mathcal{C}⟦S_1; S_2⟧ = \mathcal{C}⟦S_2⟧ \circ \mathcal{C}⟦S_1⟧
$$

或使用Kleisli组合（处理非终止）：

$$
\mathcal{C}⟦S_1; S_2⟧ = \lambda \sigma. \begin{cases}
\bot & \text{if } \mathcal{C}⟦S_1⟧\sigma = \bot \\
\mathcal{C}⟦S_2⟧(\mathcal{C}⟦S_1⟧\sigma) & \text{otherwise}
\end{cases}
$$

#### 条件语句

$$
\mathcal{C}⟦\text{if } b \text{ then } S_1 \text{ else } S_2⟧ = \lambda \sigma. \begin{cases}
\mathcal{C}⟦S_1⟧\sigma & \text{if } \mathcal{E}⟦b⟧\sigma = \text{true} \\
\mathcal{C}⟦S_2⟧\sigma & \text{if } \mathcal{E}⟦b⟧\sigma = \text{false} \\
\bot & \text{otherwise}
\end{cases}
$$

### 3.3 While循环（核心难点）

While循环需要不动点：

$$
\mathcal{C}⟦\text{while } b \text{ do } S⟧ = \text{fix}(\lambda \phi. \lambda \sigma. \begin{cases}
\sigma & \text{if } \mathcal{E}⟦b⟧\sigma = \text{false} \\
\phi(\mathcal{C}⟦S⟧\sigma) & \text{if } \mathcal{E}⟦b⟧\sigma = \text{true} \\
\bot & \text{otherwise}
\end{cases})
$$

**解释**：

- $\phi$ 是递归的语义函数
- fix找到最小的满足递归方程的函数
- 如果循环永不终止，结果为$\bot$

---

## 4. C语言的指称语义挑战

### 4.1 内存模型

C语言需要复杂的**存储模型**：

```text
Store = Loc →ᵥ Val    (部分函数，有效位置到值)
Env = Var → Loc       (变量到位置)
Mem = (Store, Alloc)  (存储 + 分配状态)
```

**指针语义**:

```text
⟦&x⟧(ρ, σ) = ρ(x)          （取地址：查找变量位置）
⟦*p⟧(ρ, σ) = σ(⟦p⟧(ρ, σ))  （解引用：查找位置值）
```

### 4.2 未定义行为处理

C标准中的未定义行为(UB)在指称语义中如何处理？

**方案1**: 错误值

```text
Val = ℤ ∪ Bool ∪ Loc ∪ {error}

⟦1/0⟧σ = error
```

**方案2**: 偏函数

```text
语义函数在某些输入上无定义
```

**方案3**: 集合论（Chibi/Scheme 48风格）

```text
允许任何行为：⟦UB⟧ = {v | v ∈ Val}  （完全非确定）
```

### 4.3 CompCert的语义选择

CompCert使用**小步骤操作语义**而非指称语义，原因是：

1. 更容易表达**非确定性**（如求值顺序）
2. 更自然地建模**内存副作用**
3. 与编译器IR转换的对应更清晰

---

## 5. 域方程与递归类型

### 5.1 递归域方程

对于递归类型，需要解**域方程**：

$$
D \cong F(D)
$$

其中 $F$ 是域构造子。

### 5.2 解的存在性 (Scott)

**定理**: 如果 $F$ 是局部连续的域构造子，则方程有解。

**构造方法**: 逆极限 (Inverse Limit)

```text
D₀ = {⊥}
D₁ = F(D₀)
D₂ = F(D₁)
...
D∞ = 逆极限(Dₙ)

则 D∞ ≅ F(D∞)
```

### 5.3 C语言中的递归结构

```c
struct Node {
    int value;
    struct Node* next;  // 递归类型
};
```

类型方程：

```text
Node ≅ Int × (Node ∪ {null})
```

---

## 6. 指称语义的验证应用

### 6.1 程序等价性

**定义**: $S_1 \sim S_2$ 如果 $\mathcal{C}⟦S_1⟧ = \mathcal{C}⟦S_2⟧$

**示例**: 证明优化正确

```c
// 优化前
x = e; x = f;

// 优化后（如果x不在f中自由出现）
x = f;
```

证明:

```text
⟦x=e; x=f⟧ = ⟦x=f⟧ ∘ ⟦x=e⟧
           = (λσ. σ[x↦⟦f⟧σ]) ∘ (λσ. σ[x↦⟦e⟧σ])
           = λσ. (σ[x↦⟦e⟧σ])[x↦⟦f⟧(σ[x↦⟦e⟧σ])]
           = λσ. σ[x↦⟦f⟧σ]      （x不在f中自由出现）
           = ⟦x=f⟧
```

### 6.2 完全抽象 (Full Abstraction)

**定义**: 语义是完全抽象的如果：

$$
⟦P₁⟧ = ⟦P₂⟧ \iff P₁ \text{ 和 } P₂ \text{ 在上下文中不可区分}
$$

**Milner的完全抽象问题**: PCF语言的指称语义直到加入**并行或** (parallel or) 才完全抽象。

### 6.3 与操作语义的等价性

**目标**: 证明指称语义与操作语义一致。

**adequacy (适切性)**:

```text
如果 ⟦e⟧ = v，则 e →* v
```

**computational adequacy (计算适切性)**:

```text
e 终止 ⟺ ⟦e⟧ ≠ ⊥
```

---

## 7. 现代发展

### 7.1 概率程序语义

对于随机算法：

```text
⟦x ~ Bernoulli(p)⟧ : Σ → D(Σ)

其中 D(Σ) 是Σ上的概率分布
```

### 7.2 微分语义

机器学习中的可微分编程：

```text
⟦reverse-mode AD⟧ : (ℝⁿ → ℝᵐ) → (ℝⁿ → (ℝᵐ → ℝⁿ))
```

### 7.3 博弈语义（Game Semantics）

Abramsky等发展的**交互式指称语义**：

```text
程序意义 = 与环境的博弈策略

        环境提问
           ↓
    ┌─────────────┐
    │   程序      │ → 回答/提问 →
    └─────────────┘
           ↑
        环境回答
```

---

## 8. 工具实现

### 8.1 HOL/HOL4中的指称语义

```sml
(* While循环的不动点定义 *)
val while_def = Define `
  WHILE b c = LFP (\f s. if b s then f (c s) else s)
`;

(* 证明连续性 *)
val WHILE_continuous = store_thm("WHILE_continuous",
  ``continuous (\f s. if b s then f (c s) else s)``,
  ...
);
```

### 8.2 Isabelle/HOL

```isabelle
definition while :: "(state ⇒ bool) ⇒ (state ⇒ state) ⇒ state ⇒ state" where
  "while b body = lfp (λf s. if b s then f (body s) else s)"
```

---

## 9. 对比总结

| 问题 | 操作语义 | 指称语义 |
|:-----|:---------|:---------|
| **循环语义** | 重写规则 | 最小不动点 |
| **非终止** | 无限推导链 | ⊥ 值 |
| **等价证明** | 互模拟 | 等式推理 |
| **编译器优化** | 难 | 自然 |
| **类型安全** | 进展+保持 | 适切性 |

---

## 10. 参考文献

1. Scott, D.S., Strachey, C. (1971). "Towards a Mathematical Semantics for Computer Languages"
2. Stoy, J.E. (1977). "Denotational Semantics: The Scott-Strachey Approach"
3. Winskel, G. (1993). "The Formal Semantics of Programming Languages"
4. Gunter, C.A. (1992). "Semantics of Programming Languages"
5. Abramsky, S., Jagadeesan, R. (1994). "New Foundations for the Geometry of Interaction"
6. Scott, D.S. (1976). "Data Types as Lattices"

---

> **关联文档**: [01_Operational_Semantics](./01_Operational_Semantics.md) | [03_Axiomatic_Semantics_Hoare](./03_Axiomatic_Semantics_Hoare.md) | [博弈语义理论](../01_Game_Semantics/01_Game_Semantics_Theory.md)
