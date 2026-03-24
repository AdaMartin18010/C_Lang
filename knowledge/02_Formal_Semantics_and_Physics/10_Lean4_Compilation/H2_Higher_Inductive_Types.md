# 高阶归纳类型(HITs)：几何直观与形式化

> **层级**: L6

## 一、HIT的数学动机

### 从归纳类型到高阶归纳类型

**普通归纳类型**: 由点构造子生成

```lean
inductive Nat
  | zero : Nat
  | succ : Nat → Nat
```

**高阶归纳类型**: 还有路径构造子

```lean
inductive Circle
  | base : Circle
  | loop : base = base  -- 路径构造子
```

Circle有：

- 一个点 base
- 一条路径 loop : base = base（拓扑上是一个圆）

## 二、形式化定义

### HIT的规格

HIT由以下指定：

1. 点构造子：生成元素
2. 路径构造子：生成相等证明
3. 高阶路径构造子（可选）：路径之间的相等

**圆的例子**：

```
Circle :=
  | base
  | loop : base = base
```

**环面T²**：

```
Torus :=
  | point
  | meridian : point = point
  | longitude : point = point
  | surf : meridian ⬝ longitude = longitude ⬝ meridian
```

## 三、递归原理

### 圆上的函数

**定义**：给定B:Type，b:B，l: b = b，存在唯一f: Circle → B使得：

- f base = b
- ap f loop = l

其中ap (action on paths)将路径映射到路径。

**定理**: (Circle → B) ≅ (b : B) × (b = b)

证明：使用圆的唯一性公理。

## 四、计算内容

### 路径的表示

在C中提取：

```c
// 路径表示为变体
typedef struct {
  int tag;  // 0 = refl, 1 = 其他构造子
  void* data;
} Path;
```

### ap函数的计算

```lean
def ap (f : A → B) {x y : A} (p : x = y) : f x = f y
  | _, _, rfl => rfl
```

提取为：

```c
Path* ap(void* (*f)(void*), Path* p) {
  if (p->tag == 0) return refl_path();  // refl情况
  // 其他情况...
}
```

## 五、应用：代数拓扑

### 基本群

**定义**: π₁(X, x₀) = (x₀ = x₀) 在∥·∥₀下的商

**定理**: π₁(Circle, base) ≅ ℤ

证明使用圆的encode-decode方法。

## 六、参考

- Lumsdaine (2011) "Higher Inductive Types"
- Shulman (2011) "Homotopy Type Theory"
- Univalent Foundations Program (2013)
