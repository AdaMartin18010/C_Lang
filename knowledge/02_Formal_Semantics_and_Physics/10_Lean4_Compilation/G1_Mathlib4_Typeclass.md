# Mathlib4类型类层次与代数结构

> **层级**: L6

## 一、类型类层次结构

Mathlib4的代数层次从最基础构建：

```lean
class Zero (α : Type u) where
  zero : α

class Add (α : Type u) where
  add : α → α → α

class AddSemigroup (α : Type u) extends Add α where
  add_assoc : ∀ a b c, a + b + c = a + (b + c)

class AddMonoid (α : Type u) extends AddSemigroup α, Zero α where
  zero_add : ∀ a, 0 + a = a
  add_zero : ∀ a, a + 0 = a
```

## 二、范畴论形式化

**范畴的定义**：

```lean
structure Category (obj : Type u) where
  Hom : obj → obj → Type v
  id : ∀ X, Hom X X
  comp : ∀ {X Y Z}, Hom Y Z → Hom X Y → Hom X Z
  id_comp : ∀ {X Y} (f : Hom X Y), comp (id Y) f = f
  comp_id : ∀ {X Y} (f : Hom X Y), comp f (id X) = f
  assoc : ∀ {W X Y Z} (f : Hom Y Z) (g : Hom X Y) (h : Hom W X),
    comp (comp f g) h = comp f (comp g h)
```

**定理**: Set范畴满足范畴公理。

证明：验证id_comp, comp_id, assoc。

## 三、性能优化

### outParam减少搜索空间

```lean
class HMul (α : Type u) (β : Type v) (γ : outParam (Type w))
```

使用outParam将解析从O(n²)降到O(n)。

### Discrimination Tree索引

simp引理查找从O(n)降到O(log n)。

## 四、代码提取示例

**验证的gcd提取到C**：

```lean
def gcd : Nat → Nat → Nat
  | 0, b => b
  | a, 0 => a
  | a, b => if a ≤ b then gcd a (b - a) else gcd (a - b) b
```

提取为优化的C代码：

```c
uint64_t gcd(uint64_t a, uint64_t b) {
  while (b != 0) {
    uint64_t t = b;
    b = a % b;  // 自动优化为模运算
    a = t;
  }
  return a;
}
```

## 五、引用

- Buzzard et al. "Formalising Mathematics" (2020)
- Mathlib4 Documentation
