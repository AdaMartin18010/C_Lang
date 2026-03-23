# Frama-C/WP 集成与实战指南

> **定位**: CompCert Verification / 工具集成 | **难度**: ⭐⭐⭐⭐ | **目标**: 掌握 Frama-C 验证 C 程序
> **版本**: Frama-C 32.0 (Germanium) | **更新**: 2026-03-15

---

## 目录

- [Frama-C/WP 集成与实战指南](#frama-cwp-集成与实战指南)
  - [目录](#目录)
  - [一、Frama-C 简介](#一frama-c-简介)
  - [二、安装配置](#二安装配置)
  - [三、ACSL 规格语言](#三acsl-规格语言)
  - [四、WP 插件使用](#四wp-插件使用)
  - [五、实战案例](#五实战案例)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 一、Frama-C 简介

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Frama-C 平台架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  Frama-C 是一个模块化平台，用于 C 代码的静态分析和形式化验证。                 │
│                                                                              │
│  核心插件:                                                                   │
│  ─────────────────────────────────────────                                   │
│  • WP (Weakest Precondition): 演绎验证                                       │
│  • Eva (Evolved Value Analysis): 抽象解释                                    │
│  • E-ACSL: 运行时验证                                                        │
│  • PathCrawler: 测试生成                                                     │
│                                                                              │
│  工作流:                                                                     │
│  ─────────────────────────────────────────                                   │
│  C 代码 + ACSL 规格 → Frama-C → 验证条件 → SMT 求解器 → 结果               │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 二、安装配置

```bash
# 通过 OPAM 安装 (推荐)
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
opam install frama-c

# 验证安装
frama-c --version
# 32.0 (Germanium) - 2026-03发布
# 新增: ACSL++ 支持, Clang 20 兼容, \aligned 谓词

# 基本使用
frama-c -wp source.c

# 完整配置
frama-c -wp -rte -warn-signed-overflow -warn-unsigned-overflow source.c
```

---

## 三、ACSL 规格语言

```c
// ACSL (ANSI C Specification Language) 语法示例

/*@
  // 函数合约
  requires \valid(p);           // 前置条件
  requires *p >= 0;
  ensures \result == *p + 1;     // 后置条件
  assigns \nothing;              // 不修改内存
*/
int increment(const int* p);

/*@
  // 循环不变式
  loop invariant 0 <= i <= n;
  loop invariant \forall integer j; 0 <= j < i ==> a[j] == b[j];
  loop assigns i, a[0..n-1];
  loop variant n - i;
*/
void array_copy(int* a, const int* b, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = b[i];
    }
}

/*@
  // 内存谓词
  predicate is_valid_array(int* a, integer n) =
    \valid(a + (0..n-1));

  // 逻辑函数
  logic integer sum(int* a, integer n) reads a[0..n-1];
  axiom sum_empty: \forall int* a; sum(a, 0) == 0;
  axiom sum_step: \forall int* a, integer n; n > 0 ==>
    sum(a, n) == sum(a, n-1) + a[n-1];
*/
```

---

## 四、WP 插件使用

```bash
# 基本验证
frama-c -wp program.c

# 指定证明器
frama-c -wp -wp-prover alt-ergo program.c
frama-c -wp -wp-prover z3 program.c
frama-c -wp -wp-prover cvc4 program.c

# 生成验证报告
frama-c -wp -report-json report.json program.c

# 调试模式
frama-c -wp -wp-debug program.c
```

---

## 五、实战案例

```c
// 验证安全的链表操作
#include <stdlib.h>
#include <stddef.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

/*@
  predicate list(struct Node* n, list<int> values) =
    n == \null ? values == nil :
    \exists list<int> tail; values == cons(n->value, tail) &&
    list(n->next, tail);
*/

/*@
  requires list(head, ?values);
  requires \valid(value);
  ensures list(\result, cons(*value, values));
  allocates \result;
*/
Node* list_prepend(Node* head, const int* value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) return head;

    new_node->value = *value;
    new_node->next = head;
    return new_node;
}

/*@
  requires list(head, ?values);
  frees head;
  ensures values == nil ==> \result == \null;
  ensures values != nil ==> \exists int v, list<int> tail;
    values == cons(v, tail) && list(\result, tail);
*/
Node* list_tail(Node* head) {
    if (head == NULL) return NULL;
    Node* next = head->next;
    free(head);
    return next;
}
```

---

> **参考**: Frama-C 32.0 (Germanium) 文档 | **ACSL 手册**: <https://frama-c.com/acsl.html>
> **新特性**: ACSL++ (C++规格语言), MetAcsl v0.10 (安全属性), Frama-Clang v0.0.19 (Clang 20支持)


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
