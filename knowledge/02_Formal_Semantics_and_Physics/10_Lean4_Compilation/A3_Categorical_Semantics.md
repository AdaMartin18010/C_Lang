# 范畴论语义：从依赖类型到 C 的遗忘函子

> **层级**: L6 (元理论层)
> **引用**: Lambek & Scott (1986), Jacobs (1999)

## 一、范畴论语义基础

### 1.1 笛卡尔闭范畴 (CCC)

类型系统可以建模为笛卡尔闭范畴：

- 类型 → 对象
- 函数 → 态射
- 积类型 A × B → 范畴积
- 函数类型 A → B → 指数对象 B^A

### 1.2 Lean 4 的类型范畴

Lean 4 的类型系统构成范畴 L：

- 对象：类型 (Type u)
- 态射：函数 (f : A → B)
- 恒等：id : A → A
- 复合：(g ∘ f) x = g (f x)

### 1.3 C 语言的类型范畴

C 语言的类型系统构成范畴 C：

- 对象：C 类型 (int, float, struct, ...)
- 态射：函数指针/调用

## 二、遗忘函子：L → C

### 2.1 遗忘函子的定义

编译过程可视为遗忘函子 F : L → C：

`
F(依赖类型) = 简单类型
F(证明项) = 空/Unit
F(高阶函数) = 函数指针 + 环境
`

### 2.2 结构保持性

F 必须保持范畴结构：

- F(id_A) = id_{F(A)}
- F(g ∘ f) = F(g) ∘ F(f)
- F(A × B) ≅ F(A) × F(B)

### 2.3 证明：复合保持

设 f : A → B, g : B → C 在 Lean 中

F(g ∘ f) = λx. F(g) (F(f) x)
         = F(g) ∘ F(f)

## 三、具体构造

### 3.1 积类型的映射

```lean
-- Lean: 积类型
def prod (p : A × B) : A := p.1
```

```c
// C: 结构体
typedef struct {
    lean_object* fst;
    lean_object* snd;
} lean_pair;
```

### 3.2 指数对象的实现

Lean 的 A → B 对应 C 的函数指针：

`c
lean_object* (*func)(lean_object*);
`

但闭包需要额外环境：
`c
typedef struct {
    void (*code)(closure*, lean_object*);
    lean_object* env[];
} closure;
`

## 四、余代数与无限结构

### 4.1 Stream 的余代数语义

`lean
codata Stream (α : Type) : Type
  | head : α
  | tail : Stream α
`

终余代数 semantics：

- 载体：无限序列 α^ω
- 结构映射：<head, tail> : α^ω → α × α^ω

### 4.2 在 C 中的实现

`c
typedef struct stream {
    lean_object* (*head)(struct stream*);
    struct stream* (*tail)(struct stream*);
    lean_object* state;
} stream;
`

## 五、单子与计算效应

### 5.1 IO 单子

`lean
def IO (α : Type) : Type := RealWorld → α × RealWorld
`

范畴论语义：状态单子

### 5.2 在 C 中的实现

`c
typedef struct {
    lean_object* value;
    lean_object* world;  // 抽象的世界状态
} io_result;
`

## 六、理论总结

范畴论语义揭示了编译的本质：

1. **结构保持**：编译是遗忘函子
2. **信息丢失**：依赖类型、证明被擦除
3. **实现策略**：CCC 结构在 C 中的编码

这为编译器正确性证明提供了数学基础。
