/-
示例3: 类型类
展示如何定义和使用类型类
-/

namespace TypeClassExamples

-- 定义可打印类型类
class Printable (α : Type) where
  toString : α → String

-- 为Nat实现Printable
instance : Printable Nat where
  toString n := s!"自然数: {n}"

-- 为String实现Printable
instance : Printable String where
  toString s := s!"字符串: \"{s}\""

-- 为列表实现Printable (需要元素可打印)
instance {α} [Printable α] : Printable (List α) where
  toString xs := "[" ++ String.intercalate ", " (xs.map Printable.toString) ++ "]"

-- 使用类型类
def printValue {α} [Printable α] (a : α) : IO Unit :=
  IO.println (Printable.toString a)

-- 数值类型类层次
class HasZero (α : Type) where
  zero : α

class HasOne (α : Type) where
  one : α

class HasAdd (α : Type) where
  add : α → α → α

class HasMul (α : Type) where
  mul : α → α → α

class Semiring (α : Type) extends HasZero α, HasOne α, HasAdd α, HasMul α where
  add_assoc : ∀ a b c, add (add a b) c = add a (add b c)
  zero_add : ∀ a, add zero a = a
  add_zero : ∀ a, add a zero = a
  mul_assoc : ∀ a b c, mul (mul a b) c = mul a (mul b c)
  one_mul : ∀ a, mul one a = a
  mul_one : ∀ a, mul a one = a
  left_distrib : ∀ a b c, mul a (add b c) = add (mul a b) (mul a c)
  right_distrib : ∀ a b c, mul (add a b) c = add (mul a c) (mul b c)
  zero_mul : ∀ a, mul zero a = zero
  mul_zero : ∀ a, mul a zero = zero

-- 为Nat实现Semiring
instance : Semiring Nat where
  zero := 0
  one := 1
  add := Nat.add
  mul := Nat.mul
  add_assoc := Nat.add_assoc
  zero_add := Nat.zero_add
  add_zero := Nat.add_zero
  mul_assoc := Nat.mul_assoc
  one_mul := Nat.one_mul
  mul_one := Nat.mul_one
  left_distrib := Nat.left_distrib
  right_distrib := Nat.right_distrib
  zero_mul := Nat.zero_mul
  mul_zero := Nat.mul_zero

-- 使用Semiring的通用函数
def square {α} [Semiring α] (a : α) : α :=
  HasMul.mul a a

-- 自动推导类型类实例
example : square 5 = 25 := rfl

-- 多参数类型类
class HAdd (α β γ : Type) where
  hAdd : α → β → γ

instance : HAdd Nat Nat Nat where
  hAdd := Nat.add

instance : HAdd String String String where
  hAdd a b := a ++ b

-- 使用HAdd
#eval HAdd.hAdd 1 2  -- 3
#eval HAdd.hAdd "Hello, " "World!"  -- "Hello, World!"

end TypeClassExamples
