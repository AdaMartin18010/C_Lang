# 商类型与集合论：数学构造的形式化

> **层级**: L6

## 一、商类型的数学基础

### 集合论中的商

给定集合X和等价关系~，商集X/~是等价类的集合。

**典范映射**: π : X → X/~, π(x) = [x]

### 商类型的形式化

**Lean 4定义**：

```lean
structure Quotient {α : Sort u} (s : Setoid α) where
  mk' ::
  val : α

def Quotient.mk {α : Sort u} {s : Setoid α} (a : α) : Quotient s :=
  ⟨a⟩
```

**公理**:

1. 存在性: ∀ a, Quotient.mk a 存在
2. 商归纳: 若P在等价类上良定义，则可证∀ q, P q
3. 商递归: 若f尊重等价关系，则可提升到X/~ → Y

## 二、商类型的计算内容

### 计算性商 vs 公理性商

**不可计算公理版本**：

```lean
constant Quotient.lift {α : Sort u} {β : Sort v} {s : Setoid α}
  (f : α → β) (h : ∀ a b, a ≈ b → f a = f b) : Quotient s → β
```

**可计算版本**（使用代表元选择）：

```lean
def Quotient.repr {α : Sort u} {s : Setoid α} : Quotient s → α := ...
```

### 提取到C的表示

商类型提取为：

- 结构体：{ tag: int, repr: void* }
- tag标识等价类
- repr是代表元

## 三、应用：有理数构造

### 从整数对构造

```lean
def Rational := Quotient
  (⟨Int × Nat,
    λ (a,b) (c,d) => a * d = c * b⟩)
```

**代表元选择**: 约分到最简形式。

**提取到C**：

```c
typedef struct {
  int64_t num;  // 分子
  uint64_t den; // 分母（已约分）
} Rational;
```

## 四、高阶路径与同伦类型论

### 商作为截断

**定义**: ∥A∥₀ 是A的0-截断（集合层商）

在HoTT中：

```
∥A∥₀ := A / (λ a b => ∥a = b∥₋₁)
```

### 计算性内容

**定理**: ∥A∥₀的计算内容恰好是A的等价类。

## 五、参考

- Hofmann (1995) "Extensional Concepts in Intensional Type Theory"
- Luo (1994) "Computation and Reasoning"
- Voevodsky (2006) "Homotopy Type Theory"
