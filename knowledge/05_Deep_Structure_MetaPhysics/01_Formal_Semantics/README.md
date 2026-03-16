# 形式语义学 (Formal Semantics)

## 目录概述

本目录系统阐述**程序设计语言形式语义学**的完整理论体系，涵盖操作语义、指称语义、公理语义三大经典范式，以及Hoare逻辑和类型系统形式化等核心内容。

从元数学视角探讨程序行为的严格定义方法，建立程序正确性验证的理论基础。
内容涵盖从操作语义的计算过程建模到指称语义的数学对象映射，Hoare逻辑的公理化推理体系，以及现代类型系统的形式化理论。

---

## 理论背景

### 形式语义学的三大支柱

程序语义学的核心问题：**程序的含义是什么？**

形式语义学提供三种互补的解答途径：

```text
                    程序 P
                      |
        +-------------+-------------+
        |             |             |
        v             v             v
   操作语义      公理语义       指称语义
   (怎么做)     (正确性)       (是什么)
        |             |             |
        v             v             v
   抽象机器      逻辑推理       数学对象
   状态转换      前后条件       域论构造
```

---

## 1. 操作语义 (Operational Semantics)

### 核心思想

通过抽象状态机的**计算步骤**定义程序含义。

### 结构化操作语义 (SOS)

由Plotkin提出，使用推理规则定义转换关系。

### 小步语义 (Small-Step Semantics)

表达式 `<S, sigma>` 表示语句 S 在状态 sigma 下。
小步语义描述单步转换：`<S, sigma> -> <S', sigma'>`

表示语句 S 在状态 sigma 下执行一步，到达语句 S' 和状态 sigma'。

**示例**：赋值语句的小步语义规则
如果 `<a, sigma>` 求值为 n，则 `<x := a, sigma>` 一步转换为 `<skip, sigma[x -> n]>`

### 大步语义 (Big-Step Semantics)

`<S, sigma> Downarrow sigma'`

表示语句 S 在状态 sigma 下执行终止于状态 sigma'。

### C语言示例：小步语义实现

```c
// 表达式小步语义的C实现框架
typedef enum {
    EXPR_CONST,
    EXPR_VAR,
    EXPR_ADD,
    EXPR_MUL
} ExprType;

typedef struct Expr {
    ExprType type;
    union {
        int value;           // 常量
        char var[32];        // 变量
        struct { struct Expr *left, *right; } binop;
    };
} Expr;

// 状态：变量到值的映射
typedef struct State {
    char vars[100][32];
    int values[100];
    int count;
} State;

// 小步求值：返回是否可继续规约
int small_step(Expr *e, State *s, Expr **result) {
    switch (e->type) {
        case EXPR_CONST:
            // 常量已是最简形式，不可规约
            return 0;

        case EXPR_VAR:
            // 变量查找替换为值
            for (int i = 0; i < s->count; i++) {
                if (strcmp(s->vars[i], e->var) == 0) {
                    *result = malloc(sizeof(Expr));
                    (*result)->type = EXPR_CONST;
                    (*result)->value = s->values[i];
                    return 1;
                }
            }
            return 0; // 未定义变量

        case EXPR_ADD:
            if (e->binop.left->type == EXPR_CONST &&
                e->binop.right->type == EXPR_CONST) {
                // 两个子表达式都是常量，可以规约
                *result = malloc(sizeof(Expr));
                (*result)->type = EXPR_CONST;
                (*result)->value = e->binop.left->value +
                                   e->binop.right->value;
                return 1;
            }
            // 先尝试规约左子表达式
            if (small_step(e->binop.left, s, result)) {
                Expr *new_expr = malloc(sizeof(Expr));
                new_expr->type = EXPR_ADD;
                new_expr->binop.left = *result;
                new_expr->binop.right = e->binop.right;
                *result = new_expr;
                return 1;
            }
            // 再尝试规约右子表达式
            if (small_step(e->binop.right, s, result)) {
                Expr *new_expr = malloc(sizeof(Expr));
                new_expr->type = EXPR_ADD;
                new_expr->binop.left = e->binop.left;
                new_expr->binop.right = *result;
                *result = new_expr;
                return 1;
            }
            return 0;
    }
    return 0;
}
```

