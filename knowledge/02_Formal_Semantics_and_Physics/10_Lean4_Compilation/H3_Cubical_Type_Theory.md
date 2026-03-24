# 立方类型论：路径与相等性的计算理解

> **层级**: L6

## 一、立方类型论的动机

### 同伦类型论中的问题

在Book HoTT中：

- 函数外延性（funext）是公理，无计算内容
- 高阶归纳类型的路径计算不直接

**立方类型论(Cubical Type Theory)**提供：

- 内在的路径类型
- 可计算的funext
- 规范的路径操作

## 二、立方模型

### De Morgan立方类型论

**区间I**: 有端点i0, i1的特殊类型。

**路径类型**: Path A a b = (i : I) → A，端点a(i0), b(i1)。

### 基本操作

```lean
-- 路径抽象
λ i => t : Path A t[i0/i] t[i1/i]

-- 路径应用
p i : A，其中p : Path A a b

-- 路径归纳(path induction)
pathInd : {A : Type} {a : A} (P : (b : A) → Path A a b → Type) →
  P a refl → {b : A} (p : Path A a b) → P b p
```

## 三、可计算的函数外延性

### 定理与证明

**定理**: funext可计算。

**证明**: 给定f, g : (x : A) → B x，p : (x : A) → Path (B x) (f x) (g x)

构造路径：

```
funext f g p := λ i x => p x i
```

这是一个真正的λ项，不是公理！

## 四、传输与替代

### 传输(transport)

```lean
transport : {A : Type} (B : A → Type) {x y : A} → Path A x y → B x → B y
```

**计算规则**:
transport B p b 沿着路径p移动b从Bx到By。

### 立方填充

给定方形的三个边，计算第四条边。

**Kan条件**: 任何开放立方可填充。

## 五、在Lean 4中的应用

### Lean 4的立方类型

Lean 4实验性支持立方类型：

```lean
def Path (A : Type) (a b : A) :=
  { f : I → A // f i0 = a ∧ f i1 = b }
```

**提取到C**：
路径表示为区间上的函数指针。

```c
typedef void* (*PathFn)(int /* i : I */);
```

## 六、参考

- Cohen et al. "Cubical Type Theory" (2018)
- Angiuli et al. "Computational Higher Type Theory" (2018)
- Cavallo & Harper (2019)
