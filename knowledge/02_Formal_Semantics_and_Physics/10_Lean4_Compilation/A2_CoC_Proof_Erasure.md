# Calculus of Constructions 与证明擦除理论

> **层级**: L6 (元理论层)
> **引用**: Coquand & Huet (1988), Barendregt (1992)

## 一、CoC 的形式定义

Calculus of Constructions 是 Dependent Type Theory 的基础。

### 1.1 语法

`
M, N ::= x | λx:A.M | M N | Πx:A.B | Prop | Type
`

### 1.2 类型规则

- **变量**: Γ ⊢ x : A  如果 x:A ∈ Γ
- **抽象**: Γ ⊢ λx:A.M : Πx:A.B  如果 Γ,x:A ⊢ M : B
- **应用**: Γ ⊢ M N : B[x:=N]  如果 Γ ⊢ M : Πx:A.B 且 Γ ⊢ N : A

## 二、证明项与计算内容

### 2.1 Curry-Howard 在 CoC 中的体现

| 逻辑 | 类型论 |
|:-----|:-------|
| 命题 P | 类型 P : Prop |
| 证明 p : P | 项 p : P |
| P → Q | 函数类型 P → Q |
| ∀x.P(x) | 依赖类型 Πx:A.P(x) |

### 2.2 证明即程序

证明项是可执行的程序，但：

- 证明项的计算结果不重要
- 重要的是证明项的类型（命题）

## 三、证明擦除的形式化

### 3.1 擦除函数定义

`
erase(Prop) = Unit
erase(Type) = Type
erase(Πx:A.B) =
  if A : Prop then erase(B)
  else erase(A) → erase(B)
`

### 3.2 擦除保持类型

定理：如果 Γ ⊢ M : A，则 erase(Γ) ⊢ erase(M) : erase(A)

### 3.3 在 Lean 4 中的实现

Lean 4 编译器在执行以下擦除：

1. **Prop 完全擦除**
2. **依赖参数擦除**
3. **宇宙层级擦除**

## 四、对 C 代码生成的影响

### 4.1 函数签名简化

`lean
def proof_irrelevant (p : Prop) (h1 h2 : p) : h1 = h2 := rfl
`

擦除后：无运行时代码！

### 4.2 运行时效率

证明擦除使运行时只执行计算内容，证明验证在编译期完成。

## 五、理论意义

1. **逻辑一致性**：CoC 是一致的（无矛盾）
2. **强规范化**：所有项都有范式
3. **类型检查可判定**：存在算法判断 Γ ⊢ M : A

这些性质保证了 Lean 4 编译的正确性。
