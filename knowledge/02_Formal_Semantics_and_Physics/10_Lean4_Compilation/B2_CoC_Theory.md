# Calculus of Constructions：CoC 理论与 Lean 4 编译

> **层级**: L6 (元理论层)
> **引用**: Coquand & Huet (1988), Barendregt (1992)

---

## 一、CoC 的语法与类型规则

### 1.1 语法定义

**定义 1.1** (CoC 语法)

M, N, A, B ::= x | λx:A.M | M N | Πx:A.B | Prop | Type

### 1.2 核心类型规则

变量规则：
  x : A ∈ Γ
  ----------- (Var)
  Γ ⊢ x : A

抽象规则：
  Γ, x : A ⊢ M : B
  ------------------- (Abs)
  Γ ⊢ λx:A.M : Πx:A.B

应用规则：
  Γ ⊢ M : Πx:A.B    Γ ⊢ N : A
  ------------------------------ (App)
  Γ ⊢ M N : B[x:=N]

---

## 二、证明无关性原理

**定理 2.1** (Proof Irrelevance)

对于 P : Prop，如果 p : P 且 q : P，则 p ≡ q。

这意味着证明的具体构造不重要，只有存在性重要。

Coquand 原理：Proofs are irrelevant for computation

---

## 三、擦除函数的形式定义

**定义 3.1** (擦除函数 ε)

ε(x) = x

ε(λx:A.M) =
  if A : Prop then ε(M)
  else λx:ε(A).ε(M)

ε(M N) =
  if N : Prop then ε(M)
  else ε(M) ε(N)

ε(Prop) = Unit
ε(Type) = Type

**定理 3.2** (擦除保持类型)

如果 Γ ⊢ M : A，则 ε(Γ) ⊢ ε(M) : ε(A)。

---

## 四、从 CoC 到 Lean 4 的演进

### 4.1 Lean 4 的扩展

Lean 4 在 CoC 基础上扩展了：

- 归纳类型（inductive）
- 宇宙层级（Type u）
- 类型类（type class）

### 4.2 归纳类型的 CoC 编码

自然数的 Church 编码：

Nat : Type := ΠX:Type. (Unit → X) → (X → X) → X

zero := λX z s. z ()
succ := λn X z s. s (n X z s)

---

## 五、代码提取与 C 生成

### 5.1 提取签名

提取函数 ⌈-⌉ : CoC_Term → C_Code

### 5.2 提取规则

⌈x⌉ = x

⌈λx:A.M⌉ =
  lean_object* (*)(lean_object*) { return ⌈M⌉; }

⌈M N⌉ = ⌈M⌉(⌈N⌉)

### 5.3 提取正确性

**定理 5.1** (提取保持观察等价)

如果 M ≅obs N，则 ⌈M⌉ ≅obs ⌈N⌉。

---

## 六、对 Lean 4 编译的影响

1. **Prop 完全擦除**：编译时移除所有证明项
2. **Type 保留计算**：数据类型生成 C 结构体
3. **依赖类型简化**：依赖参数在 C 中消失
4. **高阶函数闭包化**：使用函数指针+环境实现

---

**参考文献**：

- Coquand, T. & Huet, G. "The Calculus of Constructions" (1988)
- Barendregt, H. "Lambda Calculi with Types" (1992)
- Paulin-Mohring, C. "Extracting Programs from Proofs" (1989)
