# 操作语义基础 (Operational Semantics)

> **难度**: L5 | **预估学习时间**: 8-10小时
> **参考**: Plotkin (1981), Kahn (1987), Winskel (1993)

---

## 1. 操作语义概述

操作语义通过定义程序如何执行来描述程序的行为。它是一种**面向计算**的语义方法，直接描述程序状态如何转换。

### 1.1 两种主要形式

| 形式 | 英文名称 | 特点 | 适用场景 |
|:-----|:---------|:-----|:---------|
| 小步骤语义 | Small-Step (Structural Operational Semantics, SOS) | 逐步展示每个计算步骤 | 并发、异常、非终止 |
| 大步骤语义 | Big-Step (Natural Semantics) | 直接给出最终结果 | 纯计算、易于理解 |

### 1.2 为什么需要形式语义

```c
// C代码中的未定义行为如何精确定义？
int x = INT_MAX;
x = x + 1;  // 有符号溢出 → 未定义行为 (C11 §6.5/5)
```

形式语义提供：

1. **精确性**: 消除自然语言规范的歧义
2. **可验证性**: 支持数学证明
3. **工具支持**: 为解释器、分析工具提供基础

---

## 2. 小步骤操作语义 (Small-Step)

### 2.1 基本形式

小步骤语义使用**转换关系** (→) 表示单步计算：

$$
\langle \text{表达式}, \text{状态} \rangle \rightarrow \langle \text{表达式'}, \text{状态'} \rangle
$$

### 2.2 C语言表达式语义示例

**配置 (Configuration)**: $\langle e, \sigma \rangle$

- $e$: 表达式
- $\sigma$: 存储 (变量到值的映射)

**算术表达式规则**:

```text
─────────────────────  [CONST]
⟨n, σ⟩ → n

    σ(x) = v
─────────────────────  [VAR]
⟨x, σ⟩ → v

⟨e₁, σ⟩ → ⟨e₁', σ⟩
─────────────────────────────  [ADD-LEFT]
⟨e₁ + e₂, σ⟩ → ⟨e₁' + e₂, σ⟩

⟨e₂, σ⟩ → ⟨e₂', σ⟩
─────────────────────────────  [ADD-RIGHT]
⟨n + e₂, σ⟩ → ⟨n + e₂', σ⟩

─────────────────────  [ADD-EVAL]
⟨n + m, σ⟩ → ⟨n +ₘ m, σ⟩
```

### 2.3 C语言语句语义

**语句配置**: $\langle S, \sigma \rangle \rightarrow \langle S', \sigma' \rangle$ 或 $\sigma'$

**赋值语句**:

```text
⟨e, σ⟩ →* n
─────────────────────────────  [ASSIGN]
⟨x = e;, σ⟩ → σ[x ↦ n]
```

**顺序执行**:

```text
⟨S₁, σ⟩ → ⟨S₁', σ'⟩
─────────────────────────────  [SEQ-STEP]
⟨S₁; S₂, σ⟩ → ⟨S₁'; S₂, σ'⟩

─────────────────────────────  [SEQ-DONE]
⟨skip; S₂, σ⟩ → ⟨S₂, σ⟩
```

**条件语句**:

```text
⟨b, σ⟩ →* true
─────────────────────────────────────────  [IF-TRUE]
⟨if (b) S₁ else S₂, σ⟩ → ⟨S₁, σ⟩

⟨b, σ⟩ →* false
─────────────────────────────────────────  [IF-FALSE]
⟨if (b) S₁ else S₂, σ⟩ → ⟨S₂, σ⟩
```

### 2.4 While循环的语义（小步骤）

```text
⟨b, σ⟩ →* true
─────────────────────────────────────────────────────  [WHILE-TRUE]
⟨while (b) S, σ⟩ → ⟨S; while (b) S, σ⟩

⟨b, σ⟩ →* false
─────────────────────────────────────────────────────  [WHILE-FALSE]
⟨while (b) S, σ⟩ → σ
```

---

## 3. 大步骤操作语义 (Big-Step)

### 3.1 基本形式

大步骤语义直接关联输入和最终输出：

$$
\langle e, \sigma \rangle \Downarrow v \quad \text{或} \quad \langle S, \sigma \rangle \Downarrow \sigma'
$$

### 3.2 表达式求值规则

