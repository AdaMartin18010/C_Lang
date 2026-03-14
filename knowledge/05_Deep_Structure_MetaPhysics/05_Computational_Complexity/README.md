# 计算复杂性 (Computational Complexity)

> **模块**: 05_Deep_Structure_MetaPhysics | **子模块**: 05_Computational_Complexity
> **最后更新**: 2026-03-14 | **状态**: ✅ 已完成

---

## 模块简介

计算复杂性理论研究算法的计算资源需求（时间、空间），为算法设计和分析提供理论基础。
本模块探讨复杂性类别、P vs NP问题以及算法下界分析。

---

## 包含内容

| 文件 | 主题 | 大小 | 说明 |
|:-----|:-----|:----:|:-----|
| [01_Complexity_Classes.md](./01_Complexity_Classes.md) | 复杂性类别 | ~13 KB | P、NP、NP完全、PSPACE等类别详解 |

---

## 核心概念

### 复杂性类别层次

```text
P ⊆ NP ⊆ PSPACE ⊆ EXPTIME ⊆ NEXPTIME
│     │       │          │
│     └─ NP完全(NPC)     │
│            │           │
└─ 多项式时间可解   └─ 指数时间问题
```

### 关键问题

| 问题 | 类别 | 状态 |
|:-----|:-----|:-----|
| P vs NP | 决定性开放问题 | 千禧年大奖难题 |
| 快速排序 | O(n log n) | 最优比较排序 |
| 矩阵乘法 | O(n^2.373) | 理论最优待突破 |
| 布尔可满足性(SAT) | NP完全 | 首个NP完全问题 |

---

## 与计算理论的关联

```text
07_Computability_Theory/
    │
    ├── 可计算性 (哪些问题可解)
    │
    └── 05_Computational_Complexity/
            │
            └── 复杂性 (哪些问题高效可解)
```

---

## 推荐阅读顺序

1. **复杂性类别** - 理解P、NP等基本类别
2. **算法分析** - 结合09_Data_Structures_Algorithms模块
3. **复杂度下界** - 理解问题固有难度

---

> 返回: [上层目录](../README.md)
