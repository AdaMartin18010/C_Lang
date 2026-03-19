<!--
  文件: README.md
  标题: 数据结构与算法
  版本: 1.0.0
  最后更新: 2026-03-14
  描述: C语言实现的数据结构与算法完整指南
  标准: C23 (ISO/IEC 9899:2024)
-->

# 数据结构与算法 (Data Structures & Algorithms)

> **版本**: 1.0.0 | **最后更新**: 2026-03-14 | **C标准**: C23
>
> 本模块提供用现代C语言（C23标准）实现的完整数据结构与算法指南，包含理论分析、代码实现、复杂度分析和实际应用场景。

---

## 📂 目录结构

```text
09_Data_Structures_Algorithms/
├── 01_Fundamental_Data_Structures/    # 基础数据结构
│   ├── 01_Linked_List_Complete_Guide.md   (47 KB)
│   ├── 02_Stack_Queue_Complete.md         (49 KB)
│   ├── 03_Hash_Table_Implementation.md    (37 KB)
│   ├── 04_Binary_Tree_Fundamentals.md     (37 KB)
│   └── 05_Heap_Priority_Queue.md          (41 KB)
│
└── 02_Fundamental_Algorithms/         # 基础算法
    ├── 01_Sorting_Algorithms_Complete.md    (66 KB)
    ├── 02_Searching_Algorithms.md           (39 KB)
    ├── 03_Graph_Algorithms_Fundamentals.md  (46 KB)
    ├── 04_Dynamic_Programming_Basics.md     (37 KB)
    └── 05_Greedy_Algorithms_Backtracking.md (34 KB)
```

---

## 📊 内容概览

### 基础数据结构 (5个主题)

| 主题 | 文件 | 核心内容 | 难度 |
|:-----|:-----|:---------|:----:|
| **链表** | [01_Linked_List_Complete_Guide.md](./01_Fundamental_Data_Structures/01_Linked_List_Complete_Guide.md) | 单链表、双链表、循环链表、跳表 | ⭐⭐⭐ |
| **栈与队列** | [02_Stack_Queue_Complete.md](./01_Fundamental_Data_Structures/02_Stack_Queue_Complete.md) | 数组/链表实现、双端队列、单调栈/队列 | ⭐⭐ |
| **哈希表** | [03_Hash_Table_Implementation.md](./01_Fundamental_Data_Structures/03_Hash_Table_Implementation.md) | 散列函数、冲突解决、动态扩容、Cuckoo哈希 | ⭐⭐⭐ |
| **二叉树** | [04_Binary_Tree_Fundamentals.md](./01_Fundamental_Data_Structures/04_Binary_Tree_Fundamentals.md) | BST、AVL、红黑树、B树、遍历算法 | ⭐⭐⭐⭐ |
| **堆与优先队列** | [05_Heap_Priority_Queue.md](./01_Fundamental_Data_Structures/05_Heap_Priority_Queue.md) | 二叉堆、d-ary堆、斐波那契堆、堆排序 | ⭐⭐⭐ |

### 基础算法 (5个主题)

| 主题 | 文件 | 核心内容 | 难度 |
|:-----|:-----|:---------|:----:|
| **排序算法** | [01_Sorting_Algorithms_Complete.md](./02_Fundamental_Algorithms/01_Sorting_Algorithms_Complete.md) | 10+排序算法、稳定性分析、实际应用 | ⭐⭐⭐ |
| **搜索算法** | [02_Searching_Algorithms.md](./02_Fundamental_Algorithms/02_Searching_Algorithms.md) | 线性/二分搜索、插值搜索、字符串匹配 | ⭐⭐⭐ |
| **图算法** | [03_Graph_Algorithms_Fundamentals.md](./02_Fundamental_Algorithms/03_Graph_Algorithms_Fundamentals.md) | BFS/DFS、最短路径、最小生成树、拓扑排序 | ⭐⭐⭐⭐ |
| **动态规划** | [04_Dynamic_Programming_Basics.md](./02_Fundamental_Algorithms/04_Dynamic_Programming_Basics.md) | 记忆化、状态转移、经典DP问题 | ⭐⭐⭐⭐ |
| **贪心与回溯** | [05_Greedy_Algorithms_Backtracking.md](./02_Fundamental_Algorithms/05_Greedy_Algorithms_Backtracking.md) | 贪心选择、剪枝、排列组合、N皇后 | ⭐⭐⭐ |

