# 代数拓扑

## 概述

代数拓扑是数学中研究拓扑空间在连续变形下保持不变的性质的学科。
通过将拓扑问题转化为代数问题，我们可以使用代数工具（如群、环、模）来分类和研究空间的拓扑结构。

## 目录

- [代数拓扑](#代数拓扑)
  - [概述](#概述)
  - [目录](#目录)
  - [拓扑空间](#拓扑空间)
    - [定义与公理](#定义与公理)
    - [度量拓扑](#度量拓扑)
  - [连续映射](#连续映射)
    - [连续性的定义](#连续性的定义)
    - [同伦](#同伦)
  - [基本群](#基本群)
    - [环路和同伦类](#环路和同伦类)
    - [计算方法](#计算方法)
  - [覆盖空间](#覆盖空间)
    - [定义与性质](#定义与性质)
    - [提升性质](#提升性质)
  - [高级主题](#高级主题)
    - [高阶同伦群](#高阶同伦群)
    - [参考资料](#参考资料)

---

## 拓扑空间

### 定义与公理

拓扑空间是研究连续性、收敛性等概念的基础框架。

```python
class Topology:
    """
    拓扑 (X, τ)：
    X 是一个集合，τ ⊆ P(X) 是 X 的幂集的子集，满足：
    1. ∅ ∈ τ, X ∈ τ
    2. 任意并封闭：若 {U_i} ⊆ τ，则 ∪U_i ∈ τ
    3. 有限交封闭：若 U, V ∈ τ，则 U ∩ V ∈ τ
    """
    def __init__(self, underlying_set, open_sets):
        self.X = underlying_set
        self.tau = open_sets
        self._verify_axioms()

    def _verify_axioms(self):
        """验证拓扑公理"""
        # 公理1: 空集和全集
        assert frozenset() in self.tau, "空集必须是开集"
        assert self.X in self.tau, "全集必须是开集"

        # 公理2: 任意并
        from itertools import combinations, chain
        for r in range(1, len(self.tau) + 1):
            for combo in combinations(self.tau, r):
                union = frozenset().union(*combo)
                assert union in self.tau, f"并集 {union} 不在拓扑中"

        # 公理3: 有限交
        for u in self.tau:
            for v in self.tau:
                assert u & v in self.tau, f"交集 {u&v} 不在拓扑中"

    def is_open(self, subset):
        """判断子集是否为开集"""
        return frozenset(subset) in self.tau

    def is_closed(self, subset):
        """判断子集是否为闭集（补集是开集）"""
        complement = self.X - frozenset(subset)
        return complement in self.tau

    def closure(self, subset):
        """
        闭包：包含 subset 的最小闭集
        cl(A) = ∩{F : F 是闭集且 A ⊆ F}
        """
        subset = frozenset(subset)
        closed_supersets = [f for f in self._all_closed_sets() if subset <= f]
        if not closed_supersets:
            return self.X
        return frozenset.intersection(*closed_supersets)

    def _all_closed_sets(self):
        """所有闭集的集合"""
        return [self.X - u for u in self.tau]

    def interior(self, subset):
        """
        内部：包含于 subset 的最大开集
        int(A) = ∪{U : U 是开集且 U ⊆ A}
        """
        subset = frozenset(subset)
        open_subsets = [u for u in self.tau if u <= subset]
        if not open_subsets:
            return frozenset()
        return frozenset.union(*open_subsets)

# 示例：离散拓扑和平凡拓扑
print("=== 拓扑空间示例 ===")
X = frozenset({1, 2, 3})

# 离散拓扑：所有子集都是开集
discrete_tau = {frozenset(s) for s in [{}, {1}, {2}, {3}, {1,2}, {1,3}, {2,3}, {1,2,3}]}
discrete_top = Topology(X, discrete_tau)
print(f"离散拓扑：所有子集都是开集")

# 平凡拓扑：只有 ∅ 和 X 是开集
trivial_tau = {frozenset(), X}
trivial_top = Topology(X, trivial_tau)
print(f"平凡拓扑：开集数量 = {len(trivial_tau)}")
```

### 度量拓扑

```python
import math

class MetricSpace:
    """
    度量空间 (X, d)：
    d: X × X -> [0, ∞) 满足：
    1. d(x, y) = 0 ⟺ x = y （正定性）
    2. d(x, y) = d(y, x) （对称性）
    3. d(x, z) ≤ d(x, y) + d(y, z) （三角不等式）
    """
    def __init__(self, points, distance_func):
        self.points = set(points)
        self.d = distance_func

    def open_ball(self, center, radius):
        """开球 B(x, r) = {y : d(x, y) < r}"""
        return {p for p in self.points if self.d(center, p) < radius}

    def closed_ball(self, center, radius):
        """闭球 B̄(x, r) = {y : d(x, y) ≤ r}"""
        return {p for p in self.points if self.d(center, p) <= radius}

    def induced_topology(self):
        """
        度量诱导的拓扑：
        开集是开球的任意并
        """
        # 简化实现：返回所有可能的开球并集
        all_open_balls = []
        for p in self.points:
            for r in [0.5, 1, 2, 5]:
                all_open_balls.append(frozenset(self.open_ball(p, r)))

        # 生成拓扑（包含所有并集）
        from itertools import combinations
        tau = {frozenset()}
        for r in range(1, len(all_open_balls) + 1):
            for combo in combinations(all_open_balls, r):
                tau.add(frozenset.union(*combo) if combo else frozenset())
        tau.add(frozenset(self.points))
        return Topology(frozenset(self.points), tau)

# 欧几里得度量示例
def euclidean_distance(p1, p2):
    return math.sqrt(sum((a - b) ** 2 for a, b in zip(p1, p2)))

# 有限点集上的欧几里得度量
grid_points = [(i, j) for i in range(3) for j in range(3)]
euclidean_space = MetricSpace(grid_points, euclidean_distance)
print(f"\n度量空间中的开球 B((0,0), 1.5):")
print(euclidean_space.open_ball((0, 0), 1.5))
```

---

## 连续映射

### 连续性的定义

```python
class ContinuousMap:
    """
    连续映射 f: (X, τ_X) -> (Y, τ_Y)
    等价条件：
    1. Y 中任意开集的原像是 X 中的开集
    2. Y 中任意闭集的原像是 X 中的闭集
    3. ∀A ⊆ X: f(cl_X(A)) ⊆ cl_Y(f(A))
    """
    def __init__(self, domain: Topology, codomain: Topology, mapping: dict):
        self.X = domain
        self.Y = codomain
        self.f = mapping  # 点级的映射
        self._verify_well_defined()

    def _verify_well_defined(self):
        """验证映射是良定义的"""
        assert set(self.f.keys()) == self.X.X, "定义域不匹配"
        assert set(self.f.values()).issubset(self.Y.X), "值域不匹配"

    def is_continuous(self) -> bool:
        """验证连续性（开集定义）"""
        for v in self.Y.tau:
            preimage = frozenset({x for x in self.X.X if self.f[x] in v})
            if preimage not in self.X.tau:
                return False
        return True

    def preimage(self, subset):
        """原像 f^{-1}(B) = {x ∈ X : f(x) ∈ B}"""
        return frozenset({x for x in self.X.X if self.f[x] in subset})

    def image(self, subset):
        """像 f(A) = {f(x) : x ∈ A}"""
        return frozenset({self.f[x] for x in subset})

    def compose(self, other: 'ContinuousMap') -> 'ContinuousMap':
        """
        映射复合 (f ∘ g)(x) = f(g(x))
        连续映射的复合仍是连续的
        """
        assert self.X == other.Y, "复合条件不满足"
        composed = {x: self.f[other.f[x]] for x in other.X.X}
        return ContinuousMap(other.X, self.Y, composed)

    def is_homeomorphism(self) -> bool:
        """
        同胚：双射且 f 和 f^{-1} 都连续
        同胚的空间具有相同的拓扑性质
        """
        # 检查双射
        if set(self.f.values()) != self.Y.X:
            return False
        if len(set(self.f.values())) != len(self.f):
            return False

        # 检查连续性
        if not self.is_continuous():
            return False

        # 检查逆映射连续性
        inverse = {v: k for k, v in self.f.items()}
        inv_map = ContinuousMap(self.Y, self.X, inverse)
        return inv_map.is_continuous()

# 示例：恒等映射是连续的
X = frozenset({1, 2})
discrete_tau = {frozenset(), frozenset({1}), frozenset({2}), X}
X_top = Topology(X, discrete_tau)

identity = {1: 1, 2: 2}
id_map = ContinuousMap(X_top, X_top, identity)
print(f"\n恒等映射是连续的: {id_map.is_continuous()}")
print(f"恒等映射是同胚: {id_map.is_homeomorphism()}")
```

### 同伦

```python
class Homotopy:
    """
    同伦 H: X × [0,1] -> Y
    连接两个连续映射 f, g: X -> Y
    H(x, 0) = f(x), H(x, 1) = g(x)

    同伦是映射空间上的等价关系
    """
    def __init__(self, H, X: Topology, Y: Topology):
        self.H = H  # 同伦映射
        self.X = X
        self.Y = Y

    def at_time(self, t: float):
        """在时间 t 的切片映射 H_t(x) = H(x, t)"""
        return {x: self.H(x, t) for x in self.X.X}

    def connects(self, f: dict, g: dict) -> bool:
        """验证 H 是否连接 f 和 g"""
        h0 = self.at_time(0)
        h1 = self.at_time(1)
        return h0 == f and h1 == g

class HomotopyEquivalence:
    """
    同伦等价：f: X -> Y 和 g: Y -> X 满足
    g ∘ f ≃ id_X 且 f ∘ g ≃ id_Y

    同伦等价的空间具有相同的同伦型
    """
    def __init__(self, f: ContinuousMap, g: ContinuousMap):
        self.f = f
        self.g = g

    def is_equivalence(self) -> bool:
        """验证同伦等价"""
        # g ∘ f ≃ id_X
        gf = self.g.compose(self.f)
        id_X = {x: x for x in self.f.X.X}

        # f ∘ g ≃ id_Y
        fg = self.f.compose(self.g)
        id_Y = {y: y for y in self.f.Y.X}

        # 这里简化处理，实际需要构造同伦
        return True
```

---

## 基本群

### 环路和同伦类

```python
from typing import Callable, List
import numpy as np

class Path:
    """
    道路 γ: [0,1] -> X，连续映射
    γ(0) = 起点，γ(1) = 终点
    """
    def __init__(self, map_func: Callable[[float], tuple], space: MetricSpace):
        self.gamma = map_func
        self.space = space

    def start(self):
        return self.gamma(0.0)

    def end(self):
        return self.gamma(1.0)

    def is_loop(self, basepoint) -> bool:
        """判断是否是基于 basepoint 的环路"""
        return self.start() == basepoint and self.end() == basepoint

    def reverse(self) -> 'Path':
        """逆道路 γ⁻¹(t) = γ(1-t)"""
        return Path(lambda t: self.gamma(1 - t), self.space)

    def concatenate(self, other: 'Path') -> 'Path':
        """
        道路连接 (γ * η)(t) =
          γ(2t)      如果 0 ≤ t ≤ 0.5
          η(2t-1)    如果 0.5 ≤ t ≤ 1
        要求 γ(1) = η(0)
        """
        assert self.end() == other.start(), "道路终点不匹配"

        def concatenated(t):
            if t < 0.5:
                return self.gamma(2 * t)
            else:
                return other.gamma(2 * t - 1)
        return Path(concatenated, self.space)

class FundamentalGroup:
    """
    基本群 π₁(X, x₀)：基于 x₀ 的所有环路的同伦类构成的群

    群运算：[γ] · [η] = [γ * η]（道路连接）
    单位元：常值道路 [c_{x₀}]
    逆元：[γ]⁻¹ = [γ⁻¹]
    """
    def __init__(self, space: MetricSpace, basepoint: tuple):
        self.X = space
        self.x0 = basepoint
        self.elements: List[Path] = []  # 环路代表元

    def add_loop(self, path: Path):
        """添加环路代表元"""
        assert path.is_loop(self.x0)
        self.elements.append(path)

    def is_simply_connected(self) -> bool:
        """
        单连通：基本群是平凡群
        即所有环路都同伦于常值道路
        """
        return len(self.elements) == 0  # 简化判断

    def fundamental_group_order(self) -> int:
        """返回基本群的阶（元素个数），∞ 表示无限"""
        # 简化实现
        return len(self.elements) + 1  # +1 为单位元

# 示例：圆 S¹ 的基本群
print("\n=== 基本群示例 ===")
print("圆 S¹ 的基本群 π₁(S¹) ≅ ℤ（整数加法群）")
print("n 对应绕圆 n 圈的环路")
```

### 计算方法

```python
class VanKampenTheorem:
    """
    Seifert-van Kampen 定理：
    若 X = U ∪ V，U、V、U∩V 道路连通
    则 π₁(X) ≅ π₁(U) *_{π₁(U∩V)} π₁(V)（融合自由积）
    """
    def __init__(self, U: FundamentalGroup, V: FundamentalGroup,
                 intersection: FundamentalGroup):
        self.pi1_U = U
        self.pi1_V = V
        self.pi1_intersection = intersection

    def compute(self) -> str:
        """计算基本群的表示"""
        # 简化实现：返回群的表示形式
        return "π₁(X) ≅ π₁(U) *_{π₁(U∩V)} π₁(V)"

class CoveringSpace:
    """
    覆盖空间 p: E -> B
    局部同胚且纤维离散
    """
    def __init__(self, total_space, base_space, projection):
        self.E = total_space
        self.B = base_space
        self.p = projection

    def monodromy_action(self, loop: Path, fiber_point):
        """
        单值作用：基本群作用在纤维上
        给出覆盖空间与基本群子群之间的对应
        """
        # 提升环路并追踪终点
        pass
```

---

## 覆盖空间

### 定义与性质

```python
class CoveringMap:
    """
    覆盖映射 p: E → B 满足：
    1. p 是连续满射
    2. ∀b ∈ B，存在开邻域 U 使得 p^{-1}(U) 是 E 中不相交开集的并
    3. p 限制在每个这样的开集上是到 U 的同胚
    """
    def __init__(self, total: Topology, base: Topology,
                 projection: dict, fiber_size: int):
        self.E = total
        self.B = base
        self.p = projection
        self.fiber_cardinality = fiber_size

    def fiber(self, b):
        """纤维 p^{-1}(b)"""
        return {e for e in self.E.X if self.p[e] == b}

    def is_universal_cover(self) -> bool:
        """
        万有覆盖：E 是单连通的
        万有覆盖在覆盖空间中是初始对象
        """
        # 简化判断
        return True

    def deck_transformations(self) -> List[dict]:
        """
        覆叠变换（Deck Transformations）：
        满足 p ∘ φ = p 的同胚 φ: E -> E
        构成群 Deck(E/B)
        """
        # 寻找所有这样的同胚
        return []

# 示例：圆的万有覆盖
print("\n=== 覆盖空间示例 ===")
print("指数映射 exp: ℝ -> S¹, t ↦ e^{2πit}")
print("ℝ 是 S¹ 的万有覆盖")
print("覆叠变换群 ≅ ℤ，由 t ↦ t + n 生成")
```

### 提升性质

```python
class LiftingProperty:
    """
    道路提升和映射提升是覆盖空间的核心性质
    """
    def __init__(self, covering: CoveringMap):
        self.cover = covering

    def lift_path(self, path_in_B: Path, start_in_E) -> Path:
        """
        道路提升：给定 B 中的道路 γ 和起点 ẽ ∈ p^{-1}(γ(0))
        存在唯一的提升 γ̃: [0,1] -> E 使得 γ̃(0) = ẽ 且 p ∘ γ̃ = γ
        """
        # 实现道路提升算法
        def lifted_map(t):
            # 逐步提升
            return start_in_E  # 简化

        return Path(lifted_map, self.cover.E)

    def lift_map(self, f: ContinuousMap, E_start) -> ContinuousMap:
        """
        映射提升准则：
        f: X -> B 有提升 f̃: X -> E 当且仅当
        f_*(π₁(X)) ⊆ p_*(π₁(E))
        """
        # 验证提升条件
        pass
```

---

## 高级主题

### 高阶同伦群

```python
class HigherHomotopyGroup:
    """
    高阶同伦群 π_n(X, x₀)：n 维球面到 X 的映射的同伦类

    n ≥ 2 时，π_n 是 Abel 群
    """
    def __init__(self, space, n: int, basepoint):
        self.X = space
        self.n = n
        self.x0 = basepoint

    def is_abelian(self) -> bool:
        """n ≥ 2 时同伦群是 Abel 群"""
        return self.n >= 2

    def homotopy_groups_of_sphere(self) -> str:
        """
        球面的同伦群 π_n(S^k) 是代数拓扑中的经典问题
        对于 n < k: π_n(S^k) = 0
        对于 n = k: π_n(S^n) ≅ ℤ
        """
        return f"π_{self.n}(S^n) 的计算是复杂的问题"
```

### 参考资料

1. Hatcher, A. (2002). *Algebraic Topology*. Cambridge University Press.
2. Munkres, J. R. (2000). *Topology* (2nd ed.). Prentice Hall.
3. May, J. P. (1999). *A Concise Course in Algebraic Topology*.
