# 可计算性理论 (Computability Theory)

## 目录

- [可计算性理论 (Computability Theory)](#可计算性理论-computability-theory)
  - [目录](#目录)
  - [概述](#概述)
    - [核心问题](#核心问题)
  - [图灵机 (Turing Machine)](#图灵机-turing-machine)
    - [形式化定义](#形式化定义)
    - [图灵机工作原理](#图灵机工作原理)
    - [C语言模拟图灵机示例](#c语言模拟图灵机示例)
    - [图灵机的变种](#图灵机的变种)
  - [Lambda演算 (Lambda Calculus)](#lambda演算-lambda-calculus)
    - [基本语法](#基本语法)
    - [核心概念](#核心概念)
    - [C语言中的Lambda思想](#c语言中的lambda思想)
  - [停机问题 (Halting Problem)](#停机问题-halting-problem)
    - [问题陈述](#问题陈述)
    - [反证法证明](#反证法证明)
    - [C语言代码演示](#c语言代码演示)
    - [不可判定问题的意义](#不可判定问题的意义)
  - [丘奇-图灵论题 (Church-Turing Thesis)](#丘奇-图灵论题-church-turing-thesis)
    - [论题陈述](#论题陈述)
    - [等价计算模型](#等价计算模型)
    - [有效可计算性](#有效可计算性)
  - [复杂性类基础 (Complexity Classes)](#复杂性类基础-complexity-classes)
    - [时间复杂性类](#时间复杂性类)
    - [P vs NP 问题](#p-vs-np-问题)
    - [NP完全问题 (NP-Complete)](#np完全问题-np-complete)
  - [实践意义与编程启示](#实践意义与编程启示)
    - [对软件开发的启示](#对软件开发的启示)
    - [C语言中的实际考虑](#c语言中的实际考虑)
  - [参考资源](#参考资源)

---

## 概述

可计算性理论（Computability Theory）是理论计算机科学的基石，研究什么问题可以被算法解决，什么问题是不可计算的。
它定义了计算的数学边界，为理解现代计算机的能力与局限性提供了理论基础。

### 核心问题

- **可判定性**：给定一个问题，是否存在算法能在有限时间内给出答案？
- **可计算性**：给定一个函数，是否存在算法能计算其值？
- [复杂性](../05_Computational_Complexity/README.md)：解决问题需要多少资源（时间、空间）？

---

## 图灵机 (Turing Machine)

### 形式化定义

图灵机是一个七元组：$M = (Q, \Sigma, \Gamma, \delta, q_0, B, F)$

| 符号 | 含义 |
|------|------|
| $Q$ | 有限状态集合 |
| $\Sigma$ | 输入字母表（不含空白符） |
| $\Gamma$ | 带字母表，$\Sigma \subset \Gamma$ |
| $\delta$ | 转移函数：$Q \times \Gamma \rightarrow Q \times \Gamma \times \{L, R, S\}$ |
| $q_0$ | 初始状态，$q_0 \in Q$ |
| $B$ | 空白符号，$B \in \Gamma - \Sigma$ |
| $F$ | 接受状态集，$F \subseteq Q$ |

### 图灵机工作原理

```
┌─────────────────────────────────────────────────────────────┐
│                      图灵机结构示意                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   有限状态控制器                    无限长磁带               │
│   ┌───────────────┐              ┌──┬──┬──┬──┬──┬──┬──┐     │
│   │  当前状态: q  │              │a │b │c │d │e │B │B │...  │
│   │  指令表: δ    │              └──┴──┴──┴──┴──┴──┴──┘     │
│   └───────┬───────┘                 ▲                       │
│           │                      读写头                      │
│           │                      (指向c)                     │
└───────────┼─────────────────────────────────────────────────┘
            │
            ▼
      ┌─────────────┐
      │ δ(q, c) =   │
      │  (q', x, R) │ → 写入x，右移，进入q'状态
      └─────────────┘
```

### C语言模拟图灵机示例

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 图灵机模拟器 - 识别语言 0^n1^n */

typedef enum {
    STATE_START,    // q0: 初始状态
    STATE_FIND_1,   // q1: 寻找1
    STATE_BACK,     // q2: 返回
    STATE_ACCEPT,   // q_accept: 接受
    STATE_REJECT    // q_reject: 拒绝
} State;

typedef struct {
    char *tape;     // 磁带
    int head;       // 读写头位置
    State state;    // 当前状态
    int tape_size;  // 磁带大小
} TuringMachine;

TuringMachine* tm_create(const char *input) {
    TuringMachine *tm = malloc(sizeof(TuringMachine));
    tm->tape_size = strlen(input) + 100;  // 额外空间
    tm->tape = malloc(tm->tape_size);
    memset(tm->tape, 'B', tm->tape_size); // 填充空白符
    strcpy(tm->tape, input);              // 写入输入
    tm->head = 0;
    tm->state = STATE_START;
    return tm;
}

void tm_step(TuringMachine *tm) {
    char symbol = tm->tape[tm->head];

    switch (tm->state) {
        case STATE_START:
            if (symbol == '0') {
                tm->tape[tm->head] = 'X';  // 标记0
                tm->head++;
                tm->state = STATE_FIND_1;
            } else if (symbol == 'Y') {
                tm->head++;
            } else if (symbol == 'B') {
                tm->state = STATE_ACCEPT;
            } else {
                tm->state = STATE_REJECT;
            }
            break;

        case STATE_FIND_1:
            if (symbol == '0' || symbol == 'Y') {
                tm->head++;
            } else if (symbol == '1') {
                tm->tape[tm->head] = 'Y';  // 标记1
                tm->head--;
                tm->state = STATE_BACK;
            } else {
                tm->state = STATE_REJECT;
            }
            break;

        case STATE_BACK:
            if (symbol == '0' || symbol == 'Y') {
                tm->head--;
            } else if (symbol == 'X') {
                tm->head++;
                tm->state = STATE_START;
            }
            break;

        default:
            break;
    }
}

int tm_run(TuringMachine *tm) {
    int steps = 0;
    const int MAX_STEPS = 10000;

    while (tm->state != STATE_ACCEPT && tm->state != STATE_REJECT) {
        tm_step(tm);
        if (++steps > MAX_STEPS) {
            printf("Error: 超出最大步数限制\n");
            return -1;
        }
    }

    return tm->state == STATE_ACCEPT ? 1 : 0;
}

void tm_print(TuringMachine *tm) {
    printf("磁带: ");
    for (int i = 0; i < 20 && tm->tape[i] != 'B'; i++) {
        if (i == tm->head) {
            printf("[%c]", tm->tape[i]);
        } else {
            printf(" %c ", tm->tape[i]);
        }
    }
    printf("  (状态: %d, 头位置: %d)\n", tm->state, tm->head);
}

int main() {
    const char *tests[] = {"0011", "000111", "011", "00111", ""};

    printf("图灵机识别语言 0^n1^n 的测试\n");
    printf("=" * 50 + "\n\n");

    for (int i = 0; i < 5; i++) {
        TuringMachine *tm = tm_create(tests[i]);
        printf("输入: \"%s\"\n", tests[i]);

        int result = tm_run(tm);
        printf("结果: %s\n\n", result == 1 ? "接受 ✓" :
                            result == 0 ? "拒绝 ✗" : "错误");

        free(tm->tape);
        free(tm);
    }

    return 0;
}
```

### 图灵机的变种

| 变种 | 描述 | 计算能力 |
|------|------|----------|
| 多带图灵机 | 拥有多条磁带和多个读写头 | 等价于标准图灵机 |
| 非确定性图灵机 | 每个步骤可有多个选择 | 等价于确定性图灵机（时间可能不同） |
| 无限带图灵机 | 双向无限延伸的磁带 | 等价于标准图灵机 |
| 通用图灵机 | 可以模拟任何其他图灵机 | 现代计算机的理论模型 |

---

## Lambda演算 (Lambda Calculus)

### 基本语法

```
<表达式> ::= <变量>
          | λ<变量>.<表达式>    (抽象/函数定义)
          | (<表达式> <表达式>)  (应用/函数调用)
```

### 核心概念

**1. λ抽象（函数定义）**

```lambda
λx. x + 1    ; 一个接受x，返回x+1的函数
λx. λy. x+y  ; 柯里化：接受x，返回一个接受y的函数
```

**2. β归约（函数应用）**

```lambda
(λx. x + 1) 5    →    5 + 1    →    6
(λx. λy. x+y) 2  →    λy. 2+y  ; 部分应用
```

### C语言中的Lambda思想

```c
#include <stdio.h>
#include <stdlib.h>

/* 函数指针模拟Lambda */

// 一元函数类型
typedef int (*UnaryFunc)(int);
typedef int (*BinaryFunc)(int, int);

// λx. x + 1
int add_one(int x) { return x + 1; }

// λx. x * 2
int double_val(int x) { return x * 2; }

// λx. λy. x + y (柯里化模拟)
typedef struct {
    int x;
} Adder;

int adder_apply(Adder *a, int y) {
    return a->x + y;
}

Adder* make_adder(int x) {
    Adder *a = malloc(sizeof(Adder));
    a->x = x;
    return a;
}

// 高阶函数：map
void map(int *arr, int n, UnaryFunc f) {
    for (int i = 0; i < n; i++) {
        arr[i] = f(arr[i]);
    }
}

// 高阶函数：reduce
int reduce(int *arr, int n, int init, BinaryFunc f) {
    int acc = init;
    for (int i = 0; i < n; i++) {
        acc = f(acc, arr[i]);
    }
    return acc;
}

int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

int main() {
    int nums[] = {1, 2, 3, 4, 5};
    int n = 5;

    printf("原始数组: ");
    for (int i = 0; i < n; i++) printf("%d ", nums[i]);
    printf("\n");

    // 应用 λx. x * 2
    map(nums, n, double_val);
    printf("×2 后:    ");
    for (int i = 0; i < n; i++) printf("%d ", nums[i]);
    printf("\n");

    // 应用 λx. x + 1
    map(nums, n, add_one);
    printf("+1 后:    ");
    for (int i = 0; i < n; i++) printf("%d ", nums[i]);
    printf("\n");

    // 使用柯里化加法
    Adder *add5 = make_adder(5);
    printf("\n柯里化加法 add5(3) = %d\n", adder_apply(add5, 3));

    // reduce 演示
    int sum = reduce(nums, n, 0, add);
    printf("数组求和: %d\n", sum);

    free(add5);
    return 0;
}
```

---

## 停机问题 (Halting Problem)

### 问题陈述

> **停机问题**：给定任意程序P和其输入I，判定P在输入I上是否会停机（终止）还是无限循环。

**定理**：停机问题是**不可判定的**（Undecidable）。

### 反证法证明

```
假设存在一个函数 H(P, I) 可以判定程序P在输入I上是否停机：
- 若P(I)停机，返回 true
- 若P(I)不停机，返回 false

构造程序 D(X):
    if H(X, X) == true:   // 如果X(X)停机
        while (1) {}      // 则无限循环
    else:
        return;           // 否则立即返回

现在考虑 D(D):
1. 如果 H(D, D) == true (即D(D)停机)
   → D执行while(1)无限循环
   → 矛盾！D(D)没有停机

2. 如果 H(D, D) == false (即D(D)不停机)
   → D立即return
   → 矛盾！D(D)停机了

因此，H不可能存在。停机问题不可判定。 ∎
```

### C语言代码演示

```c
#include <stdio.h>
#include <stdbool.h>

/*
 * 停机问题的证明演示
 * 注意：以下H函数是假设存在的，实际上无法编写
 */

// 假设的停机判定函数（实际上不可能存在）
bool H(void (*program)(void*), void *input);

// 一个会停机的程序
void halting_program(void *input) {
    int n = *(int*)input;
    for (int i = 0; i < n; i++);
    printf("程序正常结束\n");
}

// 一个不会停机的程序
void non_halting_program(void *input) {
    (void)input;
    while (1) {
        // 无限循环
    }
}

// 矛盾构造器 D(X)
void D(void *X) {
    // 如果H说X(X)停机，我们就无限循环
    // 如果H说X(X)不停机，我们就立即返回
    // 这导致了逻辑矛盾

    // 注意：以下代码是伪代码，因为H不存在
    /*
    if (H(X, X)) {
        while (1) {}  // 无限循环
    } else {
        return;       // 立即返回
    }
    */
}

int main() {
    printf("停机问题不可判定性证明\n");
    printf("======================\n\n");

    printf("1. 假设存在函数 H(P, I) 可以判定程序P在输入I上是否停机\n");
    printf("2. 构造程序 D(X):\n");
    printf("   - 如果 H(X, X) == true (X会停机)\n");
    printf("     → D进入无限循环\n");
    printf("   - 如果 H(X, X) == false (X不会停机)\n");
    printf("     → D立即返回\n\n");

    printf("3. 考虑 D(D):\n");
    printf("   - 若 H(D, D) == true  → D应该停机 → 但D无限循环 → 矛盾\n");
    printf("   - 若 H(D, D) == false → D应该不停机 → 但D立即返回 → 矛盾\n\n");

    printf("结论：假设不成立，停机问题不可判定。\n");

    return 0;
}
```

### 不可判定问题的意义

```
┌──────────────────────────────────────────────────────────────┐
│                   问题的分类                                  │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   ┌─────────────────────────────────────────────────────┐   │
│   │                  所有问题                            │   │
│   │  ┌─────────────────────────────────────────────┐   │   │
│   │  │            可判定问题                        │   │   │
│   │  │  ┌─────────────────────────────────────┐   │   │   │
│   │  │  │         可计算问题                   │   │   │   │
│   │  │  │  ┌─────────────────────────────┐   │   │   │   │
│   │  │  │  │      易解问题 (P类)          │   │   │   │   │
│   │  │  │  │  多项式时间可解              │   │   │   │   │
│   │  │  │  └─────────────────────────────┘   │   │   │   │
│   │  │  │  ┌─────────────────────────────┐   │   │   │   │
│   │  │  │  │      难解问题 (NP难问题)     │   │   │   │   │
│   │  │  │  │  指数时间或更慢              │   │   │   │   │
│   │  │  │  └─────────────────────────────┘   │   │   │   │
│   │  │  └─────────────────────────────────────┘   │   │   │
│   │  │  例如：停机问题、Post对应问题              │   │   │
│   │  └─────────────────────────────────────────────┘   │   │
│   │  例如：哥德尔不完备定理相关的数论问题              │   │
│   └─────────────────────────────────────────────────────┘   │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

## 丘奇-图灵论题 (Church-Turing Thesis)

### 论题陈述

> **丘奇-图灵论题**：任何在算法上可计算的函数，都可以用图灵机计算（或等价的Lambda演算）。

这不是一个数学定理，而是一个关于计算本质的**经验性论题**。

### 等价计算模型

| 计算模型 | 提出者 | 年份 | 与图灵机等价性 |
|----------|--------|------|----------------|
| λ演算 | Alonzo Church | 1936 | 等价 |
| 图灵机 | Alan Turing | 1936 | 标准模型 |
| 递归函数 | Gödel, Kleene | 1936 | 等价 |
| 随机存取机 (RAM) | - | 1960s | 等价（多项式时间内） |
| 细胞自动机 | John von Neumann | 1950s | 等价 |
| 量子图灵机 | David Deutsch | 1985 | 等价（对可计算函数） |

### 有效可计算性

```
┌──────────────────────────────────────────────────────────────┐
│              丘奇-图灵论题的直观图示                          │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│   直观概念                    形式化定义                      │
│   ┌──────────────┐            ┌──────────────┐               │
│   │  "算法"      │   ======   │   图灵机     │               │
│   │  "有效计算"  │   ======   │   λ演算      │               │
│   │  "机械过程"  │   ======   │   递归函数   │               │
│   └──────────────┘            └──────────────┘               │
│          │                           │                       │
│          └───────────┬───────────────┘                       │
│                      ▼                                       │
│            ┌─────────────────┐                               │
│            │   可计算函数集   │                               │
│            └─────────────────┘                               │
│                                                              │
│   论题：这三者描述的是同一类函数                              │
└──────────────────────────────────────────────────────────────┘
```

---

## 复杂性类基础 (Complexity Classes)

### 时间复杂性类

```
┌────────────────────────────────────────────────────────────────────┐
│                     时间复杂性类层次结构                            │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│   L  ⊆  NL  ⊆  P  ⊆  NP  ⊆  PSPACE  ⊆  EXPTIME  ⊆  NEXPTIME      │
│                                                                    │
│   L        = DSPACE(log n)         对数空间                        │
│   NL       = NSPACE(log n)         非确定性对数空间                │
│   P        = DTIME(n^O(1))         多项式时间（确定性）            │
│   NP       = NTIME(n^O(1))         非确定性多项式时间              │
│   PSPACE   = DSPACE(n^O(1))        多项式空间                      │
│   EXPTIME  = DTIME(2^n^O(1))       指数时间                        │
│                                                                    │
│   包含关系：L ⊆ NL ⊆ P ⊆ NP ⊆ PSPACE = NPSPACE ⊆ EXPTIME          │
│                                                                    │
│   已知真包含：L ⊂ PSPACE,  P ⊂ EXPTIME                            │
│   未解决问题：P = NP? (千禧年大奖难题之一)                          │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

### P vs NP 问题

```c
/*
 * P类问题示例：排序
 * 可以在多项式时间内解决
 */
void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);
        quicksort(arr, low, pivot - 1);
        quicksort(arr, pivot + 1, high);
    }
}
// 时间复杂度：O(n log n) - 属于P类

/*
 * NP类问题示例：布尔可满足性问题(SAT)
 * 可以在多项式时间内验证解，但不知道是否能多项式时间求解
 */
bool verify_sat(bool assignment[], Formula *f) {
    // 验证一个赋值是否满足布尔公式
    // 时间复杂度：O(|f|) - 多项式时间可验证
    return evaluate(f, assignment);
}

// 求解脱机需要指数时间（目前最好的算法）
bool solve_sat(Formula *f, bool assignment[]) {
    int n = f->num_variables;
    // 暴力枚举所有2^n种赋值
    for (int mask = 0; mask < (1 << n); mask++) {
        for (int i = 0; i < n; i++) {
            assignment[i] = (mask >> i) & 1;
        }
        if (verify_sat(assignment, f)) {
            return true;
        }
    }
    return false;
}
// 时间复杂度：O(2^n * |f|) - 指数时间
```

### NP完全问题 (NP-Complete)

**定义**：问题L是NP完全的，如果：

1. $L \in NP$
2. 所有NP问题都可以多项式归约到L

**经典NP完全问题**：

| 问题 | 描述 |
|------|------|
| SAT | 布尔可满足性 |
| 3-SAT | 每个子句最多3个文字的SAT |
| 顶点覆盖 | 图中是否存在k个顶点覆盖所有边 |
| 团问题 | 图中是否存在k个顶点的完全子图 |
| 哈密顿路径 | 图中是否存在经过每个顶点一次的路径 |
| 背包问题 | 在重量限制下最大化价值 |
| 旅行商问题 | 寻找最短的哈密顿回路 |

```
┌─────────────────────────────────────────────────────────────┐
│                   NP完全问题归约图                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    ┌─────────┐                              │
│                    │   SAT   │ ←──── 第一个NP完全问题       │
│                    └────┬────┘      (Cook-Levin定理)       │
│         ┌───────────────┼───────────────┐                   │
│         ▼               ▼               ▼                   │
│    ┌─────────┐     ┌─────────┐     ┌─────────┐             │
│    │  3-SAT  │────→│ 顶点覆盖 │────→│  团问题  │             │
│    └────┬────┘     └────┬────┘     └────┬────┘             │
│         │               │               │                   │
│         ▼               ▼               ▼                   │
│    ┌─────────┐     ┌─────────┐     ┌─────────┐             │
│    │ 图着色  │     │ 哈密顿  │────→│  背包   │             │
│    │ (3色)   │     │  路径   │     │  问题   │             │
│    └─────────┘     └────┬────┘     └─────────┘             │
│                         │                                   │
│                         ▼                                   │
│                    ┌─────────┐                              │
│                    │ 旅行商  │                              │
│                    │  问题   │                              │
│                    └─────────┘                              │
│                                                             │
│   箭头 A → B 表示 A 可以多项式归约到 B                      │
└─────────────────────────────────────────────────────────────┘
```

---

## 实践意义与编程启示

### 对软件开发的启示

1. **理解计算边界**
   - 有些问题本质上是不可计算的，不要浪费时间寻找通用算法
   - 例如：判断任意程序是否存在死循环是不可能的

2. **算法设计策略**
   - NP难问题：使用启发式算法、近似算法或限制问题规模
   - 不可判定问题：寻找可判定的特殊情况

3. **静态分析工具的局限性**
   - Rice定理：任何非平凡的程序语义属性都是不可判定的
   - 静态分析工具必然存在误报或漏报

### C语言中的实际考虑

```c
/*
 * 实际编程中遇到的可计算性限制
 */

// 1. 递归深度限制 - 避免无限递归
int safe_recursive_function(int n, int depth) {
    if (depth > MAX_DEPTH) {
        // 无法判定是否会停机，设置安全边界
        return ERROR_DEPTH_EXCEEDED;
    }
    if (n <= 0) return 1;
    return n * safe_recursive_function(n - 1, depth + 1);
}

// 2. 循环边界检查
bool has_loop_invariant(int timeout_ms) {
    clock_t start = clock();
    while (complex_condition()) {
        // 无法判定循环何时结束，设置超时
        if ((clock() - start) * 1000 / CLOCKS_PER_SEC > timeout_ms) {
            return false;  // 可能无限循环
        }
        // ... 处理逻辑
    }
    return true;
}

// 3. 状态机避免不可达状态
void state_machine_step(State *s) {
    switch (s->current) {
        case STATE_A:
            s->current = compute_next_a(s);
            break;
        case STATE_B:
            s->current = compute_next_b(s);
            break;
        // ... 所有状态都有处理
        default:
            // 避免未定义行为，进入安全状态
            s->current = STATE_ERROR;
            break;
    }
}
```

---

## 参考资源

- [Hopcroft, Motwani & Ullman] 《自动机理论、语言和计算》
- [Sipser] 《计算理论导引》
- [Knuth] 《计算机程序设计艺术》
- [Computational Complexity - Papadimitriou]

---

*"计算的边界，正是人类理性探索的前沿。"*