```text
─────────────────  [B-NUM]
⟨n, σ⟩ ⇓ n

σ(x) = v
─────────────────  [B-VAR]
⟨x, σ⟩ ⇓ v

⟨e₁, σ⟩ ⇓ n₁    ⟨e₂, σ⟩ ⇓ n₂    n = n₁ + n₂
────────────────────────────────────────────  [B-ADD]
⟨e₁ + e₂, σ⟩ ⇓ n
```

### 3.3 语句执行规则

```text
⟨e, σ⟩ ⇓ v
─────────────────────────  [B-ASSIGN]
⟨x = e;, σ⟩ ⇓ σ[x ↦ v]

⟨S₁, σ⟩ ⇓ σ'    ⟨S₂, σ'⟩ ⇓ σ''
────────────────────────────────────  [B-SEQ]
⟨S₁; S₂, σ⟩ ⇓ σ''

⟨b, σ⟩ ⇓ true    ⟨S₁, σ⟩ ⇓ σ'
─────────────────────────────────────────────────  [B-IF-TRUE]
⟨if (b) S₁ else S₂, σ⟩ ⇓ σ'

⟨b, σ⟩ ⇓ false    ⟨S₂, σ⟩ ⇓ σ'
─────────────────────────────────────────────────  [B-IF-FALSE]
⟨if (b) S₁ else S₂, σ⟩ ⇓ σ'
```

### 3.4 While循环（大步骤）

```text
⟨b, σ⟩ ⇓ false
────────────────────────────────────  [B-WHILE-FALSE]
⟨while (b) S, σ⟩ ⇓ σ

⟨b, σ⟩ ⇓ true    ⟨S, σ⟩ ⇓ σ'    ⟨while (b) S, σ'⟩ ⇓ σ''
────────────────────────────────────────────────────────────────  [B-WHILE-TRUE]
⟨while (b) S, σ⟩ ⇓ σ''
```

---

## 4. 小步骤 vs 大步骤对比

| 特性 | 小步骤 | 大步骤 |
|:-----|:-------|:-------|
| **非终止** | 可表达（无限推导序列） | 无法直接表达（无推导树） |
| **并发** | 天然支持（交错语义） | 需要额外机制 |
| **调试** | 便于逐步跟踪 | 仅显示最终结果 |
| **实现** | 对应解释器 | 对应求值器 |
| **证明** | 需要归纳于推导长度 | 直接结构归纳 |

### 4.1 Big-Stop语义（最新进展 2026）

Kahn等人 (POPL 2026) 提出**Big-Stop语义**，在保持大步骤简洁性的同时处理非终止：

```text
标准大步骤无法区分：
- 发散程序（无限循环）
- 卡住的程序（运行时错误）

Big-Stop扩展：
⟨e, σ⟩ ▽ ∞    （发散）
⟨e, σ⟩ ▽ err  （错误）
```

---

## 5. C语言特定挑战

### 5.1 未定义行为的形式化

```c
// 有符号整数溢出
int x = INT_MAX;
x + 1;  // C标准: undefined behavior
```

小步骤语义中：

```text
⟨n +ₘ m, σ⟩ → err   如果 n +ₘ m 溢出
```

### 5.2 内存模型集成

```c
int *p = malloc(sizeof(int));
*p = 42;        // 需要内存有效性检查
free(p);
*p = 43;        // 使用已释放内存 → UB
```

配置扩展为 $\langle e, (\sigma, \mu) \rangle$，其中 $\mu$ 是内存状态。

### 5.3 CompCert的Clight语义

CompCert使用**组合式小步骤语义**：

```text
Clight语言 → (小步骤语义) → 转换到 → (C#minor) → ... → 汇编
```

关键设计决策：

- **表达式无副作用**: 简化求值顺序问题
- **显式内存访问**: 所有内存操作显式建模
- **确定性**: 非确定选择被显式枚举

---

## 6. 形式化验证应用

### 6.1 程序等价性证明

证明两个程序在所有输入下行为相同：

```text
∀σ. ⟨S₁, σ⟩ ⇓ σ' ⟺ ⟨S₂, σ⟩ ⇓ σ'
```

**示例**: 证明循环展开优化正确

