# 前沿研究: 概率编程语义

**L6+++ 系统级元理论 | 概率编程语言完整语义框架**

---

## 目录

- [前沿研究: 概率编程语义](#前沿研究-概率编程语义)
  - [目录](#目录)
  - [1. 引言](#1-引言)
  - [2. 概率编程基础](#2-概率编程基础)
    - [2.1 随机变量](#21-随机变量)
    - [2.2 概率空间](#22-概率空间)
    - [2.3 贝叶斯推断](#23-贝叶斯推断)
    - [2.4 概率单子](#24-概率单子)
  - [3. 测度论语义](#3-测度论语义)
    - [3.1 可测空间](#31-可测空间)
    - [3.2 概率测度](#32-概率测度)
    - [3.3 可测函数](#33-可测函数)
    - [3.4 期望与积分](#34-期望与积分)
  - [4. 概率λ演算](#4-概率λ演算)
    - [4.1 语法](#41-语法)
    - [4.2 操作语义](#42-操作语义)
    - [4.3 指称语义](#43-指称语义)
    - [4.4 类型系统](#44-类型系统)
  - [5. 推断算法](#5-推断算法)
    - [5.1 精确推断](#51-精确推断)
    - [5.2 MCMC](#52-mcmc)
    - [5.3 变分推断](#53-变分推断)
    - [5.3 变分推断](#53-变分推断-1)
    - [5.4 重要采样](#54-重要采样)
  - [6. 语义正确性](#6-语义正确性)
    - [6.1 采样正确性](#61-采样正确性)
    - [6.2 推断一致性](#62-推断一致性)
    - [6.3 收敛性](#63-收敛性)
  - [7. 形式化定理与证明](#7-形式化定理与证明)
    - [定理 7.1 (Giry单子定律)](#定理-71-giry单子定律)
    - [定理 7.2 (采样正确性)](#定理-72-采样正确性)
    - [定理 7.3 (MCMC收敛)](#定理-73-mcmc收敛)
    - [定理 7.4 (条件概率语义)](#定理-74-条件概率语义)
    - [定理 7.5 (变分下界)](#定理-75-变分下界)
  - [8. Lean 4实现](#8-lean-4实现)
    - [8.1 概率编程DSL](#81-概率编程dsl)
    - [8.2 推断算法实现](#82-推断算法实现)
  - [9. 参考文献](#9-参考文献)
  - [10. 交叉引用](#10-交叉引用)
    - [前置依赖](#前置依赖)
    - [横向关联](#横向关联)
    - [后续扩展](#后续扩展)

---

## 1. 引言

概率编程语言(PPL)允许程序员直接表达概率模型并进行自动推断。本文档建立概率编程语言的完整语义学框架，从测度论基础到推断算法，提供形式化的数学基础。

**核心洞见**:

- **测度论语义**: 概率程序解释为测度变换
- **单子抽象**: Giry单子统一概率计算
- **推断即归约**: 贝叶斯推断作为语义保持的程序变换

**与标准编程的区别**:

| 特性 | 标准编程 | 概率编程 |
|------|----------|----------|
| 输出 | 确定值 | 概率分布 |
| 计算模型 | 函数 | 概率核 |
| 语义域 | 集合 | 测度空间 |
| 正确性标准 | 终止性 | 采样正确性 |

---

## 2. 概率编程基础

### 2.1 随机变量

**定义 2.1.1** (随机变量). 给定概率空间 (Ω, ℱ, P)，随机变量 X: Ω → S 是从样本空间到可测空间 (S, 𝒮) 的可测函数。

**离散随机变量**:

```
X: Ω → ℕ  满足  X⁻¹({n}) ∈ ℱ  对所有 n ∈ ℕ
```

**连续随机变量**:

```
X: Ω → ℝ  满足  X⁻¹((a,b]) ∈ ℱ  对所有 a < b
```

**Lean 4 示例: 随机变量类型**

```lean4
/-- 可测空间结构 -/
structure MeasurableSpace (α : Type u) where
  σ : Set (Set α)
  empty_mem : ∅ ∈ σ
  compl_mem : ∀ s ∈ σ, sᶜ ∈ σ
  union_mem : ∀ (s : ℕ → Set α), (∀ i, s i ∈ σ) → (⋃ i, s i) ∈ σ

/-- 随机变量: 可测函数 -/
structure RandomVariable (Ω S : Type u)
    [MeasurableSpace Ω] [MeasurableSpace S] where
  func : Ω → S
  measurable : ∀ s ∈ (by infer_instance : MeasurableSpace S).σ,
    func ⁻¹' s ∈ (by infer_instance : MeasurableSpace Ω).σ

-- 示例: 伯努利随机变量
def Bernoulli (p : ℝ) (hp : 0 ≤ p ∧ p ≤ 1) : RandomVariable Ω Bool where
  func ω := if p < 0.5 then true else false
  measurable := sorry
```

### 2.2 概率空间

**定义 2.2.1** (概率空间). 三元组 (Ω, ℱ, P) 其中:

- Ω: 样本空间
- ℱ ⊆ 2^Ω: σ-代数
- P: ℱ → [0,1]: 概率测度

满足:

- P(Ω) = 1
- 可列可加性: P(⨆ᵢ Aᵢ) = Σᵢ P(Aᵢ) 对不相交 {Aᵢ}

**Lean 4 示例: 概率测度**

```lean4
/-- 概率测度 -/
structure ProbabilityMeasure (Ω : Type u) [MeasurableSpace Ω] where
  measure : Set Ω → ℝ≥0∞
  empty : measure ∅ = 0
  mono : ∀ {A B}, A ⊆ B → measure A ≤ measure B
  σ_additive : ∀ (A : ℕ → Set Ω),
    (∀ i j, i ≠ j → A i ∩ A j = ∅) →
    measure (⋃ i, A i) = ∑' i, measure (A i)
  total : measure Set.univ = 1
```

### 2.3 贝叶斯推断

**定义 2.3.1** (条件概率). 给定 P(B) > 0:

```
P(A|B) = P(A ∩ B) / P(B)
```

**定理 2.3.1** (贝叶斯定理).

```
P(H|D) = P(D|H) · P(H) / P(D)
```

其中:

- P(H): 先验概率
- P(D|H): 似然
- P(H|D): 后验概率
- P(D): 证据

**贝叶斯推断作为程序变换**:

```
let x ~ prior in
let y ~ likelihood(x) in
observe y = data
```

解释为条件化操作，将先验更新为后验。

### 2.4 概率单子

**定义 2.4.1** (Giry单子). 在可测空间范畴 Meas 上:

```
𝒢(X) = {μ: X → [0,∞] | μ 是概率测度}
```

- **单位** η: X → 𝒢(X), η(x) = δₓ (Dirac测度)
- **乘法** μ: 𝒢(𝒢(X)) → 𝒢(X), μ(Φ)(A) = ∫_{𝒢(X)} ν(A) dΦ(ν)

**Lean 4 示例: Giry单子**

```lean4
/-- Giry单子 - 概率测度单子 -/
def Giry (X : Type u) [MeasurableSpace X] : Type u :=
  ProbabilityMeasure X

/-- Dirac测度 (单位) -/
def dirac {X : Type u} [MeasurableSpace X] (x : X) : Giry X where
  measure s := if x ∈ s then 1 else 0
  empty := by simp
  mono := by intro A B h; simp; split_ifs <;> simp_all
  σ_additive := sorry
  total := by simp

/-- bind操作 -/
def giryBind {X Y : Type u} [MeasurableSpace X] [MeasurableSpace Y]
    (μ : Giry X) (f : X → Giry Y) : Giry Y :=
  sorry
```

---

## 3. 测度论语义

### 3.1 可测空间

**定义 3.1.1** (σ-代数). 集合 X 上的 σ-代数 ℱ ⊆ 2^X 满足:

1. ∅ ∈ ℱ
2. A ∈ ℱ ⇒ Aᶜ ∈ ℱ
3. {Aᵢ}ᵢ∈ℕ ⊆ ℱ ⇒ ⋃ᵢ Aᵢ ∈ ℱ

**定义 3.1.2** (Borel σ-代数). ℝ 上的Borel σ-代数 ℬ(ℝ) 是由开集生成的最小σ-代数。

**积可测空间**:

```
(X, ℱ_X) × (Y, ℱ_Y) = (X × Y, ℱ_X ⊗ ℱ_Y)
```

其中 ℱ_X ⊗ ℱ_Y 是由可测矩形 A × B (A ∈ ℱ_X, B ∈ ℱ_Y) 生成的σ-代数。

### 3.2 概率测度

**定义 3.2.1** (测度). 测度 μ: ℱ → [0,∞] 满足:

- μ(∅) = 0
- σ-可加性: μ(⨆ᵢ Aᵢ) = Σᵢ μ(Aᵢ) 对不相交集列

**定义 3.2.2** (概率测度). 测度 P 满足 P(X) = 1。

**重要测度示例**:

| 测度 | 定义域 | 密度函数 |
|------|--------|----------|
| 高斯 | ℝ | f(x) = (2πσ²)^(-1/2) exp(-(x-μ)²/(2σ²)) |
| 伯努利 | {0,1} | P(X=1) = p, P(X=0) = 1-p |
| 均匀 | [a,b] | f(x) = 1/(b-a) |
| Gamma | ℝ⁺ | f(x) = β^α/Γ(α) x^(α-1) e^(-βx) |

**Lean 4 示例: 常见分布**

```lean4
/-- 高斯分布 -/
def Gaussian (μ σ : ℝ) (hσ : σ > 0) : ProbabilityMeasure ℝ where
  measure s := ∫⁻ x in s, ENNReal.ofReal
    (Real.exp (-(x - μ) ^ 2 / (2 * σ ^ 2)) / Real.sqrt (2 * Real.pi * σ ^ 2))
  empty := by simp
  mono := sorry
  σ_additive := sorry
  total := sorry
```

### 3.3 可测函数

**定义 3.3.1** (可测函数). f: (X, ℱ_X) → (Y, ℱ_Y) 可测当:

```
∀ B ∈ ℱ_Y, f⁻¹(B) ∈ ℱ_X
```

**性质**:

- 连续函数是Borel可测的
- 可测函数的复合可测
- 可测函数列的极限可测

**概率核**:

**定义 3.3.2** (马尔可夫核). κ: X × ℱ_Y → [0,1] 满足:

1. 对每个 x ∈ X, κ(x, ·) 是 Y 上的概率测度
2. 对每个 B ∈ ℱ_Y, κ(·, B) 是 X 上的可测函数

**Lean 4 示例: 概率核**

```lean4
/-- 马尔可夫核 (条件概率分布) -/
structure MarkovKernel (X Y : Type u)
    [MeasurableSpace X] [MeasurableSpace Y] where
  kernel : X → ProbabilityMeasure Y
  measurable : ∀ s,
    Measurable (fun x => (kernel x).measure s)
```

### 3.4 期望与积分

**定义 3.4.1** (Lebesgue积分). 对可测函数 f: X → [0,∞]:

```
∫ f dμ = sup{Σᵢ aᵢ μ(Aᵢ) | Σᵢ aᵢ 𝟙_{Aᵢ} ≤ f, 简单函数}
```

**定义 3.4.2** (期望). 随机变量 X 的期望:

```
𝔼[X] = ∫ X dP
```

**条件期望**:

**定义 3.4.3** (条件期望). 给定子σ-代数 𝒢 ⊆ ℱ，条件期望 𝔼[X|𝒢] 是满足以下条件的唯一𝒢-可测随机变量:

```
∀ G ∈ 𝒢, ∫_G 𝔼[X|𝒢] dP = ∫_G X dP
```

**Lean 4 示例: 期望计算**

```lean4
/-- 期望 -/
def ProbabilityMeasure.expectation {X : Type u} [MeasurableSpace X]
    (μ : ProbabilityMeasure X) (f : X → ℝ) : ℝ :=
  sorry

-- 方差
def variance {X : Type u} [MeasurableSpace X]
    (μ : ProbabilityMeasure X) (f : X → ℝ) : ℝ :=
  let μf := μ.expectation f
  μ.expectation (fun x => (f x - μf) ^ 2)
```

---

## 4. 概率λ演算

### 4.1 语法

**扩展λ演算**:

```
M, N ::= x | λx.M | M N | c
       | sample(D)
       | observe(E, D)
       | factor(w)
       | return(V)
```

**分布表达式**:

```
D ::= Normal(μ, σ) | Bernoulli(p) | Uniform(a, b) | ...
```

**Lean 4 示例: 语法定义**

```lean4
/-- 分布类型 -/
inductive DistExpr where
  | normal (μ σ : ℝ)
  | bernoulli (p : ℝ)
  | uniform (a b : ℝ)

/-- 概率λ演算项 -/
inductive ProbTerm where
  | var (x : String)
  | lam (x : String) (body : ProbTerm)
  | app (fn arg : ProbTerm)
  | const (c : ℝ)
  | sample (dist : DistExpr)
  | observe (expr : ProbTerm) (dist : DistExpr)
  | factor (weight : ProbTerm)
```

### 4.2 操作语义

**采样语义** (sampling semantics):

```
⟨sample(D), σ, ω⟩ ⇓ ⟨v, σ, ω'⟩    其中 v ~ D, ω' 是剩余随机性
```

**观测语义**:

```
⟨observe(E, D), σ, ω⟩ ⇓ ⟨(), σ', ω⟩    若 ⟦E⟧σ = v 且 weight = D.density(v)
```

**权重语义**:

```
⟨factor(w), σ, ω⟩ ⇓ ⟨(), σ · w, ω⟩
```

**Lean 4 示例: 采样器单子**

```lean4
/-- 采样器状态 -/
structure SamplerState where
  rng : StdGen
  weight : ℝ

/-- 采样器单子 -/
def SamplerM (α : Type) := SamplerState → α × SamplerState

instance : Monad SamplerM where
  pure a := fun s => (a, s)
  bind m f := fun s =>
    let (a, s') := m s
    f a s'

/-- 采样操作 -/
def sampleNormal (μ σ : ℝ) : SamplerM ℝ := fun s =>
  let (u, g') := s.rng.next
  let z := Real.sqrt (-2 * Real.log (Float.ofUInt64 u)) *
           Real.cos (2 * Real.pi * Float.ofUInt64 (u >>> 33))
  (μ + σ * z, { s with rng := g' })
```

### 4.3 指称语义

**类型指称**:

```
〚Real〛 = ℝ
〚τ → σ〛 = 〚τ〛 → 𝒢(〚σ〛)
〚Dist(τ)〛 = 𝒢(〚τ〛)
```

**项指称** (带权重的测度):

```
〚x〛ρ = δ_{ρ(x)}
〚λx.M〛ρ = λv. 〚M〛ρ[x↦v]
〚M N〛ρ = ∫ 〚N〛ρ(dv) · 〚M〛ρ[v]

〚sample(D)〛ρ = 〚D〛ρ
〚observe(E, D)〛ρ = δ_{()} · (〚D〛ρ.density)(〚E〛ρ)
〚factor(w)〛ρ = δ_{()} · 〚w〛ρ
```

**Lean 4 示例: 指称语义**

```lean4
/-- 带权重的概率分布 -/
structure WeightedMeasure (α : Type u) [MeasurableSpace α] where
  measure : ProbabilityMeasure α
  weight : ℝ≥0

/-- 项指称 -/
def denoteTerm (env : String → ℝ) : ProbTerm → WeightedMeasure ℝ
  | .const c => ⟨dirac c, 1⟩
  | .var x => ⟨dirac (env x), 1⟩
  | .sample (.normal μ σ) => ⟨Gaussian μ σ (by linarith), 1⟩
  | _ => sorry
```

### 4.4 类型系统

**概率类型**:

```
τ, σ ::= Real | Bool | τ × σ | τ → σ | Dist(τ) | M(τ)
```

其中 M(τ) 是概率计算类型。

**类型规则**:

```
Γ ⊢ D: Dist(τ)
────────────────
Γ ⊢ sample(D): M(τ)

Γ ⊢ E: τ    Γ ⊢ D: Dist(τ)
──────────────────────────
Γ ⊢ observe(E, D): M(Unit)

Γ ⊢ w: Real
────────────
Γ ⊢ factor(w): M(Unit)
```

**概率类型与单子对应**:

```
M(τ) ≅ 𝒢(τ)
```

---

## 5. 推断算法

### 5.1 精确推断

**变量消除** (Variable Elimination):

```
P(Q|e) = Σ_{X\Q} P(X, e) / Σ_{X} P(X, e)
```

**消元顺序**影响计算复杂度:

```
复杂度 = O(n · d^{tw+1})
```

其中 tw 是消元顺序的树宽。

### 5.2 MCMC

**Metropolis-Hastings算法**:

```
1. 初始化 x₀
2. 对 t = 1, 2, ...:
   a. 从 Q(·|x_{t-1}) 采样候选 y
   b. 计算接受率 α = min(1, P(y)Q(x_{t-1}|y) / (P(x_{t-1})Q(y|x_{t-1})))
   c. 以概率 α 接受 x_t = y，否则 x_t = x_{t-1}
```

**Gibbs采样**:

```
对每个变量 xᵢ:
  xᵢ ~ P(xᵢ | x_{-i})
```

### 5.3 变分推断


其中 x_{-i} 表示除 x_i 外的所有变量。

**Lean 4 示例: MCMC采样器**

```lean4
/-- Metropolis-Hastings采样器 -/
def metropolisHastings {X : Type}
    (target : X → ℝ)              -- 目标密度(未归一化)
    (proposal : X → SamplerM X)   -- 提议分布
    (init : X)                    -- 初始状态
    (n : ℕ) : SamplerM (List X) := do
  let mut samples := [init]
  let mut current := init
  for _ in [0:n] do
    let candidate ← proposal current
    let alpha := min 1.0 (target candidate / target current)
    let u ← sampleUniform 0 1
    if u < alpha then
      current := candidate
    samples := current :: samples
  return samples.reverse
```

### 5.3 变分推断

**变分推断** (Variational Inference) 将推断问题转化为优化问题:

```
Q* = argmin_{Q ∈ Q} KL(Q || P(·|D))
```

其中 Q 是变分分布族，KL 是Kullback-Leibler散度。

**证据下界 (ELBO)**:

```
log P(D) ≥ 𝔼_Q[log P(X,D)] - 𝔼_Q[log Q(X)] =: ELBO(Q)
```

最大化ELBO等价于最小化KL散度。

**Lean 4 示例: 变分推断**

```lean4
/-- 变分分布族 -/
structure VariationalFamily (X : Type u) [MeasurableSpace X] where
  paramSpace : Type v
  distribution : paramSpace → ProbabilityMeasure X

/-- KL散度 -/
def KL_divergence {X : Type u} [MeasurableSpace X]
    (Q P : ProbabilityMeasure X) : ℝ :=
  sorry  -- ∫ log(dQ/dP) dQ

/-- ELBO计算 -/
def ELBO {X D : Type u} [MeasurableSpace X] [MeasurableSpace D]
    (joint : ProbabilityMeasure (X × D))
    (Q : ProbabilityMeasure X) : ℝ :=
  sorry
```

### 5.4 重要采样

**重要采样** (Importance Sampling) 从提议分布 Q 采样，通过权重纠正偏差:

```
w(x) = P(x) / Q(x)

𝔼_P[f(X)] = 𝔼_Q[f(X) · w(X)] ≈ (1/N) Σᵢ f(xᵢ) · w(xᵢ)
```

**自归一化重要采样**:

```
𝔼_P[f(X)] ≈ Σᵢ f(xᵢ) · w̃(xᵢ) / Σᵢ w̃(xᵢ)
```

其中 w̃(x) 是未归一化权重。

---

## 6. 语义正确性

### 6.1 采样正确性

**定义 6.1.1** (采样正确性). 采样算法 A 对目标分布 P 是正确的，如果:

```
lim_{n→∞} (1/n) Σᵢ δ_{xᵢ} = P    (弱收敛)
```

其中 xᵢ ~ A(P)。

### 6.2 推断一致性

**定义 6.2.1** (推断一致性). 推断算法是一致的，如果:

```
lim_{n→∞} P̂ₙ(·|D) = P(·|D)    (后验收敛)
```

其中 P̂ₙ 是基于 n 个样本的估计。

### 6.3 收敛性

**总变差距离**:

```
||μ - ν||_TV = sup_A |μ(A) - ν(A)|
```

**收敛度量**:

| 度量 | 定义 | 用途 |
|------|------|------|
| TV距离 | sup_A |P(A) - Q(A)| | 强收敛 |
| KL散度 | ∫ log(dP/dQ) dP | 变分推断 |
| Wasserstein | inf_γ 𝔼[||X-Y||] | 弱收敛 |

---

## 7. 形式化定理与证明

### 定理 7.1 (Giry单子定律)

**陈述**: Giry单子 (𝒢, η, μ) 满足单子定律:

1. **结合律**: μ_X ∘ 𝒢(μ_X) = μ_X ∘ μ_{𝒢(X)}
2. **左单位律**: μ_X ∘ η_{𝒢(X)} = id_{𝒢(X)}
3. **右单位律**: μ_X ∘ 𝒢(η_X) = id_{𝒢(X)}

**证明**:

**(1) 结合律**:

对 Φ ∈ 𝒢(𝒢(𝒢(X))) 和 A ∈ ℱ_X:

```
(μ_X ∘ 𝒢(μ_X))(Φ)(A)
= μ_X(𝒢(μ_X)(Φ))(A)
= ∫_{𝒢(X)} ν(A) d(𝒢(μ_X)(Φ))(ν)
= ∫_{𝒢(𝒢(X))} μ_X(ξ)(A) dΦ(ξ)
= ∫_{𝒢(𝒢(X))} (∫_{𝒢(X)} ν(A) dξ(ν)) dΦ(ξ)

(μ_X ∘ μ_{𝒢(X)})(Φ)(A)
= ∫_{𝒢(X)} ν(A) d(μ_{𝒢(X)}(Φ))(ν)
= ∫_{𝒢(𝒢(X))} ν(A) d(∫_{𝒢(X)} ξ dΦ(ξ))(ν)
```

由Fubini定理，两者相等。

**(2) 左单位律**:

对 ν ∈ 𝒢(X):

```
(μ_X ∘ η_{𝒢(X)})(ν)(A)
= ∫_{𝒢(X)} ξ(A) d(η_{𝒢(X)}(ν))(ξ)
= ν(A)    (因为 η_{𝒢(X)}(ν) = δ_ν)
```

**(3) 右单位律**:

对 ν ∈ 𝒢(X):

```
(μ_X ∘ 𝒢(η_X))(ν)(A)
= ∫_{𝒢(X)} ξ(A) d(𝒢(η_X)(ν))(ξ)
= ∫_X η_X(x)(A) dν(x)
= ∫_X δ_x(A) dν(x)
= ν(A)
```

**证毕** ∎

### 定理 7.2 (采样正确性)

**陈述**: 对概率测度 μ ∈ 𝒢(X)，若 xᵢ ~ μ 独立同分布，则:

```
(1/n) Σᵢ₌₁ⁿ δ_{xᵢ} → μ    (几乎必然弱收敛)
```

**证明**:

由大数定律，对任意有界连续函数 f:

```
∫ f d((1/n) Σᵢ δ_{xᵢ}) = (1/n) Σᵢ f(xᵢ) → 𝔼[f(X)] = ∫ f dμ    (a.s.)
```

这正是弱收敛的定义。

**证毕** ∎

### 定理 7.3 (MCMC收敛)

**陈述**: 设 P 是遍历马尔可夫链的转移核，π 是平稳分布。则从任意初始分布 ν₀ 出发:

```
ν₀ Pⁿ → π    (总变差收敛)
```

**证明**:

**步骤 1**: 证明 π 是平稳的，即 πP = π。

由细致平衡条件:

```
π(x) P(x → y) = π(y) P(y → x)
```

对 y 求和:

```
(πP)(y) = Σ_x π(x) P(x → y) = π(y) Σ_x P(y → x) = π(y)
```

**步骤 2**: 证明遍历性 (几何遍历)。

对任意状态 x, y，由遍历假设存在 n 使 Pⁿ(x, y) > 0。

**步骤 3**: 应用遍历定理。

由马尔可夫链收敛定理，||ν₀Pⁿ - π||_TV → 0。

**证毕** ∎

### 定理 7.4 (条件概率语义)

**陈述**: observe(E, D) 的指称语义等于条件化操作:

```
〚observe(E, D)〛ρ = δ_{()} · P(X = ⟦E⟧ρ | D)
```

其中 P(·|D) 是条件概率测度。

**证明**:

设 e = ⟦E⟧ρ，D 的密度为 f_D。

由 observe 的操作语义:

```
〚observe(E, D)〛ρ = δ_{()} · f_D(e)
```

由贝叶斯定理:

```
P(X = e | D) = P(D | X = e) · P(X = e) / P(D) ∝ f_D(e)
```

因此权重 f_D(e) 正比于条件概率。

**证毕** ∎

### 定理 7.5 (变分下界)

**陈述**: 对任意分布 Q，ELBO 是证据下界:

```
log P(D) ≥ ELBO(Q)
```

等号成立当且仅当 Q = P(·|D)。

**证明**:

```
log P(D) = log ∫ P(D|X) P(X) dX
         = log ∫ P(D|X) P(X) / Q(X) · Q(X) dX
         = log 𝔼_Q[P(D|X) P(X) / Q(X)]
         ≥ 𝔼_Q[log(P(D|X) P(X) / Q(X))]    (Jensen不等式)
         = 𝔼_Q[log P(D|X) + log P(X) - log Q(X)]
         = 𝔼_Q[log P(X,D)] - 𝔼_Q[log Q(X)]
         = ELBO(Q)
```

等号成立条件: P(D|X)P(X)/Q(X) = 常数，即 Q(X) ∝ P(D|X)P(X) = P(X|D)P(D)。

因此 Q = P(·|D)。

**证毕** ∎

---

## 8. Lean 4实现

### 8.1 概率编程DSL

```lean4
/-- 概率编程DSL -/
syntax "sample" "(" term ")" : term
syntax "observe" "(" term "," term ")" : term
syntax "factor" "(" term ")" : term
syntax "return" "(" term ")" : term

-- 示例: 贝叶斯线性回归
example : ProbTerm :=
  .let "m" (.sample (.normal 0 1)) (
  .let "b" (.sample (.normal 0 1)) (
  .let "y" (.sample (.normal (.var "m" * .const 2 + .var "b") 0.1)) (
  .observe (.const 4.1) (.normal (.var "m" * .const 2 + .var "b") 0.1))))
```

### 8.2 推断算法实现

```lean4
/-- MCMC推断 -/
def mcmcInference (term : ProbTerm) (n : ℕ) : SamplerM (List ℝ) := do
  sorry  -- 实现MCMC采样

/-- 变分推断 -/
def viInference (term : ProbTerm) (epochs : ℕ) : VariationalFamily ℝ :=
  sorry  -- 实现VI优化

/-- 重要采样 -/
def importanceSampling (term : ProbTerm) (n : ℕ) : SamplerM (List (ℝ × ℝ)) :=
  sorry  -- 返回(样本, 权重)对
```

---

## 9. 参考文献

1. **Giry, M.** (1982). A categorical approach to probability theory. *Lecture Notes in Mathematics*, 915, 68-85.
2. **Kozen, D.** (1981). Semantics of probabilistic programs. *JCSS*, 22(3), 328-350.
3. **Panangaden, P.** (2009). *Labelled Markov Processes*. Imperial College Press.
4. **Borgström, J., et al.** (2011). Measure transformer semantics for Bayesian machine learning. *ESOP*, 77-96.
5. **Staton, S., et al.** (2016). Semantics for probabilistic programming. *POPL*, 525-531.
6. **Hur, C.K., et al.** (2015). A provably correct sampler for probabilistic programs. *POPL*, 189-200.
7. **Kaminski, B.L. & Katoen, J.P.** (2017). A weakest pre-expectation semantics for mixed-sign expectations. *LICS*, 1-12.
8. **Bichsel, B., et al.** (2018). Fine-grained semantics for probabilistic programs. *ESOP*, 145-185.

---

## 10. 交叉引用

### 前置依赖

- **[SEMANTIC_NOTATIONS.md](./SEMANTIC_NOTATIONS.md)** — 符号标准与约定
- **[CATEGORICAL_SEMANTICS.md](./CATEGORICAL_SEMANTICS.md)** — 单子理论基础
- **[ALGEBRAIC_EFFECTS.md](./ALGEBRAIC_EFFECTS.md)** — 效应系统与概率效应

### 横向关联

- **[SEMANTICS_UNIFIED_REVISED.md](./SEMANTICS_UNIFIED_REVISED.md)** — 统一语义框架
- **[ADVANCED_SEMANTICS.md](./ADVANCED_SEMANTICS.md)** — 高级语义主题
- **[BRIDGE_THEOREMS.md](./BRIDGE_THEOREMS.md)** — 桥梁定理

### 后续扩展

- 连续时间随机过程语义
- 量子概率编程
- 因果推断与do-calculus
- 近似正确推断(PAC)

---

*版本: 1.0 | 级别: L6+++ | 更新: 2026-03-24*
