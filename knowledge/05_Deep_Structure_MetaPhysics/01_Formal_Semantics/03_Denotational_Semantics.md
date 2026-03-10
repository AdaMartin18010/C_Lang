# 指称语义学

## 目录

- [指称语义学](#指称语义学)
  - [目录](#目录)
  - [引言](#引言)
  - [指称语义数学基础](#指称语义数学基础)
    - [2.1 基本思想](#21-基本思想)
    - [2.2 语义函数](#22-语义函数)
    - [2.3 合成性原则](#23-合成性原则)
    - [2.4 C语言示例：表达式求值](#24-c语言示例表达式求值)
  - [域论(Domain Theory)简介](#域论domain-theory简介)
    - [3.1 为什么需要域论](#31-为什么需要域论)
    - [3.2 偏序集与完全偏序(CPO)](#32-偏序集与完全偏序cpo)
    - [3.3 连续函数与不动点](#33-连续函数与不动点)
    - [3.4 应用到递归](#34-应用到递归)
    - [3.5 提升域(Lifted Domain)](#35-提升域lifted-domain)
  - [C语言构造的语义函数](#c语言构造的语义函数)
    - [4.1 状态表示](#41-状态表示)
    - [4.2 语句语义](#42-语句语义)
    - [4.3 循环语义（不动点形式）](#43-循环语义不动点形式)
  - [类型指称和求值语义](#类型指称和求值语义)
    - [5.1 类型作为域](#51-类型作为域)
    - [5.2 类型安全的语义](#52-类型安全的语义)
    - [5.3 环境模型](#53-环境模型)
  - [实践应用](#实践应用)
    - [6.1 语义与编译器优化](#61-语义与编译器优化)
    - [6.2 程序分析工具](#62-程序分析工具)
    - [6.3 总结](#63-总结)

---

## 引言

指称语义(Denotational Semantics)由Dana Scott和Christopher Strachey在1970年代提出，其核心思想是：**每个程序构造的指称（意义）是一个数学对象**。
与操作语义（关注如何执行）不同，指称语义关注程序"是什么"。

---

## 指称语义数学基础

### 2.1 基本思想

```text
程序 → 数学对象
```

- **表达式** → 值（数字、布尔值等）
- **语句** → 状态转换函数
- **程序** → 输入到输出的函数

### 2.2 语义函数

语义函数通常用双括号表示：

```text
⟦·⟧ : 语法域 → 语义域
```

例如：

- `⟦3 + 5⟧ = 8`
- `⟦x := 5⟧ = λσ.σ[x↦5]` (将状态σ中x映射为5)

### 2.3 合成性原则

复合构造的语义由其部分的语义组合而成：

```text
⟦S1; S2⟧ = ⟦S2⟧ ∘ ⟦S1⟧
```

### 2.4 C语言示例：表达式求值

```c
#include <stdio.h>
#include <stdlib.h>

/*
 * 表达式指称语义示例
 * ⟦n⟧ = n                    (整数常量)
 * ⟦e1 + e2⟧ = ⟦e1⟧ + ⟦e2⟧   (加法)
 * ⟦e1 * e2⟧ = ⟦e1⟧ * ⟦e2⟧   (乘法)
 */

typedef enum { CONST, ADD, MUL } ExprType;

typedef struct Expr {
    ExprType type;
    int value;           // 用于CONST
    struct Expr *left;   // 用于ADD, MUL
    struct Expr *right;  // 用于ADD, MUL
} Expr;

// 创建常量表达式
Expr* make_const(int n) {
    Expr *e = malloc(sizeof(Expr));
    e->type = CONST;
    e->value = n;
    return e;
}

// 创建加法表达式
Expr* make_add(Expr *l, Expr *r) {
    Expr *e = malloc(sizeof(Expr));
    e->type = ADD;
    e->left = l;
    e->right = r;
    return e;
}

// 语义函数 ⟦·⟧: Expr → int
int eval(Expr *e) {
    switch (e->type) {
        case CONST:
            return e->value;                    // ⟦n⟧ = n
        case ADD:
            return eval(e->left) + eval(e->right);  // ⟦e1+e2⟧ = ⟦e1⟧ + ⟦e2⟧
        case MUL:
            return eval(e->left) * eval(e->right);  // ⟦e1*e2⟧ = ⟦e1⟧ * ⟦e2⟧
        default:
            return 0;
    }
}

void free_expr(Expr *e) {
    if (e->type != CONST) {
        free_expr(e->left);
        free_expr(e->right);
    }
    free(e);
}

int main(void) {
    printf("=== 表达式指称语义 ===\n");

    // 构建表达式: (3 + 5) * 2
    // ⟦(3+5)*2⟧ = ⟦3+5⟧ * ⟦2⟧ = (⟦3⟧+⟦5⟧) * 2 = (3+5) * 2 = 16
    Expr *expr = make_add(make_const(3), make_const(5));
    Expr *full = make_mul(expr, make_const(2));

    printf("⟦(3 + 5) * 2⟧ = %d\n", eval(full));

    free_expr(full);
    return 0;
}
```

---

## 域论(Domain Theory)简介

### 3.1 为什么需要域论

程序可能出现：

- 非终止（无限循环）
- 运行时错误
- 部分定义

域论提供数学结构来处理这些"不完善"的情况。

### 3.2 偏序集与完全偏序(CPO)

**定义**：完全偏序(CPO)是带有最小元素⊥(bottom)的偏序集，其中每个链都有上确界。

```text
⊥ 表示：未定义、发散、错误
```

### 3.3 连续函数与不动点

**不动点定理**：连续函数f: D→D在CPO上有最小不动点 fix(f)

```text
fix(f) = ⊔{⊥, f(⊥), f(f(⊥)), ...}
```

### 3.4 应用到递归

```c
#include <stdio.h>

/*
 * 递归函数的指称语义
 * 阶乘的不动点定义:
 * F(f)(n) = if n==0 then 1 else n * f(n-1)
 *
 * ⟦fact⟧ = fix(F)  (F的最小不动点)
 */

typedef unsigned long (*FactFunc)(unsigned long);

// 泛函 F
FactFunc make_fact_step(FactFunc f) {
    (void)f; // 实际实现直接递归
    return NULL; // 占位
}

// 实际的阶乘是其最小不动点
unsigned long fact(unsigned long n) {
    if (n == 0) return 1;
    return n * fact(n - 1);
}

int main(void) {
    printf("=== 不动点与递归 ===\n");

    // 展示阶乘作为不动点
    // fact = F(fact)
    for (unsigned long i = 0; i <= 10; i++) {
        unsigned long result = fact(i);
        // 验证: fact(n) == F(fact)(n)
        unsigned long expected = (i == 0) ? 1 : i * fact(i - 1);
        printf("fact(%lu) = %lu, F(fact)(%lu) = %lu\n",
               i, result, i, expected);
    }

    return 0;
}
```

### 3.5 提升域(Lifted Domain)

```c
#include <stdio.h>
#include <stdbool.h>

/*
 * 提升域 D⊥ = D ∪ {⊥}
 * 用于处理可能发散的计算
 */

typedef struct {
    bool defined;  // false 表示 ⊥
    int value;
} LiftedInt;

#define BOTTOM ((LiftedInt){false, 0})
#define LIFT(n) ((LiftedInt){true, n})

LiftedInt add_lifted(LiftedInt a, LiftedInt b) {
    if (!a.defined || !b.defined) return BOTTOM;
    return LIFT(a.value + b.value);
}

LiftedInt div_lifted(LiftedInt a, LiftedInt b) {
    if (!a.defined || !b.defined) return BOTTOM;
    if (b.value == 0) return BOTTOM;  // 除以零 = ⊥
    return LIFT(a.value / b.value);
}

int main(void) {
    printf("=== 提升域运算 ===\n");

    LiftedInt x = LIFT(10);
    LiftedInt y = LIFT(5);
    LiftedInt z = BOTTOM;

    LiftedInt r1 = add_lifted(x, y);
    printf("10 + 5 = %s%d\n", r1.defined ? "" : "⊥", r1.value);

    LiftedInt r2 = add_lifted(x, z);
    printf("10 + ⊥ = %s\n", r2.defined ? "定义" : "⊥");

    LiftedInt r3 = div_lifted(x, LIFT(0));
    printf("10 / 0 = %s\n", r3.defined ? "定义" : "⊥");

    return 0;
}
```

---

## C语言构造的语义函数

### 4.1 状态表示

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 状态 σ: 变量名 → 值
 * 用简单哈希表模拟
 */

#define MAX_VARS 100

typedef struct {
    char name[32];
    int value;
} VarBinding;

typedef struct {
    VarBinding bindings[MAX_VARS];
    int count;
} State;

State* new_state(void) {
    State *s = calloc(1, sizeof(State));
    return s;
}

// 状态更新 σ[x ↦ v]
void state_update(State *s, const char *name, int value) {
    for (int i = 0; i < s->count; i++) {
        if (strcmp(s->bindings[i].name, name) == 0) {
            s->bindings[i].value = value;
            return;
        }
    }
    if (s->count < MAX_VARS) {
        strcpy(s->bindings[s->count].name, name);
        s->bindings[s->count].value = value;
        s->count++;
    }
}

int state_lookup(State *s, const char *name) {
    for (int i = 0; i < s->count; i++) {
        if (strcmp(s->bindings[i].name, name) == 0) {
            return s->bindings[i].value;
        }
    }
    return 0; // 默认为0
}

// 状态复制
State* state_copy(State *s) {
    State *new = malloc(sizeof(State));
    memcpy(new, s, sizeof(State));
    return new;
}
```

### 4.2 语句语义

```c
/* 接上 */

/*
 * 语句语义: ⟦S⟧ : State → State
 * - 赋值: ⟦x := E⟧(σ) = σ[x ↦ ⟦E⟧(σ)]
 * - 顺序: ⟦S1; S2⟧(σ) = ⟦S2⟧(⟦S1⟧(σ))
 * - 条件: ⟦if B then S1 else S2⟧(σ) = ⟦S1⟧(σ) if ⟦B⟧(σ) else ⟦S2⟧(σ)
 */

// 赋值语句语义
State* semantic_assign(State *sigma, const char *var, int value) {
    State *new_sigma = state_copy(sigma);
    state_update(new_sigma, var, value);
    return new_sigma;
}

// 顺序语句语义
State* semantic_seq(State* (*s1)(State*), State* (*s2)(State*), State *sigma) {
    State *sigma1 = s1(sigma);
    State *sigma2 = s2(sigma1);
    free(sigma1);
    return sigma2;
}

// 示例: x := 5; y := x + 3
State* example_program(State *sigma) {
    State *s1 = semantic_assign(sigma, "x", 5);
    State *s2 = semantic_assign(s1, "y", state_lookup(s1, "x") + 3);
    free(s1);
    return s2;
}

int main(void) {
    printf("=== 状态语义 ===\n");

    State *sigma0 = new_state();
    printf("初始状态: {}\n");

    State *sigma_final = example_program(sigma0);
    printf("执行后: x=%d, y=%d\n",
           state_lookup(sigma_final, "x"),
           state_lookup(sigma_final, "y"));

    free(sigma0);
    free(sigma_final);
    return 0;
}
```

### 4.3 循环语义（不动点形式）

```c
#include <stdio.h>
#include <stdbool.h>

/*
 * while 循环的指称语义:
 * ⟦while B do S⟧ = fix(λf.λσ. if ⟦B⟧(σ) then f(⟦S⟧(σ)) else σ)
 *
 * 这是通过不动点定义的递归函数
 */

int factorial_loop(int n) {
    int result = 1;
    int i = 0;

    // while (i < n) { i++; result *= i; }
    // 语义: 重复应用直到条件为假
    while (i < n) {
        i = i + 1;
        result = result * i;
    }

    return result;
}

// 展示语义等价于不动点迭代
typedef int (*LoopFunc)(int, int);

int loop_body(int i, int result, int n) {
    if (i >= n) return result;
    // 继续迭代
    return loop_body(i + 1, result * (i + 1), n);
}

int factorial_unfolded(int n) {
    // 模拟不动点展开
    return loop_body(0, 1, n);
}

int main(void) {
    printf("=== 循环不动点语义 ===\n");

    for (int i = 0; i <= 5; i++) {
        int r1 = factorial_loop(i);
        int r2 = factorial_unfolded(i);
        printf("fact(%d): 循环=%d, 展开=%d\n", i, r1, r2);
    }

    return 0;
}
```

---

## 类型指称和求值语义

### 5.1 类型作为域

```text
⟦int⟧ = ℤ⊥          (整数提升域)
⟦bool⟧ = {true, false, ⊥}
⟦T1 → T2⟧ = ⟦T1⟧ → ⟦T2⟧  (函数域)
```

### 5.2 类型安全的语义

```c
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

/*
 * 类型指称示例
 * 每个C类型对应一个数学域
 */

typedef enum { TYPE_INT, TYPE_BOOL, TYPE_ERROR } ValType;

typedef struct {
    ValType type;
    union {
        int int_val;
        bool bool_val;
    };
} TypedValue;

// 类型正确的加法
TypedValue typed_add(TypedValue a, TypedValue b) {
    assert(a.type == TYPE_INT && b.type == TYPE_INT);
    return (TypedValue){TYPE_INT, .int_val = a.int_val + b.int_val};
}

// 类型正确的比较
TypedValue typed_lt(TypedValue a, TypedValue b) {
    assert(a.type == TYPE_INT && b.type == TYPE_INT);
    return (TypedValue){TYPE_BOOL, .bool_val = a.int_val < b.int_val};
}

// 语义解释: 类型到值的映射
void print_value(TypedValue v) {
    switch (v.type) {
        case TYPE_INT:  printf("%d", v.int_val); break;
        case TYPE_BOOL: printf("%s", v.bool_val ? "true" : "false"); break;
        case TYPE_ERROR: printf("ERROR"); break;
    }
}

int main(void) {
    printf("=== 类型指称语义 ===\n");

    // ⟦3⟧ = 3 ∈ ℤ
    TypedValue three = {TYPE_INT, .int_val = 3};
    printf("⟦3⟧ = "); print_value(three); printf(" ∈ ℤ\n");

    // ⟦3 + 5⟧ = 8 ∈ ℤ
    TypedValue five = {TYPE_INT, .int_val = 5};
    TypedValue sum = typed_add(three, five);
    printf("⟦3 + 5⟧ = "); print_value(sum); printf(" ∈ ℤ\n");

    // ⟦3 < 5⟧ = true ∈ 𝔹
    TypedValue cmp = typed_lt(three, five);
    printf("⟦3 < 5⟧ = "); print_value(cmp); printf(" ∈ 𝔹\n");

    return 0;
}
```

### 5.3 环境模型

```c
#include <stdio.h>
#include <stdlib.h>

/*
 * 环境(Env): 变量 → 位置
 * 存储(Store): 位置 → 值
 *
 * 支持引用和别名分析
 */

#define MAX_LOCS 100

typedef int Location;

typedef struct {
    int values[MAX_LOCS];
    bool used[MAX_LOCS];
} Store;

typedef struct EnvNode {
    char name[32];
    Location loc;
    struct EnvNode *next;
} EnvNode;

typedef EnvNode* Environment;

Store* new_store(void) {
    return calloc(1, sizeof(Store));
}

Location alloc_location(Store *store) {
    for (int i = 0; i < MAX_LOCS; i++) {
        if (!store->used[i]) {
            store->used[i] = true;
            return i;
        }
    }
    return -1;
}

void env_bind(Environment *env, const char *name, Location loc) {
    EnvNode *node = malloc(sizeof(EnvNode));
    strcpy(node->name, name);
    node->loc = loc;
    node->next = *env;
    *env = node;
}

Location env_lookup(Environment env, const char *name) {
    for (EnvNode *n = env; n; n = n->next) {
        if (strcmp(n->name, name) == 0) return n->loc;
    }
    return -1;
}

int main(void) {
    printf("=== 环境-存储模型 ===\n");

    Store *store = new_store();
    Environment env = NULL;

    // 分配变量 x
    Location x_loc = alloc_location(store);
    store->values[x_loc] = 42;
    env_bind(&env, "x", x_loc);

    // 分配变量 y 指向同一位置(别名)
    env_bind(&env, "y", x_loc);

    Location loc_x = env_lookup(env, "x");
    Location loc_y = env_lookup(env, "y");

    printf("x 在位置 %d，值为 %d\n", loc_x, store->values[loc_x]);
    printf("y 在位置 %d，值为 %d\n", loc_y, store->values[loc_y]);
    printf("x 和 y %s别名\n", (loc_x == loc_y) ? "是" : "不是");

    return 0;
}
```

---

## 实践应用

### 6.1 语义与编译器优化

指称语义为编译器优化提供理论基础：

- **常量折叠**：`⟦3 + 5⟧ = ⟦8⟧`
- **死代码消除**：分析状态的定义-使用链
- **循环优化**：基于不动点性质

### 6.2 程序分析工具

```c
/*
 * 简单常量传播分析
 * 基于指称语义的抽象解释
 */

typedef enum { UNKNOWN, KNOWN } ConstStatus;

typedef struct {
    ConstStatus status;
    int value;
} ConstProp;

ConstProp eval_const(ConstProp a, ConstProp b, char op) {
    if (a.status == KNOWN && b.status == KNOWN) {
        int result;
        switch (op) {
            case '+': result = a.value + b.value; break;
            case '-': result = a.value - b.value; break;
            case '*': result = a.value * b.value; break;
            default: return (ConstProp){UNKNOWN, 0};
        }
        return (ConstProp){KNOWN, result};
    }
    return (ConstProp){UNKNOWN, 0};
}
```

### 6.3 总结

指称语义的核心价值：

1. **抽象层次高**：不依赖具体机器
2. **数学基础严谨**：便于形式化证明
3. **组合性强**：支持模块化推理
4. **工具基础**：为静态分析、验证提供理论支撑

掌握指称语义，能够更深入理解程序本质，设计更可靠的工具和系统。
