# Mathlib：Lean 4的统一数学库

> **层级定位**: 05 Deep Structure MetaPhysics / 05 Lean 4
> **目标读者**: 希望使用Mathlib进行数学形式化的Lean用户
> **难度级别**: L4 应用 - L6 创造
> **预估学习时间**: 8-10 小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **库规模** | 600万+行代码，4000+定义文件 |
| **覆盖范围** | 数学全分支 |
| **社区** | 最活跃的形式化数学社区 |
| **前置要求** | 掌握Lean 4基础和证明策略 |

---

## 📑 目录

- [Mathlib：Lean 4的统一数学库](#mathliblean-4的统一数学库)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [1. Mathlib简介](#1-mathlib简介)
    - [1.1 什么是Mathlib？](#11-什么是mathlib)
    - [1.2 发展历程](#12-发展历程)
    - [1.3 设计理念](#13-设计理念)
    - [1.4 安装与配置](#14-安装与配置)
  - [2. 核心模块架构](#2-核心模块架构)
    - [2.1 模块层次图](#21-模块层次图)
    - [2.2 类型类层次结构](#22-类型类层次结构)
    - [2.3 核心类型类速查](#23-核心类型类速查)
  - [3. 代数结构](#3-代数结构)
    - [3.1 群论 (Algebra.Group)](#31-群论-algebragroup)
    - [3.2 环论与域论 (Algebra.Ring, Algebra.Field)](#32-环论与域论-algebraring-algebrafield)
    - [3.3 线性代数 (LinearAlgebra)](#33-线性代数-linearalgebra)
  - [4. 分析与拓扑](#4-分析与拓扑)
    - [4.1 数学分析 (Analysis)](#41-数学分析-analysis)
    - [4.2 拓扑学 (Topology)](#42-拓扑学-topology)
  - [5. 数论](#5-数论)
    - [5.1 基础数论 (NumberTheory)](#51-基础数论-numbertheory)
    - [5.2 高级数论](#52-高级数论)
  - [6. 组合与离散数学](#6-组合与离散数学)
    - [6.1 组合数学 (Combinatorics)](#61-组合数学-combinatorics)
    - [6.2 序理论 (Order)](#62-序理论-order)
  - [7. 数值计算](#7-数值计算)
    - [7.1 数值类型 (Data)](#71-数值类型-data)
    - [7.2 浮点数与近似计算](#72-浮点数与近似计算)
  - [8. 在C验证中的应用](#8-在c验证中的应用)
    - [8.1 使用Mathlib验证C算法](#81-使用mathlib验证c算法)
    - [8.2 数值算法正确性](#82-数值算法正确性)
    - [8.3 概率与统计](#83-概率与统计)
  - [9. 使用Mathlib的最佳实践](#9-使用mathlib的最佳实践)
    - [9.1 导入策略](#91-导入策略)
    - [9.2 搜索与发现](#92-搜索与发现)
    - [9.3 常见模式与技巧](#93-常见模式与技巧)
    - [9.4 性能考虑](#94-性能考虑)

---

## 1. Mathlib简介

### 1.1 什么是Mathlib？

```
┌─────────────────────────────────────────────────────────────────────┐
│                          Mathlib 概览                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   规模:     600万+ 行代码                                            │
│   贡献者:   300+ 活跃贡献者                                          │
│   文件数:   4000+ .lean 文件                                         │
│   定理数:   100,000+ 已证明定理                                      │
│   定义数:   20,000+ 定义                                             │
│                                                                      │
│   愿景:     将本科到研究生水平的数学完全形式化                        │
│   特色:     统一、连贯的数学库设计                                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 1.2 发展历程

| 时间 | 里程碑 | 说明 |
|------|--------|------|
| 2017 | Mathlib创立 | 基于Lean 3 |
| 2019 | 100万行 | 初步成熟 |
| 2021 | Lean 4迁移启动 | 重大架构升级 |
| 2023 | Mathlib4发布 | 完整迁移到Lean 4 |
| 2024 | 600万行 | 覆盖大部分本科数学 |
| 2025+ | FLT项目 | 费马大定理形式化 |

### 1.3 设计理念

```lean
-- ============================================
-- Mathlib的设计原则
-- ============================================

/-
1. 泛化优先：在最大适用范围内定义概念
2. 类型类层次：构建丰富的代数结构层次
3. 记号友好：支持标准数学记号
4. 证明可复用：建立通用的证明模式
5. 文档完备：每个定义都有文档字符串
-/

-- 示例: 群在Mathlib中的泛化定义
class Group (G : Type u) extends DivInvMonoid G where
  inv_mul_cancel : ∀ a : G, a⁻¹ * a = 1

-- 自动继承所有Monoid性质
-- 自动获得幂运算、单位元性质等
```

### 1.4 安装与配置

```bash
# 创建使用Mathlib的新项目
lake new my_math_project math

cd my_math_project
# 自动下载mathlib依赖
lake update

# 构建项目
lake build
```

```lean
-- lakefile.lean配置示例
import Lake
open Lake DSL

package my_project where
  -- 添加Mathlib依赖
  require mathlib from git
    "https://github.com/leanprover-community/mathlib4.git"
```

---

## 2. 核心模块架构

### 2.1 模块层次图

```
Mathlib/
├── Algebra/           # 代数结构
│   ├── Group/         # 群论
│   ├── Ring/          # 环论
│   ├── Field/         # 域论
│   ├── Module/        # 模论/线性代数
│   └── Category/      # 范畴论
├── Analysis/          # 数学分析
│   ├── NormedSpace/   # 赋范空间
│   ├── InnerProduct/  # 内积空间
│   ├── Calculus/      # 微积分
│   └── Complex/       # 复分析
├── Topology/          # 拓扑学
│   ├── Basic/         # 基础拓扑
│   ├── Algebra/       # 拓扑代数
│   └── Sheaves/       # 层论
├── NumberTheory/      # 数论
│   ├── Diophantine/   # 丢番图方程
│   ├── Zsqrtd/        # 二次整数环
│   └── Legendre/      # 二次互反律
├── Data/              # 数据结构
│   ├── Nat/           # 自然数
│   ├── Int/           # 整数
│   ├── Rat/           # 有理数
│   └── Real/          # 实数
├── Logic/             # 逻辑学
├── Order/             # 序理论
├── MeasureTheory/     # 测度论
├── Probability/       # 概率论
├── LinearAlgebra/     # 线性代数
├── Combinatorics/     # 组合数学
└── SetTheory/         # 集合论
```

### 2.2 类型类层次结构

```lean
-- ============================================
-- 代数结构类型类层次
-- ============================================

/-
类型类继承图（简化）:

Monoid
├── CommMonoid
│   └── CancelCommMonoid
└── Group
    ├── CommGroup
    └── AddGroup (加法记号)

Ring
├── CommRing
│   ├── EuclideanDomain
│   │   └── Field
│   │       └── DiscreteField
│   └── IsDomain
└── DivisionRing

Module R
├── LieModule
└── NormedSpace
    └── InnerProductSpace

-/

-- 查看完整层次使用#synth
#synth Group ℤ  -- 证明整数构成加法群
#synth Field ℚ  -- 证明有理数构成域
```

### 2.3 核心类型类速查

| 类型类 | 数学概念 | 示例实例 |
|--------|----------|----------|
| `Group` | 群 | `ℤ`, `ℝ⁺`, 置换群 |
| `Ring` | 环 | `ℤ`, `ℤ[i]`, 多项式环 |
| `Field` | 域 | `ℚ`, `ℝ`, `ℂ`, `ℤ/pℤ` |
| `Module R` | R-模 | 向量空间、阿贝尔群 |
| `Algebra R A` | R-代数 | `ℂ`作为`ℝ`-代数 |
| `TopologicalSpace` | 拓扑空间 | `ℝ`, 度量空间 |
| `MetricSpace` | 度量空间 | `ℝⁿ`, 赋范空间 |
| `NormedSpace` | 赋范空间 | `ℝⁿ`, 函数空间 |
| `InnerProductSpace` | 内积空间 | `ℝⁿ`, `ℂⁿ`, `L²` |
| `MeasureSpace` | 测度空间 | Lebesgue测度空间 |

---

## 3. 代数结构

### 3.1 群论 (Algebra.Group)

```lean
import Mathlib.GroupTheory.Subgroup
import Mathlib.GroupTheory.QuotientGroup

-- ============================================
-- 基本群论概念
-- ============================================

-- 定义子群
variable {G : Type*} [Group G]

def isSubgroup (H : Set G) : Prop :=
  1 ∈ H ∧ ∀ a b, a ∈ H → b ∈ H → a * b ∈ H ∧ a⁻¹ ∈ H

-- 使用Mathlib的子群类型
#check Subgroup G  -- G的所有子群

-- 子群示例：偶数子群
example : AddSubgroup ℤ where
  carrier := Set.range (2 * · : ℤ → ℤ)
  zero_mem' := ⟨0, by simp⟩
  add_mem' := by
    rintro _ _ ⟨a, rfl⟩ ⟨b, rfl⟩
    use a + b
    ring
  neg_mem' := by
    rintro _ ⟨a, rfl⟩
    use -a
    ring

-- ============================================
-- 商群
-- ============================================

-- ℤ/nℤ 的定义和性质
example (n : ℕ) : AddGroup (ZMod n) := by
  infer_instance  -- Mathlib自动提供实例

-- 商群同态基本定理
example {G H : Type*} [Group G] [Group H] (φ : G →* H) :
    G ⧸ φ.ker ≃* φ.range := by
  apply QuotientGroup.quotientKerEquivRange
```

### 3.2 环论与域论 (Algebra.Ring, Algebra.Field)

```lean
import Mathlib.RingTheory.Ideal
import Mathlib.RingTheory.Polynomial.Basic

-- ============================================
-- 多项式环
-- ============================================

-- 多项式环 R[X]
variable {R : Type*} [CommRing R]

#check R[X]  -- R上的多项式环

-- 多项式求值
example (p : ℝ[X]) (x : ℝ) : ℝ :=
  p.eval x

-- 多项式次数
example (p : ℝ[X]) : WithBot ℕ :=
  p.natDegree

-- 多项式除法
example (p q : ℝ[X]) (hq : q ≠ 0) : ℝ[X] × ℝ[X] :=
  p.divModByMonic q  -- 返回商和余式

-- ============================================
-- 理想与商环
-- ============================================

-- 主理想
example (a : ℤ) : Ideal ℤ :=
  Ideal.span {a}

-- 商环 ℤ/nℤ
example (n : ℕ) : ℤ ⧸ Ideal.span {(n : ℤ)} ≃+* ZMod n :=
  Int.quotientSpanEquivZMod n

-- ============================================
-- 域扩张
-- ============================================

-- 代数元素与超越元素
example : IsAlgebraic ℚ (Real.sqrt 2) := by
  use X ^ 2 - 2
  constructor
  · -- 证明多项式非零
    intro h
    have := congr_arg (eval 0) h
    simp at this
  · -- 证明是根
    simp [eval_sub, eval_pow, eval_X]
    norm_num
```

### 3.3 线性代数 (LinearAlgebra)

```lean
import Mathlib.LinearAlgebra.FiniteDimensional
import Mathlib.LinearAlgebra.Matrix

-- ============================================
-- 向量空间与线性映射
-- ============================================

-- 有限维向量空间
variable {K : Type*} [Field K] {V : Type*} [AddCommGroup V] [Module K V]

-- 维度
#check FiniteDimensional.finrank K V  -- V在K上的维度

-- 线性映射
variable {W : Type*} [AddCommGroup W] [Module K W]
#check V →ₗ[K] W  -- K-线性映射类型

-- 矩阵表示
#check Matrix (Fin m) (Fin n) K  -- m×n矩阵

-- 线性映射与矩阵的转换
example (f : V →ₗ[K] W) [FiniteDimensional K V] [FiniteDimensional K W] :
    Matrix (Fin (finrank K W)) (Fin (finrank K V)) K :=
  LinearMap.toMatrix (Basis.ofVectorSpace K V) (Basis.ofVectorSpace K W) f

-- ============================================
-- 特征值与特征向量
-- ============================================

-- 特征多项式
example (A : Matrix (Fin n) (Fin n) ℝ) : ℝ[X] :=
  Matrix.charpoly A

-- 行列式
example (A : Matrix (Fin n) (Fin n) ℝ) : ℝ :=
  A.det

-- 迹
example (A : Matrix (Fin n) (Fin n) ℝ) : ℝ :=
  A.trace
```

---

## 4. 分析与拓扑

### 4.1 数学分析 (Analysis)

```lean
import Mathlib.Analysis.Calculus.Deriv.Basic
import Mathlib.Analysis.SpecialFunctions.ExpLog

-- ============================================
-- 微积分基础
-- ============================================

-- 导数
example (f : ℝ → ℝ) (x : ℝ) : ℝ :=
  deriv f x

-- 导数计算示例
example : deriv (fun x => x ^ 2) 3 = 6 := by
  simp [deriv_pow]

-- 链式法则
example (f g : ℝ → ℝ) (x : ℝ) (hf : DifferentiableAt ℝ f (g x))
    (hg : DifferentiableAt ℝ g x) :
    deriv (fun x => f (g x)) x = deriv f (g x) * deriv g x := by
  apply deriv.comp
  exacts [hf, hg]

-- ============================================
-- 极限与连续性
-- ============================================

-- 序列极限
example (a : ℕ → ℝ) (L : ℝ) : Prop :=
  Filter.Tendsto a Filter.atTop (nhds L)

-- 等价于标准ε-N定义
example (a : ℕ → ℝ) (L : ℝ) :
    Filter.Tendsto a Filter.atTop (nhds L) ↔
    ∀ ε > 0, ∃ N, ∀ n ≥ N, |a n - L| < ε := by
  rw [Metric.tendsto_atTop]

-- 函数连续性
example (f : ℝ → ℝ) : Prop :=
  Continuous f

-- ============================================
-- 特殊函数
-- ============================================

-- 指数函数和对数
#check Real.exp  -- ℝ → ℝ
#check Real.log  -- ℝ → ℝ

-- 三角函数
#check Real.sin
#check Real.cos
#check Real.tan

-- 性质证明
example : Real.deriv_exp = Real.exp := by
  exact Real.deriv_exp

example (x : ℝ) : Real.sin (x + Real.pi / 2) = Real.cos x := by
  rw [Real.sin_add, Real.sin_pi_div_two, Real.cos_pi_div_two]
  ring
```

### 4.2 拓扑学 (Topology)

```lean
import Mathlib.Topology.Basic
import Mathlib.Topology.Instances.Real

-- ============================================
-- 拓扑空间基础
-- ============================================

-- 开集
variable {X : Type*} [TopologicalSpace X]

#check IsOpen  -- Set X → Prop

-- 闭集
#check IsClosed  -- Set X → Prop

-- 连续性
variable {Y : Type*} [TopologicalSpace Y]
#check Continuous  -- (X → Y) → Prop

-- 紧致性
#check IsCompact  -- Set X → Prop

-- ============================================
-- 实数的标准拓扑
-- ============================================

-- ℝ上的标准拓扑
#check (by infer_instance : TopologicalSpace ℝ)

-- 开区间是开集
example (a b : ℝ) : IsOpen (Set.Ioo a b) := by
  apply isOpen_Ioo

-- 闭区间是紧致的（Heine-Borel定理）
example (a b : ℝ) (h : a ≤ b) : IsCompact (Set.Icc a b) := by
  apply isCompact_Icc

-- ============================================
-- 度量空间
-- ============================================

-- 度量空间实例
#check (by infer_instance : MetricSpace ℝ)

-- 距离函数
example (x y : ℝ) : ℝ :=
  dist x y  -- = |x - y|

-- 开球
example (x : ℝ) (r : ℝ) : Set ℝ :=
  Metric.ball x r  -- {y | dist y x < r}
```

---

## 5. 数论

### 5.1 基础数论 (NumberTheory)

```lean
import Mathlib.NumberTheory.Divisors
import Mathlib.NumberTheory.Padics.PadicNumbers
import Mathlib.NumberTheory.Zsqrtd.Basic

-- ============================================
-- 整除与模运算
-- ============================================

-- 整除
example : 6 ∣ 24 := by
  norm_num

-- 最大公约数
#check Nat.gcd  -- Nat → Nat → Nat

-- 欧几里得算法
example (a b : ℕ) : Nat.gcd a b ∣ a ∧ Nat.gcd a b ∣ b :=
  ⟨Nat.gcd_dvd_left a b, Nat.gcd_dvd_right a b⟩

-- ============================================
-- 素数
-- ============================================

-- 素数判定
#check Nat.Prime  -- Nat → Prop

-- 素数无穷多
example : ∀ n : ℕ, ∃ p > n, Nat.Prime p := by
  apply Nat.infinite_setOf_prime

-- 算术基本定理（唯一分解）
example (n : ℕ) (hn : n ≠ 0) :
    ∃! factors : Multiset ℕ,
      (∀ p ∈ factors, Nat.Prime p) ∧ factors.prod = n :=
  Nat.factorization n

-- ============================================
-- 二次整数环
-- ============================================

-- ℤ[√d] 高斯整数是特例 d = -1
#check ℤ√(-1)  -- 高斯整数

-- 范数
example (z : ℤ√(-1)) : ℤ :=
  z.norm  -- = a² + b² for z = a + bi

-- 高斯整数是欧几里得整环
example : EuclideanDomain (ℤ√(-1)) := by
  infer_instance
```

### 5.2 高级数论

```lean
import Mathlib.NumberTheory.LegendreSymbol.QuadraticReciprocity

-- ============================================
-- 二次互反律
-- ============================================

-- Legendre符号
#check legendreSym  -- (p : ℕ) [Fact p.Prime] → ℤ → ℤ

-- 二次互反律
theorem quadratic_reciprocity {p q : ℕ} [Fact p.Prime] [Fact q.Prime]
    (hp : p ≠ 2) (hq : q ≠ 2) (hpq : p ≠ q) :
    legendreSym p q * legendreSym q p = (-1) ^ ((p / 2) * (q / 2)) := by
  apply quadratic_reciprocity'
```

---

## 6. 组合与离散数学

### 6.1 组合数学 (Combinatorics)

```lean
import Mathlib.Combinatorics.SimpleGraph.Basic
import Mathlib.Combinatorics.SetFamily.Compression

-- ============================================
-- 图论
-- ============================================

-- 简单图
variable {V : Type*} [Fintype V] [DecidableEq V]
#check SimpleGraph V  -- V上的简单图

-- 图的定义
example (G : SimpleGraph V) : V → V → Prop :=
  G.Adj  -- 邻接关系

-- 完全图
#check completeGraph V

-- 路径与连通性
#check SimpleGraph.Connected  -- 连通图
#check SimpleGraph.Path  -- 路径

-- ============================================
-- 组合计数
-- ============================================

-- 组合数
#check Nat.choose  -- Nat → Nat → Nat

-- 组合恒等式
example (n k : ℕ) (hk : k ≤ n) : Nat.choose n k = Nat.choose n (n - k) := by
  apply Nat.choose_symm
  exact hk

-- 二项式定理
example (x y : ℝ) (n : ℕ) :
    (x + y) ^ n = ∑ k in Finset.range (n + 1), Nat.choose n k * x ^ k * y ^ (n - k) := by
  rw [add_pow]
```

### 6.2 序理论 (Order)

```lean
import Mathlib.Order.Lattice
import Mathlib.Order.BooleanAlgebra

-- ============================================
-- 格与布尔代数
-- ============================================

-- 格结构
variable {L : Type*} [Lattice L]

#check (· ⊔ ·)  -- 上确界（并）
#check (· ⊓ ·)  -- 下确界（交）

-- 分配律
example [DistribLattice L] (a b c : L) :
    a ⊓ (b ⊔ c) = (a ⊓ b) ⊔ (a ⊓ c) := by
  apply inf_sup_left

-- 布尔代数
variable {B : Type*} [BooleanAlgebra B]

#check (·ᶜ)  -- 补运算

-- De Morgan定律
example (a b : B) : (a ⊔ b)ᶜ = aᶜ ⊓ bᶜ := by
  apply compl_sup
```

---

## 7. 数值计算

### 7.1 数值类型 (Data)

```lean
import Mathlib.Data.Real.Basic
import Mathlib.Data.Complex.Basic
import Mathlib.Data.Nat.Factorial.Basic

-- ============================================
-- 数系层次
-- ============================================

/-
Mathlib中的数系层次:

ℕ (Nat)    : 自然数
  ↓
ℤ (Int)    : 整数（Nat的群完备化）
  ↓
ℚ (Rat)    : 有理数（ℤ的域化）
  ↓
ℝ (Real)   : 实数（ℚ的完备化）
  ↓
ℂ (Complex): 复数（ℝ的代数闭包）

-/

-- 类型转换（提升）
example (n : ℕ) : ℤ := n  -- 自动提升
example (z : ℤ) : ℚ := z
example (q : ℚ) : ℝ := q
example (r : ℝ) : ℂ := r

-- ============================================
-- 实数构造
-- ============================================

-- Cauchy序列构造
#check Real  -- Cauchy序列的等价类

-- 实数性质
example : ConditionallyCompleteLinearOrder ℝ := by
  infer_instance

-- 上确界
example (S : Set ℝ) (h : S.Nonempty) (hb : BddAbove S) : ℝ :=
  sSup S  -- S的最小上界

-- ============================================
-- 复数运算
-- ============================================

-- 复数定义
example (z : ℂ) : ℂ :=
  z.re + z.im * Complex.I

-- 共轭
example (z : ℂ) : ℂ :=
  star z  -- 或 z.conj

-- 模
example (z : ℂ) : ℝ :=
  Complex.abs z  -- = √(z.re² + z.im²)

-- 欧拉公式
example (θ : ℝ) : Complex.exp (Complex.I * θ) = Real.cos θ + Complex.I * Real.sin θ :=
  Complex.exp_mul_I θ
```

### 7.2 浮点数与近似计算

```lean
-- ============================================
-- 浮点数表示（实验性支持）
-- ============================================

-- IEEE 754浮点数
#check Float  -- 内置浮点类型

-- 注意：浮点数计算在Lean中不是纯数学的
-- 通常用于与外部代码交互
```

---

## 8. 在C验证中的应用

### 8.1 使用Mathlib验证C算法

```lean
import Mathlib.Data.Nat.Basic
import Mathlib.Algebra.Order.Floor

-- ============================================
-- 整数运算验证
-- ============================================

/- C代码: 整数平方根
int isqrt(int n) {
    int r = 0;
    while ((r+1)*(r+1) <= n) r++;
    return r;
}
-/

-- Lean定义
def isqrt (n : ℕ) : ℕ :=
  Nat.sqrt n  -- Mathlib内置

-- 验证: 结果满足平方根定义
theorem isqrt_spec (n : ℕ) :
    (isqrt n) ^ 2 ≤ n ∧ n < (isqrt n + 1) ^ 2 := by
  apply Nat.sqrt_le'  -- Mathlib已证明

-- ============================================
-- 浮点近似验证
-- ============================================

/- C代码: Taylor级数近似sin(x)
double sin_approx(double x, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        int sign = (i % 2 == 0) ? 1 : -1;
        double term = sign * pow(x, 2*i+1) / factorial(2*i+1);
        sum += term;
    }
    return sum;
}
-/

-- 使用Mathlib分析Taylor级数余项
open Real

theorem sin_taylor_remainder (x : ℝ) (n : ℕ) :
    ∃ c, |c| ≤ |x| ∧
    |Real.sin x - ∑ i in Finset.range n, ((-1 : ℝ) ^ i * x ^ (2*i+1) / (2*i+1).factorial)|
      ≤ |x| ^ (2*n+1) / (2*n+1).factorial := by
  sorry  -- 需要Taylor定理的完整形式
```

### 8.2 数值算法正确性

```lean
import Mathlib.LinearAlgebra.Matrix.NonsingularInverse

-- ============================================
-- 矩阵运算验证
-- ============================================

/- C代码: 矩阵乘法
void matmul(double A[N][N], double B[N][N], double C[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}
-/

-- Mathlib矩阵乘法已验证
theorem matrix_mul_assoc {n : Type} [Fintype n] [DecidableEq n]
    (A B C : Matrix n n ℝ) :
    (A * B) * C = A * (B * C) := by
  apply Matrix.mul_assoc

-- 验证: 单位矩阵是乘法单位元
theorem matrix_mul_one {n : Type} [Fintype n] [DecidableEq n]
    (A : Matrix n n ℝ) :
    A * 1 = A := by
  apply Matrix.mul_one

-- ============================================
-- 线性方程组求解
-- ============================================

-- 可逆矩阵判定
example {n : Type} [Fintype n] [DecidableEq n] (A : Matrix n n ℝ) : Prop :=
  Invertible A  -- A可逆

-- 逆矩阵计算
example {n : Type} [Fintype n] [DecidableEq n] (A : Matrix n n ℝ) [Invertible A] :
    Matrix n n ℝ :=
  ⅟A  -- A的逆
```

### 8.3 概率与统计

```lean
import Mathlib.Probability.Distributions.Gaussian

-- ============================================
-- 随机数生成器验证（概念性）
-- ============================================

/- C代码: 高斯随机数（Box-Muller变换）
void box_muller(double *u1, double *u2, double *z1, double *z2) {
    double r = sqrt(-2 * log(*u1));
    double theta = 2 * M_PI * (*u2);
    *z1 = r * cos(theta);
    *z2 = r * sin(theta);
}
-/

-- 使用Mathlib的测度论定义高斯分布
#check MeasureTheory.gaussianReal  -- ℝ上的高斯测度

-- 验证Box-Muller产生标准正态分布（骨架）
theorem box_muller_correct (u1 u2 : ℝ) (hu1 : u1 ∈ Set.Ioo 0 1) (hu2 : u2 ∈ Set.Ioo 0 1) :
    let r := Real.sqrt (-2 * Real.log u1)
    let θ := 2 * Real.pi * u2
    let z1 := r * Real.cos θ
    let z2 := r * Real.sin θ
    -- z1和z2是独立的标准正态
    sorry := by
  sorry
```

---

## 9. 使用Mathlib的最佳实践

### 9.1 导入策略

```lean
-- ============================================
-- 推荐导入模式
-- ============================================

-- 1. 使用项目级lakefile配置依赖

-- 2. 在文件开头选择性导入
import Mathlib.Data.Nat.Basic       -- 自然数基础
import Mathlib.Data.Int.Basic       -- 整数基础
import Mathlib.Data.Rat.Basic       -- 有理数基础
import Mathlib.Data.Real.Basic      -- 实数基础

-- 3. 按需导入代数结构
import Mathlib.Algebra.Group.Basic  -- 群论基础
import Mathlib.Algebra.Ring.Basic   -- 环论基础
import Mathlib.Algebra.Field.Basic  -- 域论基础

-- 4. 导入分析相关（如需要）
import Mathlib.Analysis.Calculus.Deriv.Basic
import Mathlib.Analysis.SpecialFunctions.ExpLog

-- 5. 使用通配导入（开发时方便，但编译慢）
-- import Mathlib  -- 导入整个Mathlib（不推荐用于生产代码）
```

### 9.2 搜索与发现

```lean
-- ============================================
-- 在Mathlib中查找定理
-- ============================================

-- 1. 使用#check和#synth探索类型类实例
#synth Group ℤ  -- 查看整数群实例
#synth Field ℚ  -- 查看有理数域实例

-- 2. 使用loogle搜索（在线工具）
-- https://loogle.lean-lang.org/

-- 3. 在VS Code中使用Ctrl+T跳转到定义

-- 4. 阅读Mathlib文档
-- https://leanprover-community.github.io/mathlib4_docs/
```

### 9.3 常见模式与技巧

```lean
-- ============================================
-- 类型类实例推导
-- ============================================

-- 让Lean自动推导实例
example : Field ℝ := by
  infer_instance

-- 使用类型类继承
example {K : Type*} [Field K] : Ring K := by
  infer_instance  -- 域自动是环

-- ============================================
-- 数学记号使用
-- ============================================

-- 开启常用记号
open BigOperators
open Real
open Complex

-- 使用∑记号
example (n : ℕ) : ℝ :=
  ∑ i in Finset.range n, (i : ℝ)

-- 使用积分（需要测度论）
example : ℝ :=
  ∫ x : ℝ in (0)..1, x ^ 2  -- 定积分

-- ============================================
-- 处理非构造性证明
-- ============================================

-- 使用choice提取存在量词
example {α : Type*} {P : α → Prop} (h : ∃ x, P x) : α :=
  Classical.choose h  -- 提取见证

example {α : Type*} {P : α → Prop} (h : ∃ x, P x) : P (Classical.choose h) :=
  Classical.choose_spec h  -- 见证满足性质
```

### 9.4 性能考虑

```lean
-- ============================================
-- 编译优化
-- ============================================

-- 1. 使用noncomputable标记纯数学定义
noncomputable def my_real_function (x : ℝ) : ℝ :=
  if x < 0 then 0 else Real.sqrt x

-- 2. 使用irreducible控制展开
attribute [irreducible] my_complex_definition

-- 3. 使用simp配置控制简化
attribute [simp] my_useful_lemma

-- 4. 避免深层类型类搜索（使用显式实例参数）
def explicit_version {K : Type*} (h : Field K) (x : K) : K :=
  x * x  -- 不使用[Field K]隐式参数
```

---

> **核心观点**: Mathlib是Lean生态系统的核心，为形式化数学和验证提供了坚实的基础。对于C语言验证项目，Mathlib提供了数值计算、代数结构和分析工具的完整支持，使开发者能够将数学确定性引入软件系统。

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**维护者**: C_Lang Knowledge Base Team
**相关文档**:

- [01_Lean_4_Introduction.md](01_Lean_4_Introduction.md)
- [06_Lean_Proof_Examples.md](06_Lean_Proof_Examples.md)
- [Mathlib官方文档](https://leanprover-community.github.io/mathlib4_docs/)
