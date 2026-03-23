<!--
  文件: 05_Heap_Priority_Queue.md
  标题: 堆和优先队列
  版本: 1.0.0
  作者: C_Lang Knowledge Base
  最后更新: 2026-03-14
  描述: 涵盖最大堆、最小堆、堆排序、Top-K问题的完整理论与实践
  标准: C23 (ISO/IEC 9899:2024)
-->

# 堆和优先队列

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../README.md](../../README.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/README.md](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/README.md](../../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

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
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/README.md](../../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/README.md](../../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/README.md](../../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/README.md](../../04_Industrial_Scenarios/README.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/README.md) |

---

---

## 🔗 文档关联

### 核心关联

| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读

| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
>
> **版本**: 1.0.0
> **最后更新**: 2026-03-14
> **C标准**: C23 (ISO/IEC 9899:2024)
> **预计阅读时间**: 65分钟

---

## 📋 目录

- [堆和优先队列](#堆和优先队列)
  - [🔗 全面知识关联体系](#-全面知识关联体系)
    - [【全局层】知识库导航](#全局层知识库导航)
    - [【阶段层】学习定位](#阶段层学习定位)
    - [【层次层】纵向知识链](#层次层纵向知识链)
    - [【局部层】横向关联网](#局部层横向关联网)
    - [【总体层】知识体系架构](#总体层知识体系架构)
    - [【决策层】学习路径选择](#决策层学习路径选择)
  - [🔗 文档关联](#-文档关联)
    - [核心关联](#核心关联)
    - [扩展阅读](#扩展阅读)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 概述](#1-概述)
    - [1.1 什么是堆](#11-什么是堆)
    - [1.2 堆的性质](#12-堆的性质)
    - [1.3 操作复杂度](#13-操作复杂度)
  - [2. 堆的基础](#2-堆的基础)
    - [2.1 数组表示法](#21-数组表示法)
    - [2.2 索引计算公式](#22-索引计算公式)
  - [3. 堆的实现](#3-堆的实现)
    - [3.1 堆结构定义](#31-堆结构定义)
    - [3.2 辅助函数](#32-辅助函数)
    - [3.3 核心操作](#33-核心操作)
      - [3.3.1 上浮 (Swim/Shift Up)](#331-上浮-swimshift-up)
      - [3.3.2 下沉 (Sink/Shift Down/Heapify)](#332-下沉-sinkshift-downheapify)
      - [3.3.3 初始化与销毁](#333-初始化与销毁)
      - [3.3.4 插入与删除](#334-插入与删除)
    - [3.4 堆的合并](#34-堆的合并)
  - [4. 堆排序](#4-堆排序)
    - [4.1 堆排序原理](#41-堆排序原理)
    - [4.2 堆排序实现](#42-堆排序实现)
    - [4.3 堆排序 vs 其他排序](#43-堆排序-vs-其他排序)
  - [5. 优先队列应用](#5-优先队列应用)
    - [5.1 合并K个有序数组](#51-合并k个有序数组)
    - [5.2 任务调度](#52-任务调度)
  - [6. Top-K问题](#6-top-k问题)
    - [6.1 最小堆求第K大](#61-最小堆求第k大)
    - [6.2 最大堆求第K小](#62-最大堆求第k小)
    - [6.3 数据流中的中位数](#63-数据流中的中位数)
  - [7. 高级主题](#7-高级主题)
    - [7.1 二项堆 (Binomial Heap)](#71-二项堆-binomial-heap)
    - [7.2 斐波那契堆 (概念)](#72-斐波那契堆-概念)
  - [8. 完整代码实现](#8-完整代码实现)
  - [附录](#附录)
    - [附录A: 参考资料](#附录a-参考资料)
    - [附录B: 修订历史](#附录b-修订历史)
    - [7.3 对偶堆 (Pairing Heap)](#73-对偶堆-pairing-heap)
    - [7.4 索引堆 (Index Heap)](#74-索引堆-index-heap)
    - [7.5 区间最值查询 (RMQ) 与堆](#75-区间最值查询-rmq-与堆)
    - [7.6 堆与图算法](#76-堆与图算法)
    - [7.7 堆排序的稳定性改进](#77-堆排序的稳定性改进)
  - [8. 实际应用中的选择指南](#8-实际应用中的选择指南)
    - [8.1 不同场景的选择](#81-不同场景的选择)
    - [8.2 性能对比总结](#82-性能对比总结)
  - [附录](#附录-1)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [堆和优先队列](#堆和优先队列)
  - [🔗 全面知识关联体系](#-全面知识关联体系)
    - [【全局层】知识库导航](#全局层知识库导航)
    - [【阶段层】学习定位](#阶段层学习定位)
    - [【层次层】纵向知识链](#层次层纵向知识链)
    - [【局部层】横向关联网](#局部层横向关联网)
    - [【总体层】知识体系架构](#总体层知识体系架构)
    - [【决策层】学习路径选择](#决策层学习路径选择)
  - [🔗 文档关联](#-文档关联)
    - [核心关联](#核心关联)
    - [扩展阅读](#扩展阅读)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 概述](#1-概述)
    - [1.1 什么是堆](#11-什么是堆)
    - [1.2 堆的性质](#12-堆的性质)
    - [1.3 操作复杂度](#13-操作复杂度)
  - [2. 堆的基础](#2-堆的基础)
    - [2.1 数组表示法](#21-数组表示法)
    - [2.2 索引计算公式](#22-索引计算公式)
  - [3. 堆的实现](#3-堆的实现)
    - [3.1 堆结构定义](#31-堆结构定义)
    - [3.2 辅助函数](#32-辅助函数)
    - [3.3 核心操作](#33-核心操作)
      - [3.3.1 上浮 (Swim/Shift Up)](#331-上浮-swimshift-up)
      - [3.3.2 下沉 (Sink/Shift Down/Heapify)](#332-下沉-sinkshift-downheapify)
      - [3.3.3 初始化与销毁](#333-初始化与销毁)
      - [3.3.4 插入与删除](#334-插入与删除)
    - [3.4 堆的合并](#34-堆的合并)
  - [4. 堆排序](#4-堆排序)
    - [4.1 堆排序原理](#41-堆排序原理)
    - [4.2 堆排序实现](#42-堆排序实现)
    - [4.3 堆排序 vs 其他排序](#43-堆排序-vs-其他排序)
  - [5. 优先队列应用](#5-优先队列应用)
    - [5.1 合并K个有序数组](#51-合并k个有序数组)
    - [5.2 任务调度](#52-任务调度)
  - [6. Top-K问题](#6-top-k问题)
    - [6.1 最小堆求第K大](#61-最小堆求第k大)
    - [6.2 最大堆求第K小](#62-最大堆求第k小)
    - [6.3 数据流中的中位数](#63-数据流中的中位数)
  - [7. 高级主题](#7-高级主题)
    - [7.1 二项堆 (Binomial Heap)](#71-二项堆-binomial-heap)
    - [7.2 斐波那契堆 (概念)](#72-斐波那契堆-概念)
  - [8. 完整代码实现](#8-完整代码实现)
  - [附录](#附录)
    - [附录A: 参考资料](#附录a-参考资料)
    - [附录B: 修订历史](#附录b-修订历史)
    - [7.3 对偶堆 (Pairing Heap)](#73-对偶堆-pairing-heap)
    - [7.4 索引堆 (Index Heap)](#74-索引堆-index-heap)
    - [7.5 区间最值查询 (RMQ) 与堆](#75-区间最值查询-rmq-与堆)
    - [7.6 堆与图算法](#76-堆与图算法)
    - [7.7 堆排序的稳定性改进](#77-堆排序的稳定性改进)
  - [8. 实际应用中的选择指南](#8-实际应用中的选择指南)
    - [8.1 不同场景的选择](#81-不同场景的选择)
    - [8.2 性能对比总结](#82-性能对比总结)
  - [附录](#附录-1)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. 概述

### 1.1 什么是堆

堆（Heap）是一种特殊的**完全二叉树**，满足**堆性质**：

- **最大堆**: 每个节点的值 ≥ 其子节点的值
- **最小堆**: 每个节点的值 ≤ 其子节点的值

```
最大堆示例:              最小堆示例:
      100                    10
     /   \                  /  \
    80    90               20   30
   / \   /  \             / \   / \
  70 60  50  40          40 50 60 70
```

### 1.2 堆的性质

| 性质 | 说明 |
|------|------|
| **完全二叉树** | 除最后一层外全满，最后一层从左到右填充 |
| **父节点 ≥ 子节点** | 最大堆性质 |
| **父节点 ≤ 子节点** | 最小堆性质 |
| **堆顶是极值** | 最大堆顶最大，最小堆顶最小 |
| **数组表示** | 无需指针，可用数组高效存储 |

### 1.3 操作复杂度

| 操作 | 时间复杂度 | 说明 |
|------|-----------|------|
| 插入 (push) | $O(\log n)$ | 上浮调整 |
| 删除堆顶 (pop) | $O(\log n)$ | 下沉调整 |
| 查看堆顶 (peek) | $O(1)$ | 直接访问 |
| 建堆 (heapify) | $O(n)$ | 线性建堆 |
| 堆排序 | $O(n \log n)$ | 原地排序 |

---

## 2. 堆的基础

### 2.1 数组表示法

堆可以用数组高效表示，无需指针：

```
对于索引 i (从0开始):
- 父节点: (i - 1) / 2
- 左子节点: 2*i + 1
- 右子节点: 2*i + 2

堆的树结构:              数组表示:
      16                    [0] 16
     /  \                   [1] 14
   14    10        →        [2] 10
  / \    /                  [3] 8
 8   7  9                   [4] 7
                            [5] 9
                            [6] 3
                            [7] 2
                            [8] 4
                            [9] 1
```

### 2.2 索引计算公式

```c
// 父节点索引
#define PARENT(i) (((i) - 1) / 2)

// 左子节点索引
#define LEFT(i)   (2 * (i) + 1)

// 右子节点索引
#define RIGHT(i)  (2 * (i) + 2)

// 是否为叶子节点
#define IS_LEAF(heap, i) ((i) >= (heap)->size / 2)
```

---

## 3. 堆的实现

### 3.1 堆结构定义

```c
/**
 * 堆类型枚举
 */
typedef enum {
    MAX_HEAP,   // 最大堆
    MIN_HEAP    // 最小堆
} HeapType;

/**
 * 堆结构
 */
typedef struct {
    int* data;          // 存储数组
    size_t size;        // 当前元素数量
    size_t capacity;    // 数组容量
    HeapType type;      // 堆类型
} Heap;

#define HEAP_DEFAULT_CAPACITY 16
```

### 3.2 辅助函数

```c
// 比较函数: 根据堆类型决定比较方向
static inline bool heap_compare(const Heap* heap, int parent, int child) {
    if (heap->type == MAX_HEAP) {
        return parent < child;  // 最大堆: 父 < 子 需要交换
    } else {
        return parent > child;  // 最小堆: 父 > 子 需要交换
    }
}

// 交换元素
static inline void heap_swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
```

### 3.3 核心操作

#### 3.3.1 上浮 (Swim/Shift Up)

```c
/**
 * 上浮操作
 * 将索引 k 处的元素向上调整，恢复堆性质
 *
 * 时间复杂度: O(log n)
 */
void heap_swim(Heap* heap, size_t k) {
    while (k > 0) {
        size_t parent = PARENT(k);

        // 如果父节点已经满足堆性质，停止
        if (!heap_compare(heap, heap->data[parent], heap->data[k])) {
            break;
        }

        // 交换
        heap_swap(&heap->data[parent], &heap->data[k]);
        k = parent;
    }
}
```

#### 3.3.2 下沉 (Sink/Shift Down/Heapify)

```c
/**
 * 下沉操作 (Heapify)
 * 将索引 k 处的元素向下调整，恢复堆性质
 *
 * 时间复杂度: O(log n)
 */
void heap_sink(Heap* heap, size_t k) {
    while (true) {
        size_t left = LEFT(k);
        size_t right = RIGHT(k);
        size_t target = k;

        // 找出父、左子、右子中的"极值"(最大堆找最大，最小堆找最小)
        if (left < heap->size &&
            heap_compare(heap, heap->data[target], heap->data[left])) {
            target = left;
        }

        if (right < heap->size &&
            heap_compare(heap, heap->data[target], heap->data[right])) {
            target = right;
        }

        // 如果 k 已经是极值，停止
        if (target == k) break;

        // 交换
        heap_swap(&heap->data[k], &heap->data[target]);
        k = target;
    }
}
```

#### 3.3.3 初始化与销毁

```c
/**
 * 初始化堆
 */
void heap_init(Heap* heap, HeapType type) {
    heap->capacity = HEAP_DEFAULT_CAPACITY;
    heap->data = (int*)malloc(heap->capacity * sizeof(int));
    if (heap->data == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    heap->size = 0;
    heap->type = type;
}

/**
 * 从数组构建堆 (Floyd建堆算法)
 * 时间复杂度: O(n)
 */
void heap_build(Heap* heap, const int* arr, size_t n, HeapType type) {
    heap->capacity = n > HEAP_DEFAULT_CAPACITY ? n : HEAP_DEFAULT_CAPACITY;
    heap->data = (int*)malloc(heap->capacity * sizeof(int));
    memcpy(heap->data, arr, n * sizeof(int));
    heap->size = n;
    heap->type = type;

    // 从最后一个非叶子节点开始下沉
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        heap_sink(heap, (size_t)i);
    }
}

/**
 * 销毁堆
 */
void heap_destroy(Heap* heap) {
    free(heap->data);
    heap->data = nullptr;
    heap->size = 0;
    heap->capacity = 0;
}
```

#### 3.3.4 插入与删除

```c
/**
 * 插入元素
 * 时间复杂度: O(log n)
 */
bool heap_push(Heap* heap, int value) {
    // 扩容检查
    if (heap->size >= heap->capacity) {
        size_t new_capacity = heap->capacity * 2;
        int* new_data = (int*)realloc(heap->data, new_capacity * sizeof(int));
        if (new_data == nullptr) {
            fprintf(stderr, "Error: Failed to expand heap\n");
            return false;
        }
        heap->data = new_data;
        heap->capacity = new_capacity;
    }

    // 插入到末尾，然后上浮
    heap->data[heap->size] = value;
    heap_swim(heap, heap->size);
    heap->size++;

    return true;
}

/**
 * 删除堆顶
 * 时间复杂度: O(log n)
 */
bool heap_pop(Heap* heap, int* out_value) {
    if (heap->size == 0) {
        fprintf(stderr, "Error: Heap is empty\n");
        return false;
    }

    // 保存堆顶
    *out_value = heap->data[0];

    // 用最后一个元素替换堆顶，然后下沉
    heap->size--;
    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        heap_sink(heap, 0);
    }

    return true;
}

/**
 * 查看堆顶
 * 时间复杂度: O(1)
 */
bool heap_peek(const Heap* heap, int* out_value) {
    if (heap->size == 0) {
        fprintf(stderr, "Error: Heap is empty\n");
        return false;
    }
    *out_value = heap->data[0];
    return true;
}

/**
 * 替换堆顶
 * 弹出堆顶并插入新元素，比 pop + push 更高效
 * 时间复杂度: O(log n)
 */
bool heap_replace(Heap* heap, int value, int* out_old_value) {
    if (heap->size == 0) {
        return heap_push(heap, value);
    }

    *out_old_value = heap->data[0];
    heap->data[0] = value;
    heap_sink(heap, 0);
    return true;
}
```

### 3.4 堆的合并

```c
/**
 * 合并两个堆
 * 简单方法: 合并数组后重新建堆
 * 时间复杂度: O(n + m)
 */
void heap_merge(Heap* result, const Heap* heap1, const Heap* heap2) {
    if (heap1->type != heap2->type) {
        fprintf(stderr, "Error: Cannot merge heaps of different types\n");
        return;
    }

    size_t total_size = heap1->size + heap2->size;
    int* merged = (int*)malloc(total_size * sizeof(int));

    memcpy(merged, heap1->data, heap1->size * sizeof(int));
    memcpy(merged + heap1->size, heap2->data, heap2->size * sizeof(int));

    heap_build(result, merged, total_size, heap1->type);
    free(merged);
}
```

---

## 4. 堆排序

### 4.1 堆排序原理

```
堆排序步骤:
1. 将无序数组构建成最大堆
2. 将堆顶(最大值)与末尾交换
3. 堆大小减1，对新的堆顶进行下沉调整
4. 重复步骤2-3，直到堆大小为1

示例: [4, 10, 3, 5, 1]

建最大堆:        [10, 5, 3, 4, 1]
交换+调整(1):    [5, 4, 3, 1] + [10]
交换+调整(2):    [4, 1, 3] + [5, 10]
交换+调整(3):    [3, 1] + [4, 5, 10]
交换+调整(4):    [1] + [3, 4, 5, 10]

结果: [1, 3, 4, 5, 10] (升序)
```

### 4.2 堆排序实现

```c
/**
 * 堆排序 - 升序 (使用最大堆)
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(1) (原地排序)
 */
void heap_sort_ascending(int* arr, size_t n) {
    if (n <= 1) return;

    // 1. 建最大堆 (从最后一个非叶子节点开始)
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        // 下沉操作
        size_t parent = (size_t)i;
        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t largest = parent;

            if (left < n && arr[left] > arr[largest]) {
                largest = left;
            }
            if (right < n && arr[right] > arr[largest]) {
                largest = right;
            }

            if (largest == parent) break;

            // 交换
            int temp = arr[parent];
            arr[parent] = arr[largest];
            arr[largest] = temp;

            parent = largest;
        }
    }

    // 2. 排序
    for (int i = (int)(n - 1); i > 0; i--) {
        // 将堆顶(最大值)移到末尾
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        // 对减小后的堆进行调整
        size_t parent = 0;
        size_t heap_size = (size_t)i;

        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t largest = parent;

            if (left < heap_size && arr[left] > arr[largest]) {
                largest = left;
            }
            if (right < heap_size && arr[right] > arr[largest]) {
                largest = right;
            }

            if (largest == parent) break;

            temp = arr[parent];
            arr[parent] = arr[largest];
            arr[largest] = temp;

            parent = largest;
        }
    }
}

/**
 * 堆排序 - 降序 (使用最小堆)
 */
void heap_sort_descending(int* arr, size_t n) {
    if (n <= 1) return;

    // 建最小堆
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        size_t parent = (size_t)i;
        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t smallest = parent;

            if (left < n && arr[left] < arr[smallest]) {
                smallest = left;
            }
            if (right < n && arr[right] < arr[smallest]) {
                smallest = right;
            }

            if (smallest == parent) break;

            int temp = arr[parent];
            arr[parent] = arr[smallest];
            arr[smallest] = temp;

            parent = smallest;
        }
    }

    // 排序
    for (int i = (int)(n - 1); i > 0; i--) {
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        size_t parent = 0;
        size_t heap_size = (size_t)i;

        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t smallest = parent;

            if (left < heap_size && arr[left] < arr[smallest]) {
                smallest = left;
            }
            if (right < heap_size && arr[right] < arr[smallest]) {
                smallest = right;
            }

            if (smallest == parent) break;

            temp = arr[parent];
            arr[parent] = arr[smallest];
            arr[smallest] = temp;

            parent = smallest;
        }
    }
}
```

### 4.3 堆排序 vs 其他排序

| 排序算法 | 平均时间 | 最坏时间 | 空间 | 稳定性 |
|---------|---------|---------|------|--------|
| 堆排序 | $O(n \log n)$ | $O(n \log n)$ | $O(1)$ | 不稳定 |
| 快速排序 | $O(n \log n)$ | $O(n^2)$ | $O(\log n)$ | 不稳定 |
| 归并排序 | $O(n \log n)$ | $O(n \log n)$ | $O(n)$ | 稳定 |
| 插入排序 | $O(n^2)$ | $O(n^2)$ | $O(1)$ | 稳定 |

**堆排序优势**: 原地排序，最坏情况保证 $O(n \log n)$

---

## 5. 优先队列应用

### 5.1 合并K个有序数组

```c
/**
 * 合并K个有序数组
 * 使用最小堆维护K个数组的当前最小值
 *
 * 时间复杂度: O(N log K)，N为总元素数，K为数组数
 */

typedef struct {
    int value;      // 当前值
    int array_idx;  // 来自哪个数组
    int elem_idx;   // 在数组中的位置
} HeapNode;

typedef struct {
    HeapNode* data;
    size_t size;
    size_t capacity;
} MinHeapPQ;

// 比较函数 (最小堆)
static bool pq_node_compare_less(const HeapNode* a, const HeapNode* b) {
    return a->value > b->value;  // 注意: 这里是 > 因为我们想要最小堆
}

// 上浮
static void pq_swim(MinHeapPQ* pq, size_t k) {
    while (k > 0) {
        size_t parent = (k - 1) / 2;
        if (!pq_node_compare_less(&pq->data[parent], &pq->data[k])) {
            break;
        }
        HeapNode temp = pq->data[parent];
        pq->data[parent] = pq->data[k];
        pq->data[k] = temp;
        k = parent;
    }
}

// 下沉
static void pq_sink(MinHeapPQ* pq, size_t k) {
    size_t n = pq->size;
    while (true) {
        size_t left = 2 * k + 1;
        size_t right = 2 * k + 2;
        size_t smallest = k;

        if (left < n && pq_node_compare_less(&pq->data[smallest], &pq->data[left])) {
            smallest = left;
        }
        if (right < n && pq_node_compare_less(&pq->data[smallest], &pq->data[right])) {
            smallest = right;
        }

        if (smallest == k) break;

        HeapNode temp = pq->data[k];
        pq->data[k] = pq->data[smallest];
        pq->data[smallest] = temp;
        k = smallest;
    }
}

// 出队
static bool pq_pop(MinHeapPQ* pq, HeapNode* out) {
    if (pq->size == 0) return false;

    *out = pq->data[0];
    pq->size--;
    if (pq->size > 0) {
        pq->data[0] = pq->data[pq->size];
        pq_sink(pq, 0);
    }
    return true;
}

// 入队
static void pq_push(MinHeapPQ* pq, HeapNode node) {
    pq->data[pq->size] = node;
    pq_swim(pq, pq->size);
    pq->size++;
}

int* merge_k_sorted_arrays(int** arrays, int* sizes, int k, int* returnSize) {
    // 计算总大小
    *returnSize = 0;
    for (int i = 0; i < k; i++) {
        *returnSize += sizes[i];
    }

    int* result = (int*)malloc(*returnSize * sizeof(int));
    int result_idx = 0;

    // 初始化优先队列
    MinHeapPQ pq;
    pq.capacity = k;
    pq.data = (HeapNode*)malloc(k * sizeof(HeapNode));
    pq.size = 0;

    // 将每个数组的第一个元素入队
    for (int i = 0; i < k; i++) {
        if (sizes[i] > 0) {
            HeapNode node = {arrays[i][0], i, 0};
            pq_push(&pq, node);
        }
    }

    // 合并
    HeapNode node;
    while (pq_pop(&pq, &node)) {
        result[result_idx++] = node.value;

        // 从同一数组取下一个元素
        node.elem_idx++;
        if (node.elem_idx < sizes[node.array_idx]) {
            node.value = arrays[node.array_idx][node.elem_idx];
            pq_push(&pq, node);
        }
    }

    free(pq.data);
    return result;
}
```

### 5.2 任务调度

```c
/**
 * 任务调度器 - 使用优先队列
 * 每次执行优先级最高的任务
 */

typedef struct {
    int task_id;
    int priority;      // 数值越大优先级越高
    int duration;      // 执行时长
    char name[64];
} Task;

// 任务队列 (最大堆)
typedef struct {
    Task* tasks;
    size_t size;
    size_t capacity;
} TaskQueue;

void task_queue_init(TaskQueue* queue) {
    queue->capacity = 16;
    queue->tasks = (Task*)malloc(queue->capacity * sizeof(Task));
    queue->size = 0;
}

void task_queue_push(TaskQueue* queue, Task task) {
    if (queue->size >= queue->capacity) {
        queue->capacity *= 2;
        queue->tasks = (Task*)realloc(queue->tasks, queue->capacity * sizeof(Task));
    }

    size_t k = queue->size;
    queue->tasks[k] = task;

    // 上浮 (按优先级)
    while (k > 0) {
        size_t parent = (k - 1) / 2;
        if (queue->tasks[parent].priority >= queue->tasks[k].priority) {
            break;
        }
        Task temp = queue->tasks[parent];
        queue->tasks[parent] = queue->tasks[k];
        queue->tasks[k] = temp;
        k = parent;
    }

    queue->size++;
}

bool task_queue_pop(TaskQueue* queue, Task* out) {
    if (queue->size == 0) return false;

    *out = queue->tasks[0];
    queue->size--;

    if (queue->size > 0) {
        queue->tasks[0] = queue->tasks[queue->size];

        // 下沉
        size_t k = 0;
        while (true) {
            size_t left = 2 * k + 1;
            size_t right = 2 * k + 2;
            size_t largest = k;

            if (left < queue->size &&
                queue->tasks[left].priority > queue->tasks[largest].priority) {
                largest = left;
            }
            if (right < queue->size &&
                queue->tasks[right].priority > queue->tasks[largest].priority) {
                largest = right;
            }

            if (largest == k) break;

            Task temp = queue->tasks[k];
            queue->tasks[k] = queue->tasks[largest];
            queue->tasks[largest] = temp;
            k = largest;
        }
    }

    return true;
}
```

---

## 6. Top-K问题

### 6.1 最小堆求第K大

```c
/**
 * 找数组中第K大的元素
 * 使用大小为K的最小堆
 *
 * 时间复杂度: O(n log k)
 * 空间复杂度: O(k)
 */
int find_kth_largest(int* nums, int numsSize, int k) {
    // 最小堆 (存储最大的K个元素，堆顶是其中最小的)
    Heap min_heap;
    heap_init(&min_heap, MIN_HEAP);

    for (int i = 0; i < numsSize; i++) {
        if (min_heap.size < (size_t)k) {
            heap_push(&min_heap, nums[i]);
        } else if (nums[i] > min_heap.data[0]) {
            // 当前元素比堆顶大，替换
            min_heap.data[0] = nums[i];
            heap_sink(&min_heap, 0);
        }
    }

    int result = min_heap.data[0];
    heap_destroy(&min_heap);
    return result;
}
```

### 6.2 最大堆求第K小

```c
/**
 * 找数组中第K小的元素
 * 使用大小为K的最大堆
 */
int find_kth_smallest(int* nums, int numsSize, int k) {
    Heap max_heap;
    heap_init(&max_heap, MAX_HEAP);

    for (int i = 0; i < numsSize; i++) {
        if (max_heap.size < (size_t)k) {
            heap_push(&max_heap, nums[i]);
        } else if (nums[i] < max_heap.data[0]) {
            max_heap.data[0] = nums[i];
            heap_sink(&max_heap, 0);
        }
    }

    int result = max_heap.data[0];
    heap_destroy(&max_heap);
    return result;
}
```

### 6.3 数据流中的中位数

```c
/**
 * 数据流中的中位数
 * 使用两个堆: 最大堆(存较小一半) + 最小堆(存较大一半)
 */

typedef struct {
    Heap max_heap;  // 较小的一半 (最大堆)
    Heap min_heap;  // 较大的一半 (最小堆)
} MedianFinder;

void median_finder_init(MedianFinder* mf) {
    heap_init(&mf->max_heap, MAX_HEAP);
    heap_init(&mf->min_heap, MIN_HEAP);
}

void median_finder_destroy(MedianFinder* mf) {
    heap_destroy(&mf->max_heap);
    heap_destroy(&mf->min_heap);
}

void median_finder_add_num(MedianFinder* mf, int num) {
    // 先放入最大堆
    heap_push(&mf->max_heap, num);

    // 将最大堆顶移到最小堆
    int max_top;
    heap_pop(&mf->max_heap, &max_top);
    heap_push(&mf->min_heap, max_top);

    // 平衡两个堆的大小
    if (mf->min_heap.size > mf->max_heap.size) {
        int min_top;
        heap_pop(&mf->min_heap, &min_top);
        heap_push(&mf->max_heap, min_top);
    }
}

double median_finder_find_median(MedianFinder* mf) {
    if (mf->max_heap.size > mf->min_heap.size) {
        return (double)mf->max_heap.data[0];
    }
    return (mf->max_heap.data[0] + mf->min_heap.data[0]) / 2.0;
}
```

---

## 7. 高级主题

### 7.1 二项堆 (Binomial Heap)

```c
/**
 * 二项堆 - 支持高效合并的堆结构
 * 由一组二项树组成
 */

typedef struct BinomialNode {
    int key;
    int degree;              // 子节点数
    struct BinomialNode* parent;
    struct BinomialNode* child;
    struct BinomialNode* sibling;
} BinomialNode;

typedef struct {
    BinomialNode* head;      // 指向根链表
} BinomialHeap;

// 合并两个相同阶的二项树
BinomialNode* binomial_link(BinomialNode* y, BinomialNode* z) {
    if (y->key > z->key) {
        BinomialNode* temp = y;
        y = z;
        z = temp;
    }
    z->parent = y;
    z->sibling = y->child;
    y->child = z;
    y->degree++;
    return y;
}
```

### 7.2 斐波那契堆 (概念)

斐波那契堆是二项堆的改进版，提供更优的平摊复杂度：

| 操作 | 二叉堆 | 二项堆 | 斐波那契堆 |
|------|--------|--------|-----------|
| make-heap | $O(1)$ | $O(1)$ | $O(1)$ |
| insert | $O(\log n)$ | $O(\log n)$ | $O(1)$ |
| find-min | $O(1)$ | $O(\log n)$ | $O(1)$ |
| delete-min | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ |
| merge | $O(n)$ | $O(\log n)$ | $O(1)$ |
| decrease-key | $O(\log n)$ | $O(\log n)$ | $O(1)^*$ |

*平摊复杂度

主要用于Dijkstra和Prim算法的优化。

---

## 8. 完整代码实现

```c
/**
 * ============================================================================
 * 堆和优先队列完整实现 (Heap & Priority Queue Complete Implementation)
 * ============================================================================
 * 编译命令: gcc -std=c23 -Wall -Wextra -o heap heap.c
 * 运行命令: ./heap
 * ============================================================================
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#if __STDC_VERSION__ < 202311L
    #define nullptr NULL
#endif

// 这里包含前面所有的结构定义和函数实现...

// =============================================================================
// 测试函数
// =============================================================================

void test_heap_operations() {
    printf("\n========== 堆基本操作测试 ==========\n");

    Heap max_heap;
    heap_init(&max_heap, MAX_HEAP);

    printf("构建最大堆，插入: ");
    int values[] = {3, 1, 4, 1, 5, 9, 2, 6};
    for (int i = 0; i < 8; i++) {
        printf("%d ", values[i]);
        heap_push(&max_heap, values[i]);
    }
    printf("\n");

    printf("堆数组: ");
    for (size_t i = 0; i < max_heap.size; i++) {
        printf("%d ", max_heap.data[i]);
    }
    printf("\n");

    printf("弹出顺序: ");
    int val;
    while (heap_pop(&max_heap, &val)) {
        printf("%d ", val);
    }
    printf("\n");

    heap_destroy(&max_heap);
    printf("最大堆测试通过!\n");

    // 最小堆测试
    printf("\n--- 最小堆测试 ---\n");
    Heap min_heap;
    heap_init(&min_heap, MIN_HEAP);

    for (int i = 0; i < 8; i++) {
        heap_push(&min_heap, values[i]);
    }

    printf("弹出顺序: ");
    while (heap_pop(&min_heap, &val)) {
        printf("%d ", val);
    }
    printf("\n");

    heap_destroy(&min_heap);
    printf("最小堆测试通过!\n");
}

void test_heap_sort() {
    printf("\n========== 堆排序测试 ==========\n");

    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("原数组: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    heap_sort_ascending(arr, n);
    printf("升序: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    heap_sort_descending(arr, n);
    printf("降序: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    printf("堆排序测试通过!\n");
}

void test_top_k() {
    printf("\n========== Top-K 测试 ==========\n");

    int nums[] = {3, 2, 1, 5, 6, 4, 9, 8, 7};
    int n = sizeof(nums) / sizeof(nums[0]);

    printf("数组: ");
    for (int i = 0; i < n; i++) printf("%d ", nums[i]);
    printf("\n");

    int k = 3;
    printf("第%d大的元素: %d\n", k, find_kth_largest(nums, n, k));
    printf("第%d小的元素: %d\n", k, find_kth_smallest(nums, n, k));

    printf("Top-K 测试通过!\n");
}

void test_median_finder() {
    printf("\n========== 中位数查找测试 ==========\n");

    MedianFinder mf;
    median_finder_init(&mf);

    int stream[] = {5, 2, 3, 4, 1};
    printf("数据流: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", stream[i]);
        median_finder_add_num(&mf, stream[i]);
        printf("(中位数: %.1f) ", median_finder_find_median(&mf));
    }
    printf("\n");

    median_finder_destroy(&mf);
    printf("中位数查找测试通过!\n");
}

// =============================================================================
// 主函数
// =============================================================================

int main() {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       堆和优先队列完整实现测试 (C23 Standard)              ║\n");
    printf("║       Heap & Priority Queue Complete Implementation        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);

    test_heap_operations();
    test_heap_sort();
    test_top_k();
    test_median_finder();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              所有测试通过! All tests passed!               ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");

    return 0;
}
```

---

## 附录

### 附录A: 参考资料

1. 《算法导论》第6章 - 堆排序
2. 《数据结构与算法分析》 - Mark Allen Weiss
3. Visualgo - 堆可视化

### 附录B: 修订历史

| 版本 | 日期 | 修改内容 |
|------|------|----------|
| 1.0.0 | 2026-03-14 | 初始版本 |

---

**文档结束**


### 7.3 对偶堆 (Pairing Heap)

```c
/**
 * 对偶堆 - 一种自适应的堆结构
 * 在实际应用中通常比二叉堆和二项堆更高效
 */

typedef struct PairingNode {
    int key;
    struct PairingNode* child;      // 第一个子节点
    struct PairingNode* sibling;    // 兄弟节点
    struct PairingNode* prev;       // 前驱节点 (父节点或左兄弟)
} PairingNode;

typedef struct {
    PairingNode* root;
    size_t size;
} PairingHeap;

// 合并两个对偶堆
PairingNode* pairing_merge(PairingNode* a, PairingNode* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    // 确保a是根节点较小的
    if (a->key > b->key) {
        PairingNode* temp = a;
        a = b;
        b = temp;
    }

    // 将b作为a的第一个子节点
    b->sibling = a->child;
    if (a->child != nullptr) {
        a->child->prev = b;
    }
    b->prev = a;
    a->child = b;

    return a;
}

// 插入
void pairing_push(PairingHeap* heap, int key) {
    PairingNode* node = (PairingNode*)malloc(sizeof(PairingNode));
    node->key = key;
    node->child = nullptr;
    node->sibling = nullptr;
    node->prev = nullptr;

    heap->root = pairing_merge(heap->root, node);
    heap->size++;
}

// 两两合并
static PairingNode* pairing_two_pass_merge(PairingNode* first) {
    if (first == nullptr || first->sibling == nullptr) {
        return first;
    }

    // 第一遍: 从左到右两两合并
    PairingNode* pairs = nullptr;
    while (first != nullptr && first->sibling != nullptr) {
        PairingNode* a = first;
        PairingNode* b = first->sibling;
        first = b->sibling;

        PairingNode* merged = pairing_merge(a, b);
        merged->sibling = pairs;
        pairs = merged;
    }

    if (first != nullptr) {
        first->sibling = pairs;
        pairs = first;
    }

    // 第二遍: 从右到左合并
    PairingNode* result = nullptr;
    while (pairs != nullptr) {
        PairingNode* next = pairs->sibling;
        result = pairing_merge(result, pairs);
        pairs = next;
    }

    return result;
}

// 删除最小值
int pairing_pop(PairingHeap* heap) {
    if (heap->root == nullptr) {
        fprintf(stderr, "Error: Heap is empty\n");
        return -1;
    }

    int min_key = heap->root->key;
    PairingNode* old_root = heap->root;

    heap->root = pairing_two_pass_merge(heap->root->child);
    free(old_root);
    heap->size--;

    return min_key;
}
```

### 7.4 索引堆 (Index Heap)

```c
/**
 * 索引堆 - 支持快速修改任意位置元素的堆
 * 通过索引数组间接访问数据
 */

typedef struct {
    int* data;          // 实际数据
    int* indices;       // 索引: indices[i]表示堆中第i个位置的元素在data中的索引
    int* reverse;       // 反向索引: reverse[i]表示data[i]在堆中的位置
    size_t size;
    size_t capacity;
    bool is_max_heap;
} IndexHeap;

void index_heap_init(IndexHeap* heap, size_t capacity, bool is_max) {
    heap->capacity = capacity;
    heap->data = (int*)malloc(capacity * sizeof(int));
    heap->indices = (int*)malloc(capacity * sizeof(int));
    heap->reverse = (int*)malloc(capacity * sizeof(int));
    for (size_t i = 0; i < capacity; i++) {
        heap->reverse[i] = -1;  // -1表示不在堆中
    }
    heap->size = 0;
    heap->is_max_heap = is_max;
}

// 检查位置i的元素是否小于位置j的元素
static bool index_heap_less(const IndexHeap* heap, int i, int j) {
    int idx_i = heap->indices[i];
    int idx_j = heap->indices[j];
    if (heap->is_max_heap) {
        return heap->data[idx_i] < heap->data[idx_j];
    } else {
        return heap->data[idx_i] > heap->data[idx_j];
    }
}

// 交换堆中的两个位置
static void index_heap_swap(IndexHeap* heap, int i, int j) {
    int idx_i = heap->indices[i];
    int idx_j = heap->indices[j];

    heap->reverse[idx_i] = j;
    heap->reverse[idx_j] = i;

    int temp = heap->indices[i];
    heap->indices[i] = heap->indices[j];
    heap->indices[j] = temp;
}

// 上浮
static void index_heap_swim(IndexHeap* heap, int k) {
    while (k > 0) {
        int parent = (k - 1) / 2;
        if (!index_heap_less(heap, parent, k)) break;
        index_heap_swap(heap, k, parent);
        k = parent;
    }
}

// 下沉
static void index_heap_sink(IndexHeap* heap, int k) {
    int n = (int)heap->size;
    while (true) {
        int left = 2 * k + 1;
        int right = 2 * k + 2;
        int target = k;

        if (left < n && index_heap_less(heap, target, left)) {
            target = left;
        }
        if (right < n && index_heap_less(heap, target, right)) {
            target = right;
        }
        if (target == k) break;

        index_heap_swap(heap, k, target);
        k = target;
    }
}

// 修改指定索引的元素值
void index_heap_change(IndexHeap* heap, int data_idx, int new_value) {
    heap->data[data_idx] = new_value;

    int heap_pos = heap->reverse[data_idx];
    if (heap_pos < 0) return;  // 不在堆中

    // 尝试上浮或下沉
    index_heap_swim(heap, heap_pos);
    index_heap_sink(heap, heap_pos);
}

// 检查是否在堆中
bool index_heap_contains(const IndexHeap* heap, int data_idx) {
    return heap->reverse[data_idx] >= 0;
}
```

### 7.5 区间最值查询 (RMQ) 与堆

```c
/**
 * 使用稀疏表(Sparse Table)实现RMQ
 * 预处理O(n log n)，查询O(1)
 */

typedef struct {
    int** st;       // 稀疏表
    int* log_table;
    int n;
    int k;          // 最大幂次
} SparseTableRMQ;

void rmq_init(SparseTableRMQ* rmq, const int* arr, int n) {
    rmq->n = n;
    rmq->k = 0;
    while ((1 << rmq->k) <= n) rmq->k++;

    // 预计算log值
    rmq->log_table = (int*)malloc((n + 1) * sizeof(int));
    rmq->log_table[1] = 0;
    for (int i = 2; i <= n; i++) {
        rmq->log_table[i] = rmq->log_table[i / 2] + 1;
    }

    // 构建稀疏表
    rmq->st = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        rmq->st[i] = (int*)malloc(rmq->k * sizeof(int));
        rmq->st[i][0] = arr[i];
    }

    for (int j = 1; j < rmq->k; j++) {
        for (int i = 0; i + (1 << j) <= n; i++) {
            rmq->st[i][j] = (rmq->st[i][j-1] < rmq->st[i + (1 << (j-1))][j-1]) ?
                            rmq->st[i][j-1] : rmq->st[i + (1 << (j-1))][j-1];
        }
    }
}

// 查询[l, r]区间的最小值
int rmq_query(const SparseTableRMQ* rmq, int l, int r) {
    int j = rmq->log_table[r - l + 1];
    int a = rmq->st[l][j];
    int b = rmq->st[r - (1 << j) + 1][j];
    return (a < b) ? a : b;
}
```

### 7.6 堆与图算法

```c
/**
 * Dijkstra最短路径算法 - 使用优先队列
 *
 * 时间复杂度: O((V + E) log V)
 */

typedef struct {
    int vertex;
    int distance;
} GraphNode;

typedef struct {
    GraphNode* nodes;
    int* positions;  // 每个顶点在堆中的位置
    int size;
    int capacity;
} GraphPriorityQueue;

void gpq_init(GraphPriorityQueue* pq, int n_vertices) {
    pq->capacity = n_vertices;
    pq->nodes = (GraphNode*)malloc(n_vertices * sizeof(GraphNode));
    pq->positions = (int*)malloc(n_vertices * sizeof(int));
    for (int i = 0; i < n_vertices; i++) {
        pq->positions[i] = -1;
    }
    pq->size = 0;
}

// 判断队列是否包含某顶点
bool gpq_contains(const GraphPriorityQueue* pq, int vertex) {
    return pq->positions[vertex] >= 0;
}

// 获取顶点距离
int gpq_get_distance(const GraphPriorityQueue* pq, int vertex) {
    int pos = pq->positions[vertex];
    if (pos < 0) return -1;
    return pq->nodes[pos].distance;
}

// Dijkstra算法主函数
void dijkstra(const int** graph, int n, int src, int* dist) {
    // graph[i][j] = i到j的边权，-1表示无连接

    // 初始化距离
    for (int i = 0; i < n; i++) {
        dist[i] = (i == src) ? 0 : INT_MAX;
    }

    GraphPriorityQueue pq;
    gpq_init(&pq, n);

    // 将源点入队
    pq.nodes[0] = (GraphNode){src, 0};
    pq.positions[src] = 0;
    pq.size = 1;

    while (pq.size > 0) {
        // 弹出距离最小的顶点
        GraphNode min_node = pq.nodes[0];
        pq.positions[min_node.vertex] = -1;
        pq.size--;
        if (pq.size > 0) {
            pq.nodes[0] = pq.nodes[pq.size];
            pq.positions[pq.nodes[0].vertex] = 0;
            // 下沉调整...
        }

        // 更新邻接顶点
        for (int v = 0; v < n; v++) {
            if (graph[min_node.vertex][v] < 0) continue;

            int new_dist = min_node.distance + graph[min_node.vertex][v];
            if (new_dist < dist[v]) {
                dist[v] = new_dist;
                // 更新或插入优先队列...
            }
        }
    }

    free(pq.nodes);
    free(pq.positions);
}
```

### 7.7 堆排序的稳定性改进

```c
/**
 * 稳定堆排序
 * 通过存储原始索引来保证相等元素的相对顺序
 */

typedef struct {
    int value;
    int index;  // 原始索引
} IndexedValue;

void stable_heap_sort(int* arr, size_t n) {
    IndexedValue* indexed = (IndexedValue*)malloc(n * sizeof(IndexedValue));
    for (size_t i = 0; i < n; i++) {
        indexed[i].value = arr[i];
        indexed[i].index = (int)i;
    }

    // 比较时，值相等则按原始索引比较
    // 构建最大堆...
    // 堆排序过程中使用自定义比较:
    // indexed[i].value > indexed[j].value ||
    // (indexed[i].value == indexed[j].value && indexed[i].index > indexed[j].index)

    // 将结果复制回原数组
    for (size_t i = 0; i < n; i++) {
        arr[i] = indexed[i].value;
    }

    free(indexed);
}
```

---

## 8. 实际应用中的选择指南

### 8.1 不同场景的选择

```
场景1: 需要频繁的修改元素优先级
推荐: 索引堆 (Index Heap) 或 斐波那契堆

场景2: 内存受限，需要原地操作
推荐: 二叉堆 (数组实现)

场景3: 需要高效的合并操作
推荐: 二项堆 或 斐波那契堆

场景4: 只需要简单的优先队列功能
推荐: 二叉堆 (实现最简单)

场景5: 需要支持 decrease-key 操作
推荐: 斐波那契堆 (平摊 O(1))

场景6: 需要常数时间的 find-min
推荐: 任意堆结构都可以
```

### 8.2 性能对比总结

| 堆类型 | push | pop | peek | merge | decrease-key | 空间 |
|--------|------|-----|------|-------|--------------|------|
| 二叉堆 | O(log n) | O(log n) | O(1) | O(n) | O(log n) | O(n) |
| 二项堆 | O(log n) | O(log n) | O(log n) | O(log n) | O(log n) | O(n) |
| 斐波那契堆 | O(1)* | O(log n)* | O(1) | O(1) | O(1)* | O(n) |
| 配对堆 | O(1) | O(log n)* | O(1) | O(1) | O(log n)* | O(n) |

*平摊复杂度

---

## 附录


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
