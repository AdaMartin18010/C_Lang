# C语言类型理论形式化 (C Type Theory)

> **难度**: L6 | **预估学习时间**: 10-12小时
> **参考**: Cardelli (1996), TAPL (Pierce 2002), C标准 §6.2.5, CompCert

---

## 1. 类型系统概述

C语言的类型系统是一种**静态、 nominative、部分推断**的系统，在编译时保证内存布局正确性和操作合法性。

### 1.1 类型系统的形式化定义

**类型系统**由以下组件组成：

- **类型判断 (Typing Judgment)**: $\Gamma \vdash e : \tau$ （在环境$\Gamma$下，表达式$e$具有类型$\tau$）
- **类型规则 (Typing Rules)**: 归纳定义有效类型判断的推理规则
- **类型等价 (Type Equivalence)**: 判断两个类型是否相同的关系

### 1.2 C类型系统特点

| 特性 | C的表现 | 形式化影响 |
|:-----|:--------|:-----------|
| 静态类型 | 编译时检查 | 类型保持性定理适用 |
| 显式声明 | 变量需声明类型 | 类型推导简化 |
| 类型转换 | 隐式和显式 | 需要子类型关系 |
| 底层类型 | 直接对应内存布局 | 大小和对齐约束 |
| 未定义行为 | 类型违规可能导致UB | 需要类型安全证明 |

---


---

## 📑 目录

