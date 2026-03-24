/-
示例9: 范畴论基础
展示范畴、函子和自然变换
-/

import Mathlib.CategoryTheory.Category.Basic
import Mathlib.CategoryTheory.Functor.Basic
import Mathlib.CategoryTheory.NaturalTransformation
import Mathlib.CategoryTheory.Limits.Shapes.Equalizers

namespace CategoryTheoryExamples

open CategoryTheory

/- ========== 范畴实例 ========== -/

-- Set范畴
abbrev SetCat := Type

instance SetCat.category : Category SetCat where
  Hom X Y := X → Y
  id X := id
  comp f g := g ∘ f
  id_comp f := rfl
  comp_id f := rfl
  assoc f g h := rfl

-- 群范畴
structure Grp where
  carrier : Type
  [str : Group carrier]

instance : Category Grp where
  Hom G H := G.carrier →* H.carrier
  id G := MonoidHom.id G.carrier
  comp f g := MonoidHom.comp g f
  id_comp f := MonoidHom.ext fun _ => rfl
  comp_id f := MonoidHom.ext fun _ => rfl
  assoc f g h := MonoidHom.ext fun _ => rfl

/- ========== 函子 ========== -/

-- 遗忘函子 Grp → Set
def forgetGrp : Grp ⥤ SetCat where
  obj G := G.carrier
  map f := f.toFun

-- 自由群函子 Set → Grp
def freeGrp : SetCat ⥤ Grp where
  obj X := ⟨FreeGroup X, inferInstance⟩
  map f := FreeGroup.map f

-- 自由-遗忘伴随
def freeForgetAdjunction : freeGrp ⊣ forgetGrp :=
  Adjunction.mkOfHomEquiv {
    homEquiv := fun X G => FreeGroup.lift.symm
  }

/- ========== 极限 ========== -/

-- 积的范畴论定义
structure Product {C} [Category C] (X Y : C) where
  obj : C
  fst : obj ⟶ X
  snd : obj ⟶ Y
  isProduct : ∀ (Z : C) (f : Z ⟶ X) (g : Z ⟶ Y), 
    ∃! h : Z ⟶ obj, h ≫ fst = f ∧ h ≫ snd = g

-- Set范畴有所有积
instance : HasBinaryProducts SetCat where
  has_binary_product X Y := {
    cone := BinaryFan.mk (Prod.fst : X × Y → X) (Prod.snd : X × Y → Y),
    isLimit := by
      apply BinaryFan.isLimitMk
      · intro s f g x
        exact (f x, g x)
      · intro s f g
        funext x
        rfl
      · intro s f g
        funext x
        rfl
      · intro s f g m h1 h2
        funext x
        have := congrFun h1 x
        have := congrFun h2 x
        simp_all }

/- ========== 自然变换 ========== -/

-- 恒等自然变换
def idNatTrans {C D} [Category C] [Category D] (F : C ⥤ D) : F ⟶ F where
  app X := 𝟙 (F.obj X)
  naturality f := by simp

-- 垂直复合
def vcomp {C D} [Category C] [Category D] {F G H : C ⥤ D}
    (α : F ⟶ G) (β : G ⟶ H) : F ⟶ H where
  app X := α.app X ≫ β.app X
  naturality f := by
    rw [Category.assoc, ← β.naturality, Category.assoc, ← α.naturality]
    simp

/- ========== Yoneda引理 ========== -/

-- 预层范畴
def Presheaf C := C ⥤ SetCatᵒᵖ

-- Yoneda嵌入
def yoneda {C} [Category C] : C ⥤ Presheaf C where
  obj X := yoneda.obj X
  map f := yoneda.map f

-- Yoneda引理 (陈述)
theorem yoneda_lemma {C} [Category C] (F : Presheaf C) (X : C) :
    (yoneda.obj X ⟶ F) ≅ F.obj (Opposite.op X) :=
  CategoryTheory.yonedaLemma F X

end CategoryTheoryExamples
