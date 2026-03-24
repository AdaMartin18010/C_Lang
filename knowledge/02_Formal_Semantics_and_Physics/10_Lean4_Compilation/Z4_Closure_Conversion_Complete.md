# 闭包转换：λ演算到C的语义保持变换

> **层级**: L6+
> **引用**: Appel (1992), Flanagan et al. (1993), Kennedy (2007)

---

## 【核心问题·为什么需要闭包转换】

**问题**: C语言不支持λ抽象（高阶函数）
**解决**: 将λ项转换为闭包（代码指针+环境）
**挑战**: 保持语义等价

---

## 【理论维度·λ演算形式化】

### 1. 语法

```
t ::= x | λx.t | t₁ t₂
```

### 2. 操作语义（CBV）

```
v ::= λx.t    (值)

(λx.t) v → t[v/x]    (β-规约)

t₁ → t₁'
───────────
t₁ t₂ → t₁' t₂

t₂ → t₂'
───────────
v t₂ → v t₂'
```

### 3. 自由变量

**定义**: FV(t)

```
FV(x) = {x}
FV(λx.t) = FV(t) \\ {x}
FV(t₁ t₂) = FV(t₁) ∪ FV(t₂)
```

---

## 【科学维度·闭包转换算法】

### 闭包的数学定义

**定义**: 闭包是三元组 (code, env, FVs)

- code: 代码指针（函数体）
- env: 环境（自由变量绑定）
- FVs: 自由变量集合

### 转换算法

```
CC(x) = x

CC(λx.t) =
  let FVs = FV(λx.t)
  in mkclosure(CODE, env)
  where CODE = λ(x, env). let FVs = env in CC(t)
        env = (FV₁, ..., FVₙ)

CC(t₁ t₂) =
  let f = CC(t₁)
  let arg = CC(t₂)
  in appclosure(f, arg)
```

### 语义保持定理

**定理**: 如果t →*v，则CC(t) →* CC(v)。

**证明**: 对归约序列进行归纳，验证每次β-规约对应闭包应用的正确转换。

---

## 【工程维度·C实现】

### 闭包数据结构

```c
typedef struct {
    void* (*code)(void* env, void* arg);
    void* env[];
} Closure;

// 闭包调用
void* apply_closure(Closure* clos, void* arg) {
    return clos->code(clos->env, arg);
}
```

### 环境表示

**扁平环境**: 所有自由变量连续存储

```c
typedef struct {
    lean_object* fv1;
    lean_object* fv2;
    // ...
} Env;
```

**链接环境**: 嵌套作用域，每个闭包指向父环境

```c
typedef struct Env {
    struct Env* parent;
    lean_object* value;
} LinkedEnv;
```

### 优化策略

**1. 已知函数优化**

```
if f是顶层定义的函数 ∧ FV(f) = ∅
then 直接调用f，不通过闭包
```

**2. 环境收缩**

```
只保留实际使用的自由变量
```

**3. 逃逸分析**

```
if 闭包不逃逸出创建作用域
then 栈分配环境
else 堆分配环境
```

---

## 【可视化·转换示例】

```
源代码:
  let add = λx.λy.x + y
  let add5 = add 5
  add5 3

闭包转换后:
  let add_closure = mkclosure(CODE_add, ∅)
  let add5_closure = mkclosure(CODE_add5, {5})
  apply_closure(add5_closure, 3)

其中:
  CODE_add(env, x) = mkclosure(CODE_add_inner, {x})
  CODE_add_inner(env, y) = env->x + y
  CODE_add5(env, y) = 5 + y
```

---

## 【多维矩阵·转换策略对比】

```
策略          内存开销    调用开销    实现复杂度    适用场景
─────────────────────────────────────────────────────────────
扁平环境       高          低          低          少量自由变量
链接环境       低          中          中          嵌套作用域
已知函数优化    无          无          低          顶层函数
部分应用缓存    中          低          高          重复部分应用
```

---

## 【定理推理树·闭包转换正确性】

```
闭包转换正确性:
────────────────────────────────────────
目标: ∀t v. t →* v ⇒ CC(t) →* CC(v)

步骤1: 基本情况（值）
  v = λx.t
  CC(v) = mkclosure(CODE, env)
  已处于规范形式
────────────────────────────────────────
步骤2: 归纳步骤（β-规约）
  前提: (λx.t) v →* t[v/x]
  转换:
    CC((λx.t) v)
    = apply_closure(CC(λx.t), CC(v))
    = apply_closure(mkclosure(CODE, env), v')
    → CODE(env, v')
    = CC(t)[v'/x]
    = CC(t[v/x])
────────────────────────────────────────
步骤3: 归纳步骤（上下文）
  E[t] →* E[t']
  由归纳假设和上下文保持性
────────────────────────────────────────
结论: 闭包转换保持语义 ∎
```

---

## 参考

- Appel, A.W. "Compiling with Continuations" (1992)
- Flanagan, C. et al. "The Essence of Compiling with Continuations" (1993)
- Kennedy, A. "Compiling with Continuations, Continued" (2007)