- [C语言类型理论形式化 (C Type Theory)](#c语言类型理论形式化-c-type-theory)
  - [1. 类型系统概述](#1-类型系统概述)
    - [1.1 类型系统的形式化定义](#11-类型系统的形式化定义)
    - [1.2 C类型系统特点](#12-c类型系统特点)
  - [📑 目录](#-目录)
  - [2. C类型语法](#2-c类型语法)
    - [2.1 类型文法](#21-类型文法)
    - [2.2 类型派生规则](#22-类型派生规则)
  - [3. 类型判断规则](#3-类型判断规则)
    - [3.1 类型环境](#31-类型环境)
    - [3.2 常量与变量](#32-常量与变量)
    - [3.3 算术表达式](#33-算术表达式)
    - [3.4 指针运算](#34-指针运算)
    - [3.5 赋值](#35-赋值)
  - [4. 类型转换形式化](#4-类型转换形式化)
    - [4.1 隐式转换 (Implicit Conversion)](#41-隐式转换-implicit-conversion)
    - [4.2 显式转换 (Cast)](#42-显式转换-cast)
    - [4.3 指针转换安全性](#43-指针转换安全性)
  - [5. 复合类型形式化](#5-复合类型形式化)
    - [5.1 结构体类型](#51-结构体类型)
    - [5.2 类型完备性 (Type Completeness)](#52-类型完备性-type-completeness)
  - [6. 类型等价](#6-类型等价)
    - [6.1 结构等价 vs 名义等价](#61-结构等价-vs-名义等价)
    - [6.2 类型等价规则](#62-类型等价规则)
  - [7. 类型安全与未定义行为](#7-类型安全与未定义行为)
    - [7.1 类型安全定理](#71-类型安全定理)
    - [7.2 C语言的类型违规](#72-c语言的类型违规)
    - [7.3 严格别名规则形式化](#73-严格别名规则形式化)
  - [8. 类型推导与C23新特性](#8-类型推导与c23新特性)
    - [8.1 `auto` 类型推导 (C23)](#81-auto-类型推导-c23)
    - [8.2 `typeof` 与表达式类型](#82-typeof-与表达式类型)
    - [8.3 `_Generic` 选择 (C11)](#83-_generic-选择-c11)
  - [9. CompCert中的类型系统](#9-compcert中的类型系统)
    - [9.1 Clight类型](#91-clight类型)
    - [9.2 类型检查算法](#92-类型检查算法)
    - [9.3 类型保持证明](#93-类型保持证明)
  - [10. 现代类型系统扩展](#10-现代类型系统扩展)
    - [10.1 依赖类型 (研究性)](#101-依赖类型-研究性)
    - [10.2 所有权类型](#102-所有权类型)
    - [10.3 能力类型 (Capability Types)](#103-能力类型-capability-types)
  - [11. 类型系统属性总结](#11-类型系统属性总结)
  - [12. 参考文献](#12-参考文献)


---

## 2. C类型语法

### 2.1 类型文法

```text
基本类型 τ ::= void | char | short | int | long | long long
            | float | double | long double
            | _Bool | _Complex

派生类型   ::= τ *              (指针)
            | τ [n]            (数组)
            | τ (τ₁, ..., τₙ)  (函数)
            | struct { (f:τ)* }  (结构)
            | union { (f:τ)* }   (联合)
            | enum { ... }       (枚举)

限定符     ::= const | volatile | restrict | _Atomic
```

### 2.2 类型派生规则

```text
┌─────────────────────────────────────────────────────────────┐
│ 声明构造器           类型表达式            示例               │
├─────────────────────────────────────────────────────────────┤
│  T                   T                     int              │
│  *p                  pointer(T)            int *            │
│  a[N]                array(T, N)           int [10]         │
│  f()                 function(T, ...)      int ()           │
│  *a[N]               array(pointer(T), N)  int *[10]        │
│  (*p)[N]             pointer(array(T,N))   int (*)[10]      │
│  *f()                pointer(function(...)) int *()         │
│  (*p)()              function(pointer(T),...) int (*)()     │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 类型判断规则

### 3.1 类型环境

**类型环境** $\Gamma$ 映射标识符到类型：

$$
\Gamma ::= \emptyset \mid \Gamma, x : \tau
$$

### 3.2 常量与变量

$$
\frac{}{\Gamma \vdash n : \text{int}} \quad \text{(整数常量)}
$$

$$
\frac{}{\Gamma \vdash f : \text{double}} \quad \text{(浮点常量)}
$$

$$
\frac{\Gamma(x) = \tau}{\Gamma \vdash x : \tau} \quad \text{(变量)}
$$

### 3.3 算术表达式

$$
\frac{\Gamma \vdash e_1 : \tau \quad \Gamma \vdash e_2 : \tau \quad \tau \in \{\text{int}, \text{long}, \text{long long}, \text{float}, \text{double}\}}
     {\Gamma \vdash e_1 + e_2 : \tau}
$$

**常规算术转换 (Usual Arithmetic Conversions)**:

```text
如果操作数类型不同，按以下层次转换到共同类型：

long double
   ↑
double
   ↑
float
   ↑
unsigned long long
   ↑
long long
   ↑
unsigned long
   ↑
long
   ↑
unsigned int
   ↑
int
```

### 3.4 指针运算

$$
\frac{\Gamma \vdash p : \tau* \quad \Gamma \vdash n : \text{int}}
     {\Gamma \vdash p + n : \tau*}
$$

$$
\frac{\Gamma \vdash p : \tau* \quad \Gamma \vdash q : \tau*}
     {\Gamma \vdash p - q : \text{ptrdiff_t}}
$$

**语义**: $p + n$ 实际地址为 $p + n \times sizeof(\tau)$

### 3.5 赋值

$$
\frac{\Gamma \vdash e : \tau' \quad \tau' \leq \tau \quad \text{lv}(x)}
     {\Gamma \vdash x = e : \tau}
$$

其中 $\tau' \leq \tau$ 表示 $\tau'$ 可隐式转换为$\tau$。

---

## 4. 类型转换形式化

### 4.1 隐式转换 (Implicit Conversion)

**子类型关系**:

$$
\text{char} \leq \text{short} \leq \text{int} \leq \text{long} \leq \text{long long}
$$

$$
\text{float} \leq \text{double} \leq \text{long double}
$$

**转换规则**:

$$
\frac{\Gamma \vdash e : \tau' \quad \tau' \leq \tau}
     {\Gamma \vdash e : \tau} \quad \text{(子类型)}
$$

### 4.2 显式转换 (Cast)

$$
\frac{\Gamma \vdash e : \tau'}
     {\Gamma \vdash (\tau)e : \tau}
$$

**C23新增**: `typeof` 运算符

```c
typeof(int) x;  // x 具有 int 类型
```

形式化：
$$
\frac{\Gamma \vdash e : \tau'}
     {\Gamma \vdash \text{typeof}(e) : \text{type}}
$$

### 4.3 指针转换安全性

$$
\frac{\Gamma \vdash p : \tau* \quad \text{alignof}(\sigma) \leq \text{alignof}(\tau)}
     {\Gamma \vdash (\sigma*)p : \sigma*}
$$

**注意**: C允许任意指针类型转换，但可能导致未定义行为。

---

## 5. 复合类型形式化

### 5.1 结构体类型

**结构体标签查找**:

$$
\frac{\Gamma_{tag}(S) = \{(f_i : \tau_i)_{i=1}^n\}}{\Gamma \vdash \text{struct } S : \text{struct } \{(f_i : \tau_i)\}}
$$

**成员访问**:

$$
\frac{\Gamma \vdash e : \text{struct } \{(f_i : \tau_i)\} \quad f_j \in \{f_i\}}
     {\Gamma \vdash e.f_j : \tau_j}
$$

$$
\frac{\Gamma \vdash p : (\text{struct } S)* \quad \Gamma \vdash \text{struct } S \text{ has } f : \tau}
     {\Gamma \vdash p \to f : \tau}
$$

### 5.2 类型完备性 (Type Completeness)

C标准区分**完备类型**和**不完整类型**：

```text
完整类型:   int, int [5], struct S { int x; }
不完整类型: void, int [], struct T (声明但未定义)
```

$$
\text{complete}(\tau) = \begin{cases}
\text{true} & \text{if } sizeof(\tau) \text{ 已知} \\
\text{false} & \text{otherwise}
\end{cases}
$$

---

## 6. 类型等价

### 6.1 结构等价 vs 名义等价

| 等价类型 | 定义 | C语言使用 |
|:---------|:-----|:----------|
| **结构等价** | 类型构造相同 | 基本类型、指针、数组 |
| **名义等价** | 标签相同 | 结构体、联合体、枚举 |

### 6.2 类型等价规则

**结构等价**（用于大多数类型）：

$$
\frac{}{ au \equiv \tau}
$$

$$
\frac{\tau_1 \equiv \tau_2}{\tau_1* \equiv \tau_2*}
$$

$$
\frac{\tau_1 \equiv \tau_2 \quad n_1 = n_2}{\text{array}(\tau_1, n_1) \equiv \text{array}(\tau_2, n_2)}
$$

**名义等价**（用于结构体）：

$$
\frac{S_1 = S_2}{\text{struct } S_1 \equiv \text{struct } S_2}
$$

即使两个结构体有相同成员，标签不同则类型不等价：

```c
struct A { int x; };
struct B { int x; };
struct A a; struct B b;
a = b;  // 错误：类型不兼容
```

---

## 7. 类型安全与未定义行为

### 7.1 类型安全定理

**类型保持 (Type Preservation)**:

```text
如果 Γ ⊢ e : τ 且 e → e'，则 Γ ⊢ e' : τ
```

**进展 (Progress)**:

```text
如果 Γ ⊢ e : τ，则 e 是值或 ∃e'. e → e'
```

### 7.2 C语言的类型违规

以下类型违规导致**未定义行为**：

| 违规 | 形式化描述 | 后果 |
|:-----|:-----------|:-----|
| 悬空指针解引用 | $*p$ 其中 $p$ 指向已释放内存 | UB |
| 类型双关 (Type Punning) | 通过不兼容指针类型访问 | 可能UB |
| 严格别名违规 | 通过非字符指针访问对象 | UB |
| 对齐违规 | 访问未对齐地址 | UB |

### 7.3 严格别名规则形式化

C11 §6.5/7: 对象只能通过其**有效类型**或兼容类型访问。

$$
\text{valid_access}(p, \tau) = \text{effective_type}(p) \equiv \tau \lor \tau = \text{char}
$$

**违反示例**:

```c
float f = 3.14f;
int* pi = (int*)&f;  // 违反严格别名
int i = *pi;         // UB！
```

**合法方式** (C23): 使用 `memcpy` 或 `union`:

```c
union FloatInt { float f; int i; };
union FloatInt fi = { .f = 3.14f };
int i = fi.i;  // OK
```

---

## 8. 类型推导与C23新特性

### 8.1 `auto` 类型推导 (C23)

C23从C++引入 `auto`：

```c
auto x = 5;      // int
auto y = 3.14;   // double
auto z = "hi";   // char*
```

形式化：

$$
\frac{\Gamma \vdash e : \tau \quad \tau \text{ 不是 void 或数组}}
     {\Gamma \vdash \text{auto } x = e \leadsto \Gamma, x : \tau}
$$

### 8.2 `typeof` 与表达式类型

$$
\frac{\Gamma \vdash e : \tau}
     {\Gamma \vdash \text{typeof}(e) \Downarrow \tau}
$$

$$
\frac{\Gamma \vdash T : \text{type}}
     {\Gamma \vdash \text{typeof}(T) \Downarrow T}
$$

### 8.3 `_Generic` 选择 (C11)

$$
\frac{\Gamma \vdash e : \tau \quad \text{match}(\tau, \tau_i) \quad \Gamma \vdash e_i : \tau'}
     {\Gamma \vdash \text{_Generic}(e, \tau_1: e_1, ..., \tau_n: e_n) : \tau'}
$$

**示例**:

```c
#define abs(x) _Generic((x), \
    int: abs_int, \
    double: abs_double \
)(x)
```

---

## 9. CompCert中的类型系统

### 9.1 Clight类型

```coq
(* CompCert/types.v *)
Inductive type : Type :=
  | Tvoid : type                    (* void *)
  | Tint : intsize -> signedness -> attr -> type  (* 整数 *)
  | Tlong : signedness -> attr -> type            (* long long *)
  | Tfloat : floatsize -> attr -> type            (* 浮点 *)
  | Tpointer : type -> attr -> type               (* 指针 *)
  | Tarray : type -> Z -> attr -> type            (* 数组 *)
  | Tfunction : typelist -> type -> calling_convention -> type
  | Tstruct : ident -> attr -> type               (* 结构体 *)
  | Tunion : ident -> attr -> type                (* 联合体 *)
  | Tcomposite : ident -> attr -> type.           (* 复合类型占位 *)
```

### 9.2 类型检查算法

```coq
(* 类型检查表达式 *)
Fixpoint typecheck_expr (ce: composite_env) (e: expr) : res type :=
  match e with
  | Econst_int _ ty => OK ty
  | Evar id ty =>
      match (ve ! id) with
      | Some ty' => if type_eq ty ty' then OK ty else Error ...
      | None => Error ...
      end
  | Ebinop op a1 a2 ty =>
      do ty1 <- typecheck_expr ce a1;
      do ty2 <- typecheck_expr ce a2;
      if type_compatible op ty1 ty2 then OK ty else Error ...
  | ...
  end.
```

### 9.3 类型保持证明

CompCert证明：**类型正确的程序在编译后仍类型正确**。

```coq
Theorem type_preservation:
  forall ge e m v tym,
  eval_expr ge e m v ->
  typeof v = tym ->
  forall tym',
  sem_cast v tym tym' = Some v' ->
  typeof v' = tym'.
```

---

## 10. 现代类型系统扩展

### 10.1 依赖类型 (研究性)

虽然C不原生支持，但研究中有：

```text
数组类型: array(τ, n) 其中 n: nat
长度检查: Γ ⊢ a : array(τ, n) ⊢ a[i] : τ 如果 i < n
```

### 10.2 所有权类型

借鉴Rust，研究中的C扩展：

```text
借用检查: &τ (不可变借用), &mut τ (可变借用)
生命周期: 'a 标注有效范围
```

### 10.3 能力类型 (Capability Types)

跟踪内存访问权限：

```text
p : ptr(τ, R)  -- 具有读权限的τ指针
q : ptr(τ, RW) -- 具有读写权限的τ指针
```

---

## 11. 类型系统属性总结

| 属性 | C标准 | CompCert | 形式化状态 |
|:-----|:------|:---------|:-----------|
| 类型安全 | 部分（有UB） | 验证的子集 | 已证明 |
| 类型完备性 | 区分完整/不完整 | 完全实现 | 已形式化 |
| 别名分析 | 严格别名规则 | 简化模型 | 部分证明 |
| 类型等价 | 名义+结构 | 名义+结构 | 已形式化 |
| 隐式转换 | 复杂规则集 | 简化规则 | 已实现 |

---

## 12. 参考文献

1. **Pierce, B.C.** (2002). *Types and Programming Languages*. MIT Press.
2. **Cardelli, L.** (1996). "Type Systems". *ACM Computing Surveys*.
3. **ISO/IEC 9899:2023** (C23标准), §6.2.5 Types
4. **Leroy, X.** (2009). "Formal Verification of a Realistic Compiler". *CACM*.
5. **Jung, R.** et al. (2017). "Iris: Monoids and Invariants as an Orthogonal Basis for Concurrent Reasoning". *POPL*.
6. **Nuyts, A.** et al. (2022). "Defining and Verifying Subtyping in Rust with Stacked Borrows".

---

> **关联文档**: [操作语义](./01_Operational_Semantics.md) | [CompCert概述](../11_CompCert_Verification/01_Compcert_Overview.md) | [C23特性](../../01_Core_Knowledge_System/07_Modern_C/01_C11_Features.md)


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
