(*
 * Coq形式化验证案例: 安全的整数交换
 * 使用Hoare逻辑证明swap函数的正确性
 * 
 * 目标读者: 有C基础的验证初学者
 * 难度: ⭐⭐⭐
 *)

(* 加载必要库 *)
Require Import Arith.
Require Import Lia.  (* 线性整数运算自动化 *)

(* ============================================================
 * 1. 基本定义
 * ============================================================ *)

(* 定义变量名为自然数 *)
Definition var := nat.

(* 表达式: 仅包含变量和加法 *)
Inductive expr : Type :=
  | EVar : var -> expr      (* 变量 *)
  | ENum : nat -> expr      (* 常量 *)
  | EAdd : expr -> expr -> expr.  (* 加法 *)

(* 命令式语言语句 *)
Inductive stmt : Type :=
  | SSkip : stmt                    (* 空语句 *)
  | SAssign : var -> expr -> stmt   (* 赋值: x := e *)
  | SSeq : stmt -> stmt -> stmt     (* 序列: s1; s2 *)
  | SIf : expr -> stmt -> stmt -> stmt  (* 条件 *)
  | SWhile : expr -> stmt -> stmt.  (* 循环 *)

(* 状态: 变量到值的映射 *)
Definition state := var -> nat.

(* 表达式求值 *)
Fixpoint eval_expr (e : expr) (s : state) : nat :=
  match e with
  | EVar v => s v
  | ENum n => n
  | EAdd e1 e2 => eval_expr e1 s + eval_expr e2 s
  end.

(* 更新状态 *)
Definition update (s : state) (v : var) (n : nat) : state :=
  fun x => if Nat.eqb x v then n else s x.

(* ============================================================
 * 2. Hoare逻辑
 * ============================================================ *)

(* 断言: 状态上的谓词 *)
Definition assertion := state -> Prop.

(* Hoare三元组 (使用归纳定义模拟) *)
(* {P} c {Q} 表示: 如果在满足P的状态下执行c，且c终止，则最终状态满足Q *)

Inductive hoare_triple : assertion -> stmt -> assertion -> Prop :=
  | HT_Skip : forall P,
      hoare_triple P SSkip P
  
  | HT_Assign : forall P x e,
      hoare_triple (fun s => P (update s x (eval_expr e s)))
                   (SAssign x e)
                   P
  
  | HT_Seq : forall P Q R c1 c2,
      hoare_triple P c1 Q ->
      hoare_triple Q c2 R ->
      hoare_triple P (SSeq c1 c2) R
  
  | HT_If : forall P Q b c1 c2,
      hoare_triple (fun s => P s /\ eval_expr b s <> 0) c1 Q ->
      hoare_triple (fun s => P s /\ eval_expr b s = 0) c2 Q ->
      hoare_triple P (SIf b c1 c2) Q
  
  | HT_While : forall P b c,
      hoare_triple (fun s => P s /\ eval_expr b s <> 0) c P ->
      hoare_triple P (SWhile b c) (fun s => P s /\ eval_expr b s = 0)
  
  | HT_Consequence : forall P P' Q Q' c,
      (forall s, P' s -> P s) ->
      (forall s, Q s -> Q' s) ->
      hoare_triple P c Q ->
      hoare_triple P' c Q'.

(* ============================================================
 * 3. 交换函数的验证
 * ============================================================ *)

(* 定义两个变量 *)
Definition X : var := 0.
Definition Y : var := 1.
Definition T : var := 2.  (* 临时变量 *)

(* 交换函数:
   T := X;
   X := Y;
   Y := T
 *)
Definition swap : stmt :=
  SSeq (SAssign T (EVar X))
  (SSeq (SAssign X (EVar Y))
        (SAssign Y (EVar T))).

(* 断言: X = a /\ Y = b *)
Definition XY_eq (a b : nat) : assertion :=
  fun s => s X = a /\ s Y = b.

(* 交换函数的正确性定理 *)
Theorem swap_correct : forall a b,
  hoare_triple (XY_eq a b)
               swap
               (XY_eq b a).
Proof.
  intros a b.
  unfold swap, XY_eq.
  
  (* 应用序列规则 *)
  eapply HT_Seq.
  - (* 第一条赋值: T := X *)
    eapply HT_Seq.
    + (* 第二条赋值: X := Y *)
      eapply HT_Seq.
      * (* 第三条赋值: Y := T *)
        apply HT_Assign.
      * (* 中间状态 *)
        apply HT_Assign.
    + (* 中间状态 *)
      apply HT_Assign.
  - (* 最弱前置条件验证 *)
    unfold update.
    simpl.
    intros s [H1 H2].
    split.
    + (* 证明 X = b *)
      rewrite Nat.eqb_refl.
      rewrite (Nat.eqb_neq _ Y).
      * rewrite (Nat.eqb_neq _ T).
        -- assumption.
        -- lia.  (* X <> T *)
      * lia.  (* X <> Y *)
    + (* 证明 Y = a *)
      rewrite Nat.eqb_refl.
      rewrite (Nat.eqb_neq _ X).
      * rewrite (Nat.eqb_neq _ T).
        -- assumption.
        -- lia.  (* Y <> T *)
      * lia.  (* Y <> X *)
Qed.

(* ============================================================
 * 4. 辅助引理和自动化
 * ============================================================ *)

(* 简化状态查找的引理 *)
Lemma update_eq : forall s x v,
  (update s x v) x = v.
Proof.
  intros.
  unfold update.
  rewrite Nat.eqb_refl.
  reflexivity.
Qed.

Lemma update_neq : forall s x y v,
  x <> y ->
  (update s x v) y = s y.
Proof.
  intros.
  unfold update.
  rewrite <- Nat.eqb_neq in H.
  rewrite H.
  reflexivity.
Qed.

(* ============================================================
 * 5. 交换函数的替代证明 (使用引理)
 * ============================================================ *)

Theorem swap_correct_alt : forall a b,
  hoare_triple (XY_eq a b)
               swap
               (XY_eq b a).
Proof.
  intros a b.
  unfold swap, XY_eq.
  
  (* 使用程序逻辑规则 *)
  apply HT_Seq with (Q := fun s => s T = a /\ s Y = b).
  { (* T := X *)
    apply HT_Consequence with 
      (P := fun s => s X = a /\ s Y = b)
      (Q := fun s => s T = a /\ s Y = b).
    - (* 前置条件蕴含 *)
      auto.
    - (* 后置条件蕴含 *)
      intros s [H1 H2].
      unfold update.
      split.
      + rewrite Nat.eqb_refl. auto.
      + rewrite Nat.eqb_neq; lia.
    - apply HT_Assign.
  }
  
  apply HT_Seq with (Q := fun s => s T = a /\ s X = b).
  { (* X := Y *)
    apply HT_Consequence with
      (P := fun s => s T = a /\ s Y = b)
      (Q := fun s => s T = a /\ s X = b).
    - auto.
    - intros s [H1 H2].
      unfold update.
      split.
      + rewrite Nat.eqb_neq; lia.
      + rewrite Nat.eqb_refl. auto.
    - apply HT_Assign.
  }
  
  { (* Y := T *)
    apply HT_Consequence with
      (P := fun s => s T = a /\ s X = b)
      (Q := fun s => s X = b /\ s Y = a).
    - auto.
    - intros s [H1 H2].
      unfold update.
      split.
      + rewrite Nat.eqb_neq; lia.
      + rewrite Nat.eqb_refl. auto.
    - apply HT_Assign.
  }
Qed.

(* ============================================================
 * 6. 练习和扩展
 * ============================================================ *)

(*
练习1: 证明加法交换律
定义 add_x_y : stmt 为 X := X + Y
证明: {X = a /\ Y = b} add_x_y {X = a + b /\ Y = b}

练习2: 证明无临时变量的交换 (使用加减法)
X := X + Y;
Y := X - Y;
X := X - Y
注意: 需要处理整数溢出

练习3: 验证条件交换
if (X < Y) then
  swap
endif
*)

(* 练习1解答 *)
Definition add_x_y : stmt :=
  SAssign X (EAdd (EVar X) (EVar Y)).

Theorem add_correct : forall a b,
  hoare_triple (XY_eq a b)
               add_x_y
               (fun s => s X = a + b /\ s Y = b).
Proof.
  intros.
  unfold add_x_y, XY_eq.
  apply HT_Consequence with
    (P := fun s => s X = a + b /\ s Y = b)
    (Q := fun s => s X = a + b /\ s Y = b).
  - (* 前置条件 *)
    intros s [H1 H2].
    unfold update, eval_expr.
    split.
    + rewrite Nat.eqb_refl. lia.
    + rewrite Nat.eqb_neq; lia.
  - (* 后置条件 *)
    auto.
  - apply HT_Assign.
Qed.

(* ============================================================
 * 7. 验证总结
 * ============================================================ *)

(*
本案例展示了:
1. 如何在Coq中定义简单的命令式语言
2. Hoare逻辑公理的形式化
3. 基本程序正确性证明的结构
4. 前置/后置条件的推理过程

扩展到完整C语言的步骤:
1. 添加内存模型 (地址、指针)
2. 添加函数调用
3. 与CompCert的Clight语言集成
4. 使用VST (Verified Software Toolchain)
*)

Print swap_correct.
Check swap_correct.