---

## 2. 公理语义 (Axiomatic Semantics)

### 核心思想

通过**逻辑断言**描述程序执行前后的状态关系。

### Hoare三元组

`{P} S {Q}`

表示：若前置条件 P 在 S 执行前成立，且 S 终止，则后置条件 Q 在 S 执行后成立。

### 推理规则体系

#### 赋值规则 (Assignment)

`{Q[x -> E]} x := E {Q}`

#### 顺序规则 (Sequence)

```text
{P} S1 {R}    {R} S2 {Q}
------------------------
      {P} S1; S2 {Q}
```

#### 条件规则 (Conditional)

```text
{P and b} S1 {Q}    {P and not b} S2 {Q}
----------------------------------------
   {P} if b then S1 else S2 {Q}
```

#### 循环规则 (Partial Correctness)

```text
    {I and b} S {I}
-------------------------
{I} while b do S {I and not b}
```

其中 I 是**循环不变式(Loop Invariant)**。

### C代码示例：循环不变式验证

```c
// 计算数组和的程序及其不变式
int sum_array(int arr[], int n) {
    int sum = 0;
    int i = 0;

    // 循环不变式: sum = sum(arr[0..i-1]) 且 0 <= i <= n
    while (i < n) {
        // { sum = sum(arr[0..i-1]) 且 0 <= i < n }
        sum = sum + arr[i];
        // { sum = sum(arr[0..i]) 且 0 <= i < n }
        i = i + 1;
        // { sum = sum(arr[0..i-1]) 且 0 <= i <= n }
    }
    // { sum = sum(arr[0..n-1]) 且 i = n }
    return sum;
}
```

### 最弱前置条件 (Weakest Precondition)

**定义**: wp(S, Q) 是使得 `{P} S {Q}` 成立的最弱（最普遍）前置条件。

**性质**:

- `wp(skip, Q) = Q`
- `wp(x := E, Q) = Q[x -> E]`
- `wp(S1; S2, Q) = wp(S1, wp(S2, Q))`

---

## 3. 指称语义 (Denotational Semantics)

### 核心思想

将程序映射为**数学函数/对象**，组合性定义。

### 基本框架

语义解释函数: `[[.]] : Syntax -> Semantics`

### 语义域(Domain)

- **状态域**: State = Var -> Z_bottom
- **程序含义**: `[[S]]` : State -> State_bottom

其中 bottom 表示**非终止**，使用**提升域(Lifted Domain)**。

### 递归程序的语义

使用**最小不动点(Least Fixed Point)**：
`[[while b do S]] = fix(Phi)`

其中 Phi 是函数泛函：
`Phi(f) = lambda sigma. if [[b]] sigma then f([[S]] sigma) else sigma`

根据**Kleene不动点定理**，最小不动点存在且唯一。

### CPS转换示例

```c
// 直接风格
int factorial(int n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}

// CPS风格（适合指称语义分析）
typedef void (*Cont)(int);

void factorial_cps(int n, Cont k) {
    if (n == 0) {
        k(1);
    } else {
        factorial_cps(n - 1, ^(int result) {
            k(n * result);
        });
    }
}
```

---

## 4. Hoare逻辑完整理论

### 程序验证的三层体系

```text
+---------------------------------------------+
|  高层：程序规范 (Specification)              |
|  {前置条件} 程序 {后置条件}                  |
+---------------------------------------------+
|  中层：验证条件 (Verification Conditions)    |
|  自动生成的逻辑公式                          |
+---------------------------------------------+
|  底层：定理证明 (Theorem Proving)            |
|  SMT求解器/交互式证明助手                     |
+---------------------------------------------+
```

### 相对完备性定理

