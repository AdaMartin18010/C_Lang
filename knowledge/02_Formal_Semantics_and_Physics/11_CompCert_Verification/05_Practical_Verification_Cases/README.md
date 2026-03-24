# CompCert 实践验证案例

## 概述

本文档详细介绍如何使用 CompCert 编译器和 Frama-C 验证框架进行实际的程序验证。
我们将通过具体的案例研究，展示如何编写 ACSL（ANSI/ISO C Specification Language）注释、验证内存安全性以及建立函数契约。

## 目录

- [CompCert 实践验证案例](#compcert-实践验证案例)
  - [概述](#概述)
  - [目录](#目录)
  - [Frama-C ACSL 基础](#frama-c-acsl-基础)
    - [什么是 ACSL？](#什么是-acsl)
    - [ACSL 注释结构](#acsl-注释结构)
    - [核心谓词](#核心谓词)
  - [内存安全验证](#内存安全验证)
    - [数组边界检查](#数组边界检查)
    - [空指针检查](#空指针检查)
    - [内存分配验证](#内存分配验证)
  - [函数契约设计](#函数契约设计)
    - [前置条件与后置条件](#前置条件与后置条件)
    - [行为规约](#行为规约)
    - [复杂数据结构的契约](#复杂数据结构的契约)
  - [完整案例研究](#完整案例研究)
    - [案例一：安全字符串操作](#案例一安全字符串操作)
    - [案例二：排序算法验证](#案例二排序算法验证)
    - [案例三：堆栈数据结构](#案例三堆栈数据结构)
  - [验证工作流](#验证工作流)
    - [使用 Frama-C 进行验证](#使用-frama-c-进行验证)
    - [Makefile 集成](#makefile-集成)
  - [最佳实践](#最佳实践)
  - [参考资料](#参考资料)

---

## Frama-C ACSL 基础

### 什么是 ACSL？

ACSL 是一种行为规约语言，用于指定 C 程序的功能属性。它允许开发者在代码中嵌入形式化规约，使得 Frama-C 工具集能够进行静态分析和验证。

### ACSL 注释结构

```c
/*@
  @ requires \valid(p) && n >= 0;
  @ requires \valid_read(src + (0..n-1));
  @ assigns *(p+(0..n-1));
  @ ensures \forall integer i; 0 <= i < n ==> p[i] == src[i];
  @ ensures \result == p;
*/
void* my_memcpy(void* p, const void* src, size_t n);
```

### 核心谓词

| 谓词 | 含义 | 示例 |
|------|------|------|
| `\valid(p)` | 指针 p 可安全访问 | `\valid(ptr)` |
| `\valid_read(p)` | 指针 p 可安全读取 | `\valid_read(src)` |
| `\initialized(p)` | 内存已初始化 | `\initialized(&x)` |
| `\separated(p,q)` | 内存区域不重叠 | `\separated(a, b)` |
| `\old(e)` | 函数调用前的表达式值 | `\old(*p)` |
| `\result` | 函数返回值 | `\result == 0` |

---

## 内存安全验证

### 数组边界检查

```c
/*@
  @ requires \valid(a + (0..n-1));
  @ requires n >= 0;
  @ assigns \nothing;
  @ ensures \result == \sum(0, n-1, \lambda integer k; a[k]);
*/
int array_sum(const int* a, size_t n) {
    int sum = 0;
    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant sum == \sum(0, i-1, \lambda integer k; a[k]);
      @ loop assigns i, sum;
      @ loop variant n - i;
    */
    for (size_t i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum;
}
```

### 空指针检查

```c
/*@
  @ requires p != \null ==> \valid(p);
  @ assigns \nothing;
  @ behavior null_case:
  @   assumes p == \null;
  @   ensures \result == 0;
  @ behavior valid_case:
  @   assumes p != \null;
  @   ensures \result == *p;
  @ complete behaviors;
  @ disjoint behaviors;
*/
int safe_dereference(int* p) {
    if (p == NULL) {
        return 0;
    }
    return *p;
}
```

### 内存分配验证

```c
/*@
  @ requires n > 0;
  @ requires \valid(elem);
  @ allocates \result;
  @ assigns \result \from elem, n;
  @ ensures \result != \null ==> \fresh(\result, sizeof(int) * n);
  @ ensures \result != \null ==> \valid(((int*)\result) + (0..n-1));
  @ behavior success:
  @   assumes \result != \null;
  @   ensures \forall integer i; 0 <= i < n ==> ((int*)\result)[i] == *elem;
  @ behavior failure:
  @   assumes \result == \null;
  @   allocates \nothing;
*/
int* create_filled_array(size_t n, const int* elem) {
    int* arr = (int*)malloc(sizeof(int) * n);
    if (arr == NULL) {
        return NULL;
    }

    /*@
      @ loop invariant 0 <= i <= n;
      @ loop invariant \valid(arr + (0..n-1));
      @ loop assigns i, arr[0..n-1];
      @ loop variant n - i;
    */
    for (size_t i = 0; i < n; i++) {
        arr[i] = *elem;
    }

    return arr;
}
```

---

## 函数契约设计

### 前置条件与后置条件

```c
/*@
  @ requires a > 0;
  @ requires b > 0;
  @ requires \gcd(a, b) == 1;  // 互质
  @ assigns \nothing;
  @ ensures \result > 0;
  @ ensures (\result % a) == 0;
  @ ensures (\result % b) == 0;
  @ ensures \forall integer x; (x % a == 0 && x % b == 0) ==> x >= \result;
*/
int lcm(int a, int b);
```

### 行为规约

```c
/*@
  @ requires \valid(a) && \valid(b);
  @ assigns *a, *b;
  @ ensures *a == \old(*b);
  @ ensures *b == \old(*a);
*/
void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
```

### 复杂数据结构的契约

```c
/*@ predicate is_valid_list{L}(struct list* l) =
  @   l == \null ||
  @   (\valid(l) && is_valid_list(l->next));
  @
  @ logic integer list_length{L}(struct list* l) =
  @   (l == \null) ? 0 : 1 + list_length(l->next);
  @
  @ predicate no_cycle{L}(struct list* l) =
  @   l == \null ||
  @   (\valid(l) && (l->next == \null || l->next != l) && no_cycle(l->next));
*/

/*@
  @ requires is_valid_list(lst);
  @ requires no_cycle(lst);
  @ assigns \nothing;
  @ ensures \result == list_length(lst);
*/
size_t list_size(const struct list* lst);
```

---

## 完整案例研究

### 案例一：安全字符串操作

```c
#include <stddef.h>

/*@
  @ predicate is_valid_string(char* s) =
  @   \exists integer i; i >= 0 && s[i] == '\0' &&
  @   \valid(s + (0..i));
  @
  @ logic integer string_length(char* s) =
  @   *s == '\0' ? 0 : 1 + string_length(s+1);
*/

/*@
  @ requires is_valid_string(dest);
  @ requires is_valid_string(src);
  @ requires \separated(dest, src);
  @ assigns dest[0..string_length(src)];
  @ ensures \forall integer i; 0 <= i <= string_length(src) ==>
  @   dest[i] == src[i];
  @ ensures \result == dest;
*/
char* safe_strcpy(char* dest, const char* src) {
    size_t i = 0;

    /*@
      @ loop invariant 0 <= i;
      @ loop invariant \forall integer j; 0 <= j < i ==> dest[j] == src[j];
      @ loop invariant is_valid_string(src + i);
      @ loop assigns i, dest[0..i];
      @ loop variant string_length(src) - i;
    */
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';

    return dest;
}
```

### 案例二：排序算法验证

```c
/*@
  @ predicate sorted(int* a, integer n) =
  @   \forall integer i, j; 0 <= i <= j < n ==> a[i] <= a[j];
  @
  @ predicate permutation{L1,L2}(int* a, integer n) =
  @   \forall integer v; \numof(0, n-1, \lambda integer k; a[k] == v) ==
  @   \numof(0, n-1, \lambda integer k; \at(a[k], L1) == v);
*/

/*@
  @ requires \valid(a + (0..n-1));
  @ requires n >= 0;
  @ assigns a[0..n-1];
  @ ensures sorted(a, n);
  @ ensures permutation{Pre,Here}(a, n);
*/
void insertion_sort(int* a, size_t n) {
    /*@
      @ loop invariant 1 <= i <= n;
      @ loop invariant sorted(a, i);
      @ loop invariant permutation{Pre,Here}(a, n);
      @ loop assigns i, a[0..n-1];
      @ loop variant n - i;
    */
    for (size_t i = 1; i < n; i++) {
        int key = a[i];
        size_t j = i;

        /*@
          @ loop invariant 0 <= j <= i;
          @ loop invariant \forall integer k; j < k <= i ==> a[k] > key;
          @ loop assigns j, a[1..i];
          @ loop variant j;
        */
        while (j > 0 && a[j-1] > key) {
            a[j] = a[j-1];
            j--;
        }
        a[j] = key;
    }
}
```

### 案例三：堆栈数据结构

```c
#define MAX_SIZE 100

struct stack {
    int data[MAX_SIZE];
    size_t top;
};

/*@
  @ predicate is_valid_stack(struct stack* s) =
  @   \valid(s) && s->top <= MAX_SIZE;
  @
  @ predicate is_empty(struct stack* s) = s->top == 0;
  @
  @ predicate is_full(struct stack* s) = s->top == MAX_SIZE;
*/

/*@
  @ requires \valid(s);
  @ assigns s->top;
  @ ensures is_empty(s);
*/
void stack_init(struct stack* s) {
    s->top = 0;
}

/*@
  @ requires is_valid_stack(s);
  @ requires !is_full(s);
  @ assigns s->data[s->top], s->top;
  @ ensures s->top == \old(s->top) + 1;
  @ ensures s->data[s->top-1] == value;
*/
void stack_push(struct stack* s, int value) {
    s->data[s->top] = value;
    s->top++;
}

/*@
  @ requires is_valid_stack(s);
  @ requires !is_empty(s);
  @ assigns \nothing;
  @ ensures \result == s->data[s->top-1];
*/
int stack_peek(const struct stack* s) {
    return s->data[s->top - 1];
}
```

---

## 验证工作流

### 使用 Frama-C 进行验证

```bash
# 1. 基础语法检查
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
frama-c -c11 file.c

# 2. 使用 WP 插件进行程序验证
frama-c -wp -rte file.c

# 3. 使用 Alt-Ergo 作为后端求解器
frama-c -wp -wp-prover alt-ergo file.c

# 4. 生成验证报告
frama-c -wp -report file.c

# 5. 使用 CompCert 编译验证后的代码
ccomp -interp file.c
```

### Makefile 集成

```makefile
# Makefile for CompCert/Frama-C verification

CC = ccomp
FRAMA = frama-c
CFLAGS = -O2 -finline-asm

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all verify clean

all: verify $(OBJS)

verify:
 @echo "Running Frama-C verification..."
 $(FRAMA) -wp -rte -warn-signed-overflow $(SRCS)

%.o: %.c
 $(CC) $(CFLAGS) -c $< -o $@

clean:
 rm -f $(OBJS) *.ali
```

---

## 最佳实践

1. **逐步验证**：从简单的函数开始，逐步增加复杂度
2. **模块化规约**：使用谓词和逻辑函数复用规约模式
3. **充分的注释**：为每个函数提供完整的前置/后置条件
4. **边界条件**：特别关注空指针、零长度、最大值等边界情况
5. **循环不变式**：每个循环都应该有明确的不变式和变体

---

## 参考资料

- [CompCert 官方文档](https://compcert.org/man/)
- [Frama-C ACSL 手册](https://frama-c.com/acsl.html)
- [Software Foundations - Verified Functional Algorithms](https://softwarefoundations.cis.upenn.edu/)
