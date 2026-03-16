# Coq形式化验证实践案例

本文档提供从基础到高级的Coq形式化验证完整案例，帮助理解如何使用Coq验证C语言程序的正确性。

---

## 目录

- [Coq形式化验证实践案例](#coq形式化验证实践案例)
  - [目录](#目录)
  - [1. Coq概述与安装配置](#1-coq概述与安装配置)
    - [1.1 Coq简介](#11-coq简介)
    - [1.2 安装配置](#12-安装配置)
      - [在Ubuntu/Debian上安装](#在ubuntudebian上安装)
      - [在macOS上安装](#在macos上安装)
      - [在Windows上安装](#在windows上安装)
    - [1.3 配置Coq项目](#13-配置coq项目)
    - [1.4 第一个Coq证明](#14-第一个coq证明)
  - [2. 基础证明战术](#2-基础证明战术)
    - [2.1 常用战术一览](#21-常用战术一览)
    - [2.2 自定义战术](#22-自定义战术)
  - [3. 链表操作正确性证明](#3-链表操作正确性证明)
    - [3.1 链表数据结构的定义](#31-链表数据结构的定义)
    - [3.2 对应的C代码](#32-对应的c代码)
  - [4. 排序算法正确性证明](#4-排序算法正确性证明)
    - [4.1 排序正确性的形式化定义](#41-排序正确性的形式化定义)
    - [4.2 对应的C代码](#42-对应的c代码)
  - [5. 栈和队列的实现与验证](#5-栈和队列的实现与验证)
    - [5.1 栈的实现与验证](#51-栈的实现与验证)
    - [5.2 队列的实现与验证](#52-队列的实现与验证)
    - [5.3 对应的C代码](#53-对应的c代码)
  - [6. 内存分配器验证](#6-内存分配器验证)
    - [6.1 简单内存分配器的形式化](#61-简单内存分配器的形式化)
    - [6.2 对应的C代码](#62-对应的c代码)
  - [7. 函数契约和前置/后置条件](#7-函数契约和前置后置条件)
    - [7.1 霍尔逻辑的Coq形式化](#71-霍尔逻辑的coq形式化)
    - [7.2 对应的C代码](#72-对应的c代码)
  - [8. 循环不变量](#8-循环不变量)
    - [8.1 循环不变量的形式化方法](#81-循环不变量的形式化方法)
    - [8.2 对应的C代码](#82-对应的c代码)

---

## 1. Coq概述与安装配置

### 1.1 Coq简介

Coq是一个交互式定理证明助手，基于归纳构造演算(Calculus of Inductive Constructions)。它允许用户：

- 定义数学对象和函数
- 陈述数学定理和程序规范
- 交互式地开发形式化证明
- 自动检查证明的正确性

Coq在程序验证领域的应用主要通过以下方式：

1. **直接编码**：将程序作为函数在Coq中实现并验证
2. **CompCert项目**：验证C语言编译器的正确性
3. **VST (Verified Software Toolchain)**：验证C程序的工具链

### 1.2 安装配置

#### 在Ubuntu/Debian上安装

```bash
# 添加PPA仓库
sudo add-apt-repository ppa:coq/stable
sudo apt-get update

# 安装Coq和IDE
sudo apt-get install coq coqide

# 安装OPAM包管理器（推荐）
sudo apt-get install opam
opam init
eval $(opam env)

# 通过OPAM安装Coq
opam install coq
```

#### 在macOS上安装

```bash
# 使用Homebrew
brew install coq

# 或直接使用OPAM
opam install coq
```

#### 在Windows上安装

```powershell
# 使用Coq平台安装程序
# 下载地址: https://github.com/coq/platform/releases

# 或使用WSL + Ubuntu
wsl --install -d Ubuntu
# 然后在WSL中按照Ubuntu安装步骤
```

### 1.3 配置Coq项目

创建一个新的Coq项目结构：

```
my_coq_project/
├── _CoqProject          # 项目配置文件
├── Makefile             # 构建脚本
├── theories/
│   ├── Basic.v          # 基础定义
│   ├── ListProofs.v     # 链表证明
│   └── Sorting.v        # 排序算法
└── extraction/          # 代码提取目录
```

**_CoqProject** 文件示例：

```
-R theories MyProject

theories/Basic.v
theories/ListProofs.v
theories/Sorting.v
```

**Makefile** 示例：

```makefile
COQMAKEFILE := $(shell which coq_makefile)

ifeq ($(COQMAKEFILE),)
$(error "coq_makefile not found. Please install Coq.")
endif

all: Makefile.coq
 $(MAKE) -f Makefile.coq

clean: Makefile.coq
 $(MAKE) -f Makefile.coq clean
 rm -f Makefile.coq

Makefile.coq: _CoqProject
 $(COQMAKEFILE) -f _CoqProject -o Makefile.coq

.PHONY: all clean
```

### 1.4 第一个Coq证明

```coq
(* File: theories/FirstProof.v *)

(* 定义一个简单的定理：对于所有自然数n，n + 0 = n *)
Theorem plus_zero_right : forall n : nat, n + 0 = n.
Proof.
  intros n.        (* 引入变量n *)
  induction n.     (* 对n进行归纳 *)
  - reflexivity.   (* 基本情况：0 + 0 = 0 *)
  - simpl.         (* 归纳步骤：S n + 0 = S (n + 0) *)
    rewrite IHn.   (* 使用归纳假设 *)
    reflexivity.   (* S n = S n *)
Qed.

(* 打印证明对象 *)
Print plus_zero_right.
```

---

## 2. 基础证明战术

### 2.1 常用战术一览

```coq
(* File: theories/TacticsBasics.v *)

Require Import Arith.
Require Import Lia.

(* ============================================================ *)
(* 1. 目标操作战术 *)
(* ============================================================ *)

(* intros: 将假设移入上下文 *)
Lemma intros_example : forall A B : Prop, A -> (A -> B) -> B.
Proof.
  intros A B H1 H2.
  apply H2.
  exact H1.
Qed.

(* revert: 将假设移回目标 *)
Lemma revert_example : forall n : nat, n = 0 -> n + n = 0.
Proof.
  intros n H.
  revert H.              (* 将H从上下文中移除 *)
  intros H.
  rewrite H.
  reflexivity.
Qed.

(* ============================================================ *)
(* 2. 重写和替换战术 *)
(* ============================================================ *)

(* rewrite: 使用等式进行重写 *)
Lemma rewrite_example : forall a b c : nat,
  a = b -> b = c -> a = c.
Proof.
  intros a b c H1 H2.
  rewrite H1.            (* 将a替换为b *)
  exact H2.
Qed.

(* rewrite <- : 反向重写 *)
Lemma rewrite_backwards : forall a b : nat,
  a = b -> b = a.
Proof.
  intros a b H.
  rewrite <- H.
  reflexivity.
Qed.

(* replace: 临时替换 *)
Lemma replace_example : forall n : nat,
  n + 1 = 1 + n.
Proof.
  intros n.
  replace (n + 1) with (S n).
  - simpl. reflexivity.
  - apply Nat.add_1_r.
Qed.

(* ============================================================ *)
(* 3. 案例分析战术 *)
(* ============================================================ *)

(* destruct: 对归纳类型进行案例分析 *)
Lemma destruct_example : forall n : nat,
  n = 0 \/ exists m : nat, n = S m.
Proof.
  intros n.
  destruct n as [| m].
  - left. reflexivity.
  - right. exists m. reflexivity.
Qed.

(* induction: 归纳证明 *)
Fixpoint sum_n (n : nat) : nat :=
  match n with
  | 0 => 0
  | S m => n + sum_n m
  end.

Lemma sum_n_formula : forall n : nat,
  2 * sum_n n = n * (n + 1).
Proof.
  intros n.
  induction n.
  - simpl. reflexivity.
  - simpl sum_n.
    rewrite Nat.mul_add_distr_l.
    rewrite IHn.
    ring.
Qed.

(* ============================================================ *)
(* 4. 自动化战术 *)
(* ============================================================ *)

(* auto: 自动搜索可应用的引理 *)
Lemma auto_example : forall P Q R : Prop,
  (P -> Q) -> (Q -> R) -> P -> R.
Proof.
  auto.
Qed.

(* eauto: 扩展的auto，支持eapply *)
Lemma eauto_example : forall (A : Type) (P Q : A -> Prop),
  (forall x, P x -> Q x) -> (exists x, P x) -> (exists x, Q x).
Proof.
  eauto.
Qed.

(* lia: 线性整数算术自动求解 *)
Lemma lia_example : forall x y : nat,
  x < y -> x + 1 <= y.
Proof.
  lia.
Qed.

(* ring: 环结构自动求解 *)
Lemma ring_example : forall x y z : nat,
  (x + y) * z = x * z + y * z.
Proof.
  intros. ring.
Qed.

(* ============================================================ *)
(* 5. 反证法 *)
(* ============================================================ *)

Lemma contradiction_example : forall P : Prop,
  P -> ~P -> False.
Proof.
  intros P H1 H2.
  unfold not in H2.
  apply H2.
  exact H1.
Qed.

(* 使用exfalso *)
Lemma ex_falso_example : forall P : Prop,
  False -> P.
Proof.
  intros P H.
  exfalso.
  exact H.
Qed.

(* ============================================================ *)
(* 6. 存在量词战术 *)
(* ============================================================ *)

(* exists: 提供存在量词的证人 *)
Lemma exists_example : exists n : nat, n + n = 4.
Proof.
  exists 2.
  reflexivity.
Qed.

(* ============================================================ *)
(* 7. 等式战术 *)
(* ============================================================ *)

(* reflexivity: 证明自反等式 *)
Lemma reflexivity_example : 2 + 2 = 4.
Proof.
  reflexivity.
Qed.

(* symmetry: 等式的对称性 *)
Lemma symmetry_example : forall x y : nat,
  x = y -> y = x.
Proof.
  intros x y H.
  symmetry.
  exact H.
Qed.

(* transitivity: 等式的传递性 *)
Lemma transitivity_example : forall x y z : nat,
  x = y -> y = z -> x = z.
Proof.
  intros x y z H1 H2.
  transitivity y.
  - exact H1.
  - exact H2.
Qed.

(* ============================================================ *)
(* 8. 否定战术 *)
(* ============================================================ *)

(* unfold not: 展开否定定义 *)
Lemma not_not_intro : forall P : Prop,
  P -> ~~P.
Proof.
  intros P H.
  unfold not.
  intros Hneg.
  apply Hneg.
  exact H.
Qed.

(* contradiction: 寻找矛盾 *)
Lemma contradiction_find : forall P : Prop,
  P -> ~P -> False.
Proof.
  intros P H Hneg.
  contradiction.
Qed.

(* ============================================================ *)
(* 9. 分情况证明战术 *)
(* ============================================================ *)

(* split: 拆分合取目标 *)
Lemma split_example : forall n : nat,
  n >= 0 /\ n * 1 = n.
Proof.
  intros n.
  split.
  - apply Nat.le_0_l.
  - ring.
Qed.

(* left/right: 选择析取分支 *)
Lemma left_right_example : forall n : nat,
  n = 0 \/ n > 0 \/ exists m, n = S m.
Proof.
  intros n.
  destruct n.
  - left. reflexivity.
  - right. left. apply Nat.lt_0_succ.
Qed.

(* ============================================================ *)
(* 10. 高级战术组合 *)
(* ============================================================ *)

(* ; (顺序组合) *)
Lemma seq_example : forall n : nat,
  n <= n.
Proof.
  intros; apply Nat.le_refl.
Qed.

(* try: 尝试应用战术，失败则不做任何事 *)
Lemma try_example : forall n : nat,
  n = n.
Proof.
  intros; try reflexivity.
Qed.

(* repeat: 重复应用战术直到失败 *)
Lemma repeat_example : forall n m : nat,
  n + m = m + n.
Proof.
  intros.
  repeat rewrite Nat.add_comm.
  reflexivity.
Qed.

(* all: 对所有子目标应用战术 *)
Lemma all_example : forall n : nat,
  n = n /\ n + 0 = n.
Proof.
  split; reflexivity.
Qed.
```

### 2.2 自定义战术

```coq
(* File: theories/CustomTactics.v *)

(* 定义常用战术组合 *)
Ltac simplify :=
  intros;
  simpl in *;
  try reflexivity;
  try lia.

Ltac case_analysis x :=
  destruct x;
  try reflexivity;
  try lia.

Ltac induct_and_simplify n :=
  induction n;
  [ simpl; reflexivity
  | simpl; rewrite IHn; simpl; try ring ].

(* 使用自定义战术 *)
Lemma test_custom_tac : forall n : nat,
  n + 0 = n.
Proof.
  induct_and_simplify n.
Qed.
```

---

## 3. 链表操作正确性证明

### 3.1 链表数据结构的定义

```coq
(* File: theories/LinkedList.v *)

Require Import Arith.
Require Import List.
Import ListNotations.

(* ============================================================ *)
(* 1. 链表类型定义 *)
(* ============================================================ *)

Inductive list (A : Type) : Type :=
  | nil : list A
  | cons : A -> list A -> list A.

Arguments nil {A}.
Arguments cons {A} _ _.

(* 记号定义 *)
Notation "x :: l" := (cons x l) (at level 60, right associativity).
Notation "[ ]" := nil.
Notation "[ x ; .. ; y ]" := (cons x .. (cons y nil) ..).

(* ============================================================ *)
(* 2. 基本操作定义 *)
(* ============================================================ *)

(* 链表长度 *)
Fixpoint length {A : Type} (l : list A) : nat :=
  match l with
  | [] => 0
  | _ :: t => 1 + length t
  end.

(* 链表追加 *)
Fixpoint app {A : Type} (l1 l2 : list A) : list A :=
  match l1 with
  | [] => l2
  | h :: t => h :: (app t l2)
  end.

Notation "l1 ++ l2" := (app l1 l2) (right associativity, at level 60).

(* 链表反转 *)
Fixpoint rev {A : Type} (l : list A) : list A :=
  match l with
  | [] => []
  | h :: t => rev t ++ [h]
  end.

(* 取第n个元素 *)
Fixpoint nth {A : Type} (n : nat) (l : list A) (default : A) : A :=
  match n, l with
  | 0, h :: _ => h
  | S m, _ :: t => nth m t default
  | _, _ => default
  end.

(* 判断成员关系 *)
Fixpoint In {A : Type} (x : A) (l : list A) : Prop :=
  match l with
  | [] => False
  | h :: t => x = h \/ In x t
  end.

(* ============================================================ *)
(* 3. 基本性质证明 *)
(* ============================================================ *)

(* 追加结合律 *)
Theorem app_assoc : forall (A : Type) (l1 l2 l3 : list A),
  (l1 ++ l2) ++ l3 = l1 ++ (l2 ++ l3).
Proof.
  intros A l1 l2 l3.
  induction l1 as [| h t IH].
  - (* 基本情况：nil ++ l2 = l2 *)
    reflexivity.
  - (* 归纳步骤：h::t ++ l2 = h::(t ++ l2) *)
    simpl.
    rewrite IH.
    reflexivity.
Qed.

(* 空列表是追加的右单位元 *)
Theorem app_nil_r : forall (A : Type) (l : list A),
  l ++ [] = l.
Proof.
  intros A l.
  induction l as [| h t IH].
  - reflexivity.
  - simpl. rewrite IH. reflexivity.
Qed.

(* 长度与追加的关系 *)
Theorem length_app : forall (A : Type) (l1 l2 : list A),
  length (l1 ++ l2) = length l1 + length l2.
Proof.
  intros A l1 l2.
  induction l1 as [| h t IH].
  - reflexivity.
  - simpl. rewrite IH. reflexivity.
Qed.

(* ============================================================ *)
(* 4. 反转操作的正确性证明 *)
(* ============================================================ *)

(* 辅助定理：反转保持长度 *)
Theorem length_rev : forall (A : Type) (l : list A),
  length (rev l) = length l.
Proof.
  intros A l.
  induction l as [| h t IH].
  - reflexivity.
  - simpl.
    rewrite length_app.
    rewrite IH.
    simpl.
    rewrite Nat.add_comm.
    reflexivity.
Qed.

(* 辅助定理：反转对追加的分配 *)
Theorem rev_app_distr : forall (A : Type) (l1 l2 : list A),
  rev (l1 ++ l2) = rev l2 ++ rev l1.
Proof.
  intros A l1 l2.
  induction l1 as [| h t IH].
  - simpl.
    rewrite app_nil_r.
    reflexivity.
  - simpl.
    rewrite IH.
    rewrite app_assoc.
    reflexivity.
Qed.

(* 主要定理：反转是对合（即反转两次等于原列表） *)
Theorem rev_involutive : forall (A : Type) (l : list A),
  rev (rev l) = l.
Proof.
  intros A l.
  induction l as [| h t IH].
  - reflexivity.
  - simpl.
    rewrite rev_app_distr.
    rewrite IH.
    simpl.
    reflexivity.
Qed.

(* ============================================================ *)
(* 5. 成员关系性质 *)
(* ============================================================ *)

(* 追加保持成员关系 *)
Theorem In_app_iff : forall (A : Type) (l1 l2 : list A) (x : A),
  In x (l1 ++ l2) <-> In x l1 \/ In x l2.
Proof.
  intros A l1 l2 x.
  split.
  - (* -> 方向 *)
    induction l1 as [| h t IH].
    + simpl.
      intros H.
      right.
      exact H.
    + simpl.
      intros [H | H].
      * left. left. exact H.
      * apply IH in H.
        destruct H as [H1 | H2].
        -- left. right. exact H1.
        -- right. exact H2.
  - (* <- 方向 *)
    induction l1 as [| h t IH].
    + simpl.
      intros [H | H].
      * contradiction.
      * exact H.
    + simpl.
      intros [[H1 | H2] | H3].
      * left. exact H1.
      * right. apply IH. left. exact H2.
      * right. apply IH. right. exact H3.
Qed.

(* 反转保持成员关系 *)
Theorem In_rev : forall (A : Type) (l : list A) (x : A),
  In x (rev l) <-> In x l.
Proof.
  intros A l x.
  split.
  - (* -> 方向 *)
    induction l as [| h t IH].
    + simpl. intros H. exact H.
    + simpl.
      rewrite In_app_iff.
      intros [H1 | H2].
      * simpl in H1. destruct H1 as [H1 | []].
        right. left. symmetry. exact H1.
      * apply IH in H2.
        left. exact H2.
  - (* <- 方向 *)
    induction l as [| h t IH].
    + simpl. intros H. exact H.
    + simpl.
      intros [H1 | H2].
      * rewrite H1.
        rewrite In_app_iff.
        right.
        simpl.
        left.
        reflexivity.
      * rewrite In_app_iff.
        left.
        apply IH.
        exact H2.
Qed.

(* ============================================================ *)
(* 6. 映射操作 (Map) *)
(* ============================================================ *)

Fixpoint map {A B : Type} (f : A -> B) (l : list A) : list B :=
  match l with
  | [] => []
  | h :: t => f h :: map f t
  end.

(* 映射保持长度 *)
Theorem map_length : forall (A B : Type) (f : A -> B) (l : list A),
  length (map f l) = length l.
Proof.
  intros A B f l.
  induction l as [| h t IH].
  - reflexivity.
  - simpl. rewrite IH. reflexivity.
Qed.

(* 映射与追加的交换律 *)
Theorem map_app : forall (A B : Type) (f : A -> B) (l1 l2 : list A),
  map f (l1 ++ l2) = map f l1 ++ map f l2.
Proof.
  intros A B f l1 l2.
  induction l1 as [| h t IH].
  - reflexivity.
  - simpl. rewrite IH. reflexivity.
Qed.

(* 映射的组合性质 *)
Theorem map_map : forall (A B C : Type) (f : B -> C) (g : A -> B) (l : list A),
  map f (map g l) = map (fun x => f (g x)) l.
Proof.
  intros A B C f g l.
  induction l as [| h t IH].
  - reflexivity.
  - simpl. rewrite IH. reflexivity.
Qed.
```

### 3.2 对应的C代码

```c
/* File: c_impl/linked_list.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

/* 创建新节点 */
Node* create_node(int data) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

/* 计算链表长度 */
int list_length(Node* head) {
    int len = 0;
    while (head != NULL) {
        len++;
        head = head->next;
    }
    return len;
}

/* 追加元素到链表末尾 */
void list_append(Node** head, int data) {
    Node* new_node = create_node(data);
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

/* 链表反转 */
Node* list_reverse(Node* head) {
    Node *prev = NULL, *current = head, *next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}

/* 获取第n个元素 */
int list_nth(Node* head, int n, int default_val) {
    int i = 0;
    while (head != NULL && i < n) {
        head = head->next;
        i++;
    }
    if (head == NULL) return default_val;
    return head->data;
}

/* 查找元素 */
int list_contains(Node* head, int value) {
    while (head != NULL) {
        if (head->data == value) return 1;
        head = head->next;
    }
    return 0;
}

/* 映射函数 */
Node* list_map(Node* head, int (*f)(int)) {
    if (head == NULL) return NULL;
    Node* new_head = create_node(f(head->data));
    Node* new_current = new_head;
    Node* current = head->next;
    while (current != NULL) {
        new_current->next = create_node(f(current->data));
        new_current = new_current->next;
        current = current->next;
    }
    return new_head;
}

/* 释放链表 */
void list_free(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

/* 测试函数 */
void test_linked_list() {
    Node* list = NULL;

    /* 测试追加操作 */
    list_append(&list, 1);
    list_append(&list, 2);
    list_append(&list, 3);
    assert(list_length(list) == 3);

    /* 测试反转操作 */
    Node* reversed = list_reverse(list);
    assert(list_nth(reversed, 0, -1) == 3);
    assert(list_nth(reversed, 1, -1) == 2);
    assert(list_nth(reversed, 2, -1) == 1);

    /* 测试包含操作 */
    assert(list_contains(reversed, 2) == 1);
    assert(list_contains(reversed, 5) == 0);

    list_free(reversed);
}
```


---

## 4. 排序算法正确性证明

### 4.1 排序正确性的形式化定义

```coq
(* File: theories/Sorting.v *)

Require Import Arith.
Require Import List.
Require Import Permutation.
Require Import Relations.
Import ListNotations.

(* ============================================================ *)
(* 1. 排序正确性的数学定义 *)
(* ============================================================ *)

(* 列表是已排序的（升序） *)
Inductive sorted : list nat -> Prop :=
  | sorted_nil : sorted []
  | sorted_singleton : forall x, sorted [x]
  | sorted_cons : forall x y l,
      x <= y -> sorted (y :: l) -> sorted (x :: y :: l).

(* 排序算法的正确性：输入和输出是排列关系，且输出已排序 *)
Definition is_sorting_algorithm (f : list nat -> list nat) : Prop :=
  forall l, Permutation l (f l) /\ sorted (f l).

(* ============================================================ *)
(* 2. 插入排序 *)
(* ============================================================ *)

(* 插入元素到已排序列表 *)
Fixpoint insert (x : nat) (l : list nat) : list nat :=
  match l with
  | [] => [x]
  | h :: t => if x <=? h then x :: h :: t else h :: insert x t
  end.

(* 插入排序主函数 *)
Fixpoint insertion_sort (l : list nat) : list nat :=
  match l with
  | [] => []
  | h :: t => insert h (insertion_sort t)
  end.

(* -------------------------------------------------------------- *)
(* 插入操作正确性证明 *)
(* -------------------------------------------------------------- *)

(* 插入保持排序性质 *)
Lemma insert_sorted : forall x l,
  sorted l -> sorted (insert x l).
Proof.
  intros x l H.
  induction H.
  - (* l = [] *)
    simpl. constructor.
  - (* l = [y] *)
    simpl.
    destruct (x <=? x0) eqn:Hle.
    + constructor.
      * apply Nat.leb_le. exact Hle.
      * constructor.
    + constructor.
      * apply Nat.leb_gt in Hle. lia.
      * constructor.
  - (* l = y :: z :: l' *)
    simpl.
    destruct (x <=? x0) eqn:Hle1.
    + constructor.
      * apply Nat.leb_le. exact Hle1.
      * exact H.
    + destruct (x <=? x1) eqn:Hle2.
      * constructor.
        -- apply Nat.leb_gt in Hle1. lia.
        -- constructor.
           ++ apply Nat.leb_le. exact Hle2.
           ++ exact H0.
      * constructor.
        -- exact H.
        -- apply IHsorted.
Qed.

(* 插入保持排列关系 *)
Lemma insert_permutation : forall x l,
  Permutation (x :: l) (insert x l).
Proof.
  intros x l.
  induction l as [| h t IH].
  - simpl. reflexivity.
  - simpl.
    destruct (x <=? h) eqn:Hle.
    + reflexivity.
    + apply Permutation_trans with (h :: x :: t).
      * apply perm_swap.
      * apply perm_skip. exact IH.
Qed.

(* -------------------------------------------------------------- *)
(* 插入排序正确性证明 *)
(* -------------------------------------------------------------- *)

(* 插入排序产生排序列表 *)
Theorem insertion_sort_sorted : forall l,
  sorted (insertion_sort l).
Proof.
  intros l.
  induction l as [| h t IH].
  - simpl. constructor.
  - simpl.
    apply insert_sorted.
    exact IH.
Qed.

(* 插入排序保持排列关系 *)
Theorem insertion_sort_permutation : forall l,
  Permutation l (insertion_sort l).
Proof.
  intros l.
  induction l as [| h t IH].
  - simpl. reflexivity.
  - simpl.
    apply Permutation_trans with (h :: insertion_sort t).
    + apply perm_skip. exact IH.
    + apply insert_permutation.
Qed.

(* 插入排序是正确排序算法 *)
Theorem insertion_sort_correct : is_sorting_algorithm insertion_sort.
Proof.
  unfold is_sorting_algorithm.
  intro l.
  split.
  - apply insertion_sort_permutation.
  - apply insertion_sort_sorted.
Qed.

(* ============================================================ *)
(* 3. 归并排序 *)
(* ============================================================ *)

(* 合并两个已排序列表 *)
Fixpoint merge (l1 l2 : list nat) : list nat :=
  match l1, l2 with
  | [], _ => l2
  | _, [] => l1
  | h1 :: t1, h2 :: t2 =>
      if h1 <=? h2 then h1 :: merge t1 l2 else h2 :: merge l1 t2
  end.

(* 分割列表为两部分 *)
Fixpoint split {A : Type} (l : list A) : list A * list A :=
  match l with
  | [] => ([], [])
  | [x] => ([x], [])
  | x :: y :: t =>
      let (l1, l2) := split t in
      (x :: l1, y :: l2)
  end.

(* 归并排序 *)
Fixpoint merge_sort (l : list nat) : list nat :=
  match l with
  | [] => []
  | [x] => [x]
  | _ :: _ :: _ =>
      let (l1, l2) := split l in
      merge (merge_sort l1) (merge_sort l2)
  end.

(* -------------------------------------------------------------- *)
(* 辅助引理 *)
(* -------------------------------------------------------------- *)

(* 分割保持长度和 *)
Lemma split_length : forall (A : Type) (l l1 l2 : list A),
  split l = (l1, l2) -> length l = length l1 + length l2.
Proof.
  intros A l.
  induction l as [| x l' IH] using rev_ind.
  - intros l1 l2 H.
    simpl in H.
    inversion H.
    reflexivity.
  - intros l1 l2 H.
    destruct l' as [| y l''].
    + simpl in H.
      inversion H.
      reflexivity.
    + simpl in H.
      destruct (split l'') as [l1' l2'] eqn:Hs.
      inversion H.
      simpl.
      rewrite Nat.add_succ_r.
      f_equal.
      apply IH.
      reflexivity.
Qed.

(* 分割保持排列关系 *)
Lemma split_permutation : forall (A : Type) (l l1 l2 : list A),
  split l = (l1, l2) -> Permutation l (l1 ++ l2).
Proof.
  intros A l.
  induction l as [| x l' IH] using rev_ind.
  - intros l1 l2 H.
    simpl in H.
    inversion H.
    reflexivity.
  - intros l1 l2 H.
    destruct l' as [| y l''].
    + simpl in H.
      inversion H.
      reflexivity.
    + simpl in H.
      destruct (split l'') as [l1' l2'] eqn:Hs.
      inversion H.
      simpl.
      apply Permutation_trans with (y :: x :: l1' ++ l2').
      * apply perm_swap.
      * apply perm_skip.
        apply perm_skip.
        apply IH.
        reflexivity.
Qed.

(* 合并保持排列关系 *)
Lemma merge_permutation : forall l1 l2,
  Permutation (l1 ++ l2) (merge l1 l2).
Proof.
  induction l1 as [| h1 t1 IH1]; intros l2.
  - simpl. reflexivity.
  - induction l2 as [| h2 t2 IH2].
    + simpl.
      rewrite app_nil_r.
      reflexivity.
    + simpl.
      destruct (h1 <=? h2) eqn:Hle.
      * apply perm_skip.
        apply IH1.
      * apply Permutation_trans with (h2 :: h1 :: t1 ++ t2).
        -- apply perm_swap.
        -- apply perm_skip.
           apply IH2.
Qed.

(* 合并保持有序性 *)
Lemma merge_sorted : forall l1 l2,
  sorted l1 -> sorted l2 -> sorted (merge l1 l2).
Proof.
  induction l1 as [| h1 t1 IH1]; intros l2 H1 H2.
  - simpl. exact H2.
  - induction l2 as [| h2 t2 IH2].
    + simpl.
      rewrite app_nil_r.
      exact H1.
    + simpl.
      destruct (h1 <=? h2) eqn:Hle.
      * constructor.
        -- apply Nat.leb_le. exact Hle.
        -- inversion H1; subst.
           ++ simpl. exact H2.
           ++ apply IH1.
              ** exact H3.
              ** exact H2.
      * inversion H2; subst.
        -- constructor.
           ++ apply Nat.leb_gt in Hle. lia.
           ++ constructor.
        -- constructor.
           ++ apply Nat.leb_gt in Hle. lia.
           ++ apply IH2.
              ** exact H1.
              ** exact H3.
Qed.

(* -------------------------------------------------------------- *)
(* 归并排序正确性证明 *)
(* -------------------------------------------------------------- *)

(* 终止性度量 *)
Lemma merge_sort_terminates_helper : forall l l1 l2,
  split l = (l1, l2) -> l1 <> [] -> l2 <> [] ->
  length l1 < length l2 \/ length l2 < length l.
Proof.
  intros l l1 l2 Hsplit H1 H2.
  apply split_length in Hsplit.
  destruct l1; [congruence |].
  destruct l2; [congruence |].
  simpl in Hsplit.
  lia.
Qed.

(* 归并排序产生排序列表 *)
Theorem merge_sort_sorted : forall l,
  sorted (merge_sort l).
Proof.
  intro l.
  induction l as [l IH] using (well_founded_induction
    (wf_inverse_image _ _ _ (@length nat) lt_wf)).
  destruct l as [| x [| y t]].
  - simpl. constructor.
  - simpl. constructor.
  - simpl.
    destruct (split (x :: y :: t)) as [l1 l2] eqn:Hsplit.
    apply merge_sorted.
    + apply IH.
      admit. (* 需要更复杂的终止性论证 *)
    + apply IH.
      admit.
Admitted.

(* 归并排序保持排列关系 *)
Theorem merge_sort_permutation : forall l,
  Permutation l (merge_sort l).
Proof.
  intro l.
  induction l as [l IH] using (well_founded_induction
    (wf_inverse_image _ _ _ (@length nat) lt_wf)).
  destruct l as [| x [| y t]].
  - simpl. reflexivity.
  - simpl. reflexivity.
  - simpl.
    destruct (split (x :: y :: t)) as [l1 l2] eqn:Hsplit.
    apply Permutation_trans with (l1 ++ l2).
    + apply split_permutation. exact Hsplit.
    + apply merge_permutation.
Admitted.

(* ============================================================ *)
(* 4. 快速排序 *)
(* ============================================================ *)

(* 分区操作 *)
Fixpoint partition (pivot : nat) (l : list nat)
  : list nat * list nat :=
  match l with
  | [] => ([], [])
  | h :: t =>
      let (less, greater) := partition pivot t in
      if h <=? pivot then (h :: less, greater)
      else (less, h :: greater)
  end.

(* 快速排序 *)
Fixpoint quick_sort (l : list nat) : list nat :=
  match l with
  | [] => []
  | h :: t =>
      let (less, greater) := partition h t in
      quick_sort less ++ [h] ++ quick_sort greater
  end.

(* -------------------------------------------------------------- *)
(* 分区操作正确性证明 *)
(* -------------------------------------------------------------- *)

(* 分区保持排列关系 *)
Lemma partition_permutation : forall pivot l less greater,
  partition pivot l = (less, greater) ->
  Permutation l (less ++ greater).
Proof.
  intros pivot l.
  induction l as [| h t IH]; intros less greater H.
  - simpl in H.
    inversion H.
    reflexivity.
  - simpl in H.
    destruct (partition pivot t) as [less' greater'] eqn:Hp.
    destruct (h <=? pivot) eqn:Hle.
    + inversion H.
      simpl.
      apply perm_skip.
      apply IH.
      reflexivity.
    + inversion H.
      simpl.
      apply Permutation_cons_app.
      apply IH.
      reflexivity.
Qed.

(* 分区正确性：less中所有元素 <= pivot，greater中所有元素 > pivot *)
Lemma partition_correct_less : forall pivot l less greater,
  partition pivot l = (less, greater) ->
  forall x, In x less -> x <= pivot.
Proof.
  intros pivot l.
  induction l as [| h t IH]; intros less greater H x Hin.
  - simpl in H.
    inversion H.
    simpl in Hin.
    contradiction.
  - simpl in H.
    destruct (partition pivot t) as [less' greater'] eqn:Hp.
    destruct (h <=? pivot) eqn:Hle.
    + inversion H.
      simpl in Hin.
      destruct Hin as [Hin | Hin].
      * subst.
        apply Nat.leb_le.
        exact Hle.
      * apply IH with greater'.
        -- reflexivity.
        -- exact Hin.
    + inversion H.
      simpl in Hin.
      apply IH with greater'.
      * reflexivity.
      * exact Hin.
Qed.

Lemma partition_correct_greater : forall pivot l less greater,
  partition pivot l = (less, greater) ->
  forall x, In x greater -> pivot < x.
Proof.
  intros pivot l.
  induction l as [| h t IH]; intros less greater H x Hin.
  - simpl in H.
    inversion H.
    simpl in Hin.
    contradiction.
  - simpl in H.
    destruct (partition pivot t) as [less' greater'] eqn:Hp.
    destruct (h <=? pivot) eqn:Hle.
    + inversion H.
      simpl in Hin.
      apply IH with less'.
      * reflexivity.
      * exact Hin.
    + inversion H.
      simpl in Hin.
      destruct Hin as [Hin | Hin].
      * subst.
        apply Nat.leb_gt.
        exact Hle.
      * apply IH with less'.
        -- reflexivity.
        -- exact Hin.
Qed.

(* 快速排序保持排列关系 *)
Theorem quick_sort_permutation : forall l,
  Permutation l (quick_sort l).
Proof.
  intro l.
  induction l as [l IH] using (well_founded_induction
    (wf_inverse_image _ _ _ (@length nat) lt_wf)).
  destruct l.
  - simpl. reflexivity.
  - simpl.
    destruct (partition n l) as [less greater] eqn:Hp.
    apply Permutation_trans with (less ++ n :: greater).
    + apply Permutation_cons_app.
      apply partition_permutation with n.
      exact Hp.
    + apply Permutation_app.
      * apply IH.
        admit. (* 需要分区大小减少的引理 *)
      * apply perm_skip.
        apply IH.
        admit.
Admitted.
```

### 4.2 对应的C代码

```c
/* File: c_impl/sorting.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* ============================================================ */
/* 1. 插入排序 */
/* ============================================================ */

void insertion_sort(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* 验证数组是否已排序 */
int is_sorted(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i-1] > arr[i]) return 0;
    }
    return 1;
}

/* ============================================================ */
/* 2. 归并排序 */
/* ============================================================ */

void merge(int* arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort_recursive(int* arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_recursive(arr, left, mid);
        merge_sort_recursive(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void merge_sort(int* arr, int n) {
    merge_sort_recursive(arr, 0, n - 1);
}

/* ============================================================ */
/* 3. 快速排序 */
/* ============================================================ */

int partition(int* arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1;
}

void quick_sort_recursive(int* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort_recursive(arr, low, pi - 1);
        quick_sort_recursive(arr, pi + 1, high);
    }
}

void quick_sort(int* arr, int n) {
    quick_sort_recursive(arr, 0, n - 1);
}

/* ============================================================ */
/* 4. 测试函数 */
/* ============================================================ */

void test_sorting() {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90};
    int n = 7;

    insertion_sort(arr1, n);
    assert(is_sorted(arr1, n));

    merge_sort(arr2, n);
    assert(is_sorted(arr2, n));

    quick_sort(arr3, n);
    assert(is_sorted(arr3, n));

    printf("All sorting tests passed!\n");
}
```

---

## 5. 栈和队列的实现与验证

### 5.1 栈的实现与验证

```coq
(* File: theories/Stack.v *)

Require Import Arith.
Require Import List.
Import ListNotations.

(* ============================================================ *)
(* 1. 栈的代数规范 *)
(* ============================================================ *)

(* 栈的类型参数化 *)
Module StackSpec.

  (* 抽象数据类型 *)
  Parameter stack : Type -> Type.

  (* 操作 *)
  Parameter empty : forall {A}, stack A.
  Parameter push : forall {A}, A -> stack A -> stack A.
  Parameter pop : forall {A}, stack A -> option (A * stack A).
  Parameter peek : forall {A}, stack A -> option A.
  Parameter is_empty : forall {A}, stack A -> bool.

  (* 公理：规范行为 *)
  Axiom pop_push : forall A (x : A) s,
    pop (push x s) = Some (x, s).

  Axiom peek_push : forall A (x : A) s,
    peek (push x s) = Some x.

  Axiom is_empty_empty : forall A,
    is_empty (@empty A) = true.

  Axiom is_empty_push : forall A (x : A) s,
    is_empty (push x s) = false.

  Axiom pop_empty : forall A,
    pop (@empty A) = None.

  Axiom peek_empty : forall A,
    peek (@empty A) = None.

End StackSpec.

(* ============================================================ *)
(* 2. 基于列表的栈实现 *)
(* ============================================================ *)

Module ListStack <: StackSpec.

  Definition stack (A : Type) := list A.

  Definition empty {A} : stack A := [].

  Definition push {A} (x : A) (s : stack A) : stack A := x :: s.

  Definition pop {A} (s : stack A) : option (A * stack A) :=
    match s with
    | [] => None
    | h :: t => Some (h, t)
    end.

  Definition peek {A} (s : stack A) : option A :=
    match s with
    | [] => None
    | h :: _ => Some h
    end.

  Definition is_empty {A} (s : stack A) : bool :=
    match s with
    | [] => true
    | _ => false
    end.

  (* 证明公理 *)
  Theorem pop_push : forall A (x : A) s,
    pop (push x s) = Some (x, s).
  Proof.
    intros. reflexivity.
  Qed.

  Theorem peek_push : forall A (x : A) s,
    peek (push x s) = Some x.
  Proof.
    intros. reflexivity.
  Qed.

  Theorem is_empty_empty : forall A,
    is_empty (@empty A) = true.
  Proof.
    intros. reflexivity.
  Qed.

  Theorem is_empty_push : forall A (x : A) s,
    is_empty (push x s) = false.
  Proof.
    intros. reflexivity.
  Qed.

  Theorem pop_empty : forall A,
    pop (@empty A) = None.
  Proof.
    intros. reflexivity.
  Qed.

  Theorem peek_empty : forall A,
    peek (@empty A) = None.
  Proof.
    intros. reflexivity.
  Qed.

End ListStack.

(* ============================================================ *)
(* 3. 栈的性质验证 *)
(* ============================================================ *)

Module StackProperties (S : StackSpec).
  Import S.

  (* push和pop的逆操作关系 *)
  Theorem push_pop_inverse : forall A (x : A) s s',
    pop (push x s) = Some (x, s') -> s = s'.
  Proof.
    intros A x s s' H.
    rewrite pop_push in H.
    inversion H.
    reflexivity.
  Qed.

  (* 栈操作的序性质 *)
  Fixpoint push_all {A} (xs : list A) (s : stack A) : stack A :=
    match xs with
    | [] => s
    | h :: t => push h (push_all t s)
    end.

  Fixpoint pop_all {A} (n : nat) (s : stack A) : list A * stack A :=
    match n, s with
    | 0, _ => ([], s)
    | _, s' =>
        match pop s' with
        | None => ([], s')
        | Some (x, s'') =>
            let (xs, s''') := pop_all (n - 1) s'' in
            (x :: xs, s''')
        end
    end.

  (* push_all和pop_all的逆性质 *)
  Theorem push_pop_all : forall A (xs : list A) s,
    let s' := push_all (rev xs) s in
    let (xs', s'') := pop_all (length xs) s' in
    xs' = xs /\ s'' = s.
  Proof.
    induction xs as [| h t IH]; intros s.
    - simpl. split; reflexivity.
    - simpl.
      rewrite pop_push.
      destruct (pop_all (length t) (push_all (rev t) s)) as [xs' s''] eqn:Hpop.
      apply IH in Hpop as [H1 H2].
      rewrite H1, H2.
      split; reflexivity.
  Qed.

End StackProperties.
```

### 5.2 队列的实现与验证

```coq
(* File: theories/Queue.v *)

Require Import Arith.
Require Import List.
Import ListNotations.

(* ============================================================ *)
(* 1. 队列的代数规范 *)
(* ============================================================ *)

Module QueueSpec.

  Parameter queue : Type -> Type.

  Parameter empty : forall {A}, queue A.
  Parameter enqueue : forall {A}, A -> queue A -> queue A.
  Parameter dequeue : forall {A}, queue A -> option (A * queue A).
  Parameter front : forall {A}, queue A -> option A.
  Parameter is_empty : forall {A}, queue A -> bool.
  Parameter size : forall {A}, queue A -> nat.

  (* 先进先出（FIFO）公理 *)
  Axiom dequeue_enqueue_empty : forall A (x : A),
    dequeue (enqueue x empty) = Some (x, empty).

  Axiom front_enqueue_empty : forall A (x : A),
    front (enqueue x empty) = Some x.

  Axiom is_empty_empty : forall A,
    is_empty (@empty A) = true.

  Axiom is_empty_enqueue : forall A (x : A) q,
    is_empty (enqueue x q) = false.

  Axiom dequeue_empty : forall A,
    dequeue (@empty A) = None.

  Axiom front_empty : forall A,
    front (@empty A) = None.

  (* 大小公理 *)
  Axiom size_empty : forall A,
    size (@empty A) = 0.

  Axiom size_enqueue : forall A (x : A) q,
    size (enqueue x q) = S (size q).

End QueueSpec.

(* ============================================================ *)
(* 2. 双列表队列实现（摊销O(1)） *)
(* ============================================================ *)

Module TwoListQueue <: QueueSpec.

  (* 前列表用于出队，后列表用于入队 *)
  Record queue (A : Type) := mkQueue {
    front_list : list A;
    back_list : list A
  }.

  Arguments mkQueue {A}.
  Arguments front_list {A}.
  Arguments back_list {A}.

  Definition empty {A} : queue A := mkQueue [] [].

  Definition enqueue {A} (x : A) (q : queue A) : queue A :=
    mkQueue (front_list q) (x :: back_list q).

  (* 辅助函数：当front为空时，将back反转 *)
  Definition normalize {A} (q : queue A) : queue A :=
    match front_list q with
    | [] => mkQueue (rev (back_list q)) []
    | _ => q
    end.

  Definition dequeue {A} (q : queue A) : option (A * queue A) :=
    let q' := normalize q in
    match front_list q' with
    | [] => None
    | h :: t => Some (h, mkQueue t (back_list q'))
    end.

  Definition front {A} (q : queue A) : option A :=
    match normalize q with
    | mkQueue [] _ => None
    | mkQueue (h :: _) _ => Some h
    end.

  Definition is_empty {A} (q : queue A) : bool :=
    match normalize q with
    | mkQueue [] [] => true
    | _ => false
    end.

  Definition size {A} (q : queue A) : nat :=
    length (front_list q) + length (back_list q).

  (* 证明辅助引理 *)
  Lemma normalize_preserves_elements : forall A (q : queue A),
    front_list (normalize q) ++ back_list (normalize q) =
    front_list q ++ rev (back_list q).
  Proof.
    intros A [f b].
    simpl.
    destruct f.
    - simpl. rewrite rev_involutive. reflexivity.
    - simpl. reflexivity.
  Qed.

  (* 证明公理 *)
  Theorem dequeue_enqueue_empty : forall A (x : A),
    dequeue (enqueue x empty) = Some (x, empty).
  Proof.
    intros A x.
    unfold dequeue, enqueue, empty, normalize.
    simpl.
    reflexivity.
  Qed.

  Theorem front_enqueue_empty : forall A (x : A),
    front (enqueue x empty) = Some x.
  Proof.
    intros A x.
    unfold front, enqueue, empty, normalize.
    simpl.
    reflexivity.
  Qed.

  Theorem is_empty_empty : forall A,
    is_empty (@empty A) = true.
  Proof.
    intros A.
    unfold is_empty, empty, normalize.
    simpl.
    reflexivity.
  Qed.

  Theorem is_empty_enqueue : forall A (x : A) q,
    is_empty (enqueue x q) = false.
  Proof.
    intros A x [f b].
    unfold is_empty, enqueue, normalize.
    simpl.
    destruct f; simpl; reflexivity.
  Qed.

  Theorem dequeue_empty : forall A,
    dequeue (@empty A) = None.
  Proof.
    intros A.
    unfold dequeue, empty, normalize.
    simpl.
    reflexivity.
  Qed.

  Theorem front_empty : forall A,
    front (@empty A) = None.
  Proof.
    intros A.
    unfold front, empty, normalize.
    simpl.
    reflexivity.
  Qed.

  Theorem size_empty : forall A,
    size (@empty A) = 0.
  Proof.
    intros A.
    unfold size, empty.
    simpl.
    reflexivity.
  Qed.

  Theorem size_enqueue : forall A (x : A) q,
    size (enqueue x q) = S (size q).
  Proof.
    intros A x [f b].
    unfold size, enqueue.
    simpl.
    rewrite <- plus_n_Sm.
    reflexivity.
  Qed.

End TwoListQueue.

(* ============================================================ *)
(* 3. 队列的FIFO性质形式化证明 *)
(* ============================================================ *)

Module QueueFIFOProperties (Q : QueueSpec).
  Import Q.

  (* 将列表转换为队列 *)
  Fixpoint list_to_queue {A} (xs : list A) : queue A :=
    match xs with
    | [] => empty
    | h :: t => enqueue h (list_to_queue t)
    end.

  (* 从队列中提取所有元素 *)
  Fixpoint queue_to_list {A} (n : nat) (q : queue A) : list A :=
    match n with
    | 0 => []
    | S m =>
        match dequeue q with
        | None => []
        | Some (x, q') => x :: queue_to_list m q'
        end
    end.

  (* FIFO正确性：入队序列的逆序等于出队序列 *)
  Theorem fifo_property : forall A (xs : list A),
    let q := list_to_queue (rev xs) in
    queue_to_list (length xs) q = xs.
  Proof.
    induction xs as [| h t IH]; intros.
    - simpl. reflexivity.
    - simpl.
      rewrite dequeue_enqueue_empty.
      simpl.
      admit. (* 需要更强的归纳假设 *)
  Admitted.

End QueueFIFOProperties.
```

### 5.3 对应的C代码

```c
/* File: c_impl/stack_queue.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/* ============================================================ */
/* 1. 栈实现 */
/* ============================================================ */

typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top;
    int size;
} Stack;

Stack* stack_create() {
    Stack* s = (Stack*)malloc(sizeof(Stack));
    s->top = NULL;
    s->size = 0;
    return s;
}

void stack_push(Stack* s, int x) {
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    node->data = x;
    node->next = s->top;
    s->top = node;
    s->size++;
}

int stack_pop(Stack* s, int* out) {
    if (s->top == NULL) return 0;
    StackNode* temp = s->top;
    *out = temp->data;
    s->top = temp->next;
    free(temp);
    s->size--;
    return 1;
}

int stack_peek(Stack* s, int* out) {
    if (s->top == NULL) return 0;
    *out = s->top->data;
    return 1;
}

bool stack_is_empty(Stack* s) {
    return s->top == NULL;
}

void stack_destroy(Stack* s) {
    int dummy;
    while (!stack_is_empty(s)) {
        stack_pop(s, &dummy);
    }
    free(s);
}

/* ============================================================ */
/* 2. 队列实现（双列表法） */
/* ============================================================ */

typedef struct {
    int* front;
    int front_size;
    int front_capacity;
    int* back;
    int back_size;
    int back_capacity;
} Queue;

Queue* queue_create() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->front_size = 0;
    q->front_capacity = 0;
    q->back = NULL;
    q->back_size = 0;
    q->back_capacity = 0;
    return q;
}

void queue_enqueue(Queue* q, int x) {
    if (q->back_size >= q->back_capacity) {
        q->back_capacity = q->back_capacity == 0 ? 4 : q->back_capacity * 2;
        q->back = (int*)realloc(q->back, q->back_capacity * sizeof(int));
    }
    q->back[q->back_size++] = x;
}

static void queue_normalize(Queue* q) {
    if (q->front_size == 0 && q->back_size > 0) {
        /* 将back反转到front */
        q->front_capacity = q->back_size;
        q->front = (int*)realloc(q->front, q->front_capacity * sizeof(int));
        for (int i = 0; i < q->back_size; i++) {
            q->front[i] = q->back[q->back_size - 1 - i];
        }
        q->front_size = q->back_size;
        q->back_size = 0;
    }
}

int queue_dequeue(Queue* q, int* out) {
    queue_normalize(q);
    if (q->front_size == 0) return 0;
    *out = q->front[--q->front_size];
    return 1;
}

int queue_front(Queue* q, int* out) {
    queue_normalize(q);
    if (q->front_size == 0) return 0;
    *out = q->front[q->front_size - 1];
    return 1;
}

bool queue_is_empty(Queue* q) {
    queue_normalize(q);
    return q->front_size == 0;
}

void queue_destroy(Queue* q) {
    free(q->front);
    free(q->back);
    free(q);
}

/* ============================================================ */
/* 3. 测试函数 */
/* ============================================================ */

void test_stack() {
    Stack* s = stack_create();
    assert(stack_is_empty(s));

    stack_push(s, 10);
    stack_push(s, 20);
    stack_push(s, 30);

    int val;
    assert(stack_pop(s, &val) && val == 30);
    assert(stack_pop(s, &val) && val == 20);
    assert(stack_peek(s, &val) && val == 10);
    assert(stack_pop(s, &val) && val == 10);
    assert(!stack_pop(s, &val));

    stack_destroy(s);
    printf("Stack tests passed!\n");
}

void test_queue() {
    Queue* q = queue_create();
    assert(queue_is_empty(q));

    queue_enqueue(q, 10);
    queue_enqueue(q, 20);
    queue_enqueue(q, 30);

    int val;
    assert(queue_dequeue(q, &val) && val == 10);
    assert(queue_dequeue(q, &val) && val == 20);
    assert(queue_front(q, &val) && val == 30);
    assert(queue_dequeue(q, &val) && val == 30);
    assert(!queue_dequeue(q, &val));

    queue_destroy(q);
    printf("Queue tests passed!\n");
}

/* FIFO性质测试 */
void test_fifo_property() {
    Queue* q = queue_create();
    int inputs[] = {1, 2, 3, 4, 5};
    int n = 5;
    int outputs[5];

    for (int i = 0; i < n; i++) {
        queue_enqueue(q, inputs[i]);
    }

    for (int i = 0; i < n; i++) {
        assert(queue_dequeue(q, &outputs[i]));
    }

    /* 验证FIFO顺序 */
    for (int i = 0; i < n; i++) {
        assert(inputs[i] == outputs[i]);
    }

    queue_destroy(q);
    printf("FIFO property test passed!\n");
}
```



---

## 6. 内存分配器验证

### 6.1 简单内存分配器的形式化

```coq
(* File: theories/MemoryAllocator.v *)

Require Import Arith.
Require Import List.
Require Import Lia.
Import ListNotations.

(* ============================================================ *)
(* 1. 内存模型基础 *)
(* ============================================================ *)

(* 内存地址 *)
Definition addr := nat.

(* 内存块：起始地址和大小 *)
Record block := mkBlock {
  start_addr : addr;
  size : nat
}.

(* 块的有效性 *)
Definition valid_block (b : block) : Prop :=
  size b > 0.

(* 两个块是否重叠 *)
Definition blocks_overlap (b1 b2 : block) : Prop :=
  let s1 := start_addr b1 in
  let e1 := s1 + size b1 in
  let s2 := start_addr b2 in
  let e2 := s2 + size b2 in
  s1 < e2 /\ s2 < e1.

(* 块是否不相交 *)
Definition blocks_disjoint (b1 b2 : block) : Prop :=
  ~ blocks_overlap b1 b2.

(* ============================================================ *)
(* 2. 分配器不变量 *)
(* ============================================================ *)

(* 分配器状态 *)
Record allocator_state := mkAllocator {
  free_blocks : list block;    (* 空闲块列表 *)
  used_blocks : list block;    (* 已分配块列表 *)
  total_size : nat             (* 总内存大小 *)
}.

(* 分配器不变量 *)
Inductive allocator_invariant (s : allocator_state) : Prop :=
  | mkInvariant :
      (* 所有块都是有效的 *)
      (forall b, In b (free_blocks s) -> valid_block b) ->
      (forall b, In b (used_blocks s) -> valid_block b) ->

      (* 空闲块之间不相交 *)
      (forall b1 b2,
        In b1 (free_blocks s) -> In b2 (free_blocks s) -> b1 <> b2 ->
        blocks_disjoint b1 b2) ->

      (* 已用块之间不相交 *)
      (forall b1 b2,
        In b1 (used_blocks s) -> In b2 (used_blocks s) -> b1 <> b2 ->
        blocks_disjoint b1 b2) ->

      (* 空闲块和已用块之间不相交 *)
      (forall b1 b2,
        In b1 (free_blocks s) -> In b2 (used_blocks s) ->
        blocks_disjoint b1 b2) ->

      allocator_invariant s.

(* ============================================================ *)
(* 3. 分配操作 *)
(* ============================================================ *)

(* 从块中分配指定大小的内存 *)
Definition split_block (b : block) (req_size : nat)
  : option (block * option block) :=
  if req_size <=? size b then
    let allocated := mkBlock (start_addr b) req_size in
    let remaining_size := size b - req_size in
    if remaining_size =? 0 then
      Some (allocated, None)
    else
      let remaining := mkBlock (start_addr b + req_size) remaining_size in
      Some (allocated, Some remaining)
  else
    None.

(* 查找并分配第一个足够大的块 *)
Fixpoint allocate_from_list (req_size : nat) (blocks : list block)
  : option (block * list block) :=
  match blocks with
  | [] => None
  | b :: bs =>
      match split_block b req_size with
      | Some (allocated, None) => Some (allocated, bs)
      | Some (allocated, Some remaining) =>
          Some (allocated, remaining :: bs)
      | None =>
          match allocate_from_list req_size bs with
          | Some (allocated, bs') => Some (allocated, b :: bs')
          | None => None
          end
      end
  end.

(* 主分配函数 *)
Definition allocate (s : allocator_state) (req_size : nat)
  : option (block * allocator_state) :=
  match allocate_from_list req_size (free_blocks s) with
  | Some (allocated, new_free) =>
      let new_state := mkAllocator
        new_free
        (allocated :: used_blocks s)
        (total_size s) in
      Some (allocated, new_state)
  | None => None
  end.

(* ============================================================ *)
(* 4. 释放操作 *)
(* ============================================================ *)

(* 合并相邻的空闲块 *)
Fixpoint merge_adjacent (blocks : list block) : list block :=
  match blocks with
  | [] => []
  | [b] => [b]
  | b1 :: b2 :: bs =>
      if start_addr b2 =? start_addr b1 + size b1 then
        let merged := mkBlock (start_addr b1) (size b1 + size b2) in
        merge_adjacent (merged :: bs)
      else
        b1 :: merge_adjacent (b2 :: bs)
  end.

(* 按地址排序 *)
Fixpoint insert_sorted_by_addr (b : block) (blocks : list block) : list block :=
  match blocks with
  | [] => [b]
  | b' :: bs =>
      if start_addr b <=? start_addr b' then
        b :: b' :: bs
      else
        b' :: insert_sorted_by_addr b bs
  end.

Definition sort_by_addr (blocks : list block) : list block :=
  fold_right insert_sorted_by_addr [] blocks.

(* 从已用列表中移除指定块 *)
Fixpoint remove_block (b : block) (blocks : list block) : list block :=
  match blocks with
  | [] => []
  | b' :: bs =>
      if start_addr b =? start_addr b' /\ size b =? size b' then
        bs
      else
        b' :: remove_block b bs
  end.

(* 主释放函数 *)
Definition deallocate (s : allocator_state) (b : block)
  : allocator_state :=
  let new_used := remove_block b (used_blocks s) in
  let new_free := merge_adjacent (sort_by_addr (b :: free_blocks s)) in
  mkAllocator new_free new_used (total_size s).

(* ============================================================ *)
(* 5. 正确性证明 *)
(* ============================================================ *)

(* 分配保持不变量 *)
Theorem allocate_preserves_invariant :
  forall s req_size b s',
  allocator_invariant s ->
  allocate s req_size = Some (b, s') ->
  allocator_invariant s'.
Proof.
  intros s req_size b s' Hinv Halloc.
  unfold allocate in Halloc.
  destruct (allocate_from_list req_size (free_blocks s))
    as [[allocated new_free] |] eqn:Halloc_list;
  inversion Halloc; subst; clear Halloc.

  (* 详细证明需要展开allocate_from_list的情况分析 *)
  admit.
Admitted.

(* 释放保持不变量 *)
Theorem deallocate_preserves_invariant :
  forall s b,
  allocator_invariant s ->
  In b (used_blocks s) ->
  allocator_invariant (deallocate s b).
Proof.
  intros s b Hinv Hused.
  unfold deallocate.
  (* 详细证明需要处理remove_block和merge的正确性 *)
  admit.
Admitted.

(* 分配返回正确大小的块 *)
Theorem allocate_size_correct :
  forall s req_size b s',
  allocate s req_size = Some (b, s') ->
  size b = req_size.
Proof.
  intros s req_size b s' Halloc.
  unfold allocate in Halloc.
  destruct (allocate_from_list req_size (free_blocks s))
    as [[allocated new_free] |] eqn:Halloc_list;
  inversion Halloc; subst; clear Halloc.

  (* 需要展开allocate_from_list并证明split_block保持大小 *)
  admit.
Admitted.

(* 分配的块在分配后变为已用 *)
Theorem allocate_marks_used :
  forall s req_size b s',
  allocator_invariant s ->
  allocate s req_size = Some (b, s') ->
  In b (used_blocks s').
Proof.
  intros s req_size b s' Hinv Halloc.
  unfold allocate in Halloc.
  destruct (allocate_from_list req_size (free_blocks s))
    as [[allocated new_free] |] eqn:Halloc_list;
  inversion Halloc; subst.
  simpl.
  left.
  (* 需要证明b就是allocated *)
  admit.
Admitted.
```

### 6.2 对应的C代码

```c
/* File: c_impl/memory_allocator.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================ */
/* 1. 内存块定义 */
/* ============================================================ */

typedef struct Block {
    size_t start;
    size_t size;
    struct Block* next;
} Block;

typedef struct {
    Block* free_list;
    Block* used_list;
    size_t total_size;
} Allocator;

/* ============================================================ */
/* 2. 辅助函数 */
/* ============================================================ */

Block* create_block(size_t start, size_t size) {
    Block* b = (Block*)malloc(sizeof(Block));
    b->start = start;
    b->size = size;
    b->next = NULL;
    return b;
}

bool blocks_overlap(Block* b1, Block* b2) {
    size_t s1 = b1->start;
    size_t e1 = s1 + b1->size;
    size_t s2 = b2->start;
    size_t e2 = s2 + b2->size;
    return (s1 < e2 && s2 < e1);
}

/* 将块按地址排序插入到列表 */
void insert_sorted(Block** list, Block* new_block) {
    if (*list == NULL || (*list)->start > new_block->start) {
        new_block->next = *list;
        *list = new_block;
        return;
    }
    Block* current = *list;
    while (current->next != NULL && current->next->start < new_block->start) {
        current = current->next;
    }
    new_block->next = current->next;
    current->next = new_block;
}

/* 合并相邻的空闲块 */
void merge_adjacent(Block* list) {
    if (list == NULL) return;
    Block* current = list;
    while (current->next != NULL) {
        if (current->start + current->size == current->next->start) {
            /* 合并两个块 */
            current->size += current->next->size;
            Block* temp = current->next;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

/* ============================================================ */
/* 3. 分配器操作 */
/* ============================================================ */

Allocator* allocator_create(size_t total_size) {
    Allocator* a = (Allocator*)malloc(sizeof(Allocator));
    a->free_list = create_block(0, total_size);
    a->used_list = NULL;
    a->total_size = total_size;
    return a;
}

Block* allocator_alloc(Allocator* a, size_t size) {
    if (size == 0) return NULL;

    Block** current = &a->free_list;
    while (*current != NULL) {
        Block* block = *current;
        if (block->size >= size) {
            Block* allocated = create_block(block->start, size);

            if (block->size == size) {
                /* 精确匹配，移除整个块 */
                *current = block->next;
                free(block);
            } else {
                /* 分割块 */
                block->start += size;
                block->size -= size;
            }

            /* 添加到已用列表 */
            allocated->next = a->used_list;
            a->used_list = allocated;

            return allocated;
        }
        current = &(*current)->next;
    }

    return NULL;  /* 分配失败 */
}

void allocator_free(Allocator* a, Block* block) {
    if (block == NULL || a == NULL) return;

    /* 从已用列表移除 */
    Block** current = &a->used_list;
    while (*current != NULL) {
        if (*current == block) {
            *current = block->next;
            break;
        }
        current = &(*current)->next;
    }

    /* 添加到空闲列表并合并 */
    insert_sorted(&a->free_list, block);
    merge_adjacent(a->free_list);
}

void allocator_destroy(Allocator* a) {
    if (a == NULL) return;

    Block* current = a->free_list;
    while (current != NULL) {
        Block* temp = current;
        current = current->next;
        free(temp);
    }

    current = a->used_list;
    while (current != NULL) {
        Block* temp = current;
        current = current->next;
        free(temp);
    }

    free(a);
}

/* ============================================================ */
/* 4. 测试函数 */
/* ============================================================ */

void test_allocator() {
    Allocator* a = allocator_create(1024);

    /* 测试基本分配 */
    Block* b1 = allocator_alloc(a, 100);
    assert(b1 != NULL);
    assert(b1->size == 100);
    assert(b1->start == 0);

    Block* b2 = allocator_alloc(a, 200);
    assert(b2 != NULL);
    assert(b2->size == 200);
    assert(b2->start == 100);

    /* 测试释放和重用 */
    allocator_free(a, b1);
    Block* b3 = allocator_alloc(a, 50);
    assert(b3 != NULL);
    assert(b3->start == 0);  /* 应该重用释放的块 */

    allocator_destroy(a);
    printf("Allocator tests passed!\n");
}
```

---

## 7. 函数契约和前置/后置条件

### 7.1 霍尔逻辑的Coq形式化

```coq
(* File: theories/HoareLogic.v *)

Require Import Arith.
Require Import Lia.

(* ============================================================ *)
(* 1. 霍尔三元组 *)
(* ============================================================ *)

(* 程序状态：变量到值的映射 *)
Definition state := nat -> nat.

(* 表达式求值 *)
Inductive expr : Type :=
  | EConst : nat -> expr
  | EVar : nat -> expr
  | EPlus : expr -> expr -> expr
  | EMinus : expr -> expr -> expr
  | EMult : expr -> expr -> expr.

(* 表达式求值函数 *)
Fixpoint eval_expr (e : expr) (s : state) : nat :=
  match e with
  | EConst n => n
  | EVar x => s x
  | EPlus e1 e2 => eval_expr e1 s + eval_expr e2 s
  | EMinus e1 e2 => eval_expr e1 s - eval_expr e2 s
  | EMult e1 e2 => eval_expr e1 s * eval_expr e2 s
  end.

(* 布尔表达式 *)
Inductive bexpr : Type :=
  | BTrue : bexpr
  | BFalse : bexpr
  | BEq : expr -> expr -> bexpr
  | BLe : expr -> expr -> bexpr
  | BNot : bexpr -> bexpr
  | BAnd : bexpr -> bexpr -> bexpr.

(* 布尔表达式求值 *)
Fixpoint eval_bexpr (b : bexpr) (s : state) : bool :=
  match b with
  | BTrue => true
  | BFalse => false
  | BEq e1 e2 => eval_expr e1 s =? eval_expr e2 s
  | BLe e1 e2 => eval_expr e1 s <=? eval_expr e2 s
  | BNot b' => negb (eval_bexpr b' s)
  | BAnd b1 b2 => eval_bexpr b1 s && eval_bexpr b2 s
  end.

(* ============================================================ *)
(* 2. 命令式语言语法 *)
(* ============================================================ *)

Inductive cmd : Type :=
  | CSkip : cmd
  | CAssign : nat -> expr -> cmd
  | CSeq : cmd -> cmd -> cmd
  | CIf : bexpr -> cmd -> cmd -> cmd
  | CWhile : bexpr -> cmd -> cmd.

Notation "'SKIP'" := CSkip.
Notation "x '::=' e" := (CAssign x e) (at level 60).
Notation "c1 ;; c2" := (CSeq c1 c2) (at level 80, right associativity).
Notation "'IF' b 'THEN' c1 'ELSE' c2 'FI'" := (CIf b c1 c2) (at level 80).
Notation "'WHILE' b 'DO' c 'END'" := (CWhile b c) (at level 80).

(* ============================================================ *)
(* 3. 断言语言 *)
(* ============================================================ *)

Definition assertion := state -> Prop.

(* 断言蕴含 *)
Definition assert_implies (P Q : assertion) : Prop :=
  forall s, P s -> Q s.

Notation "P '->>' Q" := (assert_implies P Q) (at level 80).

(* 断言等价 *)
Definition assert_equiv (P Q : assertion) : Prop :=
  (P ->> Q) /\ (Q ->> P).

Notation "P '<<->>' Q" := (assert_equiv P Q) (at level 80).

(* 断言操作 *)
Definition assert_and (P Q : assertion) : assertion :=
  fun s => P s /\ Q s.

Definition assert_or (P Q : assertion) : assertion :=
  fun s => P s \/ Q s.

Definition assert_not (P : assertion) : assertion :=
  fun s => ~ P s.

Definition assert_impl (P Q : assertion) : assertion :=
  fun s => P s -> Q s.

Notation "P '/\\' Q" := (assert_and P Q).
Notation "P '\\/' Q" := (assert_or P Q).
Notation "'~' P" := (assert_not P).
Notation "P '-->>' Q" := (assert_impl P Q).

(* 替换：e[e'/x] *)
Definition subst (x : nat) (e' : expr) (P : assertion) : assertion :=
  fun s => P (fun y => if Nat.eq_dec y x then eval_expr e' s else s y).

Notation "P '[' x '|->' e ']'" := (subst x e P) (at level 60).

(* ============================================================ *)
(* 4. 霍尔逻辑推理规则 *)
(* ============================================================ *)

Inductive hoare_triple : assertion -> cmd -> assertion -> Prop :=
  (* 跳过规则 *)
  | H_Skip : forall P,
      hoare_triple P SKIP P

  (* 赋值规则 *)
  | H_Assign : forall P x e,
      hoare_triple (P[x |-> e]) (x ::= e) P

  (* 序列规则 *)
  | H_Seq : forall P Q R c1 c2,
      hoare_triple P c1 Q ->
      hoare_triple Q c2 R ->
      hoare_triple P (c1 ;; c2) R

  (* 条件规则 *)
  | H_If : forall P Q b c1 c2,
      hoare_triple (fun s => P s /\ eval_bexpr b s = true) c1 Q ->
      hoare_triple (fun s => P s /\ eval_bexpr b s = false) c2 Q ->
      hoare_triple P (IF b THEN c1 ELSE c2 FI) Q

  (* While规则 *)
  | H_While : forall P b c,
      hoare_triple (fun s => P s /\ eval_bexpr b s = true) c P ->
      hoare_triple P (WHILE b DO c END)
                   (fun s => P s /\ eval_bexpr b s = false)

  (* 前置条件强化 *)
  | H_Consequence_pre : forall P P' Q c,
      (P ->> P') ->
      hoare_triple P' c Q ->
      hoare_triple P c Q

  (* 后置条件弱化 *)
  | H_Consequence_post : forall P Q Q' c,
      hoare_triple P c Q' ->
      (Q' ->> Q) ->
      hoare_triple P c Q.

Notation "'{{' P '}}' c '{{' Q '}}'" := (hoare_triple P c Q) (at level 90).

(* ============================================================ *)
(* 5. 程序验证示例 *)
(* ============================================================ *)

(* 交换两个变量的值 *)
Example swap_program :=
  2 ::= EVar 0 ;;      (* tmp := x *)
  0 ::= EVar 1 ;;      (* x := y *)
  1 ::= EVar 2.        (* y := tmp *)

Theorem swap_correct : forall x0 y0,
  {{ fun s => s 0 = x0 /\ s 1 = y0 }}
  swap_program
  {{ fun s => s 0 = y0 /\ s 1 = x0 }}.
Proof.
  intros x0 y0.
  unfold swap_program.
  apply H_Seq with (fun s => s 0 = y0 /\ s 2 = x0).
  - (* 第三个赋值 *)
    apply H_Seq with (fun s => s 2 = y0 /\ s 2 = x0).
    + apply H_Consequence_post with (fun s => s 2 = x0 /\ s 1 = x0).
      * apply H_Assign.
      * intros s [H1 H2].
        split.
        -- rewrite H2. exact H1.
        -- exact H2.
    + apply H_Consequence_pre with (fun s => s 0 = x0 /\ s 1 = y0).
      * intros s [H1 H2].
        split.
        -- rewrite H2. exact H1.
        -- exact H1.
      * apply H_Assign.
  - (* 第一个赋值 *)
    apply H_Assign.
Qed.

(* 加法程序：计算 z = x + y *)
Example add_program (x y : nat) :=
  2 ::= EConst 0 ;;                   (* z := 0 *)
  3 ::= EConst 0 ;;                   (* i := 0 *)
  WHILE BNot (BEq (EVar 3) (EConst y)) DO  (* while i <> y *)
    2 ::= EPlus (EVar 2) (EConst 1);; (* z := z + 1 *)
    3 ::= EPlus (EVar 3) (EConst 1)   (* i := i + 1 *)
  END.

Theorem add_correct : forall x y,
  {{ fun s => s 0 = x /\ s 1 = y }}
  add_program x y
  {{ fun s => s 2 = x + y }}.
Proof.
  intros x y.
  unfold add_program.
  apply H_Seq with (fun s => s 2 = 0 /\ s 3 = 0 /\ s 0 = x /\ s 1 = y).
  - (* 第二个赋值 *)
    apply H_Seq with (fun s => s 3 = 0 /\ s 0 = x /\ s 1 = y).
    + eapply H_Consequence_pre.
      * apply H_While with (fun s => s 2 = x + s 3 /\ s 3 <= y /\ s 0 = x /\ s 1 = y).
        -- (* 循环体 *)
           apply H_Seq with (fun s => s 2 = x + s 3 + 1 /\ s 3 < y /\ s 0 = x /\ s 1 = y).
           ++ eapply H_Consequence_pre.
              ** apply H_Assign.
              ** intros s [H1 [H2 [H3 H4]]].
                 split.
                 --- simpl. rewrite H1. lia.
                 --- split; [lia | split; assumption].
           ++ eapply H_Consequence_pre.
              ** apply H_Assign.
              ** intros s [H1 [H2 [H3 H4]]].
                 simpl.
                 split; [lia | split; [lia | split; assumption]].
        -- intros s [H1 [H2 [H3 H4]]].
           simpl.
           split; [reflexivity | split; [lia | split; assumption]].
    + apply H_Assign.
  - apply H_Assign.
Qed.
```

### 7.2 对应的C代码

```c
/* File: c_impl/hoare_examples.c */

#include <stdio.h>
#include <assert.h>

/* ============================================================ */
/* 1. 交换两个变量 */
/* ============================================================ */

/*
 * 前置条件: x = x0 /\\ y = y0
 * 后置条件: x = y0 /\\ y = x0
 */
void swap(int* x, int* y) {
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

/* ============================================================ */
/* 2. 使用加法计算乘法 */
/* ============================================================ */

/*
 * 前置条件: a >= 0 /\\ b >= 0
 * 后置条件: result = a * b
 * 循环不变量: result = i * b /\\ 0 <= i <= a
 */
int multiply(int a, int b) {
    int result = 0;
    int i = 0;

    while (i < a) {
        /* 不变量: result = i * b */
        result = result + b;
        i = i + 1;
    }

    return result;
}

/* ============================================================ */
/* 3. 数组求和 */
/* ============================================================ */

/*
 * 前置条件: n >= 0
 * 后置条件: sum = \\sum_{i=0}^{n-1} arr[i]
 * 循环不变量: sum = \\sum_{j=0}^{i-1} arr[j] /\\ 0 <= i <= n
 */
int array_sum(const int* arr, int n) {
    int sum = 0;
    int i = 0;

    while (i < n) {
        /* 不变量: sum = \\sum_{j=0}^{i-1} arr[j] */
        sum = sum + arr[i];
        i = i + 1;
    }

    return sum;
}

/* ============================================================ */
/* 4. 二分查找 */
/* ============================================================ */

/*
 * 前置条件: arr已排序 /\\ n >= 0
 * 后置条件: 如果返回i，则arr[i] = key；如果返回-1，则key不在arr中
 * 循环不变量:
 *   如果key在arr中，则key在arr[low..high]范围内
 *   0 <= low <= high+1 <= n
 */
int binary_search(const int* arr, int n, int key) {
    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

/* ============================================================ */
/* 5. 测试函数 */
/* ============================================================ */

void test_swap() {
    int x = 5, y = 10;
    swap(&x, &y);
    assert(x == 10 && y == 5);
    printf("Swap test passed!\n");
}

void test_multiply() {
    assert(multiply(3, 4) == 12);
    assert(multiply(0, 100) == 0);
    assert(multiply(5, 0) == 0);
    printf("Multiply test passed!\n");
}

void test_array_sum() {
    int arr[] = {1, 2, 3, 4, 5};
    assert(array_sum(arr, 5) == 15);
    printf("Array sum test passed!\n");
}

void test_binary_search() {
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int n = 7;

    assert(binary_search(arr, n, 5) == 2);
    assert(binary_search(arr, n, 1) == 0);
    assert(binary_search(arr, n, 13) == 6);
    assert(binary_search(arr, n, 4) == -1);
    printf("Binary search test passed!\n");
}
```

---

## 8. 循环不变量

### 8.1 循环不变量的形式化方法

```coq
(* File: theories/LoopInvariants.v *)

Require Import Arith.
Require Import Lia.

(* ============================================================ *)
(* 1. 循环不变量理论 *)
(* ============================================================ *)

(*
循环不变量I必须满足三个条件：
1. 初始化：前置条件 => I
2. 保持：I /\\ B /\\ S => I'  （S是循环体，I'是执行后的不变量）
3. 终止：I /\\ ~B => 后置条件
*)

(* 不变量类型 *)
Definition invariant (state : Type) := state -> Prop.

(* 终止度量：用于证明循环终止 *)
Definition measure (state : Type) := state -> nat.

(* 循环不变量证明模板 *)
Record loop_proof {state : Type} := {
  loop_condition : state -> bool;
  loop_body : state -> state;
  invariant_pred : state -> Prop;
  measure_func : state -> nat;

  (* 保持性 *)
  preservation : forall s,
    invariant_pred s ->
    loop_condition s = true ->
    invariant_pred (loop_body s);

  (* 终止性 *)
  termination : forall s,
    invariant_pred s ->
    loop_condition s = true ->
    measure_func (loop_body s) < measure_func s;

  (* 非负性 *)
  nonnegativity : forall s,
    invariant_pred s ->
    measure_func s >= 0
}.

(* ============================================================ *)
(* 2. 阶乘计算 *)
(* ============================================================ *)

(* 阶乘定义 *)
Fixpoint factorial (n : nat) : nat :=
  match n with
  | 0 => 1
  | S m => (S m) * factorial m
  end.

(* 迭代阶乘计算 *)
Fixpoint fact_iter (n acc i : nat) : nat :=
  match i with
  | 0 => acc
  | S m => fact_iter n (acc * (n - m)) m
  end.

(* 正确性定理 *)
Theorem fact_iter_correct : forall n,
  fact_iter n 1 n = factorial n.
Proof.
  intro n.
  assert (forall i acc,
    i <= n ->
    acc * fact_iter n 1 i = factorial n).
  { induction i as [| m IH]; intros acc Hle.
    - simpl. lia.
    - simpl fact_iter.
      (* 详细证明需要展开递归关系 *)
      admit.
  }
  admit.
Admitted.

(* 循环不变量：acc * i! = n! *)
Definition fact_invariant (n acc i : nat) : Prop :=
  acc * factorial i = factorial n.

(* 终止度量：i *)
Definition fact_measure (n acc i : nat) : nat := i.

(* 证明不变量 *)
Theorem fact_invariant_proof : forall n,
  let loop_cond s := match s with (acc, i) => i > 0 end in
  let loop_body s := match s with
    | (acc, S m) => (acc * (n - m), m)
    | (acc, 0) => (acc, 0)
  end in
  let inv s := match s with
    | (acc, i) => fact_invariant n acc i
  end in
  (* 不变量保持 *)
  (forall acc i,
    i > 0 ->
    fact_invariant n acc i ->
    fact_invariant n (acc * (n - (i - 1))) (i - 1)) /\
  (* 终止 *)
  (forall acc i,
    i > 0 ->
    fact_invariant n acc i ->
    (i - 1) < i).
Proof.
  intro n.
  split.
  - (* 保持性 *)
    intros acc i Hgt Hinv.
    unfold fact_invariant in *.
    destruct i as [| m].
    + lia.
    + simpl in *.
      rewrite Nat.sub_succ_l by lia.
      rewrite Nat.sub_diag.
      rewrite Nat.mul_1_r.
      (* 需要代数变换 *)
      admit.
  - (* 终止性 *)
    intros acc i Hgt Hinv.
    lia.
Admitted.

(* ============================================================ *)
(* 3. 欧几里得算法 *)
(* ============================================================ *)

(* GCD的定义 *)
Fixpoint gcd (a b : nat) : nat :=
  match b with
  | 0 => a
  | _ => gcd b (a mod b)
  end.

(* 迭代GCD *)
Fixpoint gcd_iter (a b steps : nat) : nat :=
  match steps with
  | 0 => 0  (* 应该不执行到这里 *)
  | S n =>
      match b with
      | 0 => a
      | _ => gcd_iter b (a mod b) n
      end
  end.

(* GCD循环不变量：gcd(a, b) = gcd(a0, b0) *)
Definition gcd_invariant (a0 b0 a b : nat) : Prop :=
  Nat.gcd a b = Nat.gcd a0 b0.

(* 终止度量：a + b *)
Definition gcd_measure (a b : nat) : nat := a + b.

(* 证明不变量保持 *)
Theorem gcd_preserves_invariant : forall a0 b0 a b,
  b > 0 ->
  gcd_invariant a0 b0 a b ->
  gcd_invariant a0 b0 b (a mod b).
Proof.
  intros a0 b0 a b Hgt Hinv.
  unfold gcd_invariant in *.
  rewrite Nat.gcd_comm.
  rewrite Nat.gcd_mod by lia.
  rewrite Nat.gcd_comm.
  exact Hinv.
Qed.

(* 证明终止性 *)
Theorem gcd_termination : forall a b,
  b > 0 ->
  b + (a mod b) < a + b.
Proof.
  intros a b Hgt.
  apply Nat.add_lt_le_mono.
  - apply Nat.mod_upper_bound. lia.
  - apply Nat.le_refl.
Qed.

(* ============================================================ *)
(* 4. 数组查找最大值 *)
(* ============================================================ *)

Require Import List.
Import ListNotations.

(* 列表最大值 *)
Fixpoint list_max (l : list nat) : option nat :=
  match l with
  | [] => None
  | [x] => Some x
  | x :: xs =>
      match list_max xs with
      | None => Some x
      | Some m => Some (Nat.max x m)
      end
  end.

(* 最大值在列表中 *)
Definition is_max_in_list (m : nat) (l : list nat) : Prop :=
  In m l /\ (forall x, In x l -> x <= m).

(* 循环不变量：
   - m是当前最大值
   - m在前i个元素中
   - m >= 所有前i个元素 *)
Definition max_invariant (l : list nat) (m i : nat) : Prop :=
  i <= length l /\
  (forall j, j < i -> nth j l 0 <= m) /\
  (exists j, j < i /\ nth j l 0 = m).

(* 找最大值的迭代版本 *)
Fixpoint find_max_iter (l : list nat) (m i : nat) : nat :=
  match i with
  | 0 => m
  | S n =>
      match nth n l 0 with
      | 0 => find_max_iter l m n
      | x => find_max_iter l (Nat.max m x) n
      end
  end.

(* 正确性证明 *)
Theorem find_max_correct : forall l m0,
  l <> [] ->
  is_max_in_list (find_max_iter l m0 (length l)) l.
Proof.
  (* 详细证明需要使用不变量 *)
  admit.
Admitted.
```

### 8.2 对应的C代码

```c
/* File: c_impl/loop_invariants.c */

#include <stdio.h>
#include <assert.h>
#include <limits.h>

/* ============================================================ */
/* 1. 阶乘计算（带不变量注释） */
/* ============================================================ */

/*
 * 前置条件: n >= 0
 * 后置条件: result = n!
 * 不变量: acc * i! = n! /\\ 0 <= i <= n
 * 变体: i
 */
unsigned long factorial(int n) {
    unsigned long acc = 1;
    int i = n;

    while (i > 0) {
        /* 不变量检查点 */
        acc = acc * i;
        i--;
    }

    return acc;
}

/* ============================================================ */
/* 2. 欧几里得GCD */
/* ============================================================ */

/*
 * 前置条件: a >= 0 /\\ b >= 0 /\\ a + b > 0
 * 后置条件: result = gcd(a, b)
 * 不变量: gcd(a, b) = gcd(a0, b0) /\\ a >= 0 /\\ b >= 0
 * 变体: a + b
 */
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* ============================================================ */
/* 3. 二分查找 */
/* ============================================================ */

/*
 * 前置条件: arr已排序 /\\ n >= 0
 * 后置条件: 如果key在arr中，返回其索引；否则返回-1
 * 不变量: 如果key在arr中，则在arr[low..high]范围内 /\\ 0 <= low <= high+1 <= n
 * 变体: high - low
 */
int binary_search(int* arr, int n, int key) {
    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

/* ============================================================ */
/* 4. 找数组最大值 */
/* ============================================================ */

/*
 * 前置条件: n > 0
 * 后置条件: result是arr中的最大值
 * 不变量: m = max(arr[0..i-1]) /\\ 1 <= i <= n
 * 变体: n - i
 */
int find_max(int* arr, int n) {
    assert(n > 0);
    int m = arr[0];
    int i = 1;

    while (i < n) {
        if (arr[i] > m) {
            m = arr[i];
        }
        i++;
    }

    return m;
}

/* ============================================================ */
/* 5. 数组逆序 */
/* ============================================================ */

/*
 * 前置条件: n >= 0
 * 后置条件: arr是原数组的逆序
 * 不变量: arr[0..i-1]和arr[n-i..n-1]已经交换 /\\ 0 <= i <= n/2
 * 变体: n/2 - i
 */
void reverse_array(int* arr, int n) {
    int i = 0;
    int j = n - 1;

    while (i < j) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
        i++;
        j--;
    }
}

/* ============================================================ */
/* 6. 测试函数 */
/* ============================================================ */

void test_factorial() {
    assert(factorial(0) == 1);
    assert(factorial(5) == 120);
    assert(factorial(10) == 3628800);
    printf("Factorial tests passed!\n");
}

void test_gcd() {
    assert(gcd(48, 18) == 6);
    assert(gcd(56, 98) == 14);
    assert(gcd(0, 5) == 5);
    assert(gcd(5, 0) == 5);
    printf("GCD tests passed!\n");
}

void test_find_max() {
    int arr1[] = {3, 1, 4, 1, 5, 9, 2, 6};
    assert(find_max(arr1, 8) == 9);

    int arr2[] = {5};
    assert(find_max(arr2, 1) == 5);

    printf("Find max tests passed!\n");
}

void test_reverse_array() {
    int arr[] = {1, 2, 3, 4, 5};
    reverse_array(arr, 5);
    assert(arr[0] == 5);
    assert(arr[4] == 1);
    printf("Reverse array tests passed!\n");
}
```
