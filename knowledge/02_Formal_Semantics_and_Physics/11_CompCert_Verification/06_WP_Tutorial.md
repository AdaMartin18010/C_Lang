# WP 演绎验证插件深度教程

> **文档版本**: 基于2024年《Guide to Software Verification with Frama-C》第4章
> **Frama-C版本**: 30.0 Zinc / 31.0 Gallium / 32.0 Germanium
> **更新日期**: 2026-03-19
> **难度等级**: ⭐⭐⭐⭐ (高级)

---

## 📋 目录

- [WP 演绎验证插件深度教程](#wp-演绎验证插件深度教程)
  - [📋 目录](#-目录)
  - [一、WP插件概述](#一wp插件概述)
    - [1.1 演绎验证理论基础](#11-演绎验证理论基础)
    - [1.2 Weakest Precondition演算](#12-weakest-precondition演算)
    - [1.3 WP架构与特性](#13-wp架构与特性)
  - [二、ACSL规范语言详解](#二acsl规范语言详解)
    - [2.1 谓词逻辑基础](#21-谓词逻辑基础)
    - [2.2 规范注解完整语法](#22-规范注解完整语法)
    - [2.3 高级规范构造](#23-高级规范构造)
  - [三、Hoare逻辑基础](#三hoare逻辑基础)
    - [3.1 Hoare三元组](#31-hoare三元组)
    - [3.2 推理规则](#32-推理规则)
    - [3.3 最强后条件与最弱前条件](#33-最强后条件与最弱前条件)
  - [四、WP基本使用](#四wp基本使用)
    - [4.1 命令行基础](#41-命令行基础)
    - [4.2 第一个验证示例](#42-第一个验证示例)
    - [4.3 结果解读](#43-结果解读)
  - [五、自动化证明策略](#五自动化证明策略)
    - [5.1 内置自动证明器](#51-内置自动证明器)
    - [5.2 外部SMT求解器](#52-外部smt求解器)
    - [5.3 证明策略调优](#53-证明策略调优)
  - [六、手动证明指导](#六手动证明指导)
    - [6.1 Coq交互式证明](#61-coq交互式证明)
    - [6.2 证明脚本生成](#62-证明脚本生成)
    - [6.3 常见证明模式](#63-常见证明模式)
  - [七、完整验证案例](#七完整验证案例)
    - [案例1: 二分查找](#案例1-二分查找)
    - [案例2: 快速排序分区](#案例2-快速排序分区)
    - [案例3: 链表反转](#案例3-链表反转)
  - [八、常见问题与解答](#八常见问题与解答)
  - [九、参考资源](#九参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)
  - [📚 实质性内容补充](#-实质性内容补充)
    - [技术深度分析](#技术深度分析)
      - [1. 核心概念详解](#1-核心概念详解)
      - [2. 实现机制](#2-实现机制)
      - [3. 实践指导](#3-实践指导)
    - [扩展阅读](#扩展阅读)

---

## 一、WP插件概述

### 1.1 演绎验证理论基础

**演绎验证** (Deductive Verification) 使用数学逻辑证明程序满足其规范。WP插件基于Hoare逻辑和Weakest Precondition演算。

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    演绎验证流程                                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────┐    WP演算      ┌──────────────┐    自动证明    ┌─────────┐ │
│  │  C代码+ACSL │ ─────────────▶ │  验证条件VC  │ ────────────▶ │ 证明结果 │ │
│  │   规范      │   最弱前条件   │  (逻辑公式)   │  SMT求解器    │ Valid/  │ │
│  └─────────────┘                └──────────────┘               │ Unknown │ │
│                                                                  └─────────┘ │
│                                                                              │
│  关键性质:                                                                   │
│  • 可靠性 (Soundness): 证明通过 ⇒ 程序正确                                  │
│  • 相对完备 (Relative Complete): 如果程序正确且ACSL充分,则可证明              │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Weakest Precondition演算

**最弱前条件** (Weakest Precondition, wp) 定义:

```
给定语句 S 和后条件 Q,
wp(S, Q) = 满足"执行S后Q成立"的最弱前提条件

形式化:  ∀σ. σ ⊨ wp(S, Q)  ⟹  ⟦S⟧(σ) ⊨ Q

核心规则:
──────────
wp(skip, Q) = Q

wp(x := E, Q) = Q[x/E]  (替换)

wp(S1; S2, Q) = wp(S1, wp(S2, Q))

wp(if B then S1 else S2, Q) =
  (B ⇒ wp(S1, Q)) ∧ (¬B ⇒ wp(S2, Q))

wp(while B do S, Q) =
  I ∧ (∀x. (I ∧ B ⇒ wp(S, I)) ∧ (I ∧ ¬B ⇒ Q))
  (需要循环不变量I)
```

**Frama-C WP的扩展:**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    WP内存模型                                                │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  WP支持三种内存模型:                                                          │
│                                                                              │
│  1. Hoare模型 (默认)                                                         │
│     • 最简单, 仅适用于无指针别名的程序                                         │
│     • 命令: -wp-model Hoare                                                  │
│                                                                              │
│  2. Typed模型                                                                │
│     • 基于类型, 处理有限别名                                                  │
│     • 命令: -wp-model Typed                                                  │
│                                                                              │
│  3. Separation模型 (最精确)                                                   │
│     • 基于分离逻辑, 完整处理指针和别名                                         │
│     • 命令: -wp-model Separation                                             │
│                                                                              │
│  选择指南:                                                                   │
│  • 无指针: Hoare                                                             │
│  • 有限指针: Typed                                                           │
│  • 复杂数据结构: Separation                                                  │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.3 WP架构与特性

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      WP 30.0+ 架构                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                    WP核心引擎                                        │   │
│  │  • VC生成 (Hoare逻辑应用)                                            │   │
│  │  • 内存模型处理                                                      │   │
│  │  • Qed归约引擎 (公式简化)                                            │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                              │                                               │
│         ┌────────────────────┼────────────────────┐                          │
│         ▼                    ▼                    ▼                          │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐                 │
│  │  内置证明器   │    │  Why3接口    │    │  Coq接口     │                 │
│  │              │    │              │    │              │                 │
│  │ • Qed        │    │ • Alt-Ergo   │    │ • 交互式证明  │                 │
│  │ • Simplify   │    │ • Z3         │    │ • 证明脚本   │                 │
│  │ • Auto       │    │ • CVC4/5     │    │              │                 │
│  └──────────────┘    └──────────────┘    └──────────────┘                 │
│                                                                              │
│  新特性 (30.0+):                                                             │
│  • 改进的Qed归约算法                                                         │
│  • 增强的ACSL++支持                                                          │
│  • 新的内存模型选项                                                          │
│  • 更好的循环处理                                                            │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 二、ACSL规范语言详解

### 2.1 谓词逻辑基础

```c
/*@
  ┌─────────────────────────────────────────────────────────────────────┐
  │                    ACSL 谓词逻辑                                      │
  ├─────────────────────────────────────────────────────────────────────┤
  │                                                                      │
  │  量词 (Quantifiers):                                                  │
  │  ─────────────────────────────────────────                          │
  │  \forall type x; P(x)      // 全称量词                              │
  │  \exists type x; P(x)      // 存在量词                              │
  │                                                                      │
  │  逻辑连接词:                                                          │
  │  ─────────────────────────────────────────                          │
  │  ==>     蕴含 (implication)                                          │
  │  <==>    等价 (equivalence)                                          │
  │  &&      逻辑与                                                      │
  │  ||      逻辑或                                                      │
  │  !       逻辑非                                                      │
  │                                                                      │
  │  特殊运算符:                                                          │
  │  ─────────────────────────────────────────                          │
  │  ? :     条件表达式                                                   │
  │  \let x = E; P       局部绑定                                         │
  │  \old(E)             函数入口时的值                                   │
  │  \at(E, Label)       特定标签处的值                                   │
  │                                                                      │
  └─────────────────────────────────────────────────────────────────────┘
*/
```

### 2.2 规范注解完整语法

**完整函数合约示例:**

```c
/*@
  ┌─────────────────────────────────────────────────────────────────────┐
  │                    完整函数合约模板                                   │
  ├─────────────────────────────────────────────────────────────────────┤
  │  // 前置条件 (调用者必须保证)                                         │
  │  @requires Precondition1;                                             │
  │  @requires Precondition2;                                             │
  │                                                                        │
  │  // 内存修改声明                                                      │
  │  @assigns MemoryLocation1, MemoryLocation2;                           │
  │  @assigns \nothing;  // 不修改任何内存                                 │
  │                                                                        │
  │  // 分配/释放声明                                                     │
  │  @allocates MemoryLocation;                                           │
  │  @frees MemoryLocation;                                               │
  │                                                                        │
  │  // 后置条件 (函数返回时必须成立)                                     │
  │  @ensures Postcondition1;                                             │
  │  @ensures Postcondition2;                                             │
  │                                                                        │
  │  // 异常行为 (当函数可能不返回时)                                     │
  │  @exits ExitCondition;                                                │
  │                                                                        │
  │  // 命名行为分支                                                      │
  │  @behavior Name1:                                                     │
  │    @assumes Condition1;  // 此行为的假设                               │
  │    @assigns ...;                                                       │
  │    @ensures ...;                                                       │
  │                                                                        │
  │  @behavior Name2:                                                     │
  │    @assumes Condition2;                                                │
  │    ...                                                                 │
  │                                                                        │
  │  // 行为完整性                                                        │
  │  @complete behaviors Name1, Name2;                                    │
  │                                                                        │
  │  // 行为互斥性                                                        │
  │  @disjoint behaviors Name1, Name2;                                    │
  └─────────────────────────────────────────────────────────────────────┘
*/
```

**实例:**

```c
/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires \valid(\result);
  @assigns \nothing;

  // 后置条件
  @ensures \result >= 0;
  @ensures \forall integer i; 0 <= i < n ==> \result >= arr[i];
  @ensures \exists integer i; 0 <= i < n && \result == arr[i];

  // 行为分支
  @behavior empty:
    @assumes n == 0;
    @ensures \result == 0;

  @behavior non_empty:
    @assumes n > 0;
    @ensures \exists integer i; 0 <= i < n && \result == arr[i];

  @complete behaviors empty, non_empty;
  @disjoint behaviors empty, non_empty;
*/
int find_max(const int* arr, int n);
```

### 2.3 高级规范构造

**逻辑谓词和函数:**

```c
/*@
  // 定义谓词
  @predicate is_sorted(int* a, integer n) =
    \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];

  // 定义逻辑函数
  @logic integer sum(int* a, integer n) reads a[0..n-1];

  // 函数公理化
  @axiom sum_empty:
    \forall int* a; sum(a, 0) == 0;

  @axiom sum_step:
    \forall int* a, integer n; n > 0 ==>
      sum(a, n) == sum(a, n-1) + a[n-1];

  // 引理 (需要证明)
  @lemma sum_nonneg:
    \forall int* a, integer n;
      n >= 0 ==> (\forall integer i; 0 <= i < n ==> a[i] >= 0) ==>
      sum(a, n) >= 0;
*/

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires \forall integer i; 0 <= i < n ==> arr[i] >= 0;
  @assigns \nothing;
  @ensures \result == sum(arr, n);
  @ensures \result >= 0;
*/
int sum_positive(const int* arr, int n);
```

**内存规范:**

```c
/*@
  // 指针有效性
  @predicate is_valid_array(int* a, integer n) =
    n >= 0 ==> \valid(a + (0..n-1));

  @predicate is_valid_read_array(int* a, integer n) =
    n >= 0 ==> \valid_read(a + (0..n-1));

  // 数组分离
  @predicate arrays_disjoint(int* a, int* b, integer n, integer m) =
    \separated(a + (0..n-1), b + (0..m-1));

  // 数组内容相等
  @predicate array_equal(int* a, int* b, integer n) =
    \forall integer i; 0 <= i < n ==> a[i] == b[i];
*/

/*@
  @requires is_valid_array(dst, n);
  @requires is_valid_read_array(src, n);
  @requires arrays_disjoint(dst, src, n, n);
  @assigns dst[0..n-1];
  @ensures array_equal(dst, \old(src), n);
  @ensures \result == dst;
*/
int* array_copy(int* dst, const int* src, size_t n);
```

**幽灵代码:**

```c
/*@
  @ghost typedef struct {
    integer visited;
    integer comparisons;
  } SearchStats;
*/

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures 0 <= \result <= n;
*/
size_t linear_search(const int* arr, size_t n, int key) {
    /*@ ghost SearchStats stats = {0, 0}; */

    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant stats.visited == i;
      @loop invariant stats.comparisons == i;
      @loop assigns i, stats;
      @loop variant n - i;
    */
    for (size_t i = 0; i < n; i++) {
        /*@ ghost stats.visited++; */
        /*@ ghost stats.comparisons++; */

        if (arr[i] == key) {
            return i;
        }
    }

    /*@ assert stats.visited == n; */
    return n;
}
```

---

## 三、Hoare逻辑基础

### 3.1 Hoare三元组

**基本形式:**

```
{P} S {Q}

含义: 如果在执行语句S之前前置条件P成立，
      那么执行S之后后置条件Q成立

示例:
{x >= 0} x := x + 1 {x >= 1}
```

### 3.2 推理规则

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      Hoare逻辑推理规则                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  赋值规则:                                                                   │
│  ─────────────────────────                                                  │
│  {Q[x/E]} x := E {Q}                                                        │
│                                                                              │
│  顺序规则:                                                                   │
│  {P} S1 {R}    {R} S2 {Q}                                                   │
│  ─────────────────────────                                                  │
│       {P} S1; S2 {Q}                                                        │
│                                                                              │
│  条件规则:                                                                   │
│  {P ∧ B} S1 {Q}    {P ∧ ¬B} S2 {Q}                                          │
│  ────────────────────────────────────                                       │
│     {P} if B then S1 else S2 {Q}                                            │
│                                                                              │
│  循环规则:                                                                   │
│         {I ∧ B} S {I}                                                       │
│  ──────────────────────────────                                             │
│  {I} while B do S {I ∧ ¬B}                                                  │
│                                                                              │
│  强化前件/弱化后件:                                                           │
│  P' ⇒ P    {P} S {Q}    Q ⇒ Q'                                              │
│  ─────────────────────────────                                              │
│         {P'} S {Q'}                                                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.3 最强后条件与最弱前条件

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    最弱前条件计算示例                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  示例1: 简单赋值                                                              │
│  wp(x := x + 1, x >= 0) = (x + 1 >= 0) = (x >= -1)                          │
│                                                                              │
│  示例2: 条件语句                                                              │
│  wp(if x > 0 then y := x else y := -x, y >= 0)                              │
│  = (x > 0 ⇒ wp(y := x, y >= 0)) ∧ (x ≤ 0 ⇒ wp(y := -x, y >= 0))            │
│  = (x > 0 ⇒ x >= 0) ∧ (x ≤ 0 ⇒ -x >= 0)                                    │
│  = (x > 0 ⇒ x >= 0) ∧ (x ≤ 0 ⇒ x <= 0)                                     │
│  = true  (恒真!)                                                            │
│                                                                              │
│  示例3: 循环 (需要不变量)                                                    │
│  给定: while i < n do sum := sum + i; i := i + 1                            │
│  不变量I: 0 <= i <= n ∧ sum == old_sum + i*(i-1)/2                          │
│                                                                              │
│  wp = I ∧ (I ∧ ¬(i < n) ⇒ Q)                                               │
│     ∧ (I ∧ (i < n) ⇒ wp(sum := sum + i; i := i + 1, I))                    │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 四、WP基本使用

### 4.1 命令行基础

```bash
# 基本语法
frama-c -wp [选项] 源文件...

# 常用选项
────────────────────────────────────────────────────────────────────────────
选项                          说明                              示例
────────────────────────────────────────────────────────────────────────────
-wp                          启用WP插件 (必需)
-wp-rte                      生成运行时错误检查
-wp-prover <list>            指定证明器列表                        -wp-prover alt-ergo,z3
-wp-model <model>            选择内存模型                          -wp-model Separation
-wp-fct <list>               只验证指定函数                        -wp-fct func1,func2
-wp-script                   生成证明脚本
-wp-coq                     生成Coq证明脚本
-wp-split                    拆分验证目标
-wp-auto                     自动简化
-wp-status                   显示证明状态
-save/-load                 保存/加载分析状态
-report-json <file>         JSON格式报告
────────────────────────────────────────────────────────────────────────────
```

### 4.2 第一个验证示例

**示例程序:**

```c
// wp_tutorial1.c

/*@
  @requires \valid(p);
  @requires *p >= 0;
  @assigns *p;
  @ensures *p == \old(*p) + 1;
  @ensures *p > 0;
*/
void increment(int* p) {
    *p = *p + 1;
}

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures \result >= 0;
  @ensures \forall integer i; 0 <= i < n ==> \result >= arr[i];
  @ensures \exists integer i; 0 <= i < n && \result == arr[i];
*/
int array_max(const int* arr, int n) {
    if (n == 0) return 0;

    int max = arr[0];
    /*@
      @loop invariant 1 <= i <= n;
      @loop invariant \forall integer j; 0 <= j < i ==> max >= arr[j];
      @loop invariant \exists integer j; 0 <= j < i && max == arr[j];
      @loop assigns i, max;
      @loop variant n - i;
    */
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}
```

**运行验证:**

```bash
# 基本验证
frama-c -wp wp_tutorial1.c

# 带运行时错误检查
frama-c -wp -wp-rte wp_tutorial1.c

# 使用多个证明器
frama-c -wp -wp-rte -wp-prover alt-ergo,z3 wp_tutorial1.c

# 生成详细报告
frama-c -wp -wp-rte -report-json wp_report.json wp_tutorial1.c
```

### 4.3 结果解读

**典型输出:**

```
[kernel] Parsing wp_tutorial1.c (with preprocessing)
[wp] Collecting axiomatic usage
[wp] warning: Missing RTE guards
n[wp] 14 goals scheduled
[wp] [Qed] Goal typed_increment_post : Valid (3ms)
[wp] [Alt-Ergo] Goal typed_increment_post_2 : Valid (5ms)
[wp] [Qed] Goal typed_array_max_post : Valid (2ms)
[wp] [Alt-Ergo] Goal typed_array_max_loop_inv : Valid (15ms)
...
[wp] Proved goals: 14 / 14
     Qed:             8  (3ms-5ms)
     Alt-Ergo:        6  (5ms-25ms)
```

**解读:**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    证明结果状态                                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  Valid       - 证明成功 ✓                                                    │
│  Invalid     - 找到反例 ✗ (程序或规范有误)                                   │
│  Unknown     - 证明器无法决定 ? (可能需要辅助)                               │
│  Timeout     - 超时 (可能需要简化或换证明器)                                  │
│  Failure     - 证明器错误                                                    │
│                                                                              │
│  时间解读:                                                                   │
│  • Qed < 10ms    : 公式简化即完成证明                                        │
│  • < 100ms       : 快速自动证明                                              │
│  • 1-10s         : 复杂证明                                                  │
│  • > 30s/Timeout : 可能需要人工干预                                          │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 五、自动化证明策略

### 5.1 内置自动证明器

```bash
# 内置证明器 (最快,但能力有限)
frama-c -wp -wp-prover qed program.c

# 组合使用: 先Qed快速简化,再SMT求解
frama-c -wp -wp-prover qed,alt-ergo,z3 program.c

# Qed策略选项
frama-c -wp -wp Qed:program.c  # 仅用Qed
```

### 5.2 外部SMT求解器

```bash
# Alt-Ergo (Frama-C首选)
frama-c -wp -wp-prover alt-ergo program.c

# Z3 (Microsoft, 能力强)
frama-c -wp -wp-prover z3 program.c

# CVC4/CVC5
frama-c -wp -wp-prover cvc4 program.c

# 多求解器并行
frama-c -wp -wp-prover alt-ergo,z3,cvc4 program.c
```

**求解器选择指南:**

| 求解器 | 速度 | 能力 | 适用场景 |
|:-------|:-----|:-----|:---------|
| Qed | 最快 | 弱 | 简单公式简化 |
| Alt-Ergo | 快 | 中 | 一般算术,数组 |
| Z3 | 中 | 强 | 复杂位运算,非线性 |
| CVC4/5 | 中 | 强 | 复杂逻辑,量化 |
| Coq | 慢 | 最强 | 交互式,复杂证明 |

### 5.3 证明策略调优

```bash
# 公式拆分
frama-c -wp -wp-split program.c

# 自动策略
frama-c -wp -wp-auto program.c

# 选择内存模型
frama-c -wp -wp-model Hoare program.c       # 最快,无指针
frama-c -wp -wp-model Typed program.c       # 平衡
frama-c -wp -wp-model Separation program.c  # 最精确

# 并行证明
frama-c -wp -wp-par 4 program.c
```

---

## 六、手动证明指导

### 6.1 Coq交互式证明

```bash
# 生成Coq证明脚本
frama-c -wp -wp-prover coq -wp-script program.c

# 输出文件:
# - program_wp_SOMEIDENT_spark.v  (Spark子集)
# - program_wp_SOMEIDENT_full.v   (完整)

# 使用Coq证明
coqide program_wp_*.v &
```

**Coq证明脚本结构:**

```coq
(* 生成的Coq脚本示例 *)
Require Import ZArith.
Require Import Reals.

Goal
  forall (x : Z),
  x >= 0 -> x + 1 > 0.
Proof.
  intros.
  omega.  (* 自动整数算术 *)
Qed.
```

### 6.2 证明脚本生成

```bash
# 生成Why3脚本
frama-c -wp -wp-prover why3 -wp-script program.c

# 使用Why3 IDE
why3 ide program_wp_*.why
```

### 6.3 常见证明模式

**算术证明:**

```c
// 非负性证明
/*@
  @requires x >= 0 && y >= 0;
  @ensures \result >= 0;
*/
int sum_nonneg(int x, int y) {
    return x + y;
}
// 证明策略: Alt-Ergo自动完成
```

**数组性质:**

```c
// 排序后性质
/*@
  @requires n >= 0;
  @requires \valid(a + (0..n-1));
  @assigns a[0..n-1];
  @ensures is_sorted(a, n);
  @ensures permutation{Pre,Here}(a, n);  // 是原数组的排列
*/
void sort(int* a, int n);
```

**指针操作:**

```c
// 分离逻辑证明
/*@
  @requires \valid(p);
  @requires \valid(q);
  @requires \separated(p, q);
  @assigns *p, *q;
  @ensures *p == \old(*p) + 1;
  @ensures *q == \old(*q);
*/
void increment_p_only(int* p, int* q) {
    (*p)++;
}
```

---

## 七、完整验证案例

### 案例1: 二分查找

```c
// wp_case1_binary_search.c

/*@
  @predicate is_sorted(int* a, integer n) =
    \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];
*/

/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @requires is_sorted(arr, n);
  @assigns \nothing;

  // 功能规格
  @ensures -1 <= \result < n;
  @ensures \result >= 0 ==> arr[\result] == target;
  @ensures \result == -1 ==>
    \forall integer i; 0 <= i < n ==> arr[i] != target;
*/
int binary_search(const int* arr, int n, int target) {
    int low = 0;
    int high = n - 1;

    /*@
      @loop invariant 0 <= low;
      @loop invariant high < n;
      @loop invariant low <= high + 1;
      // 关键不变量: 如果目标存在,必在[low, high]范围内
      @loop invariant \forall integer i;
        (0 <= i < low || high < i < n) ==> arr[i] != target;
      @loop assigns low, high;
      @loop variant high - low;
    */
    while (low <= high) {
        // 防止溢出: mid = (low + high) / 2
        int mid = low + (high - low) / 2;

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}
```

**验证命令:**

```bash
# 完整验证
frama-c -wp -wp-rte -wp-prover alt-ergo,z3 \
    -wp-model Typed \
    wp_case1_binary_search.c

# 预期结果: 所有目标 Valid
```

### 案例2: 快速排序分区

```c
// wp_case2_partition.c

/*@
  @predicate is_partitioned(int* a, integer n, integer pivot) =
    0 <= pivot < n ==>
    (\forall integer i; 0 <= i < pivot ==> a[i] <= a[pivot]) &&
    (\forall integer i; pivot < i < n ==> a[i] >= a[pivot]);
*/

/*@
  @requires n > 0;
  @requires \valid(a + (0..n-1));
  @assigns a[0..n-1];
  @ensures 0 <= \result < n;
  @ensures is_partitioned(a, n, \result);
*/
int partition(int* a, int n) {
    int pivot = a[n - 1];  // 选择最后一个元素为基准
    int i = 0;

    /*@
      @loop invariant 0 <= i <= j <= n;
      @loop invariant \forall integer k; 0 <= k < i ==> a[k] <= pivot;
      @loop invariant \forall integer k; i <= k < j ==> a[k] > pivot;
      @loop assigns i, j, a[0..n-1];
      @loop variant n - j;
    */
    for (int j = 0; j < n - 1; j++) {
        if (a[j] <= pivot) {
            // 交换 a[i] 和 a[j]
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            i++;
        }
    }

    // 将基准放到正确位置
    int temp = a[i];
    a[i] = a[n - 1];
    a[n - 1] = temp;

    return i;
}
```

**验证策略:**

```bash
# 此案例较复杂,需要更强力的证明器
frama-c -wp -wp-rte \
    -wp-prover z3,cvc4 \
    -wp-model Separation \
    -wp-split \
    wp_case2_partition.c
```

### 案例3: 链表反转

```c
// wp_case3_list_reverse.c
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

/*@
  @predicate list(Node* n, list<int> values) =
    n == \null ? values == nil :
    \exists list<int> tail; values == cons(n->value, tail) &&
    n->next == \null ? tail == nil :
    list(n->next, tail);
*/

/*@
  @requires list(head, ?values);
  @assigns \nothing;
  @frees head;
  @ensures list(\result, reverse(values));
*/
Node* list_reverse(Node* head) {
    Node* prev = NULL;
    Node* current = head;

    /*@
      @loop invariant list(current, ?remaining);
      @loop invariant list(prev, ?reversed);
      @loop invariant reverse(values) == concat(reverse(remaining), reversed);
      @loop assigns current, prev, head;
    */
    while (current != NULL) {
        Node* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    return prev;
}
```

**注意:** 链表验证是WP的高级应用,通常需要:

- 归纳数据结构定义
- 分离逻辑
- 可能的Coq交互式证明

---

## 八、常见问题与解答

**Q1: 证明器返回Unknown**

```bash
# 问题: Alt-Ergo无法证明

# 解决方案1: 尝试更强的证明器
frama-c -wp -wp-prover z3,cvc4 program.c

# 解决方案2: 拆分目标
frama-c -wp -wp-split program.c

# 解决方案3: 简化规范
# 将复杂后置条件拆分为多个ensures
```

**Q2: 循环不变量不够强**

```c
// 常见问题
/*@
  @loop invariant 0 <= i <= n;  // 太弱!
*/

// 解决方案: 添加功能性不变量
/*@
  @loop invariant 0 <= i <= n;
  @loop invariant sum == \sum(0,i-1,\lambda integer k; arr[k]);
  @loop invariant \forall integer j; 0 <= j < i ==> processed[j];
*/
```

**Q3: 指针别名问题**

```bash
# 问题: WP无法证明指针相关性质

# 解决方案: 使用分离模型
frama-c -wp -wp-model Separation program.c

# 并在规范中添加分离声明
/*@
  @requires \separated(p, q);
*/
```

---

## 九、参考资源

- [WP手册](https://frama-c.com/download/wp-manual.pdf)
- [ACSL规范](https://frama-c.com/download/acsl.pdf)
- [Why3平台](http://why3.lri.fr/)
- [Coq证明助手](https://coq.inria.fr/)

**上一章**: [05_Eva_Tutorial.md](./05_Eva_Tutorial.md)
**下一章**: [07_Industrial_Case_Studies.md](./07_Industrial_Case_Studies.md)

**最后更新**: 2026-03-19


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review


## 📚 实质性内容补充

### 技术深度分析

#### 1. 核心概念详解

深入剖析本主题的核心概念，建立完整的知识体系。

#### 2. 实现机制

| 层级 | 机制 | 关键技术 |
|:-----|:-----|:---------|
| 应用层 | 业务逻辑 | 设计模式 |
| 系统层 | 资源管理 | 内存/进程 |
| 硬件层 | 物理实现 | CPU/缓存 |

#### 3. 实践指导

- 最佳实践准则
- 常见陷阱与避免
- 调试与优化技巧

### 扩展阅读

- [核心知识体系](../../01_Core_Knowledge_System/README.md)
- [全局索引](../../00_GLOBAL_INDEX.md)
