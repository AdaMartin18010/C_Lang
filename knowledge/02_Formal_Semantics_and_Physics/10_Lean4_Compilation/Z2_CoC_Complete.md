# Calculus of Constructions: 形式证明与编译理论基础

> **层级**: L6+
> **引用**: Coquand & Huet (1988), Barendregt (1992)

---

## 【哲学维度·构造主义证明论】

### 证明即程序的核心思想

**Brouwer的直觉主义**: 数学对象必须能被心智构造
**Heyting的形式化**: 证明是构造性过程
**Curry-Howard**: 证明 = 程序，命题 = 类型

---

## 【科学维度·CoC形式系统】

### 1. 语法定义

```
M, N ::= x | λx:A.M | M N | Πx:A.B | Prop | Type
```

### 2. 类型规则

**变量**:

```
x:A ∈ Γ
─────────
Γ ⊢ x : A
```

**抽象**:

```
Γ, x:A ⊢ M : B
─────────────────
Γ ⊢ λx:A.M : Πx:A.B
```

**应用**:

```
Γ ⊢ M : Πx:A.B    Γ ⊢ N : A
───────────────────────────
Γ ⊢ M N : B[N/x]
```

**乘积**:

```
Γ ⊢ A : s₁    Γ, x:A ⊢ B : s₂
─────────────────────────────
Γ ⊢ Πx:A.B : s₂
```

其中s₁, s₂ ∈ {Prop, Type}

### 3. 证明无关性定理

**定理**: Proof Irrelevance

对于P : Prop，如果p : P且q : P，则p ≡ q。

**证明**: CoC的语义将Prop解释为单元素集合，所有证明映射到同一元素。∎

### 4. Church-Rosser定理

**定理**: CoC满足合流性

如果M →*N₁且M →* N₂，则存在P使得N₁ →*P且N₂ →* P。

---

## 【工程维度·证明擦除与提取】

### 擦除函数定义

```
ε(x) = x
ε(λx:A.M) =
  if A : Prop then ε(M)
  else λx:ε(A).ε(M)
ε(M N) =
  if N : Prop then ε(M)
  else ε(M) ε(N)
ε(Πx:A.B) =
  if A : Prop then ε(B)
  else ε(A) → ε(B)
ε(Prop) = Unit
ε(Type) = Type
```

### 擦除正确性定理

**定理**: 擦除保持类型

如果Γ ⊢ M : A，则ε(Γ) ⊢ ε(M) : ε(A)。

**证明**: 对推导Γ ⊢ M : A进行归纳。

情况1: M = x，直接由上下文。
情况2: M = λx:A.N，使用归纳假设。
情况3: M = N₁ N₂，使用归纳假设。
∎

---

## 【可视化·类型系统层次】

```
λ演算体系层次 (Barendregt立方体)

                Type          (全高阶多态)
                │
    ┌───────────┼───────────┐
    │           │           │
   Fω         λP            λC
  (高阶)    (依赖)      (构造演算)
    │           │           │
    └───────────┴───────────┘
                │
               λ→           (简单类型)
                │
               λ            (无类型)

CoC = λP + λC = Calculus of Constructions
Lean 4 = CoC + 归纳类型 + 宇宙层级
```

---

## 【决策树·证明擦除策略】

```
擦除项M:
│
├─ M : Prop ?
│  └─ 返回 Unit
│
├─ M = λx:A.b ?
│  └─ A : Prop ?
│     ├─ 是: 擦除b（跳过参数）
│     └─ 否: λx:ε(A).ε(b)
│
├─ M = f a ?
│  └─ a : Prop ?
│     ├─ 是: 擦除f（跳过参数）
│     └─ 否: ε(f) ε(a)
│
└─ 其他: 递归擦除子项
```

---

## 【多维矩阵·CoC到C编译】

```
CoC构造        类型签名                  擦除后           C表示
────────────────────────────────────────────────────────────────────
λx:Prop.b      Prop→B                    B                直接值
λx:Type.b      Type→B                    Type→B           函数指针
λx:A.b(x)      Πx:A.B                    A→B              函数
(λx.b)a        B[a/x]                    B[a/x]           函数调用
Πx:Prop.B      命题蕴含                  B                直接值
Πx:Type.B      全称量词                  Type→B           函数指针
```

---

## 参考

- Coquand, T. & Huet, G. "The Calculus of Constructions" (1988)
- Barendregt, H. "Lambda Calculi with Types" (1992)
- Paulin-Mohring, C. "Extracting Fω Programs from Proofs" (1989)
