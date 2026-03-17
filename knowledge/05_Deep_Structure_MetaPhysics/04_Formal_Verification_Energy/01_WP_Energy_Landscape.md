# Weakest Precondition能量景观

> **层级定位**: 05 Deep Structure MetaPhysics / 04 Formal Verification Energy
> **对应标准**: Dijkstra, Gries, VST
> **难度级别**: L6 创造
> **预估学习时间**: 20+ 小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 最弱前置条件、谓词转换器、Hoare逻辑对偶、验证条件生成 |
| **前置知识** | Hoare逻辑、谓词逻辑、程序语义 |
| **后续延伸** | 分离逻辑、Iris、VST |
| **权威来源** | Dijkstra《Guarded Commands》, VST, Why3 |

---


---

## 📑 目录

- [Weakest Precondition能量景观](#weakest-precondition能量景观)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [🧠 知识结构思维导图](#-知识结构思维导图)
  - [📖 核心概念详解](#-核心概念详解)
    - [1. 最弱前置条件基础](#1-最弱前置条件基础)
      - [1.1 谓词转换器语义](#11-谓词转换器语义)
      - [1.2 核心WP规则](#12-核心wp规则)
    - [2. 验证条件生成](#2-验证条件生成)
      - [2.1 后向VCG](#21-后向vcg)
      - [2.2 前向VCG（最强后条件）](#22-前向vcg最强后条件)
    - [3. 结构化程序验证](#3-结构化程序验证)
      - [3.1 循环不变式发现](#31-循环不变式发现)
      - [3.2 递归程序验证](#32-递归程序验证)
    - [4. 实际验证工具](#4-实际验证工具)
      - [4.1 Why3中的WP](#41-why3中的wp)
      - [4.2 Dafny中的WP](#42-dafny中的wp)
      - [4.3 VST中的WP](#43-vst中的wp)
  - [⚠️ 常见陷阱](#️-常见陷阱)
    - [陷阱 WP01: 循环不变式太弱](#陷阱-wp01-循环不变式太弱)
    - [陷阱 WP02: 忽略边界条件](#陷阱-wp02-忽略边界条件)
    - [陷阱 WP03: 终止性假设](#陷阱-wp03-终止性假设)
  - [✅ 质量验收清单](#-质量验收清单)
  - [📚 参考资源](#-参考资源)


---

## 🧠 知识结构思维导图

```mermaid
mindmap
  root((WP能量景观))
    谓词转换器
      wp[S](Q)
      sp[S](P)
      对偶关系
    验证条件
      后向传播
      前向传播
      VCG
    结构化验证
      循环不变式
      递归
      过程调用
    工具实现
      Dafny
      Why3
      VST
```

---

## 📖 核心概念详解

### 1. 最弱前置条件基础

#### 1.1 谓词转换器语义

```
Dijkstra的最弱前置条件（WP）是程序验证的基础方法。

定义：对于程序S和后条件Q，wp(S, Q)是最弱的前条件P，
使得从满足P的状态执行S，必定终止并满足Q。

形式化：
- 完全正确性：{wp(S, Q)} S {Q}
- wp(S, Q) = {σ | ∀σ'. ⟨S,σ⟩⇓σ' ⇒ σ'∈Q} ∧ ∀σ. ⟨S,σ⟩↓
  （所有从σ开始的执行都终止且满足Q）
```

```c
// C语言中WP计算的概念实现
// 注意：这是伪代码，展示WP算法的思想

typedef struct {
    char *expr;
} Predicate;

// 最弱前置条件计算
typedef struct {
    Predicate (*wp_skip)(Predicate Q);
    Predicate (*wp_assign)(char *var, char *expr, Predicate Q);
    Predicate (*wp_seq)(struct WP_Calculator *wp1,
                        struct WP_Calculator *wp2,
                        Predicate Q);
    Predicate (*wp_if)(Predicate B,
                       struct WP_Calculator *wp_then,
                       struct WP_Calculator *wp_else,
                       Predicate Q);
    Predicate (*wp_while)(Predicate B,
                          struct WP_Calculator *wp_body,
                          Predicate I,  // 不变式
                          Predicate Q);
} WP_Calculator;

// WP[skip](Q) = Q
Predicate wp_skip(Predicate Q) {
    return Q;
}

// WP[x := e](Q) = Q[x/e] （Q中x替换为e）
Predicate wp_assign(char *var, char *expr, Predicate Q) {
    return substitute(Q, var, expr);
}

// WP[S1; S2](Q) = WP[S1](WP[S2](Q))
Predicate wp_seq(Predicate (*wp1)(Predicate),
                 Predicate (*wp2)(Predicate),
                 Predicate Q) {
    return wp1(wp2(Q));
}

// WP[if B then S1 else S2](Q) =
//   (B ∧ WP[S1](Q)) ∨ (¬B ∧ WP[S2](Q))
// 或等价地：
//   (B ⇒ WP[S1](Q)) ∧ (¬B ⇒ WP[S2](Q))
Predicate wp_if(Predicate B,
                Predicate wp_then,
                Predicate wp_else,
                Predicate Q) {
    return and(
        implies(B, wp_then),
        implies(not(B), wp_else)
    );
}
```

#### 1.2 核心WP规则

```c
/*
 * 最弱前置条件的核心规则：
 *
 * 1. wp(skip, Q) = Q
 *
 * 2. wp(x := E, Q) = Q[x/E]
 *    （Q中所有x替换为E）
 *
 * 3. wp(S1; S2, Q) = wp(S1, wp(S2, Q))
 *
 * 4. wp(if B then S1 else S2, Q) =
 *    (B ⇒ wp(S1, Q)) ∧ (¬B ⇒ wp(S2, Q))
 *
 * 5. wp(while B do S, Q) =
 *    ∃k≥0. I_k
 *    其中：
 *    I_0 = ¬B ∧ Q
 *    I_{k+1} = B ∧ wp(S, I_k)
 *
 *    实际使用循环不变式I：
 *    I ∧ (I ∧ B ⇒ wp(S, I)) ∧ (I ∧ ¬B ⇒ Q)
 */

// 示例：计算简单程序的WP
/*
程序：
  if x > 0 then
    y := x
  else
    y := -x

后条件Q: y ≥ 0

WP计算：
  WP = (x > 0 ⇒ WP[y:=x](y ≥ 0)) ∧ (x ≤ 0 ⇒ WP[y:=-x](y ≥ 0))
     = (x > 0 ⇒ x ≥ 0) ∧ (x ≤ 0 ⇒ -x ≥ 0)
     = (x > 0 ⇒ x ≥ 0) ∧ (x ≤ 0 ⇒ x ≤ 0)
     = true ∧ true
     = true

结论：程序总是满足y ≥ 0（绝对值计算）
*/
```

### 2. 验证条件生成

#### 2.1 后向VCG

```c
// 验证条件生成器（后向）
// 输入：程序S和后条件Q
// 输出：验证条件集合

typedef struct {
    Predicate precondition;
    GArray *verification_conditions;  // 需要证明的引理
} VCG_Result;

// 后向VCG实现
VCG_Result *vcg_backward(Stmt *S, Predicate Q) {
    VCG_Result *result = malloc(sizeof(VCG_Result));
    result->verification_conditions = g_array_new(FALSE, FALSE, sizeof(Predicate));

    switch (S->type) {
        case STMT_SKIP:
            result->precondition = Q;
            break;

        case STMT_ASSIGN:
            result->precondition = substitute(Q, S->assign.var, S->assign.expr);
            break;

        case STMT_SEQ:
            // 先处理S2，再处理S1
            VCG_Result *r2 = vcg_backward(S->seq.s2, Q);
            VCG_Result *r1 = vcg_backward(S->seq.s1, r2->precondition);

            result->precondition = r1->precondition;
            g_array_append_vals(result->verification_conditions,
                               r2->verification_conditions->data,
                               r2->verification_conditions->len);
            g_array_append_vals(result->verification_conditions,
                               r1->verification_conditions->data,
                               r1->verification_conditions->len);
            break;

        case STMT_IF:
            VCG_Result *r_then = vcg_backward(S->if_stmt.then_branch, Q);
            VCG_Result *r_else = vcg_backward(S->if_stmt.else_branch, Q);

            // (B ∧ WP[then]) ∨ (¬B ∧ WP[else])
            // 生成条件：B ⇒ WP_then 和 ¬B ⇒ WP_else
            result->precondition = and(
                implies(S->if_stmt.condition, r_then->precondition),
                implies(not(S->if_stmt.condition), r_else->precondition)
            );
            break;

        case STMT_WHILE: {
            // 需要用户提供循环不变式
            Predicate I = S->while_stmt.invariant;

            // 验证条件1：不变式初始成立
            // 这是检查点，不是WP的一部分
            Predicate vc_init = implies(result->precondition, I);
            g_array_append_val(result->verification_conditions, vc_init);

            // 验证条件2：不变式保持
            VCG_Result *r_body = vcg_backward(S->while_stmt.body, I);
            Predicate vc_preserve = implies(
                and(I, S->while_stmt.condition),
                r_body->precondition
            );
            g_array_append_val(result->verification_conditions, vc_preserve);

            // 验证条件3：退出时满足后条件
            Predicate vc_exit = implies(
                and(I, not(S->while_stmt.condition)),
                Q
            );
            g_array_append_val(result->verification_conditions, vc_exit);

            // WP = I（进入循环前需要满足不变式）
            result->precondition = I;
            break;
        }
    }

    return result;
}
```

#### 2.2 前向VCG（最强后条件）

```c
/*
 * 最强后条件（Strongest Postcondition, SP）
 * sp(S, P) = 从满足P的状态执行S后必然满足的条件
 *
 * SP和WP是Galois连接：
 *   sp(S, P) ⊆ Q  ⟺  P ⊆ wp(S, Q)
 */

// 前向VCG：从前提推导后置条件
Predicate sp_forward(Stmt *S, Predicate P) {
    switch (S->type) {
        case STMT_SKIP:
            return P;

        case STMT_ASSIGN:
            // sp(x := E, P) = ∃v. x = E[v/x] ∧ P[v/x]
            // 即：存在原值v使得x等于E（用v替换x后的值）且P对v成立
            return exists("v",
                and(
                    equals(S->assign.var,
                          substitute(S->assign.expr, S->assign.var, "v")),
                    substitute(P, S->assign.var, "v")
                ));

        case STMT_SEQ:
            // sp(S1; S2, P) = sp(S2, sp(S1, P))
            return sp_forward(S->seq.s2, sp_forward(S->seq.s1, P));

        case STMT_IF:
            // sp(if B then S1 else S2, P) =
            //   sp(S1, P ∧ B) ∨ sp(S2, P ∧ ¬B)
            return or(
                sp_forward(S->if_stmt.then_branch,
                          and(P, S->if_stmt.condition)),
                sp_forward(S->if_stmt.else_branch,
                          and(P, not(S->if_stmt.condition)))
            );

        case STMT_WHILE:
            // 前向SP对循环较难处理，通常使用不变式
            // sp(while B do S, P) = I ∧ ¬B
            // 其中I是满足P ⇒ I且{I ∧ B} S {I}的不变式
            return and(S->while_stmt.invariant,
                      not(S->while_stmt.condition));
    }
    return P;
}

// 前向验证示例
/*
程序：
  x := 0;
  y := 0;
  while x < n do
    x := x + 1;
    y := y + x

前提：n ≥ 0
目标：验证后条件 y = n*(n+1)/2

SP计算：
  sp(x := 0, n ≥ 0) = n ≥ 0 ∧ x = 0
  sp(y := 0, n ≥ 0 ∧ x = 0) = n ≥ 0 ∧ x = 0 ∧ y = 0

循环不变式：y = x*(x+1)/2 ∧ x ≤ n

退出时：y = x*(x+1)/2 ∧ x ≤ n ∧ x ≥ n
        = y = n*(n+1)/2 ∧ x = n

证毕。
*/
```

### 3. 结构化程序验证

#### 3.1 循环不变式发现

```c
/*
 * 循环不变式发现策略：
 *
 * 1. 从后条件推导
 *    - 将循环变量替换为迭代计数
 *
 * 2. 从循环体分析
 *    - 追踪变量如何变化
 *    - 寻找保持的关系
 *
 * 3. 常见模式
 *    - 累积和：sum = Σ values
 *    - 计数器：i 与数组索引相关
 *    - 边界：low ≤ idx ≤ high
 */

// 示例：数组求和
/*
程序：
  sum := 0;
  i := 0;
  while i < n do
    sum := sum + a[i];
    i := i + 1

后条件：sum = Σ_{k=0}^{n-1} a[k]

不变式推导：
- 观察循环行为：每次迭代添加a[i]到sum
- 猜测：sum = Σ_{k=0}^{i-1} a[k] ∧ 0 ≤ i ≤ n
- 验证：
  * 初始化前：sum=0, i=0 → sum=空和=0 ✓
  * 保持性：假设成立，执行循环体后
    sum' = sum + a[i] = Σ_{k=0}^{i-1} a[k] + a[i] = Σ_{k=0}^{i} a[k]
    i' = i + 1
    所以 sum' = Σ_{k=0}^{i'-1} a[k] ✓
  * 退出时：i ≥ n 且 i ≤ n → i = n
    sum = Σ_{k=0}^{n-1} a[k] ✓
*/

// 不变式发现辅助函数
void discover_invariant(LoopStmt *loop) {
    printf("分析循环不变式...\n");

    // 1. 收集修改的变量
    GHashTable *modified = collect_modified_vars(loop->body);

    // 2. 追踪每个变量的更新模式
    GList *vars = g_hash_table_get_keys(modified);
    for (GList *l = vars; l != NULL; l = l->next) {
        char *var = l->data;
        UpdatePattern pattern = analyze_update_pattern(loop->body, var);

        switch (pattern.type) {
            case UPDATE_INCREMENT:
                // i := i + c 模式
                printf("  %s 是递增变量\n", var);
                break;
            case UPDATE_ACCUMULATE:
                // sum := sum + expr 模式
                printf("  %s 是累加变量\n", var);
                break;
            case UPDATE_MULTIPLY:
                // product := product * expr 模式
                printf("  %s 是累积乘积\n", var);
                break;
        }
    }

    // 3. 生成候选不变式
    GArray *candidates = generate_invariant_candidates(loop);

    // 4. 验证候选
    for (int i = 0; i < candidates->len; i++) {
        Predicate cand = g_array_index(candidates, Predicate, i);
        if (verify_invariant(loop, cand)) {
            printf("  找到不变式: ");
            print_predicate(cand);
        }
    }
}
```

#### 3.2 递归程序验证

```c
/*
 * 递归程序的WP计算：
 *
 * 对于递归过程P，其规范为 {pre} P {post}
 * 需要证明：
 * 1. 基本情况满足规范
 * 2. 归纳步骤：假设递归调用满足规范，证明当前调用满足规范
 */

// 阶乘的递归实现验证
/*
程序：
  int factorial(int n) {
    if (n <= 0) return 1;
    else return n * factorial(n - 1);
  }

规范：{n ≥ 0} factorial {return = n!}

证明：
- 基本情况：n ≤ 0
  返回1 = 0! = 1 ✓（假设n是非负整数）

- 归纳步骤：n > 0
  归纳假设：factorial(n-1)返回(n-1)!
  返回值：n * factorial(n-1) = n * (n-1)! = n! ✓

终止性：n每次递归减1，最终到达n ≤ 0
*/

// 递归验证的一般框架
typedef struct {
    Predicate precondition;
    Predicate postcondition;
    GHashTable *measure;  // 终止度量函数
} ProcedureSpec;

bool verify_recursive(Procedure *proc, ProcedureSpec *spec) {
    // 1. 验证基本情况
    Stmt *base_case = get_base_case(proc);
    if (!verify(base_case, spec->precondition, spec->postcondition)) {
        return false;
    }

    // 2. 验证归纳步骤
    Stmt *rec_call = get_recursive_call(proc);

    // 假设递归调用满足规范
    assume_holds(spec, rec_call);

    // 验证当前调用满足规范
    if (!verify_inductive_step(proc, spec)) {
        return false;
    }

    // 3. 验证终止性
    if (!verify_termination(proc, spec->measure)) {
        return false;
    }

    return true;
}
```

### 4. 实际验证工具

#### 4.1 Why3中的WP

```c
/*
 * Why3是用于程序验证的平台，内部使用WP计算。
 *
 * WhyML示例：
 */

// Why3代码（ML语法）
/*
module Factorial
  use int.Int
  use int.Fact

  let rec factorial (n: int) : int
    requires { n >= 0 }
    ensures  { result = fact n }
    variant  { n }
  = if n <= 0 then 1 else n * factorial (n - 1)

  let factorial_loop (n: int) : int
    requires { n >= 0 }
    ensures  { result = fact n }
  = let ref acc = 1 in
    let ref i = 0 in
    while i < n do
      invariant { acc = fact i /\ 0 <= i <= n }
      variant   { n - i }
      i := i + 1;
      acc := acc * i
    done;
    acc
end
*/

// C代码的Why3验证流程
void verify_with_why3(void) {
    /*
     * 1. 将C代码转换为WhyML（使用Frama-C或手动）
     * 2. Why3计算WP生成验证条件
     * 3. 使用SMT求解器（Alt-Ergo, Z3, CVC4）证明VC
     * 4. 报告未证明的条件
     */
}
```

#### 4.2 Dafny中的WP

```c
/*
 * Dafny集成了WP计算和自动验证。
 */

// Dafny代码示例
/*
method Max(a: int, b: int) returns (m: int)
  ensures m >= a && m >= b
  ensures m == a || m == b
{
  if a > b {
    m := a;
  } else {
    m := b;
  }
}

method FindMax(arr: array<int>) returns (max: int)
  requires arr.Length > 0
  ensures forall i :: 0 <= i < arr.Length ==> max >= arr[i]
  ensures exists i :: 0 <= i < arr.Length && max == arr[i]
{
  max := arr[0];
  var i := 1;
  while i < arr.Length
    invariant 1 <= i <= arr.Length
    invariant forall k :: 0 <= k < i ==> max >= arr[k]
    invariant exists k :: 0 <= k < i && max == arr[k]
  {
    if arr[i] > max {
      max := arr[i];
    }
    i := i + 1;
  }
}
*/

// Dafny的WP计算特点
/*
 * 1. 自动生成循环不变式（部分支持）
 * 2. 使用Z3作为后端求解器
 * 3. 支持高阶函数和泛型
 * 4. 提取到C#, Java, JavaScript
 */
```

#### 4.3 VST中的WP

```c
/*
 * VST（Verified Software Toolchain）是用于C程序的Coq框架。
 * 使用分离逻辑而非纯Hoare逻辑。
 */

// VST的断言示例（Coq语法）
/*
Definition swap_spec : ident * funspec :=
  DECLARE _swap
  WITH x: val, y: val, sh : share, contents_x : Z, contents_y : Z
  PRE [ _x OF (tptr tint), _y OF (tptr tint) ]
    PROP (writable_share sh)
    LOCAL (temp _x x; temp _y y)
    SEP (data_at sh tint (Vint (Int.repr contents_x)) x;
         data_at sh tint (Vint (Int.repr contents_y)) y)
  POST [ tvoid ]
    PROP ()
    LOCAL ()
    SEP (data_at sh tint (Vint (Int.repr contents_y)) x;
         data_at sh tint (Vint (Int.repr contents_x)) y).
*/

// VST验证流程
void vst_verification_flow(void) {
    /*
     * 1. 在Coq中定义函数规范（前置/后置条件）
     * 2. 使用VST的forward策略逐步验证
     * 3. VST自动计算WP
     * 4. 使用Coq证明剩余条件
     * 5. 提取可执行代码
     */
}
```

---

## ⚠️ 常见陷阱

### 陷阱 WP01: 循环不变式太弱

```c
// 错误：不变式不足以推导出后条件
/*
程序：
  s := 0;
  i := 0;
  while i < n do
    s := s + i;
    i := i + 1

错误不变式：i ≤ n  （太弱！）

退出时：i ≥ n ∧ i ≤ n → i = n
但无法确定s的值

正确不变式：s = Σ_{k=0}^{i-1} k ∧ i ≤ n
退出时：s = Σ_{k=0}^{n-1} k = n*(n-1)/2
*/
```

### 陷阱 WP02: 忽略边界条件

```c
// 错误：未处理整数溢出
/*
程序：
  { true }
  x := max_int;
  x := x + 1;
  { x > 0 }  // 错误！溢出后x为负

WP计算假设数学整数，实际需要考虑机器整数语义
*/

// 修正：添加无溢出条件
/*
{ x <= max_int - 1 }
x := x + 1
{ x <= max_int }
*/
```

### 陷阱 WP03: 终止性假设

```c
// 警告：WP通常假设程序终止
// 对于非终止程序，WP计算可能不可靠

/*
程序：
  while true do skip

WP(skip, Q) = Q
但这是循环内的WP，不是整个程序的WP

整个程序无定义（不终止），但WP计算可能给出错误结果
*/

// 修正：使用完全正确性
/*
wp_total(while B do S, Q) =
  存在秩函数f使得...
*/
```

---

## ✅ 质量验收清单

- [x] 最弱前置条件定义
- [x] 基本WP规则（skip, assign, seq, if）
- [x] 循环WP规则（使用不变式）
- [x] 验证条件生成（VCG）
- [x] 后向传播算法
- [x] 最强后条件（SP）
- [x] 循环不变式发现策略
- [x] 递归程序验证
- [x] 实际工具使用（Why3, Dafny, VST）
- [x] Mermaid思维导图
- [x] 常见陷阱与解决方案

---

## 📚 参考资源

| 资源 | 作者/来源 | 说明 |
|:-----|:----------|:-----|
| Guarded Commands | Dijkstra | WP奠基论文 |
| A Discipline of Programming | Dijkstra | 经典教材 |
| The Science of Programming | Gries | 系统介绍 |
| Why3 Platform | Inria | 实践工具 |
| Dafny | Microsoft | 实用验证语言 |
| VST | Princeton | C程序验证 |

---

> **更新记录**
>
> - 2025-03-09: 初版创建，包含WP能量景观完整理论
