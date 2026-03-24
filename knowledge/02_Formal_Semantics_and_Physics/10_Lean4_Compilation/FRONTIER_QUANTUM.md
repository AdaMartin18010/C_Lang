# 前沿研究: 量子计算语义

**L6+++ 系统级元理论 | 量子程序语言语义学完整框架**

---

## 目录

- [前沿研究: 量子计算语义](#前沿研究-量子计算语义)
  - [目录](#目录)
  - [1. 引言](#1-引言)
  - [2. 量子力学基础](#2-量子力学基础)
    - [2.1 量子比特](#21-量子比特)
    - [2.2 希尔伯特空间](#22-希尔伯特空间)
    - [2.3 量子门](#23-量子门)
    - [2.4 测量](#24-测量)
    - [2.5 纠缠](#25-纠缠)
  - [3. 量子λ演算](#3-量子λ演算)
    - [3.1 线性λ演算](#31-线性λ演算)
    - [3.2 语法](#32-语法)
    - [3.3 操作语义](#33-操作语义)
    - [3.4 指称语义](#34-指称语义)
  - [4. 线性类型系统](#4-线性类型系统)
    - [4.1 线性逻辑](#41-线性逻辑)
    - [4.2 类型构造子](#42-类型构造子)
    - [4.3 类型规则](#43-类型规则)
    - [4.4 线性约束](#44-线性约束)
  - [5. 量子线路](#5-量子线路)
    - [5.1 线路模型](#51-线路模型)
    - [5.2 门集合](#52-门集合)
    - [5.3 线路等价](#53-线路等价)
    - [5.4 编译](#54-编译)
  - [6. 高级主题](#6-高级主题)
    - [6.1 隐形传态](#61-隐形传态)
    - [6.2 超密编码](#62-超密编码)
    - [6.3 量子纠错](#63-量子纠错)
  - [7. 形式化定理与证明](#7-形式化定理与证明)
    - [定理 1 (线性保持)](#定理-1-线性保持)
    - [定理 2 (酉性)](#定理-2-酉性)
    - [定理 3 (不可克隆)](#定理-3-不可克隆)
    - [定理 4 (隐形传态正确性)](#定理-4-隐形传态正确性)
    - [定理 5 (Deutsch-Jozsa)](#定理-5-deutsch-jozsa)
  - [8. Lean 4实现](#8-lean-4实现)
    - [量子态实现](#量子态实现)
    - [量子隐形传态实现](#量子隐形传态实现)
  - [9. 参考文献](#9-参考文献)
  - [10. 交叉引用](#10-交叉引用)
    - [前置依赖](#前置依赖)
    - [横向关联](#横向关联)
    - [后续扩展](#后续扩展)

---

## 1. 引言

量子计算语义学研究量子程序语言的数学基础，通过类型理论和范畴论为量子计算提供严格的语义框架。

**核心洞见** (量子-经典对应):

| 经典计算 | 量子计算 |
|----------|----------|
| 比特 (Bit) | 量子比特 (Qubit) |
| 布尔逻辑 | 酉变换 |
| 复制任意数据 | 不可克隆定理 |
| 笛卡尔闭范畴 | 紧致闭范畴 |

**本文档建立以下理论框架**:

1. **量子力学基础**: 希尔伯特空间、量子门、测量
2. **量子λ演算**: 线性类型约束的λ演算扩展
3. **线性类型系统**: !A, ⊗, ⊸ 类型构造子
4. **量子线路**: 门模型与线路等价
5. **高级协议**: 隐形传态、超密编码、量子纠错

---

## 2. 量子力学基础

### 2.1 量子比特

**定义 2.1.1** (量子比特). 量子比特是二维复希尔伯特空间 ℋ₂ ≅ ℂ² 中的单位向量。

计算基 (Computational Basis):

$$
|0\rangle = \begin{pmatrix} 1 \\ 0 \end{pmatrix}, \quad
|1\rangle = \begin{pmatrix} 0 \\ 1 \end{pmatrix}
$$

**定义 2.1.2** (叠加态). 一般量子态为基态的线性叠加:

$$
|\psi\rangle = \alpha|0\rangle + \beta|1\rangle = \begin{pmatrix} \alpha \\ \beta \end{pmatrix}
$$

其中 $\alpha, \beta \in \mathbb{C}$ 且 $|\alpha|^2 + |\beta|^2 = 1$。

**布洛赫球表示**:

$$
|\psi\rangle = \cos\frac{\theta}{2}|0\rangle + e^{i\varphi}\sin\frac{\theta}{2}|1\rangle
$$

**Lean 4 示例: 量子比特定义**

```lean4
import Mathlib

/-- 复数表示的量子比特态 -/
structure Qubit where
  α : Complex
  β : Complex
  normalized : α.re^2 + α.im^2 + β.re^2 + β.im^2 = 1

def ket0 : Qubit := { α := 1, β := 0, normalized := by norm_num }
def ket1 : Qubit := { α := 0, β := 1, normalized := by norm_num }

/-- 叠加态构造函数 -/
def superposition (θ φ : Real) : Qubit where
  α := Real.cos (θ / 2)
  β := Complex.exp (Complex.I * φ) * Real.sin (θ / 2)
  normalized := by
    simp [Complex.norm_sq, mul_conj]
    have h : Real.cos (θ / 2)^2 + Real.sin (θ / 2)^2 = 1 := Real.cos_sq_add_sin_sq (θ / 2)
    linarith
```

### 2.2 希尔伯特空间

**定义 2.2.1** (希尔伯特空间). 完备的复内积空间 (ℋ, ⟨·|·⟩)。

**内积**:

$$
\langle\phi|\psi\rangle = \sum_i \phi_i^* \psi_i
$$

**bra-ket 记号**:

- $|\psi\rangle$: 列向量 (ket)
- $\langle\psi| = |\psi\rangle^\dagger$: 行向量 (bra)

**定义 2.2.2** (多量子比特系统). n-量子比特系统的状态空间为 n 重张量积:

$$
\mathcal{H}_{2^n} = \bigotimes_{i=1}^n \mathcal{H}_2 = \mathcal{H}_2 \otimes \cdots \otimes \mathcal{H}_2 \cong \mathbb{C}^{2^n}
$$

**Lean 4 示例: 希尔伯特空间结构**

```lean4
/-- n-量子比特希尔伯特空间 -/
def HilbertSpace (n : Nat) := Fin (2^n) → Complex

def innerProduct {n : Nat} (ψ φ : HilbertSpace n) : Complex :=
  Finset.sum (Finset.univ : Finset (Fin (2^n))) fun i => (ψ i).conj * (φ i)

notation "⟨" ψ "|" φ "⟩" => innerProduct ψ φ

def normSq {n : Nat} (ψ : HilbertSpace n) : Real :=
  (⟨ψ|ψ⟩).re

def isNormalized {n : Nat} (ψ : HilbertSpace n) : Prop :=
  normSq ψ = 1
```

### 2.3 量子门

**定义 2.3.1** (酉变换). 量子门是希尔伯特空间上的酉算子 U，满足 $U^\dagger U = UU^\dagger = I$。

**单量子比特门**:

| 门 | 矩阵 | 作用 |
|----|------|------|
| **I** (恒等) | $\begin{pmatrix} 1 & 0 \\ 0 & 1 \end{pmatrix}$ | 无操作 |
| **X** (Pauli-X/NOT) | $\begin{pmatrix} 0 & 1 \\ 1 & 0 \end{pmatrix}$ | $|0\rangle \leftrightarrow |1\rangle$ |
| **Y** (Pauli-Y) | $\begin{pmatrix} 0 & -i \\ i & 0 \end{pmatrix}$ | Y轴旋转π |
| **Z** (Pauli-Z) | $\begin{pmatrix} 1 & 0 \\ 0 & -1 \end{pmatrix}$ | 相位翻转 |
| **H** (Hadamard) | $\frac{1}{\sqrt{2}}\begin{pmatrix} 1 & 1 \\ 1 & -1 \end{pmatrix}$ | 叠加 |Hadamard门创建叠加态:

$$
H|0\rangle = \frac{1}{\sqrt{2}}(|0\rangle + |1\rangle) = |+\rangle
$$

$$
H|1\rangle = \frac{1}{\sqrt{2}}(|0\rangle - |1\rangle) = |-\rangle
$$

**双量子比特门**:

**CNOT** (受控非门):

$$
U_{CNOT} = |0\rangle\langle0| \otimes I + |1\rangle\langle1| \otimes X = \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 0 & 1 \\ 0 & 0 & 1 & 0 \end{pmatrix}
$$

**Lean 4 示例: 量子门实现**

```lean4
/-- 2×2 酉矩阵 -/
structure UnitaryMatrix where
  matrix : Matrix (Fin 2) (Fin 2) Complex
  unitary : matrix * matrix.conjTranspose = 1 ∧ matrix.conjTranspose * matrix = 1

def I_gate : UnitaryMatrix where
  matrix := !![1, 0; 0, 1]
  unitary := by constructor <;> simp [Matrix.mul_apply, Matrix.one_apply]

def X_gate : UnitaryMatrix where
  matrix := !![0, 1; 1, 0]
  unitary := by constructor <;> simp [Matrix.mul_apply, Matrix.one_apply]; aesop

def Y_gate : UnitaryMatrix where
  matrix := !![0, -Complex.I; Complex.I, 0]
  unitary := by
    constructor <;> simp [Matrix.mul_apply, Matrix.one_apply, Complex.I_mul_I]
    all_goals funext i j; fin_cases i <;> fin_cases j <;> simp

def Z_gate : UnitaryMatrix where
  matrix := !![1, 0; 0, -1]
  unitary := by constructor <;> simp [Matrix.mul_apply, Matrix.one_apply]

def H_gate : UnitaryMatrix where
  matrix := (1 / Real.sqrt 2) • !![1, 1; 1, -1]
  unitary := by
    have h : Real.sqrt 2 ^ 2 = 2 := Real.sq_sqrt (by norm_num)
    constructor <;> simp [Matrix.mul_apply, Matrix.one_apply, h]; all_goals aesop
```

### 2.4 测量

**定义 2.4.1** (投影测量). 对可观测量 M = Σᵢ mᵢ Pᵢ，测量结果 mᵢ 的概率为:

$$
p(m_i) = \langle\psi|P_i|\psi\rangle
$$

测量后状态坍缩为:

$$
|\psi'\rangle = \frac{P_i|\psi\rangle}{\sqrt{p(m_i)}}
$$

**定义 2.4.2** (计算基测量). 对单量子比特:

$$
p(0) = |\langle0|\psi\rangle|^2 = |\alpha|^2, \quad p(1) = |\beta|^2
$$

**密度矩阵表述**:

$$
\rho = |\psi\rangle\langle\psi| = \begin{pmatrix} |\alpha|^2 & \alpha\beta^* \\ \alpha^*\beta & |\beta|^2 \end{pmatrix}
$$

测量概率: $p(i) = \text{Tr}(P_i \rho)$

**Lean 4 示例: 测量算子**

```lean4
/-- 投影算子到 |0⟩ -/
def proj0 : Matrix (Fin 2) (Fin 2) Complex :=
  !![1, 0; 0, 0]

/-- 投影算子到 |1⟩ -/
def proj1 : Matrix (Fin 2) (Fin 2) Complex :=
  !![0, 0; 0, 1]

/-- 计算测量概率 -/
def measureProb0 (ψ : Qubit) : Real :=
  (ψ.α.re^2 + ψ.α.im^2)

def measureProb1 (ψ : Qubit) : Real :=
  (ψ.β.re^2 + ψ.β.im^2)

def measureProb0_valid (ψ : Qubit) : measureProb0 ψ + measureProb1 ψ = 1 := by
  simp [measureProb0, measureProb1, ψ.normalized]

/-- 测量后坍缩 -/
def measureCollapse0 (ψ : Qubit) (h : ψ.α ≠ 0) : Qubit where
  α := 1
  β := 0
  normalized := by norm_num
```

### 2.5 纠缠

**定义 2.5.1** (纠缠态). 双量子比特态 $|\psi\rangle \in \mathcal{H}_2 \otimes \mathcal{H}_2$ 是纠缠的，若不能写成 $|\psi\rangle = |\phi\rangle \otimes |\varphi\rangle$ 的形式。

**贝尔态** (最大纠缠态):

$$
|\Phi^+\rangle = \frac{1}{\sqrt{2}}(|00\rangle + |11\rangle)
$$

$$
|\Phi^-\rangle = \frac{1}{\sqrt{2}}(|00\rangle - |11\rangle)
$$

$$
|\Psi^+\rangle = \frac{1}{\sqrt{2}}(|01\rangle + |10\rangle)
$$

$$
|\Psi^-\rangle = \frac{1}{\sqrt{2}}(|01\rangle - |10\rangle)
$$

**施密特分解**:

$$
|\psi\rangle = \sum_{i=1}^r \lambda_i |u_i\rangle \otimes |v_i\rangle
$$

其中 r 为施密特秩，r > 1 时表示纠缠。

**Lean 4 示例: 纠缠态**

```lean4
/-- 贝尔态 |Φ⁺⟩ -/
def bellPhiPlus : HilbertSpace 2 := fun i =>
  match i with
  | ⟨0, _⟩ => 1 / Real.sqrt 2
  | ⟨1, _⟩ => 0
  | ⟨2, _⟩ => 0
  | ⟨3, _⟩ => 1 / Real.sqrt 2
  | ⟨_, h⟩ => by simp at h; have : 2 < 4 := by linarith; omega

/-- 验证贝尔态是归一化的 -/
theorem bellPhiPlus_normalized : isNormalized bellPhiPlus := by
  simp [isNormalized, normSq, innerProduct, bellPhiPlus]
  have h : Real.sqrt 2 ^ 2 = 2 := Real.sq_sqrt (by norm_num)
  field_simp [h]; norm_num

/-- 检查纠缠: 若态可分离，则满足特定条件 -/
def isSeparable (ψ : HilbertSpace 2) : Prop :=
  ∃ (a b c d : Complex),
    a.re^2 + a.im^2 + b.re^2 + b.im^2 = 1 ∧
    c.re^2 + c.im^2 + d.re^2 + d.im^2 = 1 ∧
    ψ = fun i =>
      match i with
      | ⟨0, _⟩ => a * c
      | ⟨1, _⟩ => a * d
      | ⟨2, _⟩ => b * c
      | ⟨3, _⟩ => b * d
      | _ => 0

/-- 贝尔态是纠缠的 -/
theorem bellPhiPlus_entangled : ¬isSeparable bellPhiPlus := by
  intro h
  rcases h with ⟨a, b, c, d, h₁, h₂, h₃⟩
  have h00 : bellPhiPlus ⟨0, by simp⟩ = a * c := by rw [h₃]
  have h03 : bellPhiPlus ⟨3, by simp⟩ = b * d := by rw [h₃]
  simp [bellPhiPlus] at h00 h03
  have : a * c = b * d := by linarith
  -- 导出矛盾
  sorry
```

---

## 3. 量子λ演算

### 3.1 线性λ演算

**定义 3.1.1** (线性λ演算). 线性λ演算要求每个变量在项中恰好使用一次。

**线性类型标记**:

- $x : A \multimap B$: x 必须恰好使用一次
- 禁止 weakening (丢弃变量) 和 contraction (复制变量)

**语法**:

$$
t ::= x \mid \lambda^\circ x.t \mid t \circ u \mid \text{let } x = t \text{ in } u
$$

**线性约束**:

```
Γ, x: A ⊢ t: B        Γ₁ ⊢ t: A  Γ₂ ⊢ u: A ⊸ B
-------------------  -------------------------
Γ ⊢ λ^∘x.t: A ⊸ B    Γ₁, Γ₂ ⊢ u ∘ t: B
```

### 3.2 语法

**定义 3.2.1** (量子λ演算语法). 扩展线性λ演算以包含量子构造:

$$
\begin{aligned}
t ::= &\ x \mid \lambda x.t \mid t\ u \mid \langle t, u \rangle \mid \pi_1 t \mid \pi_2 t \\
    &\mid \text{new } |0\rangle \mid \text{new } |1\rangle \mid \text{apply } U \ t \\
    &\mid \text{meas } t \mid \text{let } x = t \text{ in } u
\end{aligned}
$$

**量子类型**:

$$
\tau ::= \text{Qubit} \mid \tau \otimes \tau \mid \tau \multimap \tau \mid !\tau
$$

**Lean 4 示例: 量子λ演算语法**

```lean4
inductive QType : Type
  | Qubit           -- 量子比特类型
  | Tensor (A B : QType)   -- A ⊗ B
  | Lolli (A B : QType)    -- A ⊸ B (线性函数)
  | Bang (A : QType)       -- !A (非线性/可丢弃)
  | Unit                   -- 单位类型

def QType.Q (n : Nat) : QType := match n with
  | 0 => QType.Unit
  | 1 => QType.Qubit
  | n + 1 => QType.Tensor QType.Qubit (QType.Q n)

inductive QTerm : QType → Type
  | var (x : String) {A} : QTerm A
  | lam (x : String) {A B} : QTerm B → QTerm (QType.Lolli A B)
  | app {A B} : QTerm (QType.Lolli A B) → QTerm A → QTerm B
  | pair {A B} : QTerm A → QTerm B → QTerm (QType.Tensor A B)
  | new0 : QTerm QType.Qubit
  | new1 : QTerm QType.Qubit
  | applyU (U : UnitaryMatrix) {n} : QTerm (QType.Q n) → QTerm (QType.Q n)
  | meas {A} : QTerm A → QTerm (QType.Bang A)
  | letin {A B} (x : String) : QTerm A → QTerm B → QTerm B
```

### 3.3 操作语义

**定义 3.3.1** (配置). 配置为 ⟨t, σ, q⟩ 其中:

- t: 待求值的项
- σ: 变量到量子比特索引的映射
- q: 量子存储 (量子寄存器状态)

**归约规则**:

```
⟨(λx.t) u, σ, q⟩ → ⟨t[u/x], σ, q⟩
⟨apply U (new |0⟩), σ, q⟩ → ⟨new |ψ⟩, σ', q'⟩  where |ψ⟩ = U|0⟩
⟨meas (new |ψ⟩), σ, q⟩ → ⟨|b⟩, σ, q_b⟩        with prob |⟨b|ψ⟩|²
```

**小步语义**:

$$
\langle t, \sigma, q \rangle \xrightarrow{\alpha} \langle t', \sigma', q' \rangle
$$

其中 α 可以是 τ (内部动作) 或经典比特 b。

### 3.4 指称语义

**定义 3.4.1** (指称语义). 类型 A 的指称为密度算子空间:

$$
⟦\text{Qubit}⟧ = \mathcal{D}(\mathbb{C}^2) = \{\rho \in \mathcal{L}(\mathbb{C}^2) : \rho \geq 0, \text{Tr}(\rho) = 1\}
$$

$$
⟦A \otimes B⟧ = \mathcal{D}(\mathcal{H}_A \otimes \mathcal{H}_B)
$$

**完全正映射 (CPTP)**:

$$
⟦A \multimap B⟧ = \{ \mathcal{E} : \mathcal{D}(\mathcal{H}_A) \to \mathcal{D}(\mathcal{H}_B) \mid \text{CPTP} \}
$$

**语义解释**:

$$
⟦\text{new } |0\rangle⟧ = |0\rangle\langle0|
$$

$$
⟦\text{apply } U⟧(\rho) = U\rho U^\dagger
$$

$$
⟦\text{meas}⟧(\rho) = |0\rangle\langle0| \cdot \text{Tr}(|0\rangle\langle0|\rho) + |1\rangle\langle1| \cdot \text{Tr}(|1\rangle\langle1|\rho)
$$

---

## 4. 线性类型系统

### 4.1 线性逻辑

**定义 4.1.1** (线性逻辑). 由 Girard 引入，强调资源的有限使用。

**结构规则对比**:

| 规则 | 经典逻辑 | 线性逻辑 |
|------|----------|----------|
| Weakening | Γ ⊢ A ⇒ Γ, B ⊢ A | ❌ 禁止 |
| Contraction | Γ, B, B ⊢ A ⇒ Γ, B ⊢ A | ❌ 禁止 |
| Exchange | Γ, A, B, Δ ⊢ C ⇒ Γ, B, A, Δ ⊢ C | ✅ 允许 |

**线性蕴涵** (⊸): A ⊸ B 表示 "消耗 A 产生 B"

### 4.2 类型构造子

**定义 4.2.1** (乘法连接词):

- **张量积 (⊗)**: A ⊗ B 表示同时拥有 A 和 B 两个资源
- **单位 (1)**: 空资源

**定义 4.2.2** (加法连接词):

- **与 (&)**: A & B 表示选择拥有 A 或 B (外部选择)
- **或 (⊕)**: A ⊕ B 表示拥有 A 或 B 之一 (内部选择)

**定义 4.2.3** (指数):

- **!A** (bang): 可丢弃、可复制的资源
- **?A** (问号): 对偶概念

**导出规则**:

```
Γ ⊢ t: A    Δ ⊢ u: B
--------------------- (⊗I)
Γ, Δ ⊢ ⟨t, u⟩: A ⊗ B

Γ, x: A, y: B ⊢ t: C
------------------------ (⊗E)
Γ, z: A ⊗ B ⊢ let ⟨x, y⟩ = z in t: C
```

### 4.3 类型规则

**定义 4.3.1** (量子λ演算类型系统).

**变量**:

```
----------- (Var)
x: A ⊢ x: A
```

**线性函数**:

```
Γ, x: A ⊢ t: B
------------------- (⊸I)
Γ ⊢ λx.t: A ⊸ B

Γ ⊢ t: A ⊸ B    Δ ⊢ u: A
------------------------ (⊸E)
Γ, Δ ⊢ t u: B
```

**张量积**:

```
Γ ⊢ t: A    Δ ⊢ u: B
--------------------- (⊗I)
Γ, Δ ⊢ ⟨t, u⟩: A ⊗ B

Γ, x: A, y: B ⊢ t: C
------------------------------ (⊗E)
Γ, z: A ⊗ B ⊢ let ⟨x, y⟩ = z in t: C
```

**量子操作**:

```
------------- (New0)
⊢ new |0⟩: Qubit

Γ ⊢ t: Qubit
------------------------ (Unitary)
Γ ⊢ apply U t: Qubit

Γ ⊢ t: Qubit
---------------- (Meas)
Γ ⊢ meas t: !Bit
```

### 4.4 线性约束

**定义 4.4.1** (线性环境). 环境 Γ 包含线性假设 x : A 和非线性假设 u : !A。

**约束检查**:

1. **线性变量**: 在项中恰好出现一次
2. **非线性变量**: 通过 dereliction 使用，可任意复制/丢弃

**子类型关系**:

```
!A <: A    (可降级使用)
A <: ?A    (可升级)
```

**Lean 4 示例: 类型检查器**

```lean4
/-- 环境: 变量名到(类型, 是否线性)的映射 -/
def Env := List (String × QType × Bool)

/-- 检查变量是否线性 -/
def isLinear (Γ : Env) (x : String) : Bool :=
  match Γ.find? (λ (y, _, lin) => y = x ∧ lin) with
  | some _ => true
  | none => false

/-- 类型检查: 返回剩余环境和推导的类型 -/
def typeCheck (Γ : Env) : QTerm A → Option (Env × QType)
  | QTerm.var x => do
      let (A, lin) ← Γ.lookup x
      if lin then
        -- 线性变量: 从环境移除
        some (Γ.filter (λ (y, _, _) => y ≠ x), A)
      else
        some (Γ, A)
  | QTerm.lam x t => do
      -- 假设 x: A 线性
      let (Γ', B) ← typeCheck ((x, A, true) :: Γ) t
      -- 检查 x 被使用
      if (Γ'.any (λ (y, _, _) => y = x)) then none
      else some (Γ', QType.Lolli A B)
  | QTerm.app t u => do
      let (Γ₁, QType.Lolli A B) ← typeCheck Γ t
      let (Γ₂, A') ← typeCheck Γ₁ u
      if A = A' then some (Γ₂, B) else none
  | QTerm.pair t u => do
      let (Γ₁, A) ← typeCheck Γ t
      let (Γ₂, B) ← typeCheck Γ₁ u
      some (Γ₂, QType.Tensor A B)
  | QTerm.new0 => some (Γ, QType.Qubit)
  | QTerm.new1 => some (Γ, QType.Qubit)
  | _ => sorry
```

---

## 5. 量子线路

### 5.1 线路模型

**定义 5.1.1** (量子线路). 量子线路 C = (n, m, G, W) 其中:

- n: 输入量子比特数
- m: 输出量子比特数
- G: 门集合 {g₁, ..., gₖ}
- W: 线路布线 (门之间的连接)

**线路图表示**:

```
|0⟩ ──[H]─┬────[M]───
          │
|0⟩ ─────[⊕]─────────
          │
|ψ⟩ ──────┘
```

**定义 5.1.2** (线路语义). 线路 C 的语义为复合酉变换:

$$
⟦C⟧ = U_k \circ \cdots \circ U_2 \circ U_1
$$

### 5.2 门集合

**定义 5.2.1** (通用门集合). 门集合 G 是通用的，若任意酉变换可被 G 中的门逼近。

**标准门集合**:

| 门 | 矩阵 | 描述 |
|----|------|------|
| H | $\frac{1}{\sqrt{2}}\begin{pmatrix} 1 & 1 \\ 1 & -1 \end{pmatrix}$ | Hadamard |
| X | $\begin{pmatrix} 0 & 1 \\ 1 & 0 \end{pmatrix}$ | Pauli-X |
| Y | $\begin{pmatrix} 0 & -i \\ i & 0 \end{pmatrix}$ | Pauli-Y |
| Z | $\begin{pmatrix} 1 & 0 \\ 0 & -1 \end{pmatrix}$ | Pauli-Z |
| S | $\begin{pmatrix} 1 & 0 \\ 0 & i \end{pmatrix}$ | 相位 |
| T | $\begin{pmatrix} 1 & 0 \\ 0 & e^{i\pi/4} \end{pmatrix}$ | π/8门 |
| CNOT | 见上 | 受控非门 |
| Toffoli | 3-qubit | 受控受控非门 |

**Clifford+T 门集合**: {H, S, CNOT, T} 是通用的且容错。

**Lean 4 示例: 量子线路表示**

```lean4
/-- 单量子比特门 -/
inductive SingleQubitGate
  | I | X | Y | Z | H | S | T

def SingleQubitGate.toMatrix : SingleQubitGate → Matrix (Fin 2) (Fin 2) Complex
  | I => !![1, 0; 0, 1]
  | X => !![0, 1; 1, 0]
  | Y => !![0, -Complex.I; Complex.I, 0]
  | Z => !![1, 0; 0, -1]
  | H => (1 / Real.sqrt 2) • !![1, 1; 1, -1]
  | S => !![1, 0; 0, Complex.I]
  | T => !![1, 0; 0, Complex.exp (Complex.I * Real.pi / 4)]

/-- 双量子比特门 -/
inductive TwoQubitGate
  | CNOT | CZ | SWAP

def TwoQubitGate.toMatrix : TwoQubitGate → Matrix (Fin 4) (Fin 4) Complex
  | CNOT => !![1, 0, 0, 0; 0, 1, 0, 0; 0, 0, 0, 1; 0, 0, 1, 0]
  | CZ   => !![1, 0, 0, 0; 0, 1, 0, 0; 0, 0, 1, 0; 0, 0, 0, -1]
  | SWAP => !![1, 0, 0, 0; 0, 0, 1, 0; 0, 1, 0, 0; 0, 0, 0, 1]

/-- 门应用: 位置和类型 -/
structure GateApp where
  target : Nat
  control : Option Nat
  gate : SingleQubitGate ⊕ TwoQubitGate

/-- 量子线路 -/
structure QuantumCircuit (n : Nat) where
  gates : List GateApp
  valid : ∀ g ∈ gates, g.target < n ∧ ∀ c ∈ g.control, c < n
```

### 5.3 线路等价

**定义 5.3.1** (线路等价). 两个线路 C₁, C₂ 等价若 ∀|ψ⟩, C₁|ψ⟩ = C₂|ψ⟩。

**等价变换规则**:

```
H² = I    X² = I    Y² = I    Z² = I
HXH = Z   HYH = -Y   HZH = X
CNOT₁₂ ∘ CNOT₁₂ = I
CNOT₂₁ ∘ CNOT₁₂ ∘ CNOT₂₁ = CNOT₁₂ ∘ CNOT₂₁ ∘ CNOT₁₂
```

**定义 5.3.2** (电路恒等式).

**相位门关系**:

$$
S^2 = Z, \quad T^2 = S, \quad T^4 = Z, \quad T^8 = I
$$

**交换关系**:

$$
\text{CNOT}_{12} \circ (Z \otimes I) = (Z \otimes I) \circ \text{CNOT}_{12}
$$

### 5.4 编译

**定义 5.4.1** (高级语言到线路). 编译函数 C: QTerm → QuantumCircuit。

**编译规则**:

```
C(new |0⟩) = |0⟩
C(new |1⟩) = X|0⟩
C(apply U t) = C(t) ∘ U
C(λx.t) = 电路抽象
C(t u) = 电路应用
```

**线路优化**:

1. **门合并**: 连续的单量子比特门合并
2. **取消**: U ∘ U† = I
3. **交换**: 交换可交换的门

---

## 6. 高级主题

### 6.1 隐形传态

**定义 6.1.1** (量子隐形传态). 协议将未知量子态 |ψ⟩ 从 Alice 传输到 Bob，使用共享纠缠和经典通信。

**协议步骤**:

1. **准备**: Alice 和 Bob 共享贝尔态 |Φ⁺⟩
2. **纠缠**: Alice 对其量子比特和 |ψ⟩ 进行 CNOT 和 H
3. **测量**: Alice 测量得到经典比特 (a, b)
4. **传输**: Alice 发送 (a, b) 给 Bob
5. **纠正**: Bob 应用 Z^b X^a 恢复 |ψ⟩

**线路图**:

```
|ψ⟩ ──┬─[H]─[M]──────────────┐
      │  │                    │
      ├──⊕──────────(c=a)────[X]─|ψ⟩
      │                 │
|Φ⁺⟩─┘                 └────[Z]─
```

**数学证明**:

设 |ψ⟩ = α|0⟩ + β|1⟩，共享态 |Φ⁺⟩ = (|00⟩ + |11⟩)/√2

总态:

$$
|\psi\rangle \otimes |\Phi^+\rangle = \frac{1}{\sqrt{2}}(\alpha|000\rangle + \alpha|011\rangle + \beta|100\rangle + \beta|111\rangle)
$$

经过 CNOT 和 H:

$$
= \frac{1}{2}[|00\rangle(\alpha|0\rangle + \beta|1\rangle) + |01\rangle(\alpha|1\rangle + \beta|0\rangle) \\
   + |10\rangle(\alpha|0\rangle - \beta|1\rangle) + |11\rangle(\alpha|1\rangle - \beta|0\rangle)]
$$

Bob 根据 (a, b) 应用修正:

| ab | Bob 操作 |
|----|----------|
| 00 | I        |
| 01 | X        |
| 10 | Z        |
| 11 | ZX       |

### 6.2 超密编码

**定义 6.2.1** (超密编码). 协议使用 1 个量子比特传输 2 个经典比特。

**协议**:

1. Alice 和 Bob 共享 |Φ⁺⟩
2. Alice 根据消息 (a, b) 应用操作:
   - 00: I
   - 01: X
   - 10: Z
   - 11: ZX
3. Alice 发送其量子比特给 Bob
4. Bob 应用 CNOT 然后 H，测量得到 (a, b)

**信息论意义**: 1 个量子比特 + 1 个纠缠对 = 2 经典比特

### 6.3 量子纠错

**定义 6.3.1** (量子纠错码). 将 k 个逻辑量子比特编码到 n 个物理量子比特，纠正 t 个错误。

**Steane 码 [[7,1,3]]**:

编码 1 个逻辑量子比特到 7 个物理量子比特，纠正任意单比特错误。

**Shor 码 [[9,1,3]]**:

编码 1 个逻辑量子比特到 9 个物理量子比特。

**表面码 (Surface Code)**:

拓扑量子纠错码，容错阈值约 1%。

---

## 7. 形式化定理与证明

### 定理 1 (线性保持)

**定理 7.1.1** (良类型程序保持线性约束). 若 Γ ⊢ t : A 且 t →* t'，则 Γ ⊢ t' : A 且 t' 满足相同的线性约束。

**证明**:

对归约步骤 t → t' 进行归纳。

**情况 1** (β归约): (λx.t) u → t[u/x]

由类型规则，x: A ⊢ t : B 且 Δ ⊢ u : A，其中 Γ = Δ, Θ。

替换后，t[u/x] 中 x 的出现被 u 替代，由于 x 恰好使用一次，u 也恰好使用一次，保持线性。

**情况 2** (张量归约): let ⟨x, y⟩ = ⟨t, u⟩ in s → s[t/x, u/y]

由类型规则，Γ₁ ⊢ t : A, Γ₂ ⊢ u : B, x: A, y: B ⊢ s : C。

替换后，x 和 y 各被使用一次，线性保持。

**情况 3** (量子门应用): apply U (apply V t) → apply (U∘V) t

门应用保持输入输出类型，线性约束不变。

**证毕** ∎

### 定理 2 (酉性)

**定理 7.2.1** (无测量程序的语义是酉变换). 若 Γ ⊢ t : A 且 t 不包含 meas 构造，则 ⟦t⟧ 是完全正保迹 (CPTP) 映射，且可表示为共轭作用 ⟦t⟧(ρ) = UρU†。

**证明**:

对 t 的结构进行归纳。

**基本情况**:

- new |0⟩: 语义为 |0⟩⟨0|，可由 U = |0⟩⟨0| 的共轭作用表示
- new |1⟩: 类似

**归纳步骤**:

- apply U t: 由归纳假设 ⟦t⟧ = V·V†，则 ⟦apply U t⟧ = UV·(UV)†，仍为酉
- t ⊗ u: 张量积保持酉性，U ⊗ V 是酉的
- t ∘ u: 复合保持酉性

由于无测量，无概率性分支，语义为纯酉演化。

**证毕** ∎

### 定理 3 (不可克隆)

**定理 7.3.1** (线性类型阻止量子克隆). 不存在良类型的项 ⊢ clone : Qubit ⊸ Qubit ⊗ Qubit 使得 ⟦clone |ψ⟩⟧ = |ψ⟩ ⊗ |ψ⟩。

**证明**:

**步骤 1** (线性约束): clone 的类型 Qubit ⊸ Qubit ⊗ Qubit 要求输入量子比特被消耗。

**步骤 2** (物理不可能性): 假设存在这样的 clone，则由线性性，它必须是线性映射。

但量子克隆要求 |
ψ⟩ → |
ψ⟩ ⊗ |
ψ⟩，这不是线性的:

$$
clone(\alpha|0\rangle + \beta|1\rangle) = \alpha^2|00\rangle + \alpha\beta|01\rangle + \alpha\beta|10\rangle + \beta^2|11\rangle
$$

$$
\neq \alpha clone(|0\rangle) + \beta clone(|1\rangle) = \alpha|00\rangle + \beta|11\rangle
$$

**步骤 3** (类型系统保证): 线性类型系统禁止非线性操作，因此这样的 clone 无法被类型化。

**证毕** ∎

### 定理 4 (隐形传态正确性)

**定理 7.4.1** (隐形传态协议正确传输量子态). 设 Teleport: Qubit ⊗ Bell → !Bit ⊗ !Bit ⊗ Qubit 为隐形传态程序，则对任意 |
ψ⟩:

$$
⟦Teleport⟧(|\psi\rangle \otimes |\Phi^+\rangle) = \sum_{a,b \in \{0,1\}} \frac{1}{4} |a\rangle\langle a| \otimes |b\rangle\langle b| \otimes |\psi\rangle\langle\psi|
$$

即 Bob 的量子比特总是 |
ψ⟩，与测量结果无关。

**证明**:

**步骤 1** (协议形式化):

```
Teleport |ψ⟩ |Φ⁺⟩ =
  let ⟨q₁, q₂⟩ = |Φ⁺⟩ in
  let q₁' = apply H (apply CNOT q₁ |ψ⟩) in
  let (a, b) = meas q₁' in
  let q₂' = apply Z^b (apply X^a q₂) in
  (a, b, q₂')
```

**步骤 2** (计算总态):

初始态: |
ψ⟩ₐ ⊗ |Φ⁺⟩_{bc} = (α|0⟩ + β|1⟩)ₐ ⊗ (|00⟩ + |11⟩)_{bc}/√2

**步骤 3** (Alice 的操作):

CNOT_{ab}: |
ψ⟩ ⊗ |Φ⁺⟩ → (α|000⟩ + α|011⟩ + β|110⟩ + β|101⟩)/√2

Hₐ: → (α(|0⟩+|1⟩)|00⟩ + α(|0⟩+|1⟩)|11⟩ + β(|0⟩-|1⟩)|10⟩ + β(|0⟩-|1⟩)|01⟩)/2

**步骤 4** (展开测量基):

= [|00⟩(α|0⟩ + β|1⟩) + |01⟩(β|0⟩ + α|1⟩) + |10⟩(α|0⟩ - β|1⟩) + |11⟩(-β|0⟩ + α|1⟩)]/2

**步骤 5** (Bob 的修正):

对每个测量结果 (a, b)，Bob 应用 Z^b X^a，总能恢复 |
ψ⟩。

**证毕** ∎

### 定理 5 (Deutsch-Jozsa)

**定理 7.5.1** (DJ算法只需一次查询). 设 f: {0,1}ⁿ → {0,1} 为常数或平衡函数，Deutsch-Jozsa 算法通过一次查询 U_f 即可区分。

**证明**:

**算法步骤**:

1. 初始化: |0⟩^{⊗n} ⊗ |1⟩
2. 应用 H^{⊗(n+1)}: |+⟩^{⊗n} ⊗ |−⟩
3. 应用 U_f: Σₓ |x⟩ ⊗ |f(x)⟩
4. 应用 H^{⊗n} ⊗ I
5. 测量

**计算**:

步骤 2 后: $\frac{1}{\sqrt{2^n}} \sum_{x \in \{0,1\}^n} |x\rangle \otimes |-\rangle$

步骤 3 后:

$$
\frac{1}{\sqrt{2^n}} \sum_x |x\rangle \otimes \frac{|f(x)\rangle - |1-f(x)\rangle}{\sqrt{2}} = \frac{1}{\sqrt{2^n}} \sum_x (-1)^{f(x)} |x\rangle \otimes |-\rangle
$$

步骤 4 后，测量第一个寄存器:

$$
H^{\otimes n} \left(\sum_x (-1)^{f(x)} |x\rangle\right) = \sum_y \left(\sum_x \frac{(-1)^{x \cdot y + f(x)}}{2^n}\right) |y\rangle
$$

**情况 1** (f 常数): f(x) = c

振幅为 $(-1)^c \sum_x (-1)^{x \cdot y} / 2^n = (-1)^c \delta_{y,0}$

测量结果必为 |0...0⟩

**情况 2** (f 平衡):

对 y = 0，振幅 = Σₓ (-1)^{f(x)} / 2ⁿ = 0 (一半 f(x)=0，一半 f(x)=1)

测量结果不为 |0...0⟩ 的概率为 1。

**证毕** ∎

---

## 8. Lean 4实现

### 量子态实现

```lean4
import Mathlib

namespace Quantum

/-- n-量子比特态 -/
def QState (n : Nat) := { v : Fin (2^n) → Complex //
  Finset.sum (Finset.univ) (fun i => (v i).normSq) = 1 }

def QubitState := QState 1

/-- 计算基态 -/
def computationalBasis (n : Nat) (i : Fin (2^n)) : QState n :=
  ⟨fun j => if i = j then 1 else 0, by simp [Finset.sum_ite]
   <;> exact Finset.sum_eq_single i (by simp) (by simp)⟩

/-- 单量子比特门应用 -/
def applySingleGate (U : Matrix (Fin 2) (Fin 2) Complex)
    (h : U * U.conjTranspose = 1) (ψ : QubitState) : QubitState :=
  ⟨fun i => Finset.sum (Finset.univ) (fun j => U i j * ψ.1 j),
   by sorry⟩ -- 需证明酉变换保持归一化

/-- CNOT门应用 -/
def applyCNOT (ψ : QState 2) : QState 2 :=
  ⟨fun i => match i with
    | ⟨0, _⟩ => ψ.1 ⟨0, by simp⟩
    | ⟨1, _⟩ => ψ.1 ⟨1, by simp⟩
    | ⟨2, _⟩ => ψ.1 ⟨3, by simp⟩
    | ⟨3, _⟩ => ψ.1 ⟨2, by simp⟩
    | _ => 0,
   by sorry⟩

end Quantum
```

### 量子隐形传态实现

```lean4
/-- 贝尔态准备 -/
def prepareBell : QState 2 :=
  ⟨fun i => match i with
    | ⟨0, _⟩ => 1 / Real.sqrt 2
    | ⟨3, _⟩ => 1 / Real.sqrt 2
    | _ => 0,
   by sorry⟩

/-- 隐形传态线路 -/
def teleportationCircuit (ψ : QubitState) :
    Fin 4 → Complex := fun i =>
  -- ψ ⊗ |Φ⁺⟩ 在计算基下的展开
  -- 经过CNOT和H后的态
  sorry

/-- 隐形传态正确性定理 -/
theorem teleportation_correct (ψ : QubitState) :
  let result := teleportationCircuit ψ
  -- Bob的量子比特等于原始态
  sorry := by sorry
```

---

## 9. 参考文献

1. **Nielsen, M.A. & Chuang, I.L.** (2010). _Quantum Computation and Quantum Information_ (10th ed.). CUP.
2. **Selinger, P.** (2004). Towards a quantum programming language. _MSCS_, 14(4), 527-586.
3. **Girard, J.Y.** (1987). Linear logic. _TCS_, 50, 1-102.
4. **Wadler, P.** (1990). Linear types can change the world! _IFIP TC_.
5. **Abramsky, S. & Coecke, B.** (2004). A categorical semantics of quantum protocols. _LICS_, 415-425.
6. **Coecke, B. & Kissinger, A.** (2017). _Picturing Quantum Processes_. CUP.
7. **Paykin, J., Rand, R. & Zdancewic, S.** (2017). QWIRE: a core language for quantum circuits. _POPL_.
8. **de Lavalette, G.R.R.** et al. (2022). Quantum programming languages. _arXiv:2206.05260_.

---

## 10. 交叉引用

### 前置依赖

- **[SEMANTIC_NOTATIONS.md](../../SEMANTIC_NOTATIONS.md)** — 符号标准
- **[CATEGORICAL_SEMANTICS.md](./CATEGORICAL_SEMANTICS.md)** — 范畴论语义基础

### 横向关联

- **[ADVANCED_SEMANTICS.md](../07_Advanced_Semantics/ADVANCED_SEMANTICS.md)** — 线性类型系统深化
- **[SEMANTICS_UNIFIED_REVISED.md](../SEMANTICS_UNIFIED_REVISED.md)** — 统一语义框架

### 后续扩展

- 拓扑量子计算 — 任意子编织
- 量子误差纠正形式化
- 量子密码学语义
- 量子机器学习类型系统

---

_版本: 1.0 | 级别: L6+++ | 更新: 2026-03-24_
