# Martin-Löf 依赖类型论：数学基础与 Lean 4 实现

> **层级**: L6 (元理论层)
> **引用**: Martin-Löf (1984), Nordström (1990), de Moura (2021)

---

## 一、BHK 证明解释与类型对应

Arend Heyting 的 Brouwer-Heyting-Kolmogorov 解释建立了逻辑与计算的对应：

| 逻辑命题 | BHK 解释 | Lean 4 类型 |
|:---------|:---------|:------------|
| P → Q | 将 P 证明转换为 Q 证明的函数 | P → Q |
| P ∧ Q | P 证明与 Q 证明的有序对 | P × Q |
| P ∨ Q | 标记的 P 证明或 Q 证明 | P ⊕ Q |
| ∀x.P(x) | 返回 P(x) 证明的函数 | (x : A) → P x |
| ∃x.P(x) | 元素 x 与其 P(x) 证明的对 | Σ(x : A), P x |

这就是 Curry-Howard 同构的直觉主义基础。

---

## 二、Martin-Löf 四种判断的形式定义

**定义 2.1** (判断的形式)

设 Γ 为上下文，有以下四种判断：

1. Γ ⊢ A type  —— A 是良形式类型
2. Γ ⊢ A = B type  —— A 与 B 类型相等
3. Γ ⊢ a : A  —— a 是类型 A 的元素
4. Γ ⊢ a = b : A  —— a 与 b 在类型 A 中相等

**定理 2.2** (Lean 4 内核对应)

Lean 4 内核函数实现了上述判断：

- isType e 对应于 Γ ⊢ A type
- isDefEq e1 e2 对应于 Γ ⊢ a = b : A
- check e ty 对应于 Γ ⊢ a : A

---

## 三、Π-类型（依赖函数）的形式规则

形成规则：

```
Γ ⊢ A type    Γ, x : A ⊢ B type
──────────────────────────────
Γ ⊢ Π(x:A).B type
```

引入规则：

```
Γ, x : A ⊢ b : B
──────────────────
Γ ⊢ λx. b : Π(x:A).B
```

消去规则：

```
Γ ⊢ f : Π(x:A).B    Γ ⊢ a : A
──────────────────────────────
Γ ⊢ f a : B[a/x]
```

计算规则（β-规约）：

```
(λx. b) a = b[a/x]
```

---

## 四、W-类型编码归纳类型

**定义 4.1** (W-类型)

W-类型（良基树）定义为：

```
W(x:A).B(x)
```

其中 A : Type 是构造子标签类型，B : A → Type 是每个标签的参数类型。

**定理 4.2** (W-类型编码归纳类型)

任何严格正归纳类型都可以编码为 W-类型。

**示例**：自然数

```
Nat = W(x:Bool).if x then Unit else Unit
```

- true 对应 zero（无参数）
- false 对应 succ（一个 Nat 参数，递归引用）

Lean 4 内核将 inductive 展开为 W-类型进行类型检查。

---

## 五、证明擦除的形式化

**定义 5.1** (擦除函数 ε)

擦除函数将依赖类型映射到简单类型：

```
ε(Type) = Type
ε(Prop) = Unit
ε(Π(x:A).B) =
  if A : Prop then ε(B)
  else ε(A) → ε(B)
ε(Σ(x:A).B) = ε(A) × ε(B)
```

**定理 5.2** (擦除保持类型)

如果 Γ ⊢ M : A，那么 ε(Γ) ⊢ ε(M) : ε(A)。

**证明概要**：
通过对 Γ ⊢ M : A 的推导归纳。

- 变量情况：直接由上下文定义
- 抽象情况：应用归纳假设于主体
- 应用情况：应用归纳假设于函数和参数

---

## 六、对 Lean 4 到 C 编译的影响

### 6.1 宇宙层级擦除

Lean 4 的 Type u 在 C 中完全消失：

```lean
def id (α : Type u) (a : α) := a
```

生成的 C 代码：

```c
lean_object* id(lean_object* a) { return a; }
```

### 6.2 Π-类型编译为函数

```lean
def compose (f : A → B) (g : B → C) (x : A) := g (f x)
```

生成的 C 代码使用函数指针：

```c
lean_object* compose(
    lean_object* (*f)(lean_object*),
    lean_object* (*g)(lean_object*),
    lean_object* x
) {
    return g(f(x));
}
```

### 6.3 Σ-类型编译为结构体

```lean
def pair (a : A) (b : B) : A × B := (a, b)
```

生成的 C 代码：

```c
typedef struct {
    lean_object* fst;
    lean_object* snd;
} pair_t;

pair_t* pair(lean_object* a, lean_object* b) {
    pair_t* p = malloc(sizeof(pair_t));
    p->fst = a;
    p->snd = b;
    return p;
}
```

---

## 七、理论总结

Martin-Löf 类型论为 Lean 4 提供了：

1. **判断先于命题** — 内核验证判断而非真值
2. **Curry-Howard 同构** — 证明与程序统一
3. **依赖类型** — 类型可依赖于值
4. **归纳构造** — W-类型保证良基性
5. **证明擦除** — Prop/Type 分离优化运行时

这些理论决策直接决定了 Lean 4 到 C 的编译策略。

---

**参考文献**：

- Martin-Löf, P. "Intuitionistic Type Theory" (1984)
- Nordström, B. et al. "Programming in Martin-Löf's Type Theory" (1990)
- de Moura, L. & Ullrich, S. "The Lean 4 Theorem Prover" (2021)