**Cook完备性定理**: 若底层断言语言足够表达力，且断言逻辑是完备的，则Hoare逻辑对于部分正确性是相对完备的。

### 全正确性 (Total Correctness)

`{P} S {Q}_tot`

要求程序不仅满足部分正确性，还必须**终止**。

**循环规则（全正确性）**: 需要变体函数 t 来证明终止性

其中 t 是**变体函数(Variant)**，每次循环迭代减小，且有下界。

### 分离逻辑 (Separation Logic)

用于推理指针程序的扩展：

```text
emp                    空堆
P * Q                  分离合取（不相交堆）
P -* Q                 分离蕴含
x |-> v                单点堆（x指向v）
```

**框架规则 (Frame Rule)**:
若 `{P} C {Q}` 成立，则 `{P * R} C {Q * R}` 也成立
（要求 R 的变量不被 C 修改）

---

## 5. 类型系统形式化

### 简单类型lambda演算

**语法**:

```text
类型 T ::= int | bool | T1 -> T2
表达式 e ::= x | n | true | false | lambda x:T.e | e1 e2
```

### 类型推导规则

变量规则: 若 x:T 在环境 Gamma 中，则 Gamma |- x : T

抽象规则: 若 Gamma, x:T1 |- e : T2，则 Gamma |- lambda x:T1.e : T1 -> T2

应用规则: 若 Gamma |- e1 : T1 -> T2 且 Gamma |- e2 : T1，则 Gamma |- e1 e2 : T2

### C语言类型系统实现

```c
// 类型表示
typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_FUNC,
    TYPE_PTR
} TypeKind;

typedef struct Type {
    TypeKind kind;
    union {
        struct { struct Type *arg, *ret; } func;  // 函数类型
        struct Type *pointee;                     // 指针类型
    };
} Type;

// 类型环境
typedef struct TypeEnv {
    char name[64];
    Type *type;
    struct TypeEnv *next;
} TypeEnv;

// 类型检查函数
int type_check(Expr *e, TypeEnv *env, Type **result);

// 示例：函数类型的类型检查
int check_function_app(Expr *fn, Expr *arg, TypeEnv *env, Type **result) {
    Type *fn_type, *arg_type;

    if (!type_check(fn, env, &fn_type)) return 0;
    if (!type_check(arg, env, &arg_type)) return 0;

    if (fn_type->kind != TYPE_FUNC) {
        fprintf(stderr, "Error: expected function type\n");
        return 0;
    }

    if (!type_equal(fn_type->func.arg, arg_type)) {
        fprintf(stderr, "Error: argument type mismatch\n");
        return 0;
    }

    *result = fn_type->func.ret;
    return 1;
}
```

### 类型安全定理

**定理 (类型安全)**: 良类型程序不会陷入停滞(Stuck)。

形式化表述：若空环境推导出 e 的类型为 T，且 e 多步规约到 e'，则要么 e' 是值，要么 e' 可以继续规约。

### 渐进类型 (Gradual Typing)

```text
T ::= ... | ?           (未知类型)

一致性关系:
T ~ T'  当且仅当 T = T' 或 ? 属于 {T, T'}
```

---

## 数学基础对照

| 语义类型 | 数学工具 | 核心概念 |
|----------|----------|----------|
| 操作语义 | 关系代数、归纳定义 | 转换关系、可达性 |
| 公理语义 | 谓词逻辑、模态逻辑 | Hoare逻辑、最弱前置条件 |
| 指称语义 | 域论、范畴论 | 连续函数、不动点 |
| 类型系统 | 类型论、证明论 | 类型判断、Curry-Howard同构 |

---

## 语义等价关系

### 操作语义与指称语义的关系

**定理 (语义的对应)**:
对于良类型程序 P，以下等价：
`<P, sigma> Downarrow sigma'` 当且仅当 `[[P]](sigma) = sigma'`

**充分性(Adequacy)**: 操作终止蕴含指称非底元
**完备性(Completeness)**: 指称非底元蕴含操作终止

