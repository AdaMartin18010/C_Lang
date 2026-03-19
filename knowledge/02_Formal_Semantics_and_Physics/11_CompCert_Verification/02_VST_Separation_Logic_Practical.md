# VST分离逻辑实战指南

> **核心命题**: 使用Verified Software Toolchain (VST) 和分离逻辑验证C程序的功能正确性
>
> **参考**: POPL 2024 "An Iris Instance for Verifying CompCert C Programs"
>
> **难度**: L6 | **预估时间**: 15-20小时

---

## 目录

- [VST分离逻辑实战指南](#vst分离逻辑实战指南)
  - [目录](#目录)
  - [1. 理论基础](#1-理论基础)
    - [1.1 什么是分离逻辑](#11-什么是分离逻辑)
    - [1.2 VST架构概览](#12-vst架构概览)
    - [1.3 从VST到Iris演进](#13-从vst到iris演进)
  - [2. 环境搭建](#2-环境搭建)
    - [2.1 安装Coq和VST](#21-安装coq和vst)
    - [2.2 第一个验证项目](#22-第一个验证项目)
  - [3. 核心概念](#3-核心概念)
    - [3.1 Hoare三元组](#31-hoare三元组)
    - [3.2 Points-To断言](#32-points-to断言)
    - [3.3 分离合取](#33-分离合取)
    - [3.4 函数规约](#34-函数规约)
  - [4. 实战案例](#4-实战案例)
    - [4.1 链表操作验证](#41-链表操作验证)
    - [4.2 内存分配器验证](#42-内存分配器验证)
    - [4.3 并发数据结构验证](#43-并发数据结构验证)
  - [5. 高级主题](#5-高级主题)
    - [5.1 幽灵状态](#51-幽灵状态)
    - [5.2 高阶函数](#52-高阶函数)
    - [5.3 模块化验证](#53-模块化验证)
  - [6. 与CompCert集成](#6-与compcert集成)
  - [7. 参考资源](#7-参考资源)
    - [学术论文](#学术论文)
    - [工具与资源](#工具与资源)
    - [学习路径](#学习路径)

---

## 1. 理论基础

### 1.1 什么是分离逻辑

分离逻辑(Separation Logic)是Hoare逻辑的扩展，专门用于推理具有指针和堆内存的程序。

```
传统Hoare逻辑: {P} C {Q}
分离逻辑扩展: 引入分离合取 * 表示内存区域的分离

关键洞察:
- (p ↦ v) 表示"指针p指向值v"
- (P * Q) 表示"P和Q描述的内存区域不相交"
- 框架规则: 若 {P} C {Q}，则 {P * R} C {Q * R} (C不修改R)
```

### 1.2 VST架构概览

```
┌─────────────────────────────────────────────────────────────────┐
│                      VST 架构层次                                │
├─────────────────────────────────────────────────────────────────┤
│  Layer 4: 应用程序验证 (Floyd自动化)                              │
│     └── 实际C程序验证，自动化证明策略                              │
├─────────────────────────────────────────────────────────────────┤
│  Layer 3: Verifiable C 程序逻辑                                   │
│     └── Hoare规则、分离逻辑断言、类型系统                          │
├─────────────────────────────────────────────────────────────────┤
│  Layer 2: Clight 操作语义                                         │
│     └── CompCert中间语言的形式语义                                │
├─────────────────────────────────────────────────────────────────┤
│  Layer 1: MSL/Iris 元逻辑框架                                     │
│     └── 分离逻辑的基础框架、资源代数                               │
├─────────────────────────────────────────────────────────────────┤
│  Layer 0: Coq 证明助手                                            │
│     └── 所有形式化证明的基础                                       │
└─────────────────────────────────────────────────────────────────┘
```

### 1.3 从VST到Iris演进

**传统VST (2014-2023)**:

- 基于MSL (Meta-Separation Logic) 框架
- 专为C设计，难以扩展
- 约67k行Coq代码

**Iris上的VST (2024+)**:

- 基于Iris分离逻辑框架
- 利用Iris的高阶幽灵状态和证明模式
- 更灵活，可与其他Iris工具集成
- 约44k行Coq代码，功能更强大

```
Iris优势:
- 高阶幽灵状态: 支持复杂的协议验证
- Iris Proof Mode: 交互式证明更友好
- Diaframe: 自动化分离逻辑证明
- 与Rust验证工具(如RustBelt)共享基础设施
```

---

## 2. 环境搭建

### 2.1 安装Coq和VST

```bash
# 使用opam安装Coq
opam init
eval $(opam env)
opam install coq.8.17.1

# 安装CompCert
opam install coq-compcert

# 安装VST (传统版本)
git clone https://github.com/PrincetonUniversity/VST.git
cd VST
make

# 安装Iris (用于新版本)
opam install coq-iris
```

### 2.2 第一个验证项目

```coq
(* 最简单的VST验证: swap函数 *)
Require Import VST.floyd.proofauto.
Require Import VST.progs.swap.

(* 函数规约 *)
Definition swap_spec : ident * funspec :=
  DECLARE _swap
  WITH x: val, y: val, vx: Z, vy: Z
  PRE [tptr tint, tptr tint]
    PROP ()
    PARAMS (x; y)
    SEP (data_at Ews tint (Vint (Int.repr vx)) x;
         data_at Ews tint (Vint (Int.repr vy)) y)
  POST [tvoid]
    PROP ()
    RETURN ()
    SEP (data_at Ews tint (Vint (Int.repr vy)) x;
         data_at Ews tint (Vint (Int.repr vx)) y).

(* 证明 *)
Lemma body_swap: semax_body Vprog Gprog f_swap swap_spec.
Proof.
  start_function.
  (* 读取*x到临时变量 *)
  forward.  (* t = *x; *)
  (* 读取*y到*x *)
  forward.  (* *x = *y; *)
  (* 将t写入*y *)
  forward.  (* *y = t; *)
  (* 完成，验证后置条件 *)
  entailer!.
Qed.
```

---

## 3. 核心概念

### 3.1 Hoare三元组

```coq
(* VST中的Hoare三元组 *)
semax Δ (PROP P LOCAL L SEP S) c (PROP P' LOCAL L' SEP S')

其中:
- Δ: 类型环境
- PROP P: 纯逻辑断言(不依赖内存)
- LOCAL L: 局部变量绑定
- SEP S: 分离逻辑断言(内存状态)
- c: 命令
- POST: 后置条件
```

**示例: 交换函数规约**

```c
void swap(int *x, int *y) {
    int t = *x;
    *x = *y;
    *y = t;
}
```

```coq
(* 规约: 如果x指向vx，y指向vy，
   执行后x指向vy，y指向vx *)
Definition swap_spec :=
  WITH x: val, y: val, vx: Z, vy: Z
  PRE [tptr tint, tptr tint]
    PROP (x <> y)  (* x和y不是同一地址 *)
    PARAMS (x; y)
    SEP (x ↦ vx; y ↦ vy)
  POST [tvoid]
    PROP ()
    RETURN ()
    SEP (x ↦ vy; y ↦ vx).
```

### 3.2 Points-To断言

```coq
(* 基本points-to *)
p ↦ v          (* 指针p指向值v *)
p ↦ -          (* 指针p指向某个值(存在性) *)
p ↦ ?          (* 指针p是有效的，但不关心值 *)

(* 带类型的points-to *)
data_at sh t v p   (* 在权限sh下，位置p存储类型t的值v *)

(* 权限共享 *)
Ews  (* 独占写权限 *)
Ers  (* 独占读权限 *)
Qsh  (* 共享权限，可分割 *)
```

### 3.3 分离合取

```coq
(* 分离合取: 两个断言描述不相交的内存区域 *)
P * Q

(* 示例 *)
(p ↦ 1) * (q ↦ 2)  (* p和q指向不同地址 *)

(* 分离蕴涵: 魔术棒 *)
P -* Q  (* 如果给你P描述的内存，你就能得到Q *)

(* 应用魔术棒 *)
P * (P -* Q) ⊢ Q
```

### 3.4 函数规约

```coq
(* 函数规约的组成部分 *)
DECLARE _function_name
WITH ghost_parameters  (* 幽灵参数 *)
PRE [parameter_types]
  PROP (pure_preconditions)      (* 纯前置条件 *)
  PARAMS (actual_parameters)      (* 实际参数 *)
  GLOBALS (global_variables)      (* 全局变量 *)
  SEP (memory_preconditions)      (* 内存前置条件 *)
POST [return_type]
  PROP (pure_postconditions)      (* 纯后置条件 *)
  RETURN (return_value)           (* 返回值 *)
  SEP (memory_postconditions).    (* 内存后置条件 *)
```

---

## 4. 实战案例

### 4.1 链表操作验证

```c
// 链表节点
typedef struct Node {
    int value;
    struct Node *next;
} Node;

// 在头部插入
Node* list_prepend(Node *head, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) return NULL;
    new_node->value = value;
    new_node->next = head;
    return new_node;
}
```

```coq
(* 链表谓词定义 *)
Fixpoint list_rep (sigma: list Z) (p: val) : mpred :=
  match sigma with
  | nil => !! (p = nullval) && emp
  | x :: xs => EX next: val,
      data_at Ews t_struct_Node (Vint (Int.repr x), next) p
      * list_rep xs next
  end.

(* prepend规约 *)
Definition prepend_spec :=
  DECLARE _list_prepend
  WITH head: val, value: Z, contents: list Z
  PRE [tptr t_struct_Node, tint]
    PROP (Int.min_signed <= value <= Int.max_signed)
    PARAMS (head; Vint (Int.repr value))
    SEP (list_rep contents head)
  POST [tptr t_struct_Node]
    EX new_head: val,
    PROP ()
    RETURN new_head
    SEP (list_rep (value :: contents) new_head).

(* 证明概要 *)
Lemma body_prepend: semax_body Vprog Gprog f_list_prepend prepend_spec.
Proof.
  start_function.
  forward_call (sizeof t_struct_Node).
  { compute; split; auto. }  (* 大小为正 *)
  Intros new_node.
  forward_if.  (* if (new_node == NULL) *)
  - (* 失败情况 *)
    forward.
    Exists nullval.
    entailer!.
  - (* 成功情况 *)
    forward.  (* new_node->value = value; *)
    forward.  (* new_node->next = head; *)
    Exists new_node.
    unfold list_rep; fold list_rep.
    Exists head.
    entailer!.
Qed.
```

### 4.2 内存分配器验证

```c
// 简单的块分配器
typedef struct Block {
    size_t size;
    int free;
    struct Block *next;
} Block;

static Block *free_list = NULL;

void *my_malloc(size_t size) {
    Block *curr = free_list;
    while (curr != NULL) {
        if (curr->free && curr->size >= size) {
            curr->free = 0;
            return (void*)(curr + 1);
        }
        curr = curr->next;
    }
    // 分配新块
    Block *block = sbrk(sizeof(Block) + size);
    block->size = size;
    block->free = 0;
    block->next = free_list;
    free_list = block;
    return (void*)(block + 1);
}
```

```coq
(* 块谓词 *)
Definition block_rep (b: Block) (p: val) : mpred :=
  data_at Ews t_struct_Block
    (Vlong (Int64.repr b.(size)),
     Vint (Int.repr (if b.(free) then 1 else 0)),
     b.(next))
    p.

(* 空闲列表谓词 *)
Fixpoint freelist_rep (blocks: list Block) (p: val) : mpred :=
  match blocks with
  | nil => !! (p = nullval) && emp
  | b :: bs => EX next: val,
      block_rep b p *
      freelist_rep bs next *
      !! (b.(next) = next)
  end.

(* malloc规约 *)
Definition malloc_spec :=
  DECLARE _my_malloc
  WITH size: Z, blocks: list Block, fl: val
  PRE [tulong]
    PROP (0 <= size <= Int64.max_signed)
    PARAMS (Vlong (Int64.repr size))
    SEP (freelist_rep blocks fl; global_var _free_list fl)
  POST [tptr tvoid]
    EX p: val, EX blocks': list Block,
    PROP (p <> nullval ->
          exists b, In b blocks' /
          b.(size) >= size /\ ~b.(free))
    RETURN p
    SEP (freelist_rep blocks' fl; global_var _free_list fl).
```

### 4.3 并发数据结构验证

```coq
(* 使用VST验证并发数据结构需要额外的并发逻辑 *)
(* VST支持两种并发模型: *)

(* 1. Pthreads风格: 锁和条件变量 *)
Definition lock_inv (R: mpred) (p: val) : mpred :=
  field_at Ews tlock _lock_status p (Vint (Int.repr 0)) * R.

(* 2. C11原子操作: 无锁数据结构 *)
Definition atomic_int_at (sh: share) (v: Z) (p: val) : mpred :=
  data_at sh tint (Vint (Int.repr v)) p.

(* 原子比较交换规约 *)
Definition atomic_CAS_spec :=
  DECLARE _atomic_compare_exchange_strong
  WITH p: val, expected: val, desired: Z, old_val: Z
  PRE [tptr tint, tptr tint, tint]
    PROP ()
    PARAMS (p; expected; Vint (Int.repr desired))
    SEP (atomic_int_at Ews old_val p;
         data_at Ews tint (Vint (Int.repr old_val)) expected)
  POST [tbool]
    PROP ()
    RETURN (Vint (if Z.eqb old_val old_val then Int.one else Int.zero))
    SEP (atomic_int_at Ews (if Z.eqb old_val old_val then desired else old_val) p;
         data_at Ews tint (Vint (Int.repr old_val)) expected).
```

---

## 5. 高级主题

### 5.1 幽灵状态

```coq
(* 幽灵状态用于验证时携带逻辑信息，不对应实际内存 *)
(* 例如: 链表长度、树的高度、资源所有权令牌 *)

(* 幽灵链表长度 *)
Fixpoint list_with_length (sigma: list Z) (n: nat) (p: val) : mpred :=
  match sigma with
  | nil => !! (p = nullval /\ n = 0%nat) && emp
  | x :: xs => EX next: val,
      data_at Ews t_struct_Node (Vint (Int.repr x), next) p
      * list_with_length xs (pred n) next
  end.

(* 使用幽灵状态验证迭代器有效性 *)
Definition iterator_inv (contents: list Z) (curr: val) (idx: nat) : mpred :=
  EX suffix: list Z,
  !! (contents = firstn idx contents ++ suffix)
  * list_rep suffix curr.
```

### 5.2 高阶函数

```coq
(* 函数指针规约 *)
Definition apply_spec (f_spec: funspec) :=
  DECLARE _apply
  WITH x: Z, f: val
  PRE [tptr tvoid, tint]
    PROP ()
    PARAMS (f; Vint (Int.repr x))
    SEP (func_ptr f_spec f)
  POST [tint]
    EX y: Z,
    PROP (f_spec x y)  (* f满足其规约，将x映射到y *)
    RETURN (Vint (Int.repr y))
    SEP (func_ptr f_spec f).
```

### 5.3 模块化验证

```coq
(* 使用Ghost Credits进行资源会计 *)
(* 验证内存分配器没有泄漏 *)

Definition alloc_token (p: val) (size: Z) : mpred :=
  ccm_token p size.  (* 分配令牌，必须在free时归还 *)

Definition malloc_spec' :=
  DECLARE _malloc
  WITH size: Z
  PRE [tulong]
    PROP (0 < size)
    PARAMS (Vlong (Int64.repr size))
    SEP (emp)
  POST [tptr tvoid]
    EX p: val,
    PROP (p <> nullval)
    RETURN p
    SEP (if eq_dec p nullval then emp
         else memory_block Ews size p * alloc_token p size).

Definition free_spec' :=
  DECLARE _free
  WITH p: val, size: Z
  PRE [tptr tvoid]
    PROP ()
    PARAMS (p)
    SEP (memory_block Ews size p * alloc_token p size)
  POST [tvoid]
    PROP ()
    RETURN ()
    SEP (emp).  (* 归还令牌，资源清零 *)
```

---

## 6. 与CompCert集成

```
┌─────────────────────────────────────────────────────────────┐
│                  端到端验证流程                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   C源代码 (source.c)                                        │
│        │                                                    │
│        ▼                                                    │
│   ┌─────────────────────────────────────┐                   │
│   │  clightgen (CompCert前端)           │                   │
│   │  ├── 解析C代码                      │                   │
│   │  ├── 类型检查                       │                   │
│   │  └── 生成Clight AST (source.v)      │                   │
│   └─────────────────────────────────────┘                   │
│        │                                                    │
│        ▼                                                    │
│   Coq证明 (proof.v)                                         │
│   ├── 导入Clight AST                                        │
│   ├── 定义函数规约                                          │
│   └── 证明: semax_body Gprog f_spec                         │
│        │                                                    │
│        ▼                                                    │
│   ┌─────────────────────────────────────┐                   │
│   │  可靠性定理                         │                   │
│   │  "源代码行为 ⊆ Clight语义"          │                   │
│   │          +                            │                   │
│   │  "Clight行为 ⊆ 汇编代码行为"        │                   │
│   │          =                            │                   │
│   │  "源代码行为 ⊆ 汇编代码行为"        │                   │
│   └─────────────────────────────────────┘                   │
│        │                                                    │
│        ▼                                                    │
│   汇编代码 (CompCert编译)                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```coq
(* 完整的端到端验证示例 *)
Require Import compcert.common.Errors.
Require Import VST.floyd.proofauto.
Require Import VST.progs.verify.

(* 假设我们已经证明了所有函数的body_spec *)
Lemma all_functions_verified:
  forall id spec, In (id, spec) Gprog ->
  exists f, (prog_defs prog) ! id = Some f /\
  semax_body Vprog Gprog f spec.
Proof.
  (* 证明所有函数满足规约 *)
  intros. destruct H as [H | [H | ...]]; subst.
  - exists f_swap; split; auto. apply body_swap.
  - exists f_prepend; split; auto. apply body_prepend.
  (* ... *)
Qed.

(* 端到端正确性定理 *)
Theorem end_to_end_correctness:
  forall input output,
  source_program_spec input output ->
  assembly_program_behaves input output.
Proof.
  intros.
  (* 1. 源代码验证 *)
  apply VST_soundness with (spec := source_spec).
  apply all_functions_verified.
  (* 2. CompCert编译正确性 *)
  apply CompCert_correctness.
  (* 3. 组合结果 *)
  assumption.
Qed.
```

---

## 7. 参考资源

### 学术论文

| 论文 | 作者 | 年份 | 贡献 |
|:-----|:-----|:----:|:-----|
| Verified Software Toolchain | Appel et al. | 2014 | VST原始框架 |
| VST-Floyd | Cao et al. | 2018 | 自动化证明助手 |
| An Iris Instance for VST | Mansky et al. | POPL 2024 | Iris重构版本 |
| Compositional Verification with Templates | Nguyen et al. | CPP 2024 | 并发数据结构验证 |

### 工具与资源

- **VST GitHub**: <https://github.com/PrincetonUniversity/VST>
- **CompCert**: <https://compcert.org/>
- **Iris Project**: <https://iris-project.org/>
- **Software Foundations**: <https://softwarefoundations.cis.upenn.edu/>

### 学习路径

```
Phase 1: Coq基础 (20小时)
  └── Software Foundations Volume 1-2

Phase 2: 分离逻辑理论 (15小时)
  └── Program Logics for Certified Compilers

Phase 3: VST入门 (20小时)
  └── VST教程和简单示例

Phase 4: 实际项目验证 (30小时)
  └── 选择一个小项目完整验证

Phase 5: Iris高级特性 (20小时)
  └── Iris文献和POPL 2024论文
```

---

> **核心洞察**: 形式化验证的成本正在快速下降。随着Iris等框架的成熟，以及自动化工具的发展，验证实际C代码的功能正确性正变得越来越可行。在安全关键领域，这种投资已经变得必不可少。

---

*最后更新: 2026-03-14 | 与POPL 2024 Iris-VST论文对齐*


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
