# 范畴语义学深化

**L6+++ 系统级元理论 | 范畴论语义学完整框架**

---

## 目录

- [范畴语义学深化](#范畴语义学深化)
  - [目录](#目录)
  - [1. 引言](#1-引言)
  - [2. 范畴论基础](#2-范畴论基础)
    - [2.1 范畴、函子、自然变换](#21-范畴函子自然变换)
    - [2.2 积与余积](#22-积与余积)
    - [2.3 笛卡尔闭范畴](#23-笛卡尔闭范畴)
  - [3. CCC与λ演算](#3-ccc与λ演算)
    - [3.1 Curry-Howard-Lambek对应](#31-curry-howard-lambek对应)
    - [3.2 λ→CCC翻译](#32-λccc翻译)
    - [3.3 CCC→λ翻译](#33-cccλ翻译)
    - [3.4 等价性定理](#34-等价性定理)
  - [4. 伴随函子](#4-伴随函子)
    - [4.1 伴随的定义](#41-伴随的定义)
    - [4.2 单位与余单位](#42-单位与余单位)
    - [4.3 伴随与逻辑](#43-伴随与逻辑)
    - [4.4 乘积⊣对角⊣指数](#44-乘积对角指数)
  - [5. 单子语义](#5-单子语义)
    - [5.1 单子的定义](#51-单子的定义)
    - [5.2 Kleisli范畴](#52-kleisli范畴)
    - [5.3 强单子](#53-强单子)
    - [5.4 标准单子示例](#54-标准单子示例)
  - [6. 余代数与无限结构](#6-余代数与无限结构)
    - [6.1 余代数的定义](#61-余代数的定义)
    - [6.2 终余代数](#62-终余代数)
    - [6.3 无限数据类型](#63-无限数据类型)
    - [6.4 互模拟](#64-互模拟)
  - [7. 形式化定理与证明](#7-形式化定理与证明)
    - [7.1 CCC↔λ演算等价性](#71-cccλ演算等价性)
    - [7.2 积的唯一性](#72-积的唯一性)
    - [7.3 指数的伴随性](#73-指数的伴随性)
    - [7.4 单子定律](#74-单子定律)
    - [7.5 Kleisli范畴良定义性](#75-kleisli范畴良定义性)
    - [7.6 终余代数唯一性](#76-终余代数唯一性)
  - [8. 参考文献](#8-参考文献)
  - [9. 交叉引用](#9-交叉引用)
    - [前置依赖](#前置依赖)
    - [横向关联](#横向关联)
    - [后续扩展](#后续扩展)

## 1. 引言

范畴语义学提供程序语言语义的数学基础，通过范畴论结构理解类型系统、计算效应和程序等价。
本文档建立从笛卡尔闭范畴到伴随、单子的完整框架，揭示类型理论与范畴论的深层同构。

**核心洞见** (Curry-Howard-Lambek对应):

- **逻辑世界**: 直觉主义命题逻辑
- **类型世界**: 简单类型λ演算
- **范畴世界**: 笛卡尔闭范畴

---

## 2. 范畴论基础

### 2.1 范畴、函子、自然变换

**定义 2.1.1** (范畴). 范畴 **C** = (Obj, Hom, ∘, id) 满足:

- 结合律: (h ∘ g) ∘ f = h ∘ (g ∘ f)
- 恒等律: f ∘ idₐ = f = id_B ∘ f

**Lean 4 示例: 范畴结构**

```lean4
class Category (C : Type u) where
  Hom : C → C → Type v
  comp {A B C : C} : Hom B C → Hom A B → Hom A B
  id (A : C) : Hom A A
  assoc {A B C D} (f : Hom A B) (g : Hom B C) (h : Hom C D) :
    comp (comp h g) f = comp h (comp g f)
  id_comp {A B} (f : Hom A B) : comp (id B) f = f
  comp_id {A B} (f : Hom A B) : comp f (id A) = f

infixr:80 " ⟶ " => Category.Hom
infixr:80 " ⊚ " => Category.comp
notation "𝟙" => Category.id
```

**定义 2.1.2** (函子). F: **C** → **D** 保持复合和恒等:

- F(f ∘ g) = F(f) ∘ F(g)
- F(idₐ) = id_{F(A)}

**Lean 4 示例: 函子**

```lean4
structure Functor (C D : Type u) [Category C] [Category D] where
  obj : C → D
  map {A B : C} : (A ⟶ B) → (obj A ⟶ obj B)
  map_id (A : C) : map (𝟙 A) = 𝟙 (obj A)
  map_comp {A B C} (f : A ⟶ B) (g : B ⟶ C) : map (g ⊚ f) = map g ⊚ map f

infixr:26 " ⥤ " => Functor
```

**定义 2.1.3** (自然变换). α: F ⇒ G 是满足自然性的态射族:

```
α_B ∘ F(f) = G(f) ∘ αₐ    (∀ f: A → B)
```

### 2.2 积与余积

**定义 2.2.1** (积). A × B 配 π₁: A × B → A, π₂: A × B → B 满足泛性质:
对任意 f: C → A, g: C → B，存在唯一 ⟨f, g⟩: C → A × B 使 π₁ ∘ ⟨f, g⟩ = f, π₂ ∘ ⟨f, g⟩ = g。

**Lean 4 示例: 积**

```lean4
class HasProducts (C : Type u) [Category C] where
  prod (A B : C) : C
  π₁ {A B} : prod A B ⟶ A
  π₂ {A B} : prod A B ⟶ B
  pair {C A B} (f : C ⟶ A) (g : C ⟶ B) : C ⟶ prod A B
  pair_π₁ {C A B} (f : C ⟶ A) (g : C ⟶ B) : π₁ ⊚ pair f g = f
  pair_π₂ {C A B} (f : C ⟶ A) (g : C ⟶ B) : π₂ ⊚ pair f g = g
  pair_unique {C A B} (f : C ⟶ A) (g : C ⟶ B) (h : C ⟶ prod A B) :
    π₁ ⊚ h = f → π₂ ⊚ h = g → h = pair f g

infixr:70 " × " => HasProducts.prod
notation "⟨" f "," g "⟩" => HasProducts.pair f g
```

### 2.3 笛卡尔闭范畴

**定义 2.3.1** (指数对象). Bᴬ 配 eval: Bᴬ × A → B 满足: 对任意 f: C × A → B，存在唯一 curry(f): C → Bᴬ 使 eval ∘ (curry(f) × idₐ) = f。

**定义 2.3.2** (笛卡尔闭范畴, CCC). 具有有限积和指数的范畴。

**Lean 4 示例: 指数对象**

```lean4
class HasExponentials (C : Type u) [Category C] [HasProducts C] where
  exp (A B : C) : C
  eval {A B} : (exp A B) × A ⟶ B
  curry {C A B} (f : C × A ⟶ B) : C ⟶ exp A B
  curry_eval {C A B} (f : C × A ⟶ B) : eval ⊚ (curry f × 𝟙 A) = f
  curry_unique {C A B} (f : C × A ⟶ B) (h : C ⟶ exp A B) :
    eval ⊚ (h × 𝟙 A) = f → h = curry f

notation:80 B:80 "^" A:81 => HasExponentials.exp A B
notation:80 A:80 " ⇒ " B:81 => HasExponentials.exp A B
```

---

## 3. CCC与λ演算

### 3.1 Curry-Howard-Lambek对应

| 逻辑 | 类型理论 | 范畴论 |
|------|----------|--------|
| 命题 P | 类型 τ | 对象 A |
| 证明 p: P | 项 t: τ | 态射 f: 1 → A |
| P ⇒ Q | τ → σ | Bᴬ |
| P ∧ Q | τ × σ | A × B |
| 真 | Unit | 终对象 1 |

### 3.2 λ→CCC翻译

**类型翻译**:

```
〚Unit〛 = 1
〚A → B〛 = 〚B〛^{〚A〛}
〚A × B〛 = 〚A〛 × 〚B〛
```

**项翻译**:

```
〚Γ ⊢ x: A〛 = πᵢ                    (投影)
〚Γ ⊢ λx: A. t: A → B〛 = curry(〚Γ, x: A ⊢ t: B〛)
〚Γ ⊢ t u: B〛 = eval ∘ ⟨〚t〛, 〚u〛⟩
```

### 3.3 CCC→λ翻译

**对象翻译**:

```
⟦1⟧ = Unit
⟦A × B⟧ = ⟦A⟧ × ⟦B⟧
⟦Bᴬ⟧ = ⟦A⟧ → ⟦B⟧
```

### 3.4 等价性定理

**定理 3.4.1** (CCC↔λ演算). 存在范畴等价 CCC ≃ STLC(βη)。

**证明概要**: 构造互逆函子 L: STLC → CCC 和 Λ: CCC → STLC，验证 L ∘ Λ ≅ Id 且 Λ ∘ L ≅ Id。

---

## 4. 伴随函子

### 4.1 伴随的定义

**定义 4.1.1** (伴随). F: **C** → **D**, G: **D** → **C** 形成伴随 F ⊣ G 当:

```
Hom_D(F(A), B) ≅ Hom_C(A, G(B))   (自然同构)
```

**Lean 4 示例: 伴随**

```lean4
class Adjunction {C D : Type u} [Category C] [Category D]
    (F : C ⥤ D) (G : D ⥤ C) where
  homEquiv {A : C} {B : D} : (F.obj A ⟶ B) ≃ (A ⟶ G.obj B)
  unit : 𝟭 C ⟶ F.comp G
  counit : G.comp F ⟶ 𝟭 D
  left_triangle (A : C) :
    G.map (counit.app (F.obj A)) ⊚ unit.app A = 𝟙 (G.obj (F.obj A))
  right_triangle (B : D) :
    counit.app B ⊚ F.map (unit.app (G.obj B)) = 𝟙 (F.obj (G.obj B))

notation:80 F:80 " ⊣ " G:81 => Adjunction F G
```

### 4.2 单位与余单位

**单位**: η: Id ⇒ G ∘ F, ηₐ: A → G(F(A))
**余单位**: ε: F ∘ G ⇒ Id, ε_B: F(G(B)) → B

**三角等式**:

```
ε_{F(A)} ∘ F(ηₐ) = id_{F(A)}
G(ε_B) ∘ η_{G(B)} = id_{G(B)}
```

### 4.3 伴随与逻辑

| 伴随 | 逻辑解释 |
|------|----------|
| ∃ ⊣ Δ ⊣ ∀ | 存在 ⊣ 对角 ⊣ 全称 |
| (− × A) ⊣ (A ⇒ −) | 合取 ⊣ 蕴涵 |

### 4.4 乘积⊣对角⊣指数

**定理 4.4.1** (乘积⊣对角⊣指数). 在CCC中: (− × A) ⊣ (A ⇒ −)

**证明**: 由指数泛性质直接得到同构 Hom(B × A, C) ≅ Hom(B, C^A)。

---

## 5. 单子语义

### 5.1 单子的定义

**定义 5.1.1** (单子). (T, η, μ) 其中:

- T: **C** → **C** 是endo-函子
- η: Id ⇒ T (单位)
- μ: T² ⇒ T (乘法)

满足结合律和单位律。

**Lean 4 示例: 单子**

```lean4
class Monad (C : Type u) [Category C] (T : C ⥤ C) where
  η : 𝟭 C ⟶ T
  μ : T.comp T ⟶ T
  assoc (A : C) :
    μ.app A ⊚ T.map (μ.app A) = μ.app A ⊚ μ.app (T.obj A)
  left_unit (A : C) : μ.app A ⊚ η.app (T.obj A) = 𝟙 (T.obj A)
  right_unit (A : C) : μ.app A ⊚ T.map (η.app A) = 𝟙 (T.obj A)

def bind {C : Type u} [Category C] {T : C ⥤ C} [Monad C T]
    {A B : C} (f : A ⟶ T.obj B) : T.obj A ⟶ T.obj B :=
  μ.app B ⊚ T.map f
```

### 5.2 Kleisli范畴

**定义 5.2.1** (Kleisli范畴). 对象同 **C**，态射 A →_T B := A → T(B)，复合 g ∘_T f = μ ∘ T(g) ∘ f。

**Lean 4 示例: Kleisli范畴**

```lean4
def KleisliCategory (C : Type u) [Category C] (T : C ⥤ C) [Monad C T] :
    Category C where
  Hom A B := A ⟶ T.obj B
  comp {A B C} (g : B ⟶ T.obj C) (f : A ⟶ T.obj B) : A ⟶ T.obj C :=
    μ.app C ⊚ T.map g ⊚ f
  id A := η.app A
  assoc {A B C D} f g h := by
    simp [comp]; rw [←Category.assoc, Monad.assoc D, Category.assoc]
    rw [←Functor.map_comp, ←Category.assoc]; rfl
  id_comp {A B} f := by simp [comp, Monad.left_unit]
  comp_id {A B} f := by simp [comp, Monad.right_unit]
```

### 5.3 强单子

**定义 5.3.1** (强单子). 配备强度 σ_{A,B}: A × T(B) → T(A × B) 的单子，允许效应通过上下文传播。

### 5.4 标准单子示例

| 单子 | T(A) | 效应 |
|------|------|------|
| Maybe | 1 + A | 部分性 |
| List | List(A) | 非确定性 |
| State S | S → A × S | 可变状态 |
| Reader R | R → A | 环境 |
| Writer W | A × W | 日志 |
| Cont R | (A → R) → R | 延续 |

**Lean 4 示例: Maybe单子**

```lean4
inductive OptionObj (A : Type u) | none | some (a : A)

def OptionFunctor : Type u ⥤ Type u where
  obj := OptionObj
  map f | OptionObj.none => OptionObj.none | OptionObj.some a => OptionObj.some (f a)
  map_id A := by funext a; cases a <;> rfl
  map_comp f g := by funext a; cases a <;> rfl

instance OptionMonad : Monad (Type u) OptionFunctor where
  η := { app := fun A a => OptionObj.some a, naturality := by intros; rfl }
  μ := { app := fun A mm => match mm with | OptionObj.none => OptionObj.none | OptionObj.some m => m,
         naturality := by intros; funext a; cases a <;> rfl }
  assoc A := by funext mm; cases mm <;> rfl
  left_unit A := by funext m; rfl
  right_unit A := by funext m; cases m <;> rfl
```

**Lean 4 示例: State单子**

```lean4
def State (S A : Type u) := S → A × S

def StateFunctor (S : Type u) : Type u ⥤ Type u where
  obj A := State S A
  map {A B} (f : A → B) (st : State S A) : State S B :=
    fun s => let (a, s') := st s in (f a, s')
  map_id A := by funext st s; simp
  map_comp f g := by funext st s; simp

instance StateMonad (S : Type u) : Monad (Type u) (StateFunctor S) where
  η := { app := fun A a s => (a, s), naturality := by intros; funext a s; simp }
  μ := { app := fun A mma s => let (ma, s') := mma s in ma s',
         naturality := by intros; funext mm s; simp }
  assoc := sorry; left_unit := sorry; right_unit := sorry

def get {S : Type u} : State S S := fun s => (s, s)
def put {S : Type u} (s : S) : State S Unit := fun _ => ((), s)
```

---

## 6. 余代数与无限结构

### 6.1 余代数的定义

**定义 6.1.1** (余代数). 对 F: **C** → **C**，F-余代数 (A, α: A → F(A))。

**Lean 4 示例: 余代数**

```lean4
structure Coalgebra {C : Type u} [Category C] (F : C ⥤ C) where
  carrier : C
  struct : carrier ⟶ F.obj carrier

structure CoalgebraHom {C : Type u} [Category C] {F : C ⥤ C}
    (A B : Coalgebra F) where
  hom : A.carrier ⟶ B.carrier
  cond : F.map hom ⊚ A.struct = B.struct ⊚ hom
```

### 6.2 终余代数

**定义 6.2.1** (终余代数). (νF, out) 使任意余代数存在唯一同态到 νF。

**Lean 4 示例: 流作为终余代数**

```lean4
def StreamF (A : Type u) := Nat × A

def StreamFFunctor : Type u ⥤ Type u where
  obj := StreamF
  map f := fun (n, a) => (n, f a)
  map_id _ := rfl; map_comp _ _ := rfl

def Stream (A : Type u) := Nat → A

def StreamCoalgebra {A : Type u} : Coalgebra (StreamFFunctor : Type u ⥤ Type u) where
  carrier := Stream A
  struct := fun s => (s 0, fun n => s (n + 1))
```

### 6.3 无限数据类型

**流**: Nat → A 满足 out(s) = (s(0), λn. s(n+1))

**Lean 4 示例: 共归纳流**

```lean4
codata Stream' (A : Type u) where
  head : A
  tail : Stream' A

codef ones : Stream' Nat where head := 1; tail := ones
codef natsFrom (n : Nat) : Stream' Nat where head := n; tail := natsFrom (n + 1)
def nats : Stream' Nat := natsFrom 0

codef map {A B : Type u} (f : A → B) (s : Stream' A) : Stream' B where
  head := f s.head; tail := map f s.tail
```

### 6.4 互模拟

**定义 6.4.1** (互模拟). 关系 R ⊆ A × B 使投影是余代数同态。

**共归纳原理**: 终余代数上的互模拟包含于相等。

---

## 7. 形式化定理与证明

### 7.1 CCC↔λ演算等价性

**定理 7.1.1** (CCC↔λ演算). 存在范畴等价 CCC ≃ STLC(βη)。

**证明**:

**步骤 1**: 构造 Ψ: STLC → CCC

```
Ψ(Unit) = 1, Ψ(A × B) = Ψ(A) × Ψ(B), Ψ(A → B) = Ψ(B)^{Ψ(A)}
```

**步骤 2**: 构造 Φ: CCC → STLC

```
Φ(1) = Unit, Φ(A × B) = Φ(A) × Φ(B), Φ(B^A) = Φ(A) → Φ(B)
```

**步骤 3**: 验证 Ψ ∘ Φ ≅ Id_CCC 和 Φ ∘ Ψ ≅ Id_STLC，通过结构归纳构造同构。

**证毕** ∎

### 7.2 积的唯一性

**定理 7.2.1** (积在同构意义下唯一). 若 (P, π₁, π₂) 和 (P', π₁', π₂') 都是 A, B 的积，则 P ≅ P'。

**证明**:

存在 h = ⟨π₁, π₂⟩: P → P' 和 h' = ⟨π₁', π₂'⟩: P' → P。

验证 h' ∘ h = id_P:

```
π₁ ∘ (h' ∘ h) = (π₁ ∘ h') ∘ h = π₁' ∘ h = π₁
π₂ ∘ (h' ∘ h) = π₂' ∘ h = π₂
```

由唯一性，h' ∘ h = id_P。同理 h ∘ h' = id_{P'}。

**证毕** ∎

### 7.3 指数的伴随性

**定理 7.3.1** ((- × A) ⊣ (A ⇒ -)). Hom(B × A, C) ≅ Hom(B, C^A) 自然同构。

**证明**:

定义 φ(f) = curry(f), ψ(g) = eval ∘ (g × idₐ)。

验证 ψ ∘ φ = id:

```
ψ(φ(f)) = eval ∘ (curry(f) × idₐ) = f
```

验证 φ ∘ ψ = id:

```
φ(ψ(g)) = curry(eval ∘ (g × idₐ)) = g    (由唯一性)
```

自然性由curry的泛性质得出。

**证毕** ∎

### 7.4 单子定律

**定理 7.4.1** (单子定律). 对单子 (T, η, μ):

**(1) 结合律**: μₐ ∘ T(μₐ) = μₐ ∘ μ_{T(A)}

**(2) 左单位律**: μₐ ∘ η_{T(A)} = id_{T(A)}

**(3) 右单位律**: μₐ ∘ T(ηₐ) = id_{T(A)}

**证明**: 直接由单子定义公理，通过范畴图交换验证。

**证毕** ∎

### 7.5 Kleisli范畴良定义性

**定理 7.5.1** (Kleisli范畴良定义). 单子 (T, η, μ) 诱导的 **C**_T 满足范畴公理。

**证明**:

**(1) 结合性**:

```
(h ∘_T g) ∘_T f = μ ∘ T(μ ∘ T(h) ∘ g) ∘ f = μ ∘ T(μ) ∘ T²(h) ∘ T(g) ∘ f
h ∘_T (g ∘_T f) = μ ∘ μ_T ∘ T²(h) ∘ T(g) ∘ f = μ ∘ T(μ) ∘ T²(h) ∘ T(g) ∘ f
```

由结合律相等。

**(2) 左单位**:

```
g ∘_T η = μ ∘ T(g) ∘ η = μ ∘ η_T ∘ g = g    (由自然性和左单位律)
```

**(3) 右单位**:

```
η ∘_T f = μ ∘ T(η) ∘ f = f    (由右单位律)
```

**证毕** ∎

### 7.6 终余代数唯一性

**定理 7.6.1** (终余代数唯一性). 若 (νF, out) 和 (νF', out') 都是 F 的终余代数，则 νF ≅ νF'。

**证明**:

由终性，存在唯一 !: νF → νF' 和 !': νF' → νF。

验证 !' ∘ ! = id:

```
out ∘ (!' ∘ !) = F(!') ∘ out' ∘ ! = F(!') ∘ F(!) ∘ out = F(!' ∘ !) ∘ out
```

因此 !' ∘ ! 是到自身的同态，由唯一性等于 id。同理 ! ∘ !' = id。

**证毕** ∎

---

## 8. 参考文献

1. **Lambek, J. & Scott, P.J.** (1986). *Introduction to Higher Order Categorical Logic*. CUP.
2. **Mac Lane, S.** (1998). *Categories for the Working Mathematician* (2nd ed.). Springer.
3. **Moggi, E.** (1991). Notions of computation and monads. *Inf. & Comp.*, 93(1), 55-92.
4. **Wadler, P.** (1995). Monads for functional programming. *AFP*, 24-52.
5. **Jacobs, B.** (2016). *Introduction to Coalgebra*. CUP.
6. **Rutten, J.J.M.M.** (2000). Universal coalgebra. *TCS*, 249(1), 3-80.
7. **Awodey, S.** (2010). *Category Theory* (2nd ed.). OUP.

---

## 9. 交叉引用

### 前置依赖

- **[SEMANTIC_NOTATIONS.md](../SEMANTIC_NOTATIONS.md)** — 符号标准

### 横向关联

- **[SEMANTICS_UNIFIED_REVISED.md](../SEMANTICS_UNIFIED_REVISED.md)** — 统一语义框架
- **[BRIDGE_THEOREMS.md](../BRIDGE_THEOREMS.md)** — 桥梁定理
- **[ADVANCED_SEMANTICS.md](../ADVANCED_SEMANTICS.md)** — 高级语义主题

### 后续扩展

- 指称语义深化 — 域论模型
- 代数效应处理 — 效应系统应用
- 线性逻辑与 *
- 依赖类型与局部积闭范畴

---

*版本: 1.0 | 级别: L6+++ | 更新: 2026-03-24*