### 完全抽象(Full Abstraction)

**定义**: 指称语义是完全抽象的，当且仅当：
`[[P1]] = [[P2]]` 当且仅当 P1 和 P2 上下文等价

其中上下文等价是指在所有上下文C中，C[P1]和C[P2]行为相同。

---

## 文件结构

| 文件名 | 难度 | 核心内容 |
|--------|------|----------|
| `01_Operational_Semantics.md` | 4星 | 结构化操作语义(SOS)理论，小步/大步语义，类型安全定理，上下文等价 |
| `02_Axiomatic_Semantics.md` | 5星 | Hoare逻辑完整理论，最弱前置条件演算，程序验证方法，相对完备性定理 |
| `03_Denotational_Semantics.md` | 5星 | 域论基础，Scott域，语义解释函数，完全抽象问题，Kripke逻辑关系 |
| `04_Type_Systems.md` | 5星 | 简单类型lambda演算，多态类型，依赖类型，Curry-Howard同构 |

---

## 前置知识

- **数理逻辑**: 一阶谓词演算、自然演绎
- **集合论**: 函数、关系、偏序集
- **离散数学**: 归纳法、不动点理论
- **编程语言基础**: 语法、类型系统

---

## 学术参考文献

### 经典专著

1. **Winskel, G.** (1993). *The Formal Semantics of Programming Languages*. MIT Press.

2. **Nielson, H. R., & Nielson, F.** (2007). *Semantics with Applications: An Appetizer*. Springer.

3. **Pierce, B. C.** (2002). *Types and Programming Languages*. MIT Press.

4. **Gunter, C. A.** (1992). *Semantics of Programming Languages: Structures and Techniques*. MIT Press.

5. **Harper, R.** (2016). *Practical Foundations for Programming Languages* (2nd ed.). Cambridge University Press.

### 奠基性论文

1. **Plotkin, G. D.** (1981). *A Structural Approach to Operational Semantics*. Technical Report DAIMI FN-19, Aarhus University.

2. **Hoare, C. A. R.** (1969). *An Axiomatic Basis for Computer Programming*. Communications of the ACM, 12(10), 576-580.

3. **Floyd, R. W.** (1967). *Assigning Meanings to Programs*. Proceedings of Symposia in Applied Mathematics, 19, 19-32.

4. **Scott, D., & Strachey, C.** (1971). *Toward a Mathematical Semantics for Computer Languages*. Technical Report PRG-6, Oxford.

5. **Scott, D.** (1976). *Data Types as Lattices*. SIAM Journal on Computing, 5(3), 522-587.

6. **Reynolds, J. C.** (2002). *Separation Logic: A Logic for Shared Mutable Data Structures*. In LICS '02.

---

## 学习路径建议

```text
数理逻辑基础 -> 操作语义 -> 公理语义 -> 指称语义 -> 类型系统
       |              |            |            |            |
       v              v            v            v            v
   谓词演算      状态转换      Hoare逻辑    域论构造    Curry-Howard
       |              |            |            |            |
       v              v            v            v            v
   归纳原理      类型安全      程序验证    完全抽象    依赖类型
```

---

## 相关链接

- [父目录: 深层结构与元物理学](../README.md)
- [C到汇编映射](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md)
- [自修改代码](../12_Self_Modifying_Code/README.md)

---

## 数学符号速查

| 符号 | 含义 |
|------|------|
| `<S, sigma> -> sigma'` | 小步语义转换 |
| `<S, sigma> Downarrow sigma'` | 大步语义求值 |
| `{P} S {Q}` | Hoare三元组 |
| `[[P]]` | 指称语义解释 |
| `_\|_` | 底元(非终止) |
| `fix(f)` | 最小不动点 |
| `sigma[x -> v]` | 状态更新 |
| `Gamma \|- e : T` | 类型判断 |

---

*本目录内容适合形式化方法研究者、程序验证工程师及编程语言理论学者阅读。*

*最后更新: 2026-03-13*:
