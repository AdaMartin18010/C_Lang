/-
示例1: 基础群论
展示群的定义、同态和基本性质
-/

import Mathlib.Algebra.Group.Defs
import Mathlib.GroupTheory.Subgroup.Basic

namespace GroupTheoryExample

-- 定义一个具体的群: 模n整数加法群
def ZMod (n : Nat) := Fin n

instance (n : Nat) : AddGroup (ZMod n) where
  add a b := ⟨ (a.val + b.val) % n, by apply Nat.mod_lt; exact n.pos_of_neZero ⟩
  zero := ⟨0, by apply n.zero_lt_succ⟩
  neg a := ⟨ (n - a.val) % n, by apply Nat.mod_lt; exact n.pos_of_neZero ⟩
  zero_add a := by simp [HAdd.hAdd, Add.add]
  add_zero a := by simp [HAdd.hAdd, Add.add]
  add_assoc a b c := by simp [HAdd.hAdd, Add.add]; apply Fin.eq_of_val_eq; simp [Nat.add_assoc]
  neg_add_cancel a := by simp [HAdd.hAdd, Add.add, Neg.neg]; apply Fin.eq_of_val_eq; simp

-- 证明ZMod n是交换群
instance (n : Nat) : AddCommGroup (ZMod n) where
  add_comm a b := by simp [HAdd.hAdd, Add.add]; apply Fin.eq_of_val_eq; simp [Nat.add_comm]

-- 群同态的例子: 模约化
def modHom (m n : Nat) (h : m ∣ n) : ZMod n →+ ZMod m where
  toFun a := ⟨ a.val % m, by apply Nat.mod_lt; exact m.pos_of_neZero ⟩
  map_zero' := by simp
  map_add' a b := by simp; apply Fin.eq_of_val_eq; simp [Nat.add_mod]

-- 验证这是一个同态
example (m n : Nat) (h : m ∣ n) (a b : ZMod n) :
    modHom m n h (a + b) = modHom m n h a + modHom m n h b :=
  (modHom m n h).map_add a b

end GroupTheoryExample
