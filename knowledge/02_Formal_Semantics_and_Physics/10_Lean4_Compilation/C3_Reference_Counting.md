# 引用计数与线性逻辑：内存管理的理论基础

> **层级**: L6 (元理论层)
> **引用**: Girard (1987), Baker (1994), Ullrich & de Moura (2019)
> **课程对标**: Stanford CS242, CMU 15-745

---

## 一、线性逻辑基础

### 1.1 线性逻辑的语法

**定义 1.1** (线性逻辑连接词)

- A ⊗ B : 多plicative合取（同时有A和B）
- A ⊸ B : 线性蕴含（消耗A得到B）
- A & B :  additive合取（选择A或B）
- A ⊕ B :  additive析取（A或B）
- !A : 指数模态（可复制的A）

### 1.2 线性类型系统

**规则示例**（⊸-引入）：

```
Γ, x:A ⊢ M : B
──────────────── (⊸I)
Γ ⊢ λx.M : A ⊸ B
```

**关键**：变量x必须恰好使用一次。

---

## 二、引用计数的线性逻辑解释

### 2.1 引用计数作为线性类型

**定理 2.1** (引用计数与线性逻辑的对应)

- 引用计数为1 ↔ 线性类型（必须恰好使用一次）
- 引用计数为0 ↔ 可丢弃（weakening）
- 引用计数>1 ↔ 复制（contraction）

**对应关系**：

```
引用计数操作    线性逻辑规则
─────────────────────────────
inc_ref         !A（复制）
dec_ref → 0     资源释放
借用(&)          A ⊸ B（线性使用）
```

### 2.2 借用检查的形式化

**定义 2.2** (借用)

借用 @&T 创建只读引用，不增加引用计数：

```lean
def useBorrowed (x : @&Nat) : Nat := x + 1
-- x的引用计数不变
```

**线性逻辑解释**：

```
x : T ⊢ useBorrowed x : Nat
─────────────────────────────
x : T ⊢ useBorrowed (@&x) : Nat
```

借用是线性的：必须恰好使用一次，且不消耗所有权。

---

## 三、引用计数优化

### 3.1 RC插入算法

**定义 3.1** (RC插入)

对于每个变量x，根据使用次数插入inc/dec：

```
use_count(x) = 0: 无需RC，立即释放
use_count(x) = 1: 借用优化，无RC操作
use_count(x) > 1: 需要inc/dec
```

### 3.2 破坏性更新优化

**定理 3.2** (破坏性更新条件)

当以下条件满足时，可以原地修改：

1. 引用计数为1
2. 无其他引用存在

**Lean 4实现**：

```c
if (lean_is_exclusive(o)) {
    // 可以原地修改
    o->m_data = new_value;
} else {
    // 必须复制
    lean_object* new_o = lean_copy(o);
    dec_ref(o);
    return new_o;
}
```

---

## 四、周期检测

### 4.1 引用计数的局限性

**问题**：循环引用导致内存泄漏

**示例**：

```lean
mutual
  inductive Foo | mk : Bar → Foo
  inductive Bar | mk : Foo → Bar
end
```

### 4.2 解决方案

**Lean 4策略**：

1. 主要使用引用计数
2. 周期检测作为后备
3. 或者使用弱引用(weak reference)打破循环

---

## 五、与课程的对标

### Stanford CS242: Programming Languages

- Linear Types and Resource Management
- Memory Safety without Garbage Collection

### CMU 15-745: Optimizing Compilers for Modern Architectures

- Escape Analysis
- Memory Optimization

---

**参考文献**：

- Girard, J.Y. "Linear Logic" (1987)
- Baker, H.G. "Minimizing Reference Count Updating" (1994)
- Ullrich, S. & de Moura, L. "Counting Immutable Beans" (2019)
