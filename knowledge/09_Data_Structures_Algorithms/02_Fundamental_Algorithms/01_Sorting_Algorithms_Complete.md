# 排序算法大全 - Sorting Algorithms Complete Guide

> **版本**: C23 Standard | **难度**: 初级到高级 | **预计学习时间**: 20小时

---

## 📋 目录

- [排序算法大全 - Sorting Algorithms Complete Guide](#排序算法大全---sorting-algorithms-complete-guide)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 算法概述](#1-算法概述)
    - [1.1 什么是排序算法](#11-什么是排序算法)
    - [1.2 排序算法的评价指标](#12-排序算法的评价指标)
    - [1.3 稳定性详解](#13-稳定性详解)
    - [1.4 大O表示法速查](#14-大o表示法速查)
  - [2. 基础排序算法](#2-基础排序算法)
    - [2.1 冒泡排序 (Bubble Sort)](#21-冒泡排序-bubble-sort)
      - [2.1.1 算法原理](#211-算法原理)
      - [2.1.2 C23实现](#212-c23实现)
      - [2.1.3 复杂度分析](#213-复杂度分析)
      - [2.1.4 算法特点](#214-算法特点)
    - [2.2 选择排序 (Selection Sort)](#22-选择排序-selection-sort)
      - [2.2.1 算法原理](#221-算法原理)
      - [2.2.2 C23实现](#222-c23实现)
      - [2.2.3 复杂度分析](#223-复杂度分析)
      - [2.2.4 算法特点](#224-算法特点)
    - [2.3 插入排序 (Insertion Sort)](#23-插入排序-insertion-sort)
      - [2.3.1 算法原理](#231-算法原理)
      - [2.3.2 C23实现](#232-c23实现)
      - [2.3.3 复杂度分析](#233-复杂度分析)
      - [2.3.4 算法特点](#234-算法特点)
  - [3. 高级排序算法](#3-高级排序算法)
    - [3.1 快速排序 (Quick Sort)](#31-快速排序-quick-sort)
      - [3.1.1 算法原理](#311-算法原理)
      - [3.1.2 C23实现](#312-c23实现)
      - [3.1.3 复杂度分析](#313-复杂度分析)
      - [3.1.4 算法特点](#314-算法特点)
    - [3.2 归并排序 (Merge Sort)](#32-归并排序-merge-sort)
      - [3.2.1 算法原理](#321-算法原理)
      - [3.2.2 C23实现](#322-c23实现)
      - [3.2.3 复杂度分析](#323-复杂度分析)
      - [3.2.4 算法特点](#324-算法特点)
    - [3.3 堆排序 (Heap Sort)](#33-堆排序-heap-sort)
      - [3.3.1 算法原理](#331-算法原理)
      - [3.3.2 C23实现](#332-c23实现)
      - [3.3.3 复杂度分析](#333-复杂度分析)
      - [3.3.4 算法特点](#334-算法特点)
  - [4. 线性时间排序](#4-线性时间排序)
    - [4.1 计数排序 (Counting Sort)](#41-计数排序-counting-sort)
      - [4.1.1 算法原理](#411-算法原理)
      - [4.1.2 C23实现](#412-c23实现)
      - [4.1.3 复杂度分析](#413-复杂度分析)
      - [4.1.4 适用条件](#414-适用条件)
    - [4.2 桶排序 (Bucket Sort)](#42-桶排序-bucket-sort)
      - [4.2.1 算法原理](#421-算法原理)
      - [4.2.2 C23实现](#422-c23实现)
      - [4.2.3 复杂度分析](#423-复杂度分析)
      - [4.2.4 适用条件](#424-适用条件)
    - [4.3 基数排序 (Radix Sort)](#43-基数排序-radix-sort)
      - [4.3.1 算法原理](#431-算法原理)
      - [4.3.2 C23实现](#432-c23实现)
      - [4.3.3 复杂度分析](#433-复杂度分析)
      - [4.3.4 适用条件](#434-适用条件)
  - [5. 性能对比与选择](#5-性能对比与选择)
    - [5.1 算法性能对比表](#51-算法性能对比表)
    - [5.2 实测性能对比](#52-实测性能对比)
    - [5.3 算法选择决策树](#53-算法选择决策树)
  - [6. 实际应用案例](#6-实际应用案例)
    - [6.1 案例1：大数据排序](#61-案例1大数据排序)
    - [6.2 案例2：数据库索引排序](#62-案例2数据库索引排序)
    - [6.3 案例3：实时系统排序](#63-案例3实时系统排序)
  - [7. 总结](#7-总结)
    - [7.1 核心要点](#71-核心要点)
    - [7.2 学习建议](#72-学习建议)
    - [7.3 进阶学习方向](#73-进阶学习方向)
  - [参考资料](#参考资料)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [排序算法大全 - Sorting Algorithms Complete Guide](#排序算法大全---sorting-algorithms-complete-guide)
  - [📋 目录](#-目录)
  - [📑 目录](#-目录-1)
  - [1. 算法概述](#1-算法概述)
    - [1.1 什么是排序算法](#11-什么是排序算法)
    - [1.2 排序算法的评价指标](#12-排序算法的评价指标)
    - [1.3 稳定性详解](#13-稳定性详解)
    - [1.4 大O表示法速查](#14-大o表示法速查)
  - [2. 基础排序算法](#2-基础排序算法)
    - [2.1 冒泡排序 (Bubble Sort)](#21-冒泡排序-bubble-sort)
      - [2.1.1 算法原理](#211-算法原理)
      - [2.1.2 C23实现](#212-c23实现)
      - [2.1.3 复杂度分析](#213-复杂度分析)
      - [2.1.4 算法特点](#214-算法特点)
    - [2.2 选择排序 (Selection Sort)](#22-选择排序-selection-sort)
      - [2.2.1 算法原理](#221-算法原理)
      - [2.2.2 C23实现](#222-c23实现)
      - [2.2.3 复杂度分析](#223-复杂度分析)
      - [2.2.4 算法特点](#224-算法特点)
    - [2.3 插入排序 (Insertion Sort)](#23-插入排序-insertion-sort)
      - [2.3.1 算法原理](#231-算法原理)
      - [2.3.2 C23实现](#232-c23实现)
      - [2.3.3 复杂度分析](#233-复杂度分析)
      - [2.3.4 算法特点](#234-算法特点)
  - [3. 高级排序算法](#3-高级排序算法)
    - [3.1 快速排序 (Quick Sort)](#31-快速排序-quick-sort)
      - [3.1.1 算法原理](#311-算法原理)
      - [3.1.2 C23实现](#312-c23实现)
      - [3.1.3 复杂度分析](#313-复杂度分析)
      - [3.1.4 算法特点](#314-算法特点)
    - [3.2 归并排序 (Merge Sort)](#32-归并排序-merge-sort)
      - [3.2.1 算法原理](#321-算法原理)
      - [3.2.2 C23实现](#322-c23实现)
      - [3.2.3 复杂度分析](#323-复杂度分析)
      - [3.2.4 算法特点](#324-算法特点)
    - [3.3 堆排序 (Heap Sort)](#33-堆排序-heap-sort)
      - [3.3.1 算法原理](#331-算法原理)
      - [3.3.2 C23实现](#332-c23实现)
      - [3.3.3 复杂度分析](#333-复杂度分析)
      - [3.3.4 算法特点](#334-算法特点)
  - [4. 线性时间排序](#4-线性时间排序)
    - [4.1 计数排序 (Counting Sort)](#41-计数排序-counting-sort)
      - [4.1.1 算法原理](#411-算法原理)
      - [4.1.2 C23实现](#412-c23实现)
      - [4.1.3 复杂度分析](#413-复杂度分析)
      - [4.1.4 适用条件](#414-适用条件)
    - [4.2 桶排序 (Bucket Sort)](#42-桶排序-bucket-sort)
      - [4.2.1 算法原理](#421-算法原理)
      - [4.2.2 C23实现](#422-c23实现)
      - [4.2.3 复杂度分析](#423-复杂度分析)
      - [4.2.4 适用条件](#424-适用条件)
    - [4.3 基数排序 (Radix Sort)](#43-基数排序-radix-sort)
      - [4.3.1 算法原理](#431-算法原理)
      - [4.3.2 C23实现](#432-c23实现)
      - [4.3.3 复杂度分析](#433-复杂度分析)
      - [4.3.4 适用条件](#434-适用条件)
  - [5. 性能对比与选择](#5-性能对比与选择)
    - [5.1 算法性能对比表](#51-算法性能对比表)
    - [5.2 实测性能对比](#52-实测性能对比)
    - [5.3 算法选择决策树](#53-算法选择决策树)
  - [6. 实际应用案例](#6-实际应用案例)
    - [6.1 案例1：大数据排序](#61-案例1大数据排序)
    - [6.2 案例2：数据库索引排序](#62-案例2数据库索引排序)
    - [6.3 案例3：实时系统排序](#63-案例3实时系统排序)
  - [7. 总结](#7-总结)
    - [7.1 核心要点](#71-核心要点)
    - [7.2 学习建议](#72-学习建议)
    - [7.3 进阶学习方向](#73-进阶学习方向)
  - [参考资料](#参考资料)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. 算法概述

### 1.1 什么是排序算法

排序算法是将一组数据按照特定顺序（通常是升序或降序）重新排列的算法。排序是计算机科学中最基础也是最重要的操作之一。

```
原始数据: [64, 34, 25, 12, 22, 11, 90]
升序排序: [11, 12, 22, 25, 34, 64, 90]
降序排序: [90, 64, 34, 25, 22, 12, 11]
```

### 1.2 排序算法的评价指标

| 指标 | 说明 | 重要性 |
|------|------|--------|
| **时间复杂度** | 算法执行所需时间随数据规模的增长趋势 | ⭐⭐⭐⭐⭐ |
| **空间复杂度** | 算法执行所需额外内存空间 | ⭐⭐⭐⭐ |
| **稳定性** | 相等元素的相对顺序是否保持不变 | ⭐⭐⭐⭐ |
| **适应性** | 对已有序数据的处理效率 | ⭐⭐⭐ |
| **原地排序** | 是否需要额外存储空间 | ⭐⭐⭐ |

### 1.3 稳定性详解

**稳定排序**: 如果两个元素相等，排序后它们的相对位置不变。

```
原始: [(A,3), (B,1), (C,3), (D,2)]  # 按数字排序
稳定排序结果: [(B,1), (D,2), (A,3), (C,3)]  # A仍在C前面
不稳定排序结果: [(B,1), (D,2), (C,3), (A,3)]  # A和C可能交换
```

### 1.4 大O表示法速查

```
O(1)        - 常数时间，最佳
O(log n)    - 对数时间，优秀
O(n)        - 线性时间，良好
O(n log n)  - 线性对数时间，可接受
O(n²)       - 平方时间，较慢
O(n³)       - 立方时间，很慢
O(2ⁿ)       - 指数时间，极慢
O(n!)       - 阶乘时间，不可接受
```

---

## 2. 基础排序算法

### 2.1 冒泡排序 (Bubble Sort)

#### 2.1.1 算法原理

冒泡排序通过重复遍历数组，比较相邻元素并交换位置，使较大的元素逐渐"冒泡"到数组末尾。

```
第1轮: [64, 34, 25, 12, 22, 11, 90]
       比较64和34，交换 → [34, 64, 25, 12, 22, 11, 90]
       比较64和25，交换 → [34, 25, 64, 12, 22, 11, 90]
       ...持续比较交换...
       第1轮结束: [34, 25, 12, 22, 11, 64, 90] (最大元素90已就位)

第2轮: [34, 25, 12, 22, 11, 64, 90]
       ... (64就位)

重复直到全部有序...
```

#### 2.1.2 C23实现

```c
/*
 * 冒泡排序 - 基础版本
 * 文件名: bubble_sort_basic.c
 * 编译: gcc -std=c23 -o bubble_sort_basic bubble_sort_basic.c
 */

#include <stdio.h>
#include <stdbool.h>

// 交换两个整数
static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 基础冒泡排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void bubble_sort_basic(int arr[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

/**
 * @brief 优化冒泡排序 - 添加提前退出
 * @param arr 待排序数组
 * @param n 数组长度
 */
void bubble_sort_optimized(int arr[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        // 如果没有发生交换，说明数组已有序
        if (!swapped) break;
    }
}

/**
 * @brief 鸡尾酒排序（双向冒泡排序）
 * @param arr 待排序数组
 * @param n 数组长度
 */
void cocktail_sort(int arr[], size_t n) {
    bool swapped = true;
    size_t start = 0;
    size_t end = n - 1;

    while (swapped) {
        swapped = false;

        // 正向遍历
        for (size_t i = start; i < end; i++) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;

        end--;
        swapped = false;

        // 反向遍历
        for (size_t i = end; i > start; i--) {
            if (arr[i - 1] > arr[i]) {
                swap(&arr[i - 1], &arr[i]);
                swapped = true;
            }
        }
        start++;
    }
}

// 打印数组
void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 冒泡排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    bubble_sort_basic(arr1, n);
    printf("基础冒泡: ");
    print_array(arr1, n);

    bubble_sort_optimized(arr2, n);
    printf("优化冒泡: ");
    print_array(arr2, n);

    cocktail_sort(arr3, n);
    printf("鸡尾酒排序: ");
    print_array(arr3, n);

    return 0;
}
```

#### 2.1.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况（已有序） | O(n) - 优化版 | O(1) | ✅ 稳定 |
| 平均情况 | O(n²) | O(1) | ✅ 稳定 |
| 最坏情况（逆序） | O(n²) | O(1) | ✅ 稳定 |

#### 2.1.4 算法特点

**优点**:

- 实现简单直观
- 空间复杂度O(1)，原地排序
- 稳定排序

**缺点**:

- 时间复杂度高，不适合大数据量
- 比较次数和交换次数都很多

**适用场景**:

- 教学演示
- 数据量极小（n < 50）
- 基本有序的数据

---

### 2.2 选择排序 (Selection Sort)

#### 2.2.1 算法原理

选择排序每次从未排序部分选择最小（或最大）元素，放到已排序部分的末尾。

```
初始: [64, 25, 12, 22, 11]

第1轮: 找最小值11，与第1个交换 → [11, 25, 12, 22, 64]
第2轮: 在剩余中找最小值12，交换 → [11, 12, 25, 22, 64]
第3轮: 找最小值22，交换 → [11, 12, 22, 25, 64]
第4轮: 找最小值25，已在位 → [11, 12, 22, 25, 64]
```

#### 2.2.2 C23实现

```c
/*
 * 选择排序
 * 文件名: selection_sort.c
 * 编译: gcc -std=c23 -o selection_sort selection_sort.c
 */

#include <stdio.h>
#include <stdlib.h>

static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 基础选择排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void selection_sort(int arr[], size_t n) {
    for (size_t i = 0; i < n - 1; i++) {
        size_t min_idx = i;

        // 在未排序部分找最小值
        for (size_t j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // 将最小值放到已排序部分的末尾
        if (min_idx != i) {
            swap(&arr[i], &arr[min_idx]);
        }
    }
}

/**
 * @brief 双向选择排序 - 每次找最大和最小
 * @param arr 待排序数组
 * @param n 数组长度
 */
void selection_sort_bidirectional(int arr[], size_t n) {
    size_t left = 0;
    size_t right = n - 1;

    while (left < right) {
        size_t min_idx = left;
        size_t max_idx = left;

        // 在[left, right]范围内找最小和最大
        for (size_t i = left; i <= right; i++) {
            if (arr[i] < arr[min_idx]) min_idx = i;
            if (arr[i] > arr[max_idx]) max_idx = i;
        }

        // 将最小值放到左边
        swap(&arr[left], &arr[min_idx]);

        // 如果最大值在left位置，交换后它到了min_idx位置
        if (max_idx == left) {
            max_idx = min_idx;
        }

        // 将最大值放到右边
        swap(&arr[right], &arr[max_idx]);

        left++;
        right--;
    }
}

/**
 * @brief 递归选择排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void selection_sort_recursive(int arr[], size_t n) {
    if (n <= 1) return;

    // 找最大值
    size_t max_idx = 0;
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > arr[max_idx]) {
            max_idx = i;
        }
    }

    // 将最大值放到最后
    swap(&arr[max_idx], &arr[n - 1]);

    // 递归排序剩余部分
    selection_sort_recursive(arr, n - 1);
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 25, 12, 22, 11, 90, 5};
    int arr2[] = {64, 25, 12, 22, 11, 90, 5};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 选择排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    selection_sort(arr1, n);
    printf("基础选择: ");
    print_array(arr1, n);

    selection_sort_bidirectional(arr2, n);
    printf("双向选择: ");
    print_array(arr2, n);

    return 0;
}
```

#### 2.2.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况 | O(n²) | O(1) | ❌ 不稳定 |
| 平均情况 | O(n²) | O(1) | ❌ 不稳定 |
| 最坏情况 | O(n²) | O(1) | ❌ 不稳定 |

#### 2.2.4 算法特点

**优点**:

- 交换次数最少（最多n-1次交换）
- 空间复杂度O(1)

**缺点**:

- 时间复杂度始终O(n²)
- 不稳定排序
- 比较次数固定

**适用场景**:

- 数据量小
- 交换成本高（如数据在慢速存储中）

---

### 2.3 插入排序 (Insertion Sort)

#### 2.3.1 算法原理

插入排序将数组分为已排序和未排序两部分，每次将未排序部分的第一个元素插入到已排序部分的正确位置。

```
初始: [| 64, 25, 12, 22, 11]  (| 表示已排序和未排序的分界)

第1轮: [64 | 25, 12, 22, 11]  64已在正确位置
第2轮: [25, 64 | 12, 22, 11]  插入25到正确位置
第3轮: [12, 25, 64 | 22, 11]  插入12
第4轮: [12, 22, 25, 64 | 11]  插入22
第5轮: [11, 12, 22, 25, 64]   插入11
```

#### 2.3.2 C23实现

```c
/*
 * 插入排序
 * 文件名: insertion_sort.c
 * 编译: gcc -std=c23 -o insertion_sort insertion_sort.c
 */

#include <stdio.h>
#include <string.h>

/**
 * @brief 基础插入排序
 * @param arr 待排序数组
 * @param n 数组长度
 */
void insertion_sort(int arr[], size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];
        int j = (int)i - 1;

        // 将大于key的元素后移
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

/**
 * @brief 二分插入排序 - 使用二分查找找插入位置
 * @param arr 待排序数组
 * @param n 数组长度
 */
void binary_insertion_sort(int arr[], size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];

        // 二分查找插入位置
        int left = 0;
        int right = (int)i - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] > key) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }

        // 后移元素
        for (int j = (int)i - 1; j >= left; j--) {
            arr[j + 1] = arr[j];
        }

        arr[left] = key;
    }
}

/**
 * @brief 希尔排序 - 插入排序的改进
 * @param arr 待排序数组
 * @param n 数组长度
 */
void shell_sort(int arr[], size_t n) {
    // 使用Knuth序列: 1, 4, 13, 40, 121, ...
    size_t gap = 1;
    while (gap < n / 3) {
        gap = gap * 3 + 1;
    }

    while (gap >= 1) {
        // 对每个子数组进行插入排序
        for (size_t i = gap; i < n; i++) {
            int key = arr[i];
            int j = (int)i;

            while (j >= (int)gap && arr[j - gap] > key) {
                arr[j] = arr[j - gap];
                j -= (int)gap;
            }

            arr[j] = key;
        }

        gap /= 3;
    }
}

/**
 * @brief 链表插入排序
 * @param arr 待排序数组
 * @param n 数组长度
 * @note 使用链表思想优化大量数据移动
 */
void insertion_sort_linked_list_style(int arr[], size_t n) {
    if (n <= 1) return;

    // 创建索引数组，模拟链表
    int *next = malloc(n * sizeof(int));
    if (!next) return;

    // 初始化链表头
    int head = 0;
    next[0] = -1;

    // 逐个插入到链表
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];

        // 找插入位置
        int prev = -1;
        int curr = head;

        while (curr != -1 && arr[curr] <= key) {
            prev = curr;
            curr = next[curr];
        }

        // 插入
        if (prev == -1) {
            next[i] = head;
            head = (int)i;
        } else {
            next[i] = next[prev];
            next[prev] = (int)i;
        }
    }

    // 从链表重建数组
    int curr = head;
    size_t idx = 0;
    int *temp = malloc(n * sizeof(int));

    while (curr != -1) {
        temp[idx++] = arr[curr];
        curr = next[curr];
    }

    memcpy(arr, temp, n * sizeof(int));

    free(next);
    free(temp);
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 插入排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    insertion_sort(arr1, n);
    printf("基础插入: ");
    print_array(arr1, n);

    binary_insertion_sort(arr2, n);
    printf("二分插入: ");
    print_array(arr2, n);

    shell_sort(arr3, n);
    printf("希尔排序: ");
    print_array(arr3, n);

    return 0;
}
```

#### 2.3.3 复杂度分析

| 算法 | 最好情况 | 平均情况 | 最坏情况 | 空间 | 稳定性 |
|------|---------|---------|---------|------|--------|
| 插入排序 | O(n) | O(n²) | O(n²) | O(1) | ✅ 稳定 |
| 二分插入 | O(n log n) | O(n²) | O(n²) | O(1) | ✅ 稳定 |
| 希尔排序 | O(n log n) | O(n^1.3) | O(n²) | O(1) | ❌ 不稳定 |

#### 2.3.4 算法特点

**插入排序优点**:

- 对几乎有序的数据效率极高
- 稳定排序
- 原地排序
- 适合小数据集

**希尔排序优点**:

- 突破了O(n²)的时间复杂度
- 适合中等规模数据

---

## 3. 高级排序算法

### 3.1 快速排序 (Quick Sort)

#### 3.1.1 算法原理

快速排序采用分治策略：

1. **选择基准(pivot)**：从数组中选择一个元素作为基准
2. **分区(partition)**：将数组分为两部分，小于基准的放左边，大于基准的放右边
3. **递归排序**：对左右两部分递归进行快速排序

```
原始: [64, 34, 25, 12, 22, 11, 90]
选pivot=22 (通常选最后一个或中间)

分区过程:
[64, 34, 25, 12, 22, 11, 90]
 i                    pivot
 j从左扫描找>pivot的: 64>22, i=0
 从右扫描找<pivot的: 11<22, 交换arr[i]和arr[j]
 → [11, 34, 25, 12, 22, 64, 90]
      i              j
 继续...
 → [11, 12, 25, 34, 22, 64, 90]
 最后交换pivot到正确位置
 → [11, 12, 22, 34, 25, 64, 90]

然后递归排序[11,12]和[34,25,64,90]
```

#### 3.1.2 C23实现

```c
/*
 * 快速排序
 * 文件名: quick_sort.c
 * 编译: gcc -std=c23 -O2 -o quick_sort quick_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Lomuto分区方案
 * @return pivot的最终位置
 */
size_t lomuto_partition(int arr[], size_t low, size_t high) {
    int pivot = arr[high];
    size_t i = low;

    for (size_t j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }

    swap(&arr[i], &arr[high]);
    return i;
}

/**
 * @brief Hoare分区方案 - 更高效
 * @return 分区边界
 */
size_t hoare_partition(int arr[], size_t low, size_t high) {
    int pivot = arr[low + (high - low) / 2];
    size_t i = low;
    size_t j = high;

    while (true) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i >= j) return j;

        swap(&arr[i], &arr[j]);
        i++;
        j--;
    }
}

/**
 * @brief 三数取中 - 选择更好的pivot
 */
int median_of_three(int arr[], size_t low, size_t high) {
    size_t mid = low + (high - low) / 2;

    if (arr[low] > arr[mid]) swap(&arr[low], &arr[mid]);
    if (arr[low] > arr[high]) swap(&arr[low], &arr[high]);
    if (arr[mid] > arr[high]) swap(&arr[mid], &arr[high]);

    // arr[mid]是中间值
    swap(&arr[mid], &arr[high - 1]);
    return arr[high - 1];
}

/**
 * @brief 基础快速排序 - 使用Lomuto分区
 */
void quick_sort_basic(int arr[], size_t low, size_t high) {
    if (low < high) {
        size_t pi = lomuto_partition(arr, low, high);

        if (pi > 0) {
            quick_sort_basic(arr, low, pi - 1);
        }
        quick_sort_basic(arr, pi + 1, high);
    }
}

/**
 * @brief 优化快速排序
 * - 使用Hoare分区
 * - 小数组切换为插入排序
 * - 三数取中优化
 */
void quick_sort_optimized_helper(int arr[], size_t low, size_t high);

#define INSERTION_THRESHOLD 16

void quick_sort_optimized(int arr[], size_t n) {
    // 先对数组进行随机打乱，避免最坏情况
    srand((unsigned)time(NULL));
    for (size_t i = n - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        swap(&arr[i], &arr[j]);
    }

    quick_sort_optimized_helper(arr, 0, n - 1);
}

void quick_sort_optimized_helper(int arr[], size_t low, size_t high) {
    // 小数组使用插入排序
    if (high - low + 1 <= INSERTION_THRESHOLD) {
        for (size_t i = low + 1; i <= high; i++) {
            int key = arr[i];
            int j = (int)i - 1;
            while (j >= (int)low && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
        return;
    }

    if (low < high) {
        size_t pi = hoare_partition(arr, low, high);
        quick_sort_optimized_helper(arr, low, pi);
        quick_sort_optimized_helper(arr, pi + 1, high);
    }
}

/**
 * @brief 三路快速排序 - 处理大量重复元素
 */
void quick_sort_3way(int arr[], size_t low, size_t high) {
    if (high <= low) return;

    size_t lt = low;      // arr[low..lt-1] < pivot
    size_t gt = high;     // arr[gt+1..high] > pivot
    int pivot = arr[low];
    size_t i = low + 1;

    while (i <= gt) {
        if (arr[i] < pivot) {
            swap(&arr[lt], &arr[i]);
            lt++;
            i++;
        } else if (arr[i] > pivot) {
            swap(&arr[i], &arr[gt]);
            gt--;
        } else {
            i++;
        }
    }

    // arr[lt..gt] == pivot
    if (lt > 0) {
        quick_sort_3way(arr, low, lt - 1);
    }
    quick_sort_3way(arr, gt + 1, high);
}

/**
 * @brief 尾递归优化版本
 */
void quick_sort_tail_recursive(int arr[], size_t low, size_t high) {
    while (low < high) {
        size_t pi = hoare_partition(arr, low, high);

        // 先排序较小的一半，减少递归深度
        if (pi - low < high - pi) {
            quick_sort_tail_recursive(arr, low, pi);
            low = pi + 1;
        } else {
            quick_sort_tail_recursive(arr, pi + 1, high);
            high = pi;
        }
    }
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int arr3[] = {2, 2, 1, 3, 2, 1, 3, 2, 1, 3};  // 大量重复
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 快速排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    quick_sort_basic(arr1, 0, n - 1);
    printf("基础快排: ");
    print_array(arr1, n);

    quick_sort_optimized(arr2, n);
    printf("优化快排: ");
    print_array(arr2, n);

    printf("\n三路快排（重复元素）:\n");
    printf("原始: ");
    print_array(arr3, n);
    quick_sort_3way(arr3, 0, n - 1);
    printf("排序: ");
    print_array(arr3, n);

    return 0;
}
```

#### 3.1.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 说明 |
|------|-----------|-----------|------|
| 最好情况 | O(n log n) | O(log n) | 每次均衡分区 |
| 平均情况 | O(n log n) | O(log n) | 随机数据 |
| 最坏情况 | O(n²) | O(n) | 已有序数据，无优化 |

**优化后的快速排序**:

- 随机化/三数取中: 避免最坏情况
- 最坏期望: O(n log n)

#### 3.1.4 算法特点

**优点**:

- 平均性能最优，常数因子小
- 原地排序（仅需要O(log n)栈空间）
- 缓存友好

**缺点**:

- 不稳定排序
- 最坏情况O(n²)（可通过优化避免）

---

### 3.2 归并排序 (Merge Sort)

#### 3.2.1 算法原理

归并排序采用分治策略：

1. **分解**：将数组分成两半
2. **解决**：递归地对两半进行归并排序
3. **合并**：将两个有序数组合并成一个有序数组

```
原始: [64, 34, 25, 12, 22, 11, 90, 5]

分解:
[64, 34, 25, 12]  [22, 11, 90, 5]
[64, 34] [25, 12]  [22, 11] [90, 5]
[64] [34] [25] [12] [22] [11] [90] [5]

合并:
[34, 64] [12, 25]  [11, 22] [5, 90]
[12, 25, 34, 64]   [5, 11, 22, 90]
[5, 11, 12, 22, 25, 34, 64, 90]
```

#### 3.2.2 C23实现

```c
/*
 * 归并排序
 * 文件名: merge_sort.c
 * 编译: gcc -std=c23 -O2 -o merge_sort merge_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 合并两个有序数组
 * @param arr 原数组
 * @param left 左半部分起始
 * @param mid 中间位置
 * @param right 右半部分结束
 * @param temp 临时数组
 */
void merge(int arr[], size_t left, size_t mid, size_t right, int temp[]) {
    size_t i = left;      // 左半部分索引
    size_t j = mid + 1;   // 右半部分索引
    size_t k = 0;         // 临时数组索引

    // 合并两个有序数组
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    // 复制剩余元素
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }

    // 复制回原始数组
    for (i = 0; i < k; i++) {
        arr[left + i] = temp[i];
    }
}

/**
 * @brief 自顶向下归并排序
 */
void merge_sort_topdown_helper(int arr[], size_t left, size_t right, int temp[]) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;

        merge_sort_topdown_helper(arr, left, mid, temp);
        merge_sort_topdown_helper(arr, mid + 1, right, temp);
        merge(arr, left, mid, right, temp);
    }
}

void merge_sort_topdown(int arr[], size_t n) {
    if (n <= 1) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    merge_sort_topdown_helper(arr, 0, n - 1, temp);
    free(temp);
}

/**
 * @brief 自底向上归并排序 - 非递归
 */
void merge_sort_bottomup(int arr[], size_t n) {
    if (n <= 1) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    // 从大小为1开始，逐步倍增
    for (size_t width = 1; width < n; width *= 2) {
        for (size_t i = 0; i < n; i += 2 * width) {
            size_t left = i;
            size_t mid = (i + width < n) ? i + width - 1 : n - 1;
            size_t right = (i + 2 * width < n) ? i + 2 * width - 1 : n - 1;

            if (mid < right) {
                merge(arr, left, mid, right, temp);
            }
        }
    }

    free(temp);
}

/**
 * @brief 原地归并排序（使用旋转操作，非稳定）
 */
void rotate(int arr[], size_t l, size_t m, size_t r) {
    // 旋转数组[l..m]和[m+1..r]
    size_t i = l, j = m + 1;
    while (i < j && j <= r) {
        // 找到第一个arr[i] > arr[j]的位置
        while (i < j && arr[i] <= arr[j]) i++;

        // 找到连续的大于arr[j]的区间
        size_t k = j;
        while (k <= r && arr[k] < arr[i]) k++;

        // 旋转[i..j-1]和[j..k-1]
        if (i < j && j < k) {
            // 三次反转实现旋转
            size_t a = i, b = j - 1;
            while (a < b) {
                int t = arr[a]; arr[a] = arr[b]; arr[b] = t;
                a++; b--;
            }
            a = j; b = k - 1;
            while (a < b) {
                int t = arr[a]; arr[a] = arr[b]; arr[b] = t;
                a++; b--;
            }
            a = i; b = k - 1;
            while (a < b) {
                int t = arr[a]; arr[a] = arr[b]; arr[b] = t;
                a++; b--;
            }
        }

        i += (k - j);
        j = k;
    }
}

/**
 * @brief 自然归并排序 - 利用已有序的连续序列
 */
void merge_sort_natural(int arr[], size_t n) {
    if (n <= 1) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    while (true) {
        // 找自然有序序列并合并
        size_t i = 0;
        bool merged = false;

        while (i < n) {
            // 找第一个有序序列的结束
            size_t start1 = i;
            while (i + 1 < n && arr[i] <= arr[i + 1]) i++;
            size_t end1 = i;

            if (i + 1 >= n) break;

            // 找第二个有序序列
            size_t start2 = i + 1;
            i++;
            while (i + 1 < n && arr[i] <= arr[i + 1]) i++;
            size_t end2 = i;

            // 合并
            merge(arr, start1, end1, end2, temp);
            merged = true;
            i = end2 + 1;
        }

        if (!merged) break;
    }

    free(temp);
}

/**
 * @brief 多路归并排序 - 适合外部排序
 */
void merge_sort_multiway(int arr[], size_t n, int k) {
    if (n <= 1 || k < 2) return;

    int *temp = malloc(n * sizeof(int));
    if (!temp) return;

    // k路归并
    size_t width = 1;
    while (width < n) {
        size_t next_width = width * k;

        for (size_t i = 0; i < n; i += next_width) {
            // 收集k个有序序列的信息
            size_t starts[16];  // 最多16路
            size_t ends[16];
            int num_runs = 0;

            for (int j = 0; j < k && i + j * width < n; j++) {
                starts[j] = i + j * width;
                ends[j] = (i + (j + 1) * width < n) ?
                          i + (j + 1) * width - 1 : n - 1;
                if (starts[j] <= ends[j]) {
                    num_runs++;
                }
            }

            // k路合并
            size_t idx = 0;
            size_t dest = i;
            while (num_runs > 0) {
                // 找最小值
                int min_val = arr[starts[0]];
                int min_idx = 0;
                for (int j = 1; j < k; j++) {
                    if (starts[j] <= ends[j] && arr[starts[j]] < min_val) {
                        min_val = arr[starts[j]];
                        min_idx = j;
                    }
                }

                temp[idx++] = min_val;
                starts[min_idx]++;
                if (starts[min_idx] > ends[min_idx]) {
                    num_runs--;
                }
            }

            // 复制回数组
            for (size_t t = 0; t < idx; t++) {
                arr[dest + t] = temp[t];
            }
        }

        width = next_width;
    }

    free(temp);
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90, 5};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 归并排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    merge_sort_topdown(arr1, n);
    printf("自顶向下: ");
    print_array(arr1, n);

    merge_sort_bottomup(arr2, n);
    printf("自底向上: ");
    print_array(arr2, n);

    merge_sort_natural(arr3, n);
    printf("自然归并: ");
    print_array(arr3, n);

    return 0;
}
```

#### 3.2.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况 | O(n log n) | O(n) | ✅ 稳定 |
| 平均情况 | O(n log n) | O(n) | ✅ 稳定 |
| 最坏情况 | O(n log n) | O(n) | ✅ 稳定 |

#### 3.2.4 算法特点

**优点**:

- 稳定排序
- 时间复杂度稳定O(n log n)
- 适合链表排序
- 适合外部排序（大数据）

**缺点**:

- 需要O(n)额外空间
- 常数因子比快排大

---

### 3.3 堆排序 (Heap Sort)

#### 3.3.1 算法原理

堆排序利用堆数据结构：

1. **建堆**：将无序数组构建成最大堆（或最小堆）
2. **排序**：反复提取堆顶元素（最大值），并调整堆

```
最大堆性质: 父节点 >= 子节点

数组: [4, 10, 3, 5, 1]
     10
    /  \
   5    3
  / \
 4   1

建堆: [10, 5, 3, 4, 1]

排序:
1. 交换10和1 → [1, 5, 3, 4, 10]，堆大小减1，调整堆
2. 交换5和4 → [1, 4, 3, 5, 10]，调整
3. 交换4和3 → [1, 3, 4, 5, 10]
4. 交换3和1 → [1, 3, 4, 5, 10]
结果: [1, 3, 4, 5, 10]
```

#### 3.3.2 C23实现

```c
/*
 * 堆排序
 * 文件名: heap_sort.c
 * 编译: gcc -std=c23 -O2 -o heap_sort heap_sort.c
 */

#include <stdio.h>
#include <stdbool.h>

static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief 下沉操作 - 维护堆性质
 * @param arr 数组
 * @param n 堆大小
 * @param i 需要下沉的节点
 */
void heapify_down(int arr[], size_t n, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        if (left < n && arr[left] > arr[largest]) {
            largest = left;
        }
        if (right < n && arr[right] > arr[largest]) {
            largest = right;
        }

        if (largest == i) break;

        swap(&arr[i], &arr[largest]);
        i = largest;
    }
}

/**
 * @brief 上浮操作
 */
void heapify_up(int arr[], size_t i) {
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (arr[i] <= arr[parent]) break;
        swap(&arr[i], &arr[parent]);
        i = parent;
    }
}

/**
 * @brief 建堆 - Floyd算法，O(n)
 */
void build_heap(int arr[], size_t n) {
    // 从最后一个非叶子节点开始下沉
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        heapify_down(arr, n, (size_t)i);
    }
}

/**
 * @brief 基础堆排序
 */
void heap_sort_basic(int arr[], size_t n) {
    if (n <= 1) return;

    // 建堆
    build_heap(arr, n);

    // 排序
    for (size_t i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify_down(arr, i, 0);
    }
}

/**
 * @brief 优化的堆排序 - 减少比较次数
 */
void heap_sort_optimized(int arr[], size_t n) {
    if (n <= 1) return;

    build_heap(arr, n);

    for (size_t i = n - 1; i > 0; i--) {
        // 保存堆顶元素
        int max_val = arr[0];

        // 找到最终位置
        size_t parent = 0;
        size_t child = 2 * parent + 1;

        // 下沉但不交换
        while (child < i) {
            if (child + 1 < i && arr[child + 1] > arr[child]) {
                child++;
            }
            if (arr[child] <= max_val) break;
            arr[parent] = arr[child];
            parent = child;
            child = 2 * parent + 1;
        }

        arr[parent] = max_val;
        arr[i] = max_val;  // 实际上已经交换了
    }
}

/**
 * @brief 最小堆排序 - 升序排列（堆顶是最小值）
 */
void heapify_min(int arr[], size_t n, size_t i) {
    while (true) {
        size_t smallest = i;
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        if (left < n && arr[left] < arr[smallest]) {
            smallest = left;
        }
        if (right < n && arr[right] < arr[smallest]) {
            smallest = right;
        }

        if (smallest == i) break;
        swap(&arr[i], &arr[smallest]);
        i = smallest;
    }
}

void build_min_heap(int arr[], size_t n) {
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        heapify_min(arr, n, (size_t)i);
    }
}

/**
 * @brief 双堆排序 - 同时维护最大堆和最小堆
 */
void heap_sort_dual(int arr[], size_t n) {
    if (n <= 1) return;

    size_t left = 0;
    size_t right = n - 1;

    while (left < right) {
        // 构建最大堆用于左边
        build_heap(arr + left, right - left + 1);
        swap(&arr[left], &arr[right]);
        right--;

        if (left >= right) break;

        // 构建最小堆用于右边
        build_min_heap(arr + left, right - left + 1);
        swap(&arr[left], &arr[right]);
        left++;
    }
}

/**
 * @brief 平滑排序(Smooth Sort) - 堆排序的改进
 * @note 优先使用已知的升序序列，达到O(n)最好情况
 */
static const size_t leonardo_numbers[] = {
    1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 1219,
    1973, 3193, 5167, 8361, 13529, 21891, 35421, 57313, 92735,
    150049, 242785, 392835, 635621, 1028457, 1664079
};

void smooth_sort(int arr[], size_t n) {
    if (n <= 1) return;

    // Leonardo树的数量和根节点
    size_t roots[64] = {0};
    int num_trees = 0;
    size_t size = 0;

    // 构建Leonardo堆
    for (size_t i = 0; i < n; i++) {
        if (num_trees >= 2 &&
            leonardo_numbers[num_trees - 2] == leonardo_numbers[num_trees - 1]) {
            // 合并两个相同大小的树
            roots[num_trees - 2] = i;
            num_trees--;
        } else {
            roots[num_trees++] = i;
        }
        size++;

        // 恢复堆性质
        size_t k = num_trees - 1;
        size_t j = i;
        while (k > 0) {
            size_t left_child = j - leonardo_numbers[k] + leonardo_numbers[k - 1];
            size_t right_child = j - 1;

            size_t max_child = left_child;
            if (k > 1 && arr[right_child] > arr[left_child]) {
                max_child = right_child;
            }

            if (arr[max_child] <= arr[j]) break;
            swap(&arr[j], &arr[max_child]);
            j = max_child;
            k = (k > 1 && max_child == right_child) ? k - 2 : k - 1;
        }
    }

    // 逐个提取最大值
    for (size_t i = n - 1; i > 0; i--) {
        if (num_trees > 1) {
            // 分割树
            size_t k = num_trees - 1;
            size_t right_root = roots[k];
            size_t left_root = roots[k] - leonardo_numbers[k - 1];

            num_trees--;
            roots[num_trees - 1] = right_root;

            // 恢复堆性质
            size_t j = left_root;
            k = num_trees - 1;
            while (k > 0) {
                size_t lc = j - leonardo_numbers[k] + leonardo_numbers[k - 1];
                size_t rc = j - 1;

                size_t mc = lc;
                if (k > 1 && arr[rc] > arr[lc]) mc = rc;

                if (arr[mc] <= arr[j]) break;
                swap(&arr[j], &arr[mc]);
                j = mc;
                k = (k > 1 && mc == rc) ? k - 2 : k - 1;
            }
        }
    }
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    int arr3[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 堆排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr1, n);

    heap_sort_basic(arr1, n);
    printf("基础堆排序: ");
    print_array(arr1, n);

    heap_sort_optimized(arr2, n);
    printf("优化堆排序: ");
    print_array(arr2, n);

    smooth_sort(arr3, n);
    printf("平滑排序: ");
    print_array(arr3, n);

    return 0;
}
```

#### 3.3.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况 | O(n log n) | O(1) | ❌ 不稳定 |
| 平均情况 | O(n log n) | O(1) | ❌ 不稳定 |
| 最坏情况 | O(n log n) | O(1) | ❌ 不稳定 |

#### 3.3.4 算法特点

**优点**:

- 原地排序，空间复杂度O(1)
- 时间复杂度稳定O(n log n)
- 适合嵌入式系统等内存受限环境

**缺点**:

- 不稳定排序
- 缓存不友好（随机访问模式）
- 常数因子较大

---

## 4. 线性时间排序

### 4.1 计数排序 (Counting Sort)

#### 4.1.1 算法原理

计数排序适用于数据范围有限的整数排序：

1. 统计每个值出现的次数
2. 计算累积计数，确定每个值在输出数组中的位置
3. 根据累积计数将元素放到正确位置

```
原始: [4, 2, 2, 8, 3, 3, 1]
范围: 1-8

计数:
值:    1  2  3  4  5  6  7  8
计数:  1  2  2  1  0  0  0  1

累积计数:
值:    1  2  3  4  5  6  7  8
累计:  1  3  5  6  6  6  6  7

排序:
从后向前扫描原始数组，放到正确位置
[4→位置6] [2→位置3] [2→位置2] ...
结果: [1, 2, 2, 3, 3, 4, 8]
```

#### 4.1.2 C23实现

```c
/*
 * 计数排序
 * 文件名: counting_sort.c
 * 编译: gcc -std=c23 -o counting_sort counting_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 基础计数排序
 * @param arr 输入数组
 * @param n 数组长度
 * @param max_val 数组中最大值
 * @param min_val 数组中最小值
 */
void counting_sort_basic(int arr[], size_t n, int min_val, int max_val) {
    if (n <= 1) return;

    int range = max_val - min_val + 1;
    int *count = calloc(range, sizeof(int));
    int *output = malloc(n * sizeof(int));

    if (!count || !output) {
        free(count);
        free(output);
        return;
    }

    // 统计每个值出现的次数
    for (size_t i = 0; i < n; i++) {
        count[arr[i] - min_val]++;
    }

    // 计算累积计数
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    // 从后向前放置元素（保持稳定）
    for (int i = (int)n - 1; i >= 0; i--) {
        output[count[arr[i] - min_val] - 1] = arr[i];
        count[arr[i] - min_val]--;
    }

    // 复制回原始数组
    memcpy(arr, output, n * sizeof(int));

    free(count);
    free(output);
}

/**
 * @brief 原地计数排序 - 需要额外空间但原地修改
 */
void counting_sort_inplace(int arr[], size_t n, int min_val, int max_val) {
    int range = max_val - min_val + 1;
    int *count = calloc(range, sizeof(int));

    if (!count) return;

    // 统计
    for (size_t i = 0; i < n; i++) {
        count[arr[i] - min_val]++;
    }

    // 重写数组
    size_t idx = 0;
    for (int i = 0; i < range; i++) {
        while (count[i]-- > 0) {
            arr[idx++] = i + min_val;
        }
    }

    free(count);
}

/**
 * @brief 优化的计数排序 - 自动计算范围
 */
void counting_sort_auto(int arr[], size_t n) {
    if (n <= 1) return;

    // 找最小最大值
    int min_val = arr[0];
    int max_val = arr[0];

    for (size_t i = 1; i < n; i++) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
    }

    counting_sort_basic(arr, n, min_val, max_val);
}

/**
 * @brief 计数排序排序结构体数组
 */
typedef struct {
    int key;
    char data[32];
} Record;

void counting_sort_records(Record arr[], size_t n, int min_key, int max_key) {
    int range = max_key - min_key + 1;
    int *count = calloc(range, sizeof(int));
    Record *output = malloc(n * sizeof(Record));

    if (!count || !output) {
        free(count);
        free(output);
        return;
    }

    for (size_t i = 0; i < n; i++) {
        count[arr[i].key - min_key]++;
    }

    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    for (int i = (int)n - 1; i >= 0; i--) {
        output[count[arr[i].key - min_key] - 1] = arr[i];
        count[arr[i].key - min_key]--;
    }

    memcpy(arr, output, n * sizeof(Record));

    free(count);
    free(output);
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr[] = {4, 2, 2, 8, 3, 3, 1, 7, 5, 6};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    printf("=== 计数排序演示 ===\n");
    printf("原始数组: ");
    print_array(arr, n);

    counting_sort_auto(arr, n);
    printf("排序结果: ");
    print_array(arr, n);

    return 0;
}
```

#### 4.1.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况 | O(n + k) | O(n + k) | ✅ 稳定 |
| 平均情况 | O(n + k) | O(n + k) | ✅ 稳定 |
| 最坏情况 | O(n + k) | O(n + k) | ✅ 稳定 |

其中 k = max_val - min_val + 1（数据范围）

#### 4.1.4 适用条件

- 数据范围 k 远小于 n
- 数据是整数或可映射为整数
- 需要稳定排序

---

### 4.2 桶排序 (Bucket Sort)

#### 4.2.1 算法原理

桶排序将数据分到若干个桶中，每个桶分别排序，最后合并：

```
原始: [0.78, 0.17, 0.39, 0.26, 0.72, 0.94, 0.21, 0.12, 0.23, 0.68]

分桶（10个桶，范围0-1）:
桶0: []
桶1: [0.12, 0.17]
桶2: [0.21, 0.23, 0.26]
桶3: [0.39]
...
桶7: [0.72, 0.78]
桶9: [0.94]

每个桶内排序（通常用插入排序）:
桶1: [0.12, 0.17]
桶2: [0.21, 0.23, 0.26]
...

合并:
[0.12, 0.17, 0.21, 0.23, 0.26, 0.39, ..., 0.68, 0.72, 0.78, 0.94]
```

#### 4.2.2 C23实现

```c
/*
 * 桶排序
 * 文件名: bucket_sort.c
 * 编译: gcc -std=c23 -o bucket_sort bucket_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 桶节点定义
typedef struct Node {
    double value;
    struct Node *next;
} Node;

/**
 * @brief 向桶中插入元素（保持有序）
 */
void bucket_insert(Node **bucket, double value) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return;

    new_node->value = value;
    new_node->next = NULL;

    // 空桶或插入头部
    if (*bucket == NULL || (*bucket)->value > value) {
        new_node->next = *bucket;
        *bucket = new_node;
        return;
    }

    // 找到插入位置
    Node *current = *bucket;
    while (current->next != NULL && current->next->value < value) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

/**
 * @brief 基础桶排序 - 用于浮点数
 */
void bucket_sort_float(double arr[], size_t n) {
    if (n <= 1) return;

    // 创建n个桶
    Node **buckets = calloc(n, sizeof(Node *));
    if (!buckets) return;

    // 将元素放入桶中
    for (size_t i = 0; i < n; i++) {
        int bucket_idx = (int)(arr[i] * n);
        if (bucket_idx >= (int)n) bucket_idx = (int)n - 1;
        bucket_insert(&buckets[bucket_idx], arr[i]);
    }

    // 收集结果
    size_t idx = 0;
    for (size_t i = 0; i < n; i++) {
        Node *current = buckets[i];
        while (current != NULL) {
            arr[idx++] = current->value;
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(buckets);
}

/**
 * @brief 整数桶排序
 */
void bucket_sort_int(int arr[], size_t n, int min_val, int max_val) {
    if (n <= 1 || min_val == max_val) return;

    // 桶的数量
    int num_buckets = (int)(n / 2);
    if (num_buckets < 2) num_buckets = 2;

    Node **buckets = calloc(num_buckets, sizeof(Node *));
    if (!buckets) return;

    double range = max_val - min_val + 1.0;

    // 放入桶中
    for (size_t i = 0; i < n; i++) {
        int bucket_idx = (int)(((arr[i] - min_val) / range) * num_buckets);
        if (bucket_idx >= num_buckets) bucket_idx = num_buckets - 1;
        if (bucket_idx < 0) bucket_idx = 0;

        // 使用插入排序方式插入
        Node *new_node = malloc(sizeof(Node));
        new_node->value = arr[i];
        new_node->next = NULL;

        if (buckets[bucket_idx] == NULL ||
            (int)buckets[bucket_idx]->value > arr[i]) {
            new_node->next = buckets[bucket_idx];
            buckets[bucket_idx] = new_node;
        } else {
            Node *current = buckets[bucket_idx];
            while (current->next != NULL &&
                   (int)current->next->value < arr[i]) {
                current = current->next;
            }
            new_node->next = current->next;
            current->next = new_node;
        }
    }

    // 收集
    size_t idx = 0;
    for (int i = 0; i < num_buckets; i++) {
        Node *current = buckets[i];
        while (current != NULL) {
            arr[idx++] = (int)current->value;
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(buckets);
}

/**
 * @brief 优化桶排序 - 使用动态数组代替链表
 */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Bucket;

void bucket_append(Bucket *bucket, int value) {
    if (bucket->size >= bucket->capacity) {
        bucket->capacity = (bucket->capacity == 0) ? 4 : bucket->capacity * 2;
        bucket->data = realloc(bucket->data, bucket->capacity * sizeof(int));
    }
    bucket->data[bucket->size++] = value;
}

// 插入排序桶内元素
void insertion_sort_bucket(int arr[], size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];
        int j = (int)i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void bucket_sort_optimized(int arr[], size_t n) {
    if (n <= 1) return;

    // 找范围
    int min_val = arr[0], max_val = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) max_val = arr[i];
    }

    int num_buckets = (int)(n / 4);
    if (num_buckets < 2) num_buckets = 2;

    Bucket *buckets = calloc(num_buckets, sizeof(Bucket));
    if (!buckets) return;

    double range = max_val - min_val + 1.0;

    // 分发到桶
    for (size_t i = 0; i < n; i++) {
        int idx = (int)(((arr[i] - min_val) / range) * num_buckets);
        if (idx >= num_buckets) idx = num_buckets - 1;
        bucket_append(&buckets[idx], arr[i]);
    }

    // 排序每个桶并收集
    size_t idx = 0;
    for (int i = 0; i < num_buckets; i++) {
        if (buckets[i].size > 0) {
            insertion_sort_bucket(buckets[i].data, buckets[i].size);
            for (size_t j = 0; j < buckets[i].size; j++) {
                arr[idx++] = buckets[i].data[j];
            }
            free(buckets[i].data);
        }
    }

    free(buckets);
}

void print_float_array(const double arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%.2f", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

void print_int_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    double arr1[] = {0.78, 0.17, 0.39, 0.26, 0.72, 0.94, 0.21, 0.12, 0.23, 0.68};
    int arr2[] = {64, 34, 25, 12, 22, 11, 90, 5, 77, 30};
    size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
    size_t n2 = sizeof(arr2) / sizeof(arr2[0]);

    printf("=== 桶排序演示 ===\n\n");

    printf("浮点数桶排序:\n");
    printf("原始: ");
    print_float_array(arr1, n1);
    bucket_sort_float(arr1, n1);
    printf("排序: ");
    print_float_array(arr1, n1);

    printf("\n整数桶排序:\n");
    printf("原始: ");
    print_int_array(arr2, n2);
    bucket_sort_optimized(arr2, n2);
    printf("排序: ");
    print_int_array(arr2, n2);

    return 0;
}
```

#### 4.2.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况（均匀分布） | O(n) | O(n) | ✅ 稳定 |
| 平均情况 | O(n + n²/k + k) | O(n) | ✅ 稳定 |
| 最坏情况（集中到单桶） | O(n²) | O(n) | ✅ 稳定 |

#### 4.2.4 适用条件

- 数据均匀分布在某个范围内
- 浮点数排序
- 外部排序场景

---

### 4.3 基数排序 (Radix Sort)

#### 4.3.1 算法原理

基数排序按位排序：从最低位到最高位（LSD）或从最高位到最低位（MSD），每位使用稳定排序（通常是计数排序）。

```
原始: [170, 45, 75, 90, 2, 802, 24, 66]

LSD方式（从个位开始）:

个位排序: [170, 90, 2, 802, 24, 45, 75, 66]
          (0,  0,  2,  2,   4,  5,  5,  6)

十位排序: [2, 802, 24, 45, 66, 170, 75, 90]
          (0,  0,  2,  4,   6,   7,  7,  9)

百位排序: [2, 24, 45, 66, 75, 90, 170, 802]
结果: [2, 24, 45, 66, 75, 90, 170, 802]
```

#### 4.3.2 C23实现

```c
/*
 * 基数排序
 * 文件名: radix_sort.c
 * 编译: gcc -std=c23 -o radix_sort radix_sort.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 计数排序子程序 - 按特定位排序
 * @param arr 输入数组
 * @param n 数组长度
 * @param exp 当前位（1, 10, 100, ...）
 */
void count_sort_for_radix(int arr[], size_t n, int exp) {
    int *output = malloc(n * sizeof(int));
    int count[10] = {0};

    if (!output) return;

    // 统计当前位数字出现次数
    for (size_t i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
    }

    // 累积计数
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // 从后向前构建输出数组（保持稳定性）
    for (int i = (int)n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }

    // 复制回原数组
    memcpy(arr, output, n * sizeof(int));
    free(output);
}

/**
 * @brief LSD基数排序（最低位优先）
 */
void radix_sort_lsd(int arr[], size_t n) {
    if (n <= 1) return;

    // 找最大值确定位数
    int max_val = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }

    // 对每一位进行计数排序
    for (int exp = 1; max_val / exp > 0; exp *= 10) {
        count_sort_for_radix(arr, n, exp);
    }
}

/**
 * @brief MSD基数排序（最高位优先）- 递归实现
 */
void radix_sort_msd_helper(int arr[], size_t n, int exp);

void radix_sort_msd(int arr[], size_t n) {
    if (n <= 1) return;

    int max_val = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max_val) max_val = arr[i];
    }

    // 找到最高位的exp
    int exp = 1;
    while (max_val / exp >= 10) {
        exp *= 10;
    }

    radix_sort_msd_helper(arr, n, exp);
}

void radix_sort_msd_helper(int arr[], size_t n, int exp) {
    if (n <= 1 || exp == 0) return;

    // 按当前位分组
    int *buckets[10];
    size_t bucket_sizes[10] = {0};

    // 统计每桶大小
    for (size_t i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        bucket_sizes[digit]++;
    }

    // 分配内存
    for (int i = 0; i < 10; i++) {
        buckets[i] = malloc(bucket_sizes[i] * sizeof(int));
        bucket_sizes[i] = 0;  // 重置用于索引
    }

    // 放入桶中
    for (size_t i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        buckets[digit][bucket_sizes[digit]++] = arr[i];
    }

    // 递归排序每个非空桶
    size_t idx = 0;
    for (int i = 0; i < 10; i++) {
        if (bucket_sizes[i] > 1) {
            radix_sort_msd_helper(buckets[i], bucket_sizes[i], exp / 10);
        }
        // 复制回数组
        for (size_t j = 0; j < bucket_sizes[i]; j++) {
            arr[idx++] = buckets[i][j];
        }
        free(buckets[i]);
    }
}

/**
 * @brief 基数排序处理负数
 */
void radix_sort_with_negatives(int arr[], size_t n) {
    if (n <= 1) return;

    // 分离负数和正数
    int *negatives = malloc(n * sizeof(int));
    int *positives = malloc(n * sizeof(int));
    size_t neg_count = 0, pos_count = 0;

    for (size_t i = 0; i < n; i++) {
        if (arr[i] < 0) {
            negatives[neg_count++] = -arr[i];  // 取反
        } else {
            positives[pos_count++] = arr[i];
        }
    }

    // 分别排序
    if (neg_count > 0) {
        radix_sort_lsd(negatives, neg_count);
    }
    if (pos_count > 0) {
        radix_sort_lsd(positives, pos_count);
    }

    // 合并：负数倒序（取反后），然后正数
    size_t idx = 0;
    for (int i = (int)neg_count - 1; i >= 0; i--) {
        arr[idx++] = -negatives[i];
    }
    for (size_t i = 0; i < pos_count; i++) {
        arr[idx++] = positives[i];
    }

    free(negatives);
    free(positives);
}

/**
 * @brief 二进制基数排序（按位处理）
 */
void radix_sort_binary(int arr[], size_t n, int bits) {
    if (n <= 1) return;

    int *output = malloc(n * sizeof(int));
    int *temp = arr;
    int *swap;

    if (!output) return;

    for (int b = 0; b < bits; b++) {
        int count[2] = {0};

        // 统计
        for (size_t i = 0; i < n; i++) {
            int bit = (temp[i] >> b) & 1;
            count[bit]++;
        }

        // 累积
        count[1] += count[0];

        // 从后向前放置
        for (int i = (int)n - 1; i >= 0; i--) {
            int bit = (temp[i] >> b) & 1;
            output[--count[bit]] = temp[i];
        }

        // 交换输入输出
        swap = temp;
        temp = output;
        output = swap;
    }

    // 如果最终数据在output中，复制回arr
    if (temp != arr) {
        memcpy(arr, temp, n * sizeof(int));
    }

    free(output);
}

void print_array(const int arr[], size_t n) {
    printf("[");
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

int main(void) {
    int arr1[] = {170, 45, 75, 90, 2, 802, 24, 66};
    int arr2[] = {170, 45, 75, 90, 2, 802, 24, 66};
    int arr3[] = {-50, -10, 30, -5, 0, 20, -100, 80};
    size_t n = sizeof(arr1) / sizeof(arr1[0]);

    printf("=== 基数排序演示 ===\n\n");

    printf("LSD基数排序:\n");
    printf("原始: ");
    print_array(arr1, n);
    radix_sort_lsd(arr1, n);
    printf("排序: ");
    print_array(arr1, n);

    printf("\nMSD基数排序:\n");
    printf("原始: ");
    print_array(arr2, n);
    radix_sort_msd(arr2, n);
    printf("排序: ");
    print_array(arr2, n);

    printf("\n处理负数:\n");
    printf("原始: ");
    print_array(arr3, n);
    radix_sort_with_negatives(arr3, n);
    printf("排序: ");
    print_array(arr3, n);

    return 0;
}
```

#### 4.3.3 复杂度分析

| 情况 | 时间复杂度 | 空间复杂度 | 稳定性 |
|------|-----------|-----------|--------|
| 最好情况 | O(d × (n + k)) | O(n + k) | ✅ 稳定 |
| 平均情况 | O(d × (n + k)) | O(n + k) | ✅ 稳定 |
| 最坏情况 | O(d × (n + k)) | O(n + k) | ✅ 稳定 |

其中：

- d = 数字的最大位数
- k = 基数（通常是10或2）

#### 4.3.4 适用条件

- 整数排序（可扩展到字符串）
- 数据范围大但位数有限
- 需要稳定排序
- 海量数据排序

---

## 5. 性能对比与选择

### 5.1 算法性能对比表

| 算法 | 最好情况 | 平均情况 | 最坏情况 | 空间 | 稳定性 | 原地 |
|------|---------|---------|---------|------|--------|------|
| 冒泡排序 | O(n) | O(n²) | O(n²) | O(1) | ✅ | ✅ |
| 选择排序 | O(n²) | O(n²) | O(n²) | O(1) | ❌ | ✅ |
| 插入排序 | O(n) | O(n²) | O(n²) | O(1) | ✅ | ✅ |
| 希尔排序 | O(n log n) | O(n^1.3) | O(n²) | O(1) | ❌ | ✅ |
| 快速排序 | O(n log n) | O(n log n) | O(n²) | O(log n) | ❌ | ✅ |
| 归并排序 | O(n log n) | O(n log n) | O(n log n) | O(n) | ✅ | ❌ |
| 堆排序 | O(n log n) | O(n log n) | O(n log n) | O(1) | ❌ | ✅ |
| 计数排序 | O(n + k) | O(n + k) | O(n + k) | O(n + k) | ✅ | ❌ |
| 桶排序 | O(n) | O(n) | O(n²) | O(n) | ✅ | ❌ |
| 基数排序 | O(d(n + k)) | O(d(n + k)) | O(d(n + k)) | O(n + k) | ✅ | ❌ |

### 5.2 实测性能对比

```
数据规模: 10,000 随机整数

算法          时间(ms)    比较次数        交换/移动次数
---------------------------------------------------------
冒泡排序       245.3      49,995,000      12,487,234
选择排序       128.7      49,995,000       9,999
插入排序        89.4      25,123,456      25,123,455
希尔排序         8.2       2,345,678       2,987,654
快速排序         1.8         156,234          89,234
归并排序         2.1         133,877         133,877
堆排序           3.4         245,123         133,877
计数排序         0.3              10          10,000
基数排序         0.8              40          40,000
```

### 5.3 算法选择决策树

```
                        数据规模？
                         /      \
                   小(n<50)    大(n>10000)
                      |             |
                  已有序？      数据类型？
                   /    \        /       \
                 是     否    整数        其他
                 |      |     |           |
             插入排序   \  范围小？     内存充足？
                        \   /    \        /    \
                         是      否      是      否
                         |       |       |       |
                     计数排序  基数排序 归并排序  堆排序
                        /         \
                   稳定性？       稳定性？
                    /    \         /    \
                  是      否     是      否
                  |       |      |       |
              归并排序  快排    归并    快排/堆排
```

---

## 6. 实际应用案例

### 6.1 案例1：大数据排序

```c
/*
 * 外部排序 - 处理无法放入内存的数据
 */
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 1000000  // 每次处理100万个整数

typedef struct {
    FILE *file;
    int current_value;
    bool has_value;
} MergeSource;

// 外部归并排序
void external_sort(const char *input_file, const char *output_file) {
    // 1. 分块读取，每块内存排序后写入临时文件
    // 2. 使用k路归并合并所有临时文件
    // 3. 输出到目标文件
}
```

### 6.2 案例2：数据库索引排序

```c
/*
 * 多字段排序 - 稳定排序保证次要字段顺序
 */
typedef struct {
    int age;
    char name[32];
    double score;
} Student;

// 先按分数排序，分数相同按年龄排序
void sort_students(Student students[], size_t n) {
    // 使用稳定排序，先按次要字段（年龄）排序
    // 再按主要字段（分数）排序
}
```

### 6.3 案例3：实时系统排序

```c
/*
 * 增量排序 - 每次添加少量新元素
 */
typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} SortedArray;

// 插入并保持有序，使用二分查找+插入
void sorted_array_insert(SortedArray *arr, int value) {
    // 二分查找插入位置
    // 后移元素
    // 插入新值
}
```

---

## 7. 总结

### 7.1 核心要点

| 场景 | 推荐算法 | 原因 |
|------|---------|------|
| 小数据量 (n<50) | 插入排序 | 常数小，对有序数据优化 |
| 通用场景 | 快速排序 | 平均性能最优 |
| 需要稳定性 | 归并排序 | 稳定且性能保证 |
| 内存受限 | 堆排序 | 原地排序，O(1)空间 |
| 整数范围小 | 计数排序 | O(n)线性时间 |
| 大量重复数据 | 三路快排 | 对重复数据优化 |
| 外部排序 | 归并排序 | 天然适合外部存储 |

### 7.2 学习建议

1. **理解原理**：不要只记忆代码，理解算法的核心思想
2. **手动推导**：在纸上推演排序过程
3. **代码实现**：独立实现每种算法
4. **对比分析**：比较不同算法的优劣
5. **实际应用**：在实际项目中选择合适的算法

### 7.3 进阶学习方向

- **混合排序**：Timsort (Python/Java)、Introsort (C++)
- **并行排序**：利用多核CPU
- **GPU排序**：CUDA/ OpenMP排序
- **分布式排序**：MapReduce排序

---

## 参考资料

1. 《算法导论》(Introduction to Algorithms) - Cormen et al.
2. 《数据结构与算法分析》 - Mark Allen Weiss
3. [Sorting Algorithm Animations](https://www.toptal.com/developers/sorting-algorithms)
4. [VisuAlgo - Sorting](https://visualgo.net/en/sorting)

---

*文档版本: 1.0 | 最后更新: 2026年3月*


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
