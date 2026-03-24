# 尾调用优化：从递归到循环的语义保持变换

> **层级**: L6+
> **引用**: Steele (1977), Clinger (1998), Flanagan et al. (2004)

---

## 【哲学维度·递归的本质】

### 递归vs迭代

**递归**: 自指定义，数学直观
**迭代**: 状态更新，机器高效

**核心洞见**: 尾递归是递归和迭代的统一。

---

## 【科学维度·尾调用的形式化】

### 1. 尾位置的定义

**求值上下文**:

```
E ::= [] | E t | v E | if E then t₁ else t₂
```

**尾位置**: 项t在t'中处于尾位置如果:

- t' = t
- t' = λx.t
- t' = if t₁ then t else t₂
- t' = if t₁ then t₂ else t

### 2. 尾调用定义

**定义**: 调用f a是尾调用如果它出现在尾位置。

**关键性质**: 尾调用后不需要返回调用者。

### 3. CPS转换

**Continuation Passing Style**:

```
⟦x⟧ = λk. k x
⟦λx.M⟧ = λk. k (λx.⟦M⟧)
⟦M N⟧ = λk. ⟦M⟧ (λm. ⟦N⟧ (λn. m n k))
```

**定理**: CPS中所有调用都是尾调用。

**证明**: 通过构造，CPS中的每个函数调用都是最后一个操作。

---

## 【工程维度·优化实现】

### 1. 尾递归消除

**转换**:

```
let rec fact n acc =
  if n = 0 then acc
  else fact (n-1) (n*acc)
```

→

```c
uint64_t fact(uint64_t n, uint64_t acc) {
  while (true) {
    if (n == 0) return acc;
    acc = n * acc;
    n = n - 1;
  }
}
```

### 2. 蹦床技术（互递归）

**问题**: 互递归不能直接转循环

**解决**: 使用蹦床函数

```c
typedef enum { FACT, EVEN, ODD } FunctionTag;

typedef struct {
  FunctionTag tag;
  uint64_t arg;
} Thunk;

Thunk* trampoline(Thunk* t) {
  while (true) {
    switch (t->tag) {
      case FACT: return fact_thunk(t->arg);
      case EVEN: return even_thunk(t->arg);
      case ODD: return odd_thunk(t->arg);
      default: return NULL; // 完成
    }
  }
}
```

### 3. 标签跳转（Goto）

**优化后的互递归**:

```c
void even_odd(uint64_t n, bool is_even) {
entry:
  if (n == 0) return is_even;
  if (is_even) {
    // odd(n-1)
    n = n - 1;
    is_even = false;
    goto entry;
  } else {
    // even(n-1)
    n = n - 1;
    is_even = true;
    goto entry;
  }
}
```

---

## 【可视化·TCO正确性证明】

```
尾递归转循环正确性:
────────────────────────────────────────
目标: fact n acc ≡ while-loop版本

不变式: 每次迭代维持 fact(n, acc) = fact(n₀, acc₀)

步骤1: 初始化
  n = n₀, acc = acc₀
  fact(n, acc) = fact(n₀, acc₀) ✓
────────────────────────────────────────
步骤2: 保持
  假设: fact(n, acc) = fact(n₀, acc₀)
  执行: acc' = n * acc, n' = n - 1
  需证: fact(n', acc') = fact(n₀, acc₀)

  fact(n', acc')
  = fact(n-1, n*acc)
  = if n-1=0 then n*acc else fact(n-2, (n-1)*n*acc)
  = if n=1 then n*acc else ...

  与递归定义一致 ✓
────────────────────────────────────────
步骤3: 终止
  n严格递减，n≥0，故必然到达n=0
────────────────────────────────────────
结论: TCO保持语义 ∎
```

---

## 【多维矩阵·TCO策略对比】

```
策略              栈使用      适用场景            复杂度
─────────────────────────────────────────────────────────
直接递归          O(n)        简单递归            低
尾递归优化        O(1)        自递归              低
蹦床              O(1)堆      互递归              中
标签跳转          O(1)        互递归              中
CPS转换           O(1)        通用                高
```

---

## 【定理·Steele的观察】

**定理** (Steele, 1977):

尾调用可以像goto一样高效实现，不消耗调用栈空间。

**证明概要**:

1. 尾调用前不需要保留当前栈帧
2. 可以用被调用者的栈帧复盖调用者的栈帧
3. 返回地址直接使用调用者的返回地址

---

## 参考

- Steele, G.L. "Rabbit: A Compiler for Scheme" (1978)
- Clinger, W. "Proper Tail Recursion and Space Efficiency" (1998)
- Flanagan, C. et al. "Essence of Compiling with Continuations" (2004)
