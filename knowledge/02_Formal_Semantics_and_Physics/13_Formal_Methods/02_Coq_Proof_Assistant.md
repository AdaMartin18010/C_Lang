# Coq证明助手实战

> **层级定位**: 02 Formal Semantics and Physics / 13 Formal Methods
> **参考课程**: MIT 6.170, Software Foundations (Coq教材)
> **难度级别**: L5 专家
> **预估学习时间**: 20-25 小时

---

## 目录

- [Coq证明助手实战](#coq证明助手实战)
  - [目录](#目录)
  - [1. Coq基础](#1-coq基础)
    - [1.1 什么是Coq](#11-什么是coq)
    - [1.2 基本命令](#12-基本命令)
  - [2. 归纳类型](#2-归纳类型)
    - [2.1 自然数定义](#21-自然数定义)
    - [2.2 列表类型](#22-列表类型)
  - [3. 谓词逻辑](#3-谓词逻辑)
    - [3.1 全称量词与存在量词](#31-全称量词与存在量词)
    - [3.2 关系与归纳定义](#32-关系与归纳定义)
  - [4. 程序验证](#4-程序验证)
    - [4.1 Hoare逻辑在Coq中](#41-hoare逻辑在coq中)
    - [4.2 分离逻辑](#42-分离逻辑)
  - [5. 实战案例](#5-实战案例)
    - [5.1 验证阶乘函数](#51-验证阶乘函数)
    - [5.2 排序算法验证](#52-排序算法验证)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. Coq基础

### 1.1 什么是Coq

```
Coq = 交互式定理证明器 + 函数式编程语言

特点：
• 基于依赖类型理论 (Calculus of Inductive Constructions)
• 构造性逻辑 (Constructive Logic)
• 提取可执行代码 (Haskell/OCaml)
• 广泛应用于：
  - CompCert (验证的C编译器)
  - Iris (并发分离逻辑框架)
  - VST (C程序验证工具)
  - Feit-Thompson定理证明
```

### 1.2 基本命令

```coq
(* 交互式证明命令 *)
Theorem plus_comm : forall n m : nat, n + m = m + n.
Proof.
  intros n m.           (* 引入变量 *)
  induction n.          (* 对n归纳 *)
  - simpl.              (* 化简 *)
    reflexivity.        (* 自反性 *)
  - simpl.
    rewrite IHn.        (* 使用归纳假设 *)
    reflexivity.
Qed.

(* 常用策略 *)
intros        (* 引入假设到上下文 *)
revert        (* 从上下文移除假设 *)
reflexivity   (* 证明 t = t *)
simpl         (* 化简表达式 *)
rewrite       (* 用等式重写 *)
apply         (* 应用定理/假设 *)
destruct      (* 情况分析 *)
induction     (* 归纳 *)
auto          (* 自动搜索证明 *)
tauto         (* 命题逻辑自动证明 *)
omega         (* 算术自动证明 *)
```

---

## 2. 归纳类型

### 2.1 自然数定义

```coq
(* 皮亚诺自然数 *)
Inductive nat : Type :=
  | O : nat           (* 零 *)
  | S : nat -> nat.   (* 后继 *)

(* 定义加法 *)
Fixpoint plus (n m : nat) : nat :=
  match n with
  | O => m
  | S n' => S (plus n' m)
  end.

Notation "x + y" := (plus x y).

(* 证明结合律 *)
Theorem plus_assoc : forall n m p : nat,
  n + (m + p) = (n + m) + p.
Proof.
  intros n m p.
  induction n as [| n' IHn'].
  - (* n = O *)
    reflexivity.
  - (* n = S n' *)
    simpl.
    rewrite IHn'.
    reflexivity.
Qed.

(* 证明交换律 *)
Lemma plus_n_O : forall n : nat, n = n + O.
Proof.
  induction n.
  - reflexivity.
  - simpl. rewrite <- IHn. reflexivity.
Qed.

Lemma plus_n_Sm : forall n m : nat, S (n + m) = n + (S m).
Proof.
  intros n m.
  induction n.
  - reflexivity.
  - simpl. rewrite IHn. reflexivity.
Qed.

Theorem plus_comm : forall n m : nat, n + m = m + n.
Proof.
  intros n m.
  induction n.
  - simpl. rewrite <- plus_n_O. reflexivity.
  - simpl. rewrite IHn. rewrite plus_n_Sm. reflexivity.
Qed.
```

### 2.2 列表类型

```coq
(* 多态列表 *)
Inductive list (A : Type) : Type :=
  | nil : list A
  | cons : A -> list A -> list A.

Arguments nil {A}.
Arguments cons {A} _ _.

Notation "x :: l" := (cons x l).
Notation "[ ]" := nil.
Notation "[ x ; .. ; y ]" := (cons x .. (cons y nil) ..).

(* 列表操作 *)
Fixpoint app {A : Type} (l1 l2 : list A) : list A :=
  match l1 with
  | nil => l2
  | h :: t => h :: (app t l2)
  end.

Notation "l1 ++ l2" := (app l1 l2).

Fixpoint rev {A : Type} (l : list A) : list A :=
  match l with
  | nil => nil
  | h :: t => rev t ++ [h]
  end.

Fixpoint length {A : Type} (l : list A) : nat :=
  match l with
  | nil => O
  | _ :: t => S (length t)
  end.

(* 证明性质 *)
Theorem app_assoc : forall A (l1 l2 l3 : list A),
  l1 ++ (l2 ++ l3) = (l1 ++ l2) ++ l3.
Proof.
  intros A l1 l2 l3.
  induction l1.
  - reflexivity.
  - simpl. rewrite IHl1. reflexivity.
Qed.

Lemma app_length : forall A (l1 l2 : list A),
  length (l1 ++ l2) = length l1 + length l2.
Proof.
  intros A l1 l2.
  induction l1.
  - reflexivity.
  - simpl. rewrite IHl1. reflexivity.
Qed.

(* 快速反转 - 尾递归 *)
Fixpoint tr_rev_append {A : Type} (l1 l2 : list A) : list A :=
  match l1 with
  | nil => l2
  | h :: t => tr_rev_append t (h :: l2)
  end.

Definition tr_rev {A : Type} (l : list A) : list A :=
  tr_rev_append l nil.

Theorem tr_rev_correct : forall A (l : list A), tr_rev l = rev l.
Proof.
  intros A l.
  unfold tr_rev.
  (* 需要辅助引理 *)
  assert (H: forall l2, tr_rev_append l l2 = rev l ++ l2).
  { induction l.
    - reflexivity.
    - intros. simpl. rewrite IHl. rewrite <- app_assoc. reflexivity. }
  rewrite H. rewrite app_nil_r. reflexivity.
Qed.
```

---

## 3. 谓词逻辑

### 3.1 全称量词与存在量词

```coq
(* 全称量词 *)
Theorem forall_example :
  forall n : nat, n + 0 = n.
Proof.
  intros n.
  rewrite <- plus_n_O.
  reflexivity.
Qed.

(* 存在量词 *)
Inductive ex (A : Type) (P : A -> Prop) : Prop :=
  | ex_intro : forall x : A, P x -> ex A P.

Notation "'exists' x , p" := (ex _ (fun x => p)).

(* 存在量词证明 *)
Theorem exists_example :
  exists n : nat, n + 1 = 2.
Proof.
  exists 1.
  reflexivity.
Qed.

(* 使用存在量词 *)
Theorem exists_elim_example :
  (exists n : nat, n + 1 = 3) ->
  (exists m : nat, m = 2).
Proof.
  intros H.
  destruct H as [n Hn].  (* 提取见证 *)
  exists n.
  inversion Hn.          (* 反演分析 *)
  reflexivity.
Qed.

(* 经典逻辑 vs 构造逻辑 *)
(* 排中律在Coq中不可证明（除非添加公理） *)
Definition excluded_middle := forall P : Prop, P \/ ~ P.

(* 但双重否定消除等价于排中律 *)
Theorem not_not_excluded_middle :
  forall P : Prop, ~~(P \/ ~P).
Proof.
  intros P H.
  apply H.
  right.
  intros HP.
  apply H.
  left.
  apply HP.
Qed.
```

### 3.2 关系与归纳定义

```coq
(* 小于等于关系 *)
Inductive le : nat -> nat -> Prop :=
  | le_n : forall n, le n n
  | le_S : forall n m, le n m -> le n (S m).

Notation "n <= m" := (le n m).

(* 证明性质 *)
Theorem le_trans : forall n m p, n <= m -> m <= p -> n <= p.
Proof.
  intros n m p Hnm Hmp.
  induction Hmp.
  - apply Hnm.
  - apply le_S. apply IHHmp.
Qed.

Theorem le_Sn_le : forall n m, S n <= m -> n <= m.
Proof.
  intros n m H.
  induction H.
  - apply le_S. apply le_n.
  - apply le_S. apply IHle.
Qed.

(* 偶数的归纳定义 *)
Inductive even : nat -> Prop :=
  | ev_0 : even O
  | ev_SS : forall n : nat, even n -> even (S (S n)).

Theorem ev_double : forall n, even (n + n).
Proof.
  intros n.
  induction n.
  - apply ev_0.
  - simpl.
    rewrite plus_n_Sm.
    apply ev_SS.
    apply IHn.
Qed.

(* 互归纳定义：偶数和奇数 *)
Inductive even' : nat -> Prop :=
  | even'_O : even' O
  | even'_S : forall n, odd' n -> even' (S n)
with odd' : nat -> Prop :=
  | odd'_S : forall n, even' n -> odd' (S n).
```

---

## 4. 程序验证

### 4.1 Hoare逻辑在Coq中

```coq
(* 命令式程序语法 *)
Inductive com : Type :=
  | CSkip : com
  | CAss : string -> aexp -> com
  | CSeq : com -> com -> com
  | CIf : bexp -> com -> com -> com
  | CWhile : bexp -> com -> com.

Notation "'SKIP'" := CSkip.
Notation "x '::=' a" := (CAss x a) (at level 60).
Notation "c1 ;; c2" := (CSeq c1 c2) (at level 80, right associativity).
Notation "'WHILE' b 'DO' c 'END'" := (CWhile b c) (at level 80).
Notation "'IFB' c1 'THEN' c2 'ELSE' c3 'FI'" := (CIf c1 c2 c3).

(* Hoare三元组 *)
Definition Assertion := state -> Prop.

Definition hoare_triple (P:Assertion) (c:com) (Q:Assertion) : Prop :=
  forall st st',
    P st ->
    c / st \\ st' ->
    Q st'.

Notation "{{ P }} c {{ Q }}" := (hoare_triple P c Q).

(* Hoare规则 *)
Theorem hoare_skip : forall P,
  {{P}} SKIP {{P}}.
Proof.
  intros P st st' H HP.
  inversion H. subst.
  assumption.
Qed.

Theorem hoare_seq : forall P Q R c1 c2,
  {{Q}} c2 {{R}} ->
  {{P}} c1 {{Q}} ->
  {{P}} c1 ;; c2 {{R}}.
Proof.
  intros P Q R c1 c2 H2 H1 st st' H12 Pre.
  inversion H12. subst.
  apply H2 with st'0.
  - apply H1 with st; assumption.
  - assumption.
Qed.

Theorem hoare_asgn : forall Q X a,
  {{Q [X |-> a]}} X ::= a {{Q}}.
Proof.
  intros Q X a st st' HE HQ.
  inversion HE. subst.
  assumption.
Qed.

Theorem hoare_if : forall P Q b c1 c2,
  {{fun st => P st /\\ bassn b st}} c1 {{Q}} ->
  {{fun st => P st /\\ ~(bassn b st)}} c2 {{Q}} ->
  {{P}} IFB b THEN c1 ELSE c2 FI {{Q}}.
Proof.
  (* 证明省略 *)
Admitted.

Theorem hoare_while : forall P b c,
  {{fun st => P st /\\ bassn b st}} c {{P}} ->
  {{P}} WHILE b DO c END {{fun st => P st /\\ ~(bassn b st)}}.
Proof.
  (* 证明省略 *)
Admitted.
```

### 4.2 分离逻辑

```coq
(* 堆模型 *)
Definition heap := nat -> option nat.

Definition emp : heap :=
  fun _ => None.

Definition points_to (l : nat) (v : nat) : heap :=
  fun l' => if l =? l' then Some v else None.

Notation "l '|->' v" := (points_to l v).

(* 分离合取 *)
Definition star (h1 h2 : heap -> Prop) : heap -> Prop :=
  fun h => exists h1' h2',
    h1 h1' /\\ h2 h2' /\\ disjoint h1' h2' /\\ h = join h1' h2'.

Notation "P '*' Q" := (star P Q).

(* 分离蕴涵 *)
Definition wand (P Q : heap -> Prop) : heap -> Prop :=
  fun h => forall h', disjoint h h' -> P h' -> Q (join h h').

Notation "P '-*' Q" := (wand P Q).

(* 分离逻辑规则 *)
Lemma sep_comm : forall P Q,
  P * Q = Q * P.
Proof.
  (* 证明堆的分离合取交换律 *)
Admitted.

Lemma sep_assoc : forall P Q R,
  (P * Q) * R = P * (Q * R).
Proof.
Admitted.

Lemma frame_rule : forall P Q R c,
  {{P}} c {{Q}} ->
  {{P * R}} c {{Q * R}}.
Proof.
  (* 帧规则：局部推理 *)
Admitted.
```

---

## 5. 实战案例

### 5.1 验证阶乘函数

```coq
(* 函数式实现 *)
Fixpoint fact (n : nat) : nat :=
  match n with
  | O => 1
  | S n' => n * fact n'
  end.

(* 尾递归实现 *)
Fixpoint fact_aux (n acc : nat) : nat :=
  match n with
  | O => acc
  | S n' => fact_aux n' (n * acc)
  end.

Definition fact_tr (n : nat) := fact_aux n 1.

(* 证明等价性 *)
Lemma fact_aux_correct : forall n acc,
  fact_aux n acc = fact n * acc.
Proof.
  intros n.
  induction n.
  - reflexivity.
  - intros acc. simpl.
    rewrite IHn.
    rewrite <- mult_n_Sm.
    rewrite mult_assoc.
    reflexivity.
Qed.

Theorem fact_tr_correct : forall n,
  fact_tr n = fact n.
Proof.
  intros n.
  unfold fact_tr.
  rewrite fact_aux_correct.
  rewrite mult_1_r.
  reflexivity.
Qed.

(* 使用Program和定理提取 *)
Require Extraction.
Extraction Language OCaml.
Extraction "fact.ml" fact_tr.
```

### 5.2 排序算法验证

```coq
(* 插入排序 *)
Fixpoint insert (x : nat) (l : list nat) : list nat :=
  match l with
  | nil => [x]
  | h :: t => if x <=? h then x :: l else h :: insert x t
  end.

Fixpoint sort (l : list nat) : list nat :=
  match l with
  | nil => nil
  | h :: t => insert h (sort t)
  end.

(* 排序正确性 *)
Definition sorted (l : list nat) : Prop :=
  forall i j, i < j < length l ->
    nth i l 0 <= nth j l 0.

Definition Permutation {A} (l1 l2 : list A) : Prop :=
  forall x, count_occ eq_dec l1 x = count_occ eq_dec l2 x.

Theorem sort_correct : forall l,
  sorted (sort l) /\\ Permutation l (sort l).
Proof.
  (* 分两部分证明 *)
  split.
  - apply sort_sorted.
  - apply sort_permutation.
Qed.
```

---

## 关联导航

### 前置知识

- [构造即正确](./01_Correctness_by_Construction.md)
- [类型系统](../05_Type_Semantics/README.md)

### 后续延伸

- [CompCert验证编译器](../11_Compiler_Internals/04_CompCert_Verified_Compiler.md)
- [Iris并发逻辑](../14_Concurrency_Parallelism/07_Iris_Concurrent_Logic.md)

### 参考

- Software Foundations (<https://softwarefoundations.cis.upenn.edu/>)
- Certified Programming with Dependent Types, Adam Chlipala
- Interactive Theorem Proving and Program Development, Yves Bertot
