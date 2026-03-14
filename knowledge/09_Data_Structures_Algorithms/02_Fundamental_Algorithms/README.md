# 基础算法

> **模块**: 09_Data_Structures_Algorithms | **子模块**: 02_Fundamental_Algorithms
> **最后更新**: 2026-03-14 | **状态**: ✅ 已完成

---

## 模块简介

本模块涵盖算法设计的核心范式：排序、搜索、图算法、动态规划和贪心策略。每个算法都包含C23实现、复杂度分析和实际应用场景。

---

## 包含内容

| 文件 | 主题 | 大小 | 关键算法 |
|:-----|:-----|:----:|:---------|
| [01_Sorting_Algorithms_Complete.md](./01_Sorting_Algorithms_Complete.md) | 排序算法完整指南 | 66 KB | 快排、归并、堆排、计数排序、基数排序 |
| [02_Searching_Algorithms.md](./02_Searching_Algorithms.md) | 搜索算法 | 39 KB | 二分、插值、字符串匹配(KMP) |
| [03_Graph_Algorithms_Fundamentals.md](./03_Graph_Algorithms_Fundamentals.md) | 图算法基础 | 46 KB | BFS/DFS、Dijkstra、Prim、拓扑排序 |
| [04_Dynamic_Programming_Basics.md](./04_Dynamic_Programming_Basics.md) | 动态规划基础 | 37 KB | 背包、LCS、状态压缩、经典问题 |
| [05_Greedy_Algorithms_Backtracking.md](./05_Greedy_Algorithms_Backtracking.md) | 贪心与回溯 | 34 KB | 区间调度、排列组合、剪枝、N皇后 |

---

## 学习路径

```
基础阶段:
排序算法 → 搜索算法 → 图算法

进阶阶段:
动态规划 → 贪心与回溯 → 高级图论

实战阶段:
面试题 → 算法竞赛 → 系统实现
```

---

## 算法范式对照

| 范式 | 核心思想 | 典型应用 | 代表算法 |
|:-----|:---------|:---------|:---------|
| **分治** | 分而治之 | 排序、几何 | 快排、归并、最近点对 |
| **贪心** | 局部最优 | 调度、压缩 | Huffman、Dijkstra、Prim |
| **动态规划** | 最优子结构 | 优化问题 | 背包、LCS、最短路径 |
| **回溯** | 深度搜索+剪枝 | 组合问题 | N皇后、数独、排列组合 |
| **图论** | 网络结构 | 网络流、连通性 | BFS/DFS、MST、最大流 |

---

## 复杂度对比

### 排序算法

| 算法 | 最优 | 平均 | 最差 | 空间 | 稳定性 |
|:-----|:----:|:----:|:----:|:----:|:------:|
| 冒泡排序 | O(n) | O(n²) | O(n²) | O(1) | ✅ |
| 快速排序 | O(n log n) | O(n log n) | O(n²) | O(log n) | ❌ |
| 归并排序 | O(n log n) | O(n log n) | O(n log n) | O(n) | ✅ |
| 堆排序 | O(n log n) | O(n log n) | O(n log n) | O(1) | ❌ |
| 计数排序 | O(n+k) | O(n+k) | O(n+k) | O(k) | ✅ |

### 图算法

| 算法 | 时间复杂度 | 空间复杂度 | 适用场景 |
|:-----|:-----------|:-----------|:---------|
| BFS | O(V+E) | O(V) | 最短路径(无权)、连通性 |
| DFS | O(V+E) | O(V) | 拓扑排序、强连通分量 |
| Dijkstra | O((V+E) log V) | O(V) | 单源最短路径(正权) |
| Bellman-Ford | O(VE) | O(V) | 负权边检测 |
| Floyd-Warshall | O(V³) | O(V²) | 全源最短路径 |
| Prim | O((V+E) log V) | O(V) | 最小生成树 |
| Kruskal | O(E log E) | O(V) | 最小生成树 |

---

## 应用场景

| 领域 | 常用算法 | 相关模块 |
|:-----|:---------|:---------|
| **数据库** | B+树索引、LSM树 | 03_System_Technology_Domains/11_In_Memory_Database |
| **网络路由** | Dijkstra、Bellman-Ford | 03_System_Technology_Domains/15_Network_Programming |
| **编译器** | 图着色、拓扑排序 | 02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping |
| **游戏AI** | A*、Minimax | 04_Industrial_Scenarios/05_Game_Engine |
| **调度系统** | 贪心、动态规划 | 04_Industrial_Scenarios/03_High_Frequency_Trading |

---

> 返回: [上层目录](../README.md)