```c
// 原始代码
for (int i = 0; i < n; i++) a[i] = 0;

// 展开4次
for (int i = 0; i < n - 3; i += 4) {
    a[i] = 0; a[i+1] = 0; a[i+2] = 0; a[i+3] = 0;
}
// 处理剩余元素
```

### 6.2 类型安全证明框架

类型保持定理 (Type Preservation):

```text
如果 Γ ⊢ e : τ 且 ⟨e, σ⟩ → ⟨e', σ'⟩，则 Γ ⊢ e' : τ
```

进展定理 (Progress):

```text
如果 Γ ⊢ e : τ，则 e 是值或 ∃e'. ⟨e, σ⟩ → ⟨e', σ'⟩
```

---

## 7. 与CompCert的关联

### 7.1 Clight语义结构

```coq
(* CompCert中的表达式求值 *)
Inductive eval_expr: env -> mem -> expr -> trace -> mem -> val -> Prop :=
  | eval_Econst_int: forall le m i ty,
      eval_expr le m (Econst_int i ty) E0 m (Vint i)
  | eval_Evar: forall le m x ty v,
      le!x = Some v ->
      eval_expr le m (Evar x ty) E0 m v
  | eval_Ebinop: forall le m a1 a2 t1 m1 v1 t2 m2 v2 op ty v,
      eval_expr le m a1 t1 m1 v1 ->
      eval_expr le m1 a2 t2 m2 v2 ->
      sem_binary_operation op v1 (typeof a1) v2 (typeof a2) m2 = Some v ->
      eval_expr le m (Ebinop op a1 a2 ty) (t1 ** t2) m2 v.
```

### 7.2 模拟关系 (Simulation)

CompCert证明的**向前模拟**：

```text
源程序步骤               目标程序步骤
    ↓                        ↓
  ⟨S,σ⟩ --→ ⟨S',σ'⟩    ⟹    ⟨C(S),C(σ)⟩ --→* ⟨C(S'),C(σ')⟩
    ↑                        ↑
  源语言语义             目标语言语义
```

---

## 8. 现代扩展

### 8.1 K框架 (K Framework)

K是一种**基于重写逻辑的语义框架**，支持可执行规范：

```k
// C语言的K定义片段
syntax Exp ::= Int | Id
             | Exp "+" Exp [strict]
             | Id "=" Exp [strict(2)]

rule <k> I1:Int + I2:Int => I1 +Int I2 ...</k>
rule <k> X:Id = I:Int => I ...</k>
     <env>... X |-> N ...</env>
     <mem>... N |-> (_ => I) ...</mem>
```

### 8.2 并发操作语义

多线程C的交错语义：

```text
⟨t₁, σ⟩ → ⟨t₁', σ'⟩
─────────────────────────────────────────  [PAR-LEFT]
⟨t₁ || t₂, σ⟩ → ⟨t₁' || t₂, σ'⟩

⟨t₂, σ⟩ → ⟨t₂', σ'⟩
─────────────────────────────────────────  [PAR-RIGHT]
⟨t₁ || t₂, σ⟩ → ⟨t₁ || t₂', σ'⟩
```

---

## 9. 形式化工具链

| 工具 | 用途 | 语言支持 |
|:-----|:-----|:---------|
| **CompCert** | 验证编译器 | C (Clight子集) |
| **K Framework** | 可执行语义 | 多种语言 |
| **Iris** | 高阶分离逻辑 | ML-like |
| **Coq** | 定理证明 | Gallina |
| **Isabelle/HOL** | 定理证明 | HOL |

---

## 10. 参考文献

1. Plotkin, G.D. (1981). "A Structural Approach to Operational Semantics"
2. Kahn, G. (1987). "Natural Semantics"
3. Winskel, G. (1993). "The Formal Semantics of Programming Languages"
4. Leroy, X. (2009-2021). "The CompCert C Verified Compiler"
5. Kahn, D.M., Hoffmann, J., Li, R. (2026). "Big-Stop Semantics" (POPL 2026)
6. Ellison, C., Rosu, G. (2012). "An Executable Formal Semantics of C" (POPL 2012)

---

> **关联文档**: [02_Denotational_Semantics](./02_Denotational_Semantics.md) | [03_Axiomatic_Semantics_Hoare](./03_Axiomatic_Semantics_Hoare.md) | [CompCert概述](../11_CompCert_Verification/01_Compcert_Overview.md)
