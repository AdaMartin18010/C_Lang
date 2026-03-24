# 尾调用优化(TCO)：理论与实践

> **层级**: L6 (元理论层)
> **引用**: Steele (1977), Clinger (1998), Kennedy (2007)
> **课程对标**: MIT 6.035, CMU 15-411, Stanford CS242

---

## 一、尾调用的数学定义

### 1.1 求值上下文

**定义 1.1** (求值上下文 E)

E ::= [] | E t | v E | if E then t₁ else t₂

其中 [] 是"洞"(hole)，表示求值位置。

### 1.2 尾位置的定义

**定义 1.2** (尾位置)

在项t中，位置p是尾位置当且仅当：

1. t = λx.t'，且p在t'中
2. t = if t₁ then t₂ else t₃，且p在t₂或t₃中
3. t = E[t']，其中E不是应用上下文

### 1.3 尾调用

**定义 1.3** (尾调用)

调用f a是尾调用，如果它出现在尾位置。

**关键性质**：尾调用不需要保留当前栈帧。

---

## 二、尾调用优化的形式化

### 2.1 转换规则

**定理 2.1** (TCO的正确性)

尾调用可以被优化为跳转(goto)而不改变程序语义。

**转换**：

```
let f x = ... in
let g y = f (y + 1) in  // 尾调用
g 10
```

转换为C：

```c
// 使用while循环而非递归
int g(int y) {
    goto f_entry;  // 直接跳转
f_return:
    return result;
}
```

### 2.2 累积参数模式

**示例**：尾递归阶乘

```lean
def fact (n acc : Nat) : Nat :=
  if n = 0 then acc
  else fact (n - 1) (n * acc)
```

生成的C代码：

```c
lean_object* fact(lean_object* n, lean_object* acc) {
    while (true) {
        if (lean_nat_eq(n, lean_nat_zero()))
            return acc;
        lean_object* new_n = lean_nat_sub(n, lean_nat_one());
        lean_object* new_acc = lean_nat_mul(n, acc);
        n = new_n;
        acc = new_acc;
    }
}
```

---

## 三、连续传递风格(CPS)

### 3.1 CPS转换

**定义 3.1** (CPS转换)

将直接风格(DS)转换为CPS：

```
⟦x⟧ = λk. k x
⟦λx.M⟧ = λk. k (λx.⟦M⟧)
⟦M N⟧ = λk. ⟦M⟧ (λm. ⟦N⟧ (λn. m n k))
```

### 3.2 CPS与尾调用的关系

**定理 3.2** (CPS中的调用都是尾调用)

在CPS形式中，所有函数调用都是尾调用。

---

## 四、Lean 4的TCO实现

### 4.1 IR层面的TCO标记

Lean 4编译器在IR层面识别尾调用并标记：

```
tail_call f args
```

### 4.2 生成C的goto

**策略**：

- 自递归：使用while循环
- 互递归：使用trampoline或标签跳转

---

## 五、课程对标

### MIT 6.035: Computer Language Engineering

- Lecture 10: Tail Call Optimization
- Lab 4: Compiling Scheme to C with TCO

### CMU 15-411

- Lecture 9: Tail Calls and Register Allocation

---

**参考文献**：

- Steele, G.L. "Rabbit: A Compiler for Scheme" (1978)
- Clinger, W. "Proper Tail Recursion and Space Efficiency" (1998)
