<!--
  文件: README.md
  标题: 数据结构与算法
  版本: 1.0.0
  最后更新: 2026-03-14
  描述: C语言实现的数据结构与算法完整指南
  标准: C23 (ISO/IEC 9899:2024)
-->

# 数据结构与算法 (Data Structures & Algorithms)

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位

**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链

| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构

```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择

| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---


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
