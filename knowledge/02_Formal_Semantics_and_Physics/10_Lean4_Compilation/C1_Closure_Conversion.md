# 闭包转换理论：从λ演算到C实现

> **层级**: L6 (元理论层)
> **引用**: Appel (1992), Flanagan et al. (1993)
> **课程对标**: CMU 15-411, Stanford CS243

---

## 一、λ演算与自由变量

### 1.1 λ演算的语法

**定义 1.1** (λ演算语法)

t ::= x | λx.t | t₁ t₂

### 1.2 自由变量

**定义 1.2** (自由变量 FV)

FV(x) = {x}
FV(λx.t) = FV(t) \\ {x}
FV(t₁ t₂) = FV(t₁) ∪ FV(t₂)

**示例**：

- FV(λx. x + y) = {y}
- FV(λx. λy. x + y + z) = {z}

---

## 二、闭包转换的形式化

### 2.1 闭包的数学定义

**定义 2.1** (闭包)

闭包是三元组 (Code, Env, FVs)，其中：

- Code : Value^arity → Value
- Env : FVs → Value
- FVs 是自由变量集合

### 2.2 闭包转换算法

**定义 2.2** (闭包转换函数 CC)

CC(x) = x

CC(λx.t) =
  let FVs = FV(λx.t)
  in mkclosure(Code, FVs)
  where Code = λ(x, env). let FVs = env in CC(t)

CC(t₁ t₂) = appclosure(CC(t₁), CC(t₂))

### 2.3 Lean 4到C的代码生成

**示例**：

```lean
def makeAdder (n : Nat) : Nat → Nat :=
  fun m => n + m
```

生成的C代码：

```c
typedef struct {
    lean_object* n;
} adder_env;

lean_object* adder_code(adder_env* env, lean_object* m) {
    return lean_nat_add(env->n, m);
}
```

---

## 三、优化技术

### 3.1 已知函数调用

**定理 3.1**：如果函数f在编译时已知且FVs为空，可直接调用。

### 3.2 逃逸分析

**定义 3.2**：分析闭包是否逃逸出创建作用域：

- 不逃逸：栈分配
- 逃逸：堆分配

---

## 四、大学课程对标

### CMU 15-411: Compiler Design

- 第7讲：Functional Representation
- 第8讲：Closure Conversion
- 实验3：编译函数式语言到C

### Stanford CS243: Program Analysis

- 逃逸分析算法
- 指针分析
- 栈分配优化

---

**参考文献**：

- Appel, A.W. "Compiling with Continuations" (1992)
- Flanagan, C. et al. "The Essence of Compiling with Continuations" (1993)