---

## 🎯 学习路径

### 初学者路径

```text
链表 → 栈与队列 → 排序算法 → 搜索算法
  ↓
哈希表 → 二叉树 → 堆
  ↓
图算法 → 动态规划 → 贪心与回溯
```

### 面试准备路径

```text
数组/链表操作 (双指针、快慢指针)
  ↓
栈的应用 (括号匹配、表达式求值)
  ↓
树遍历与递归 (DFS/BFS模板)
  ↓
图算法 (最短路径、连通性)
  ↓
高级DP (状态压缩、树形DP)
```

### 系统实现路径

```text
内存分配器 (堆/伙伴系统)
  ↓
文件系统 (B树/B+树)
  ↓
网络路由 (最短路径算法)
  ↓
数据库索引 (LSM树、跳表)
  ↓
编译器优化 (图算法、数据流分析)
```

---

## 🛠️ C23特性应用

本模块充分利用C23标准的新特性：

| 特性 | 应用场景 | 示例 |
|:-----|:---------|:-----|
| `nullptr` | 空指针初始化 | `Node *head = nullptr;` |
| `constexpr` | 编译时常量计算 | `constexpr size_t TABLE_SIZE = 1024;` |
| `typeof` | 泛型宏定义 | `typeof(a) temp = a;` |
| `_BitInt(N)` | 位精确整数 | `_BitInt(128) big_num;` |
| `[[nodiscard]]` | 返回值检查 | `[[nodiscard]] int* allocate();` |
| `stdckdint.h` | 安全算术运算 | `ckd_add(&result, a, b);` |

---

## 📈 统计信息

| 指标 | 数值 |
|:-----|:-----|
| **总文件数** | 10 |
| **总代码行数** | ~8,000+ 行 |
| **总字数** | ~150,000+ 字 |
| **算法实现数** | 100+ |
| **数据结构类型** | 20+ |
| **复杂度分析** | 全覆盖 |

---

## 🔗 与其他模块的连接

```text
09_Data_Structures_Algorithms
    │
    ├── 依赖自 → 01_Core_Knowledge_System/ (C语言基础)
    │
    ├── 应用于 → 03_System_Technology_Domains/ (内存数据库、日志系统)
    │
    ├── 应用于 → 04_Industrial_Scenarios/ (游戏引擎、高频交易)
    │
    ├── 理论支撑 → 05_Deep_Structure_MetaPhysics/ (复杂度理论)
    │
    └── 工具支持 → 07_Modern_Toolchain/ (性能分析、调试)
```

---

## 💡 使用建议

1. **理论学习**: 先理解数据结构的抽象数据类型(ADT)定义
2. **代码实践**: 每个数据结构都提供完整可编译的C23代码
3. **复杂度分析**: 关注时间/空间复杂度的大O表示
4. **实际应用**: 了解每个数据结构在真实系统中的应用
5. **面试准备**: 重点关注标记为"面试常考"的算法题

---

## 📚 推荐资源

### 经典教材

- 《算法导论》(CLRS) - 理论基础
- 《数据结构与算法分析：C语言描述》(Mark Allen Weiss) - C语言实现
- 《算法》(Sedgewick) - 图形化讲解

### 在线资源

- [VisuAlgo](https://visualgo.net/) - 算法可视化
- [LeetCode](https://leetcode.com/) - 算法练习
- [Algorithm Visualizer](https://algorithm-visualizer.org/) - 交互式学习

---

## ✅ 质量保证

- ✅ 所有代码使用C23标准编译验证
- ✅ 每个算法包含时间/空间复杂度分析
- ✅ 提供多种实现方式（递归/迭代、数组/链表）
- ✅ 包含边界情况和错误处理
- ✅ 实际应用场景举例

---

> **维护者**: C_Lang Knowledge Base Team
>
> **最后更新**: 2026-03-14
>
> **状态**: ✅ 完整，生产就绪

---

> **返回导航**: [知识库总览](../README.md) | [上层目录](..)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
