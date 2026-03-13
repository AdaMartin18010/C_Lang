# Frama-C/WP 集成与实战指南

> **定位**: CompCert Verification / 工具集成 | **难度**: ⭐⭐⭐⭐ | **目标**: 掌握 Frama-C 验证 C 程序

---

## 目录

- [Frama-C/WP 集成与实战指南](#frama-cwp-集成与实战指南)
  - [目录](#目录)
  - [一、Frama-C 简介](#一frama-c-简介)
  - [二、安装配置](#二安装配置)
  - [三、ACSL 规格语言](#三acsl-规格语言)
  - [四、WP 插件使用](#四wp-插件使用)
  - [五、实战案例](#五实战案例)

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
opam install frama-c

# 验证安装
frama-c --version
# 29.0 (Cobalt)

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

> **参考**: Frama-C 29.0 文档 | **ACSL 手册**: <https://frama-c.com/acsl.html>
