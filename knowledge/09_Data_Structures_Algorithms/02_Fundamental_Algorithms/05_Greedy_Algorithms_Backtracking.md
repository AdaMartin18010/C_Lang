# 贪心算法和回溯 - Greedy Algorithms and Backtracking

> **版本**: C23 Standard | **难度**: 中级到高级 | **预计学习时间**: 25小时

---

## 📋 目录

- [贪心算法和回溯 - Greedy Algorithms and Backtracking](#贪心算法和回溯---greedy-algorithms-and-backtracking)
  - [📋 目录](#-目录)
  - [1. 贪心算法概述](#1-贪心算法概述)
    - [1.1 什么是贪心算法](#11-什么是贪心算法)
    - [1.2 贪心 vs 动态规划](#12-贪心-vs-动态规划)
    - [1.3 贪心算法适用场景](#13-贪心算法适用场景)
  - [2. 经典贪心问题](#2-经典贪心问题)
    - [2.1 活动选择问题](#21-活动选择问题)
      - [2.1.1 问题描述](#211-问题描述)
      - [2.1.2 C23实现](#212-c23实现)
    - [2.2 分数背包问题](#22-分数背包问题)
    - [2.3 霍夫曼编码](#23-霍夫曼编码)
    - [2.4 任务调度问题](#24-任务调度问题)
  - [3. 回溯算法框架](#3-回溯算法框架)
    - [3.1 回溯算法概述](#31-回溯算法概述)
    - [3.2 回溯vs贪心vsDP](#32-回溯vs贪心vsdp)
  - [4. 经典回溯问题](#4-经典回溯问题)
    - [4.1 全排列问题](#41-全排列问题)
    - [4.2 N皇后问题](#42-n皇后问题)
    - [4.3 子集问题](#43-子集问题)
    - [4.4 数独求解](#44-数独求解)
  - [5. 剪枝优化技巧](#5-剪枝优化技巧)
    - [5.1 常用剪枝策略](#51-常用剪枝策略)
    - [5.2 剪枝示例](#52-剪枝示例)
  - [6. 贪心vs回溯对比](#6-贪心vs回溯对比)
    - [6.1 问题适用性对比表](#61-问题适用性对比表)
    - [6.2 复杂度对比](#62-复杂度对比)
  - [7. 实战应用](#7-实战应用)
    - [7.1 迷宫求解](#71-迷宫求解)
    - [7.2 旅行商问题（TSP）- 回溯解法](#72-旅行商问题tsp--回溯解法)
    - [7.3 任务分配问题（匈牙利算法思想）](#73-任务分配问题匈牙利算法思想)
  - [总结](#总结)
    - [核心要点](#核心要点)
    - [算法选择指南](#算法选择指南)
    - [学习建议](#学习建议)

---

## 1. 贪心算法概述

### 1.1 什么是贪心算法

贪心算法（Greedy Algorithm）在每一步选择中都采取在当前状态下**最好或最优**的选择，希望通过局部最优达到全局最优。

```
贪心算法的核心特征:
┌─────────────────────────────────────────┐
│  1. 贪心选择性质                         │
│     局部最优选择能产生全局最优解          │
│                                         │
│  2. 最优子结构                           │
│     问题的最优解包含子问题的最优解        │
└─────────────────────────────────────────┘
```

### 1.2 贪心 vs 动态规划

| 特性 | 贪心算法 | 动态规划 |
|------|---------|---------|
| 选择策略 | 仅考虑当前最优 | 考虑所有子问题 |
| 回退机制 | 不回退 | 可比较多种选择 |
| 时间复杂度 | 通常更低 | 较高 |
| 适用性 | 需要证明最优 | 更广泛 |
| 典型问题 | 活动选择、霍夫曼编码 | 背包、LCS |

### 1.3 贪心算法适用场景

```
贪心算法适用的典型问题:
├── 选择类问题
│   ├── 活动选择问题
│   ├── 区间调度问题
│   └── 任务分配问题
│
├── 编码类问题
│   └── 霍夫曼编码
│
├── 图论问题
│   ├── 最小生成树 (Prim/Kruskal)
│   ├── 最短路径 (Dijkstra)
│   └── 拓扑排序
│
└── 其他
    ├── 分数背包问题
    ├── 找零钱问题
    └── 多机调度问题
```

---

## 2. 经典贪心问题

### 2.1 活动选择问题

#### 2.1.1 问题描述

```
问题：给定n个活动的开始和结束时间，选择最大数量的互不重叠活动。

活动:    A    B    C    D    E    F
开始:    1    3    0    5    5    8
结束:    2    4    6    7    9    9

贪心策略：每次都选择结束时间最早的活动
最优解:  A (1-2), C (3-4), D (5-7), F (8-9)
```

#### 2.1.2 C23实现

```c
/*
 * 贪心算法经典问题
 * 文件名: greedy_algorithms.c
 * 编译: gcc -std=c23 -o greedy_algorithms greedy_algorithms.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ==================== 活动选择问题 ====================

typedef struct {
    int id;         // 活动编号
    int start;      // 开始时间
    int end;        // 结束时间
} Activity;

// 按结束时间排序的比较函数
int compare_by_end_time(const void *a, const void *b) {
    Activity *act1 = (Activity *)a;
    Activity *act2 = (Activity *)b;
    return act1->end - act2->end;
}

/**
 * @brief 活动选择问题 - 贪心解法
 * @param activities 活动数组
 * @param n 活动数量
 * @param selected 输出数组，标记选中的活动
 * @return 选中活动数量
 */
int activity_selection(Activity activities[], int n, bool selected[]) {
    if (n == 0) return 0;

    // 按结束时间排序
    qsort(activities, n, sizeof(Activity), compare_by_end_time);

    // 初始化选择标记
    for (int i = 0; i < n; i++) {
        selected[i] = false;
    }

    // 贪心选择
    selected[0] = true;  // 选择结束最早的活动
    int count = 1;
    int last_end = activities[0].end;

    for (int i = 1; i < n; i++) {
        if (activities[i].start >= last_end) {
            selected[i] = true;
            last_end = activities[i].end;
            count++;
        }
    }

    return count;
}

/**
 * @brief 带权重的活动选择 - 最大化总价值
 * @note 需要动态规划，贪心不能保证最优
 */
int weighted_activity_selection(Activity activities[], const int weights[],
                                 int n) {
    // 按结束时间排序
    for (int i = 0; i < n; i++) {
        activities[i].id = i;
    }
    qsort(activities, n, sizeof(Activity), compare_by_end_time);

    // dp[i] = 考虑前i个活动的最大价值
    int *dp = calloc(n + 1, sizeof(int));

    for (int i = 1; i <= n; i++) {
        // 不选第i个活动
        dp[i] = dp[i - 1];

        // 找最后一个不冲突的活动
        int last_non_conflict = 0;
        for (int j = i - 2; j >= 0; j--) {
            if (activities[j].end <= activities[i - 1].start) {
                last_non_conflict = j + 1;
                break;
            }
        }

        // 选第i个活动
        int with_activity = dp[last_non_conflict] + weights[activities[i - 1].id];
        if (with_activity > dp[i]) {
            dp[i] = with_activity;
        }
    }

    int result = dp[n];
    free(dp);
    return result;
}

/**
 * @brief 最少教室安排问题（区间划分）
 * @note 使用优先队列（最小堆）实现
 */
int min_rooms_required(Activity activities[], int n) {
    if (n == 0) return 0;

    // 按开始时间排序
    int compare_by_start(const void *a, const void *b) {
        return ((Activity *)a)->start - ((Activity *)b)->start;
    }
    qsort(activities, n, sizeof(Activity), compare_by_start);

    // 最小堆，存储每个教室最后活动的结束时间
    int *heap = malloc(n * sizeof(int));
    int heap_size = 0;

    // 堆操作函数
    #define HEAP_SWAP(i, j) do { int t = heap[i]; heap[i] = heap[j]; heap[j] = t; } while(0)

    void heap_push(int end_time) {
        heap[heap_size] = end_time;
        int i = heap_size++;
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[parent] <= heap[i]) break;
            HEAP_SWAP(parent, i);
            i = parent;
        }
    }

    int heap_pop() {
        int min = heap[0];
        heap[0] = heap[--heap_size];
        int i = 0;
        while (true) {
            int min_idx = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            if (left < heap_size && heap[left] < heap[min_idx]) min_idx = left;
            if (right < heap_size && heap[right] < heap[min_idx]) min_idx = right;
            if (min_idx == i) break;
            HEAP_SWAP(i, min_idx);
            i = min_idx;
        }
        return min;
    }

    // 第一个活动
    heap_push(activities[0].end);

    for (int i = 1; i < n; i++) {
        // 如果有教室空闲（最早结束的活动 <= 当前开始）
        if (heap[0] <= activities[i].start) {
            heap_pop();  // 复用这个教室
        }
        heap_push(activities[i].end);
    }

    int result = heap_size;
    free(heap);
    return result;
}
```

---

### 2.2 分数背包问题

```c
// ==================== 分数背包问题 ====================

typedef struct {
    int id;
    int weight;
    int value;
    double ratio;  // 单位重量价值
} Item;

int compare_by_ratio(const void *a, const void *b) {
    double r1 = ((Item *)a)->ratio;
    double r2 = ((Item *)b)->ratio;
    return (r1 > r2) ? -1 : (r1 < r2) ? 1 : 0;
}

/**
 * @brief 分数背包问题 - 贪心解法
 * @param items 物品数组
 * @param n 物品数量
 * @param capacity 背包容量
 * @param fractions 输出每个物品拿取的分数
 * @return 最大价值
 */
double fractional_knapsack(Item items[], int n, int capacity,
                           double fractions[]) {
    // 计算单位重量价值
    for (int i = 0; i < n; i++) {
        items[i].ratio = (double)items[i].value / items[i].weight;
        fractions[i] = 0;
    }

    // 按单位重量价值降序排序
    qsort(items, n, sizeof(Item), compare_by_ratio);

    double total_value = 0;
    int remaining = capacity;

    for (int i = 0; i < n && remaining > 0; i++) {
        if (items[i].weight <= remaining) {
            // 全部拿走
            fractions[items[i].id] = 1.0;
            total_value += items[i].value;
            remaining -= items[i].weight;
        } else {
            // 拿走一部分
            double fraction = (double)remaining / items[i].weight;
            fractions[items[i].id] = fraction;
            total_value += items[i].value * fraction;
            remaining = 0;
        }
    }

    return total_value;
}
```

---

### 2.3 霍夫曼编码

```c
// ==================== 霍夫曼编码 ====================

typedef struct HuffmanNode {
    char data;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

HuffmanNode* create_huffman_node(char data, int freq) {
    HuffmanNode *node = malloc(sizeof(HuffmanNode));
    node->data = data;
    node->frequency = freq;
    node->left = node->right = NULL;
    return node;
}

// 最小堆节点比较
int compare_huffman_nodes(const void *a, const void *b) {
    HuffmanNode *n1 = *(HuffmanNode **)a;
    HuffmanNode *n2 = *(HuffmanNode **)b;
    return n1->frequency - n2->frequency;
}

/**
 * @brief 构建霍夫曼树
 * @param data 字符数组
 * @param freq 频率数组
 * @param n 字符数量
 * @return 霍夫曼树根节点
 */
HuffmanNode* build_huffman_tree(const char data[], const int freq[], int n) {
    HuffmanNode **heap = malloc(n * sizeof(HuffmanNode *));

    // 初始化堆
    for (int i = 0; i < n; i++) {
        heap[i] = create_huffman_node(data[i], freq[i]);
    }
    int heap_size = n;

    // 构建霍夫曼树
    while (heap_size > 1) {
        // 排序，取最小的两个
        qsort(heap, heap_size, sizeof(HuffmanNode *), compare_huffman_nodes);

        HuffmanNode *left = heap[0];
        HuffmanNode *right = heap[1];

        // 创建新内部节点
        HuffmanNode *parent = create_huffman_node('\0',
                                                  left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        // 更新堆
        heap[0] = parent;
        heap[1] = heap[--heap_size];
    }

    HuffmanNode *root = heap[0];
    free(heap);
    return root;
}

/**
 * @brief 生成霍夫曼编码
 * @param root 霍夫曼树根
 * @param codes 输出的编码数组
 * @param code 当前编码缓冲区
 * @param depth 当前深度
 */
void generate_huffman_codes(HuffmanNode *root, char codes[256][32],
                            char code[], int depth) {
    if (root == NULL) return;

    // 叶子节点
    if (root->left == NULL && root->right == NULL) {
        code[depth] = '\0';
        strcpy(codes[(unsigned char)root->data], code);
        return;
    }

    // 左子树编码0
    code[depth] = '0';
    generate_huffman_codes(root->left, codes, code, depth + 1);

    // 右子树编码1
    code[depth] = '1';
    generate_huffman_codes(root->right, codes, code, depth + 1);
}

void free_huffman_tree(HuffmanNode *root) {
    if (root == NULL) return;
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}
```

---

### 2.4 任务调度问题

```c
// ==================== 任务调度问题 ====================

typedef struct {
    int id;
    int deadline;   // 截止时间
    int profit;     // 收益
} Job;

int compare_by_profit(const void *a, const void *b) {
    return ((Job *)b)->profit - ((Job *)a)->profit;
}

/**
 * @brief 任务调度问题 - 最大化收益
 * @note 每个任务需要1个单位时间，在截止时间前完成
 */
int job_sequencing(Job jobs[], int n, int max_deadline, bool scheduled[]) {
    // 按收益降序排序
    qsort(jobs, n, sizeof(Job), compare_by_profit);

    // slot[i] = 第i个时间槽安排的任务（-1表示空）
    int *slots = malloc(max_deadline * sizeof(int));
    for (int i = 0; i < max_deadline; i++) {
        slots[i] = -1;
        scheduled[i] = false;
    }

    int total_profit = 0;
    int count = 0;

    for (int i = 0; i < n; i++) {
        // 从截止时间往前找空槽
        for (int j = jobs[i].deadline - 1; j >= 0; j--) {
            if (slots[j] == -1) {
                slots[j] = jobs[i].id;
                scheduled[j] = true;
                total_profit += jobs[i].profit;
                count++;
                break;
            }
        }
    }

    free(slots);
    return count;
}
```

---

## 3. 回溯算法框架

### 3.1 回溯算法概述

```
回溯算法（Backtracking）是DFS的一种，用于搜索所有可能的解。

核心思想:
1. 选择一个路径
2. 如果路径有效，继续前进
3. 如果路径无效或到达终点，回溯
4. 尝试其他可能的路径

回溯框架:
void backtrack(参数) {
    if (满足终止条件) {
        记录解;
        return;
    }

    for (所有选择) {
        做出选择;
        backtrack(下一状态);
        撤销选择;  // 回溯
    }
}
```

### 3.2 回溯vs贪心vsDP

| 算法 | 特点 | 适用场景 |
|------|------|---------|
| 贪心 | 只做当前最优选择，不回溯 | 有最优子结构的问题 |
| DP | 记忆化子问题解，避免重复 | 重叠子问题 |
| 回溯 | 枚举所有可能，剪枝优化 | 需要找所有解或最优解 |

---

## 4. 经典回溯问题

### 4.1 全排列问题

```c
/*
 * 回溯算法经典问题
 * 文件名: backtracking.c
 * 编译: gcc -std=c23 -o backtracking backtracking.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// ==================== 全排列问题 ====================

/**
 * @brief 生成数组的全排列
 * @param nums 输入数组
 * @param n 数组长度
 * @param result 输出所有排列
 * @param result_count 输出排列数量
 * @param max_results 最大输出数量
 */
void backtrack_permute(int nums[], int n, bool used[], int current[],
                       int depth, int **result, int *result_count,
                       int max_results) {
    if (*result_count >= max_results) return;

    // 找到一个完整排列
    if (depth == n) {
        memcpy(result[*result_count], current, n * sizeof(int));
        (*result_count)++;
        return;
    }

    for (int i = 0; i < n; i++) {
        if (used[i]) continue;

        // 做出选择
        used[i] = true;
        current[depth] = nums[i];

        // 递归
        backtrack_permute(nums, n, used, current, depth + 1,
                          result, result_count, max_results);

        // 撤销选择（回溯）
        used[i] = false;
    }
}

int** permute(int nums[], int n, int *returnSize, int max_results) {
    // 计算排列数
    int total = 1;
    for (int i = 2; i <= n; i++) total *= i;
    if (total > max_results) total = max_results;

    // 分配内存
    int **result = malloc(total * sizeof(int *));
    for (int i = 0; i < total; i++) {
        result[i] = malloc(n * sizeof(int));
    }

    bool *used = calloc(n, sizeof(bool));
    int *current = malloc(n * sizeof(int));

    *returnSize = 0;
    backtrack_permute(nums, n, used, current, 0, result, returnSize, max_results);

    free(used);
    free(current);
    return result;
}

/**
 * @brief 全排列II - 包含重复元素
 */
void backtrack_permute_unique(int nums[], int n, bool used[], int current[],
                              int depth, int **result, int *result_count,
                              int max_results) {
    if (*result_count >= max_results) return;

    if (depth == n) {
        memcpy(result[*result_count], current, n * sizeof(int));
        (*result_count)++;
        return;
    }

    for (int i = 0; i < n; i++) {
        if (used[i]) continue;

        // 剪枝：跳过同一层重复的元素
        if (i > 0 && nums[i] == nums[i - 1] && !used[i - 1]) continue;

        used[i] = true;
        current[depth] = nums[i];

        backtrack_permute_unique(nums, n, used, current, depth + 1,
                                  result, result_count, max_results);

        used[i] = false;
    }
}

int** permute_unique(int nums[], int n, int *returnSize, int max_results) {
    // 先排序，方便剪枝
    int compare_int(const void *a, const void *b) {
        return *(int *)a - *(int *)b;
    }
    qsort(nums, n, sizeof(int), compare_int);

    int total = 1;
    for (int i = 2; i <= n; i++) total *= i;
    if (total > max_results) total = max_results;

    int **result = malloc(total * sizeof(int *));
    for (int i = 0; i < total; i++) {
        result[i] = malloc(n * sizeof(int));
    }

    bool *used = calloc(n, sizeof(bool));
    int *current = malloc(n * sizeof(int));

    *returnSize = 0;
    backtrack_permute_unique(nums, n, used, current, 0,
                             result, returnSize, max_results);

    free(used);
    free(current);
    return result;
}
```

### 4.2 N皇后问题

```c
// ==================== N皇后问题 ====================

/**
 * @brief 检查在(row, col)放置皇后是否有效
 * @param board 棋盘，board[i]表示第i行皇后所在的列
 */
bool is_valid_queen_position(int board[], int row, int col) {
    for (int i = 0; i < row; i++) {
        // 检查同一列
        if (board[i] == col) return false;

        // 检查对角线（行差等于列差）
        if (abs(row - i) == abs(col - board[i])) return false;
    }
    return true;
}

/**
 * @brief N皇后回溯求解
 */
void backtrack_n_queens(int n, int board[], int row,
                        int **solutions, int *solution_count,
                        int max_solutions) {
    if (*solution_count >= max_solutions) return;

    // 找到一种解法
    if (row == n) {
        memcpy(solutions[*solution_count], board, n * sizeof(int));
        (*solution_count)++;
        return;
    }

    // 尝试在当前行的每一列放置皇后
    for (int col = 0; col < n; col++) {
        if (!is_valid_queen_position(board, row, col)) continue;

        // 放置皇后
        board[row] = col;

        // 递归下一行
        backtrack_n_queens(n, board, row + 1,
                           solutions, solution_count, max_solutions);

        // 回溯（不需要显式撤销，因为会被覆盖）
    }
}

int** solve_n_queens(int n, int *returnSize, int max_solutions) {
    // 最大解数估算
    int max_possible = (n == 8) ? 92 : 10000;
    if (max_solutions > max_possible) max_solutions = max_possible;

    int **solutions = malloc(max_solutions * sizeof(int *));
    for (int i = 0; i < max_solutions; i++) {
        solutions[i] = malloc(n * sizeof(int));
    }

    int *board = malloc(n * sizeof(int));

    *returnSize = 0;
    backtrack_n_queens(n, board, 0, solutions, returnSize, max_solutions);

    free(board);
    return solutions;
}

/**
 * @brief 打印N皇后解
 */
void print_n_queens_solution(int board[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf(board[i] == j ? "Q " : ". ");
        }
        printf("\n");
    }
    printf("\n");
}
```

### 4.3 子集问题

```c
// ==================== 子集问题 ====================

/**
 * @brief 生成所有子集
 */
void backtrack_subsets(int nums[], int n, int current[], int current_size,
                       int **result, int *result_count, int max_results) {
    if (*result_count >= max_results) return;

    // 记录当前子集
    memcpy(result[*result_count], current, current_size * sizeof(int));
    (*result_count)++;

    // 从start开始选择元素
    for (int i = current_size; i < n; i++) {
        current[current_size] = nums[i];
        backtrack_subsets(nums, n, current, current_size + 1,
                          result, result_count, max_results);
        // 回溯（不需要显式撤销）
    }
}

/**
 * @brief 子集II - 包含重复元素
 */
void backtrack_subsets_unique(int nums[], int n, int start,
                              int current[], int current_size,
                              int **result, int *result_count,
                              int max_results) {
    if (*result_count >= max_results) return;

    memcpy(result[*result_count], current, current_size * sizeof(int));
    (*result_count)++;

    for (int i = start; i < n; i++) {
        // 剪枝：跳过重复元素
        if (i > start && nums[i] == nums[i - 1]) continue;

        current[current_size] = nums[i];
        backtrack_subsets_unique(nums, n, i + 1, current, current_size + 1,
                                  result, result_count, max_results);
    }
}

/**
 * @brief 组合总和
 * @note 找出所有和为target的组合，每个数可重复使用
 */
void backtrack_combination_sum(int candidates[], int n, int target,
                               int current[], int current_size,
                               int start, int **result, int *result_count,
                               int max_results) {
    if (*result_count >= max_results) return;

    if (target == 0) {
        memcpy(result[*result_count], current, current_size * sizeof(int));
        (*result_count)++;
        return;
    }

    if (target < 0) return;

    for (int i = start; i < n; i++) {
        current[current_size] = candidates[i];
        backtrack_combination_sum(candidates, n, target - candidates[i],
                                  current, current_size + 1, i,
                                  result, result_count, max_results);
        // 回溯
    }
}
```

### 4.4 数独求解

```c
// ==================== 数独求解 ====================

#define SIZE 9
#define BOX_SIZE 3

bool is_valid_sudoku(char board[SIZE][SIZE], int row, int col, char num) {
    // 检查行
    for (int j = 0; j < SIZE; j++) {
        if (board[row][j] == num) return false;
    }

    // 检查列
    for (int i = 0; i < SIZE; i++) {
        if (board[i][col] == num) return false;
    }

    // 检查3x3方格
    int box_row = (row / BOX_SIZE) * BOX_SIZE;
    int box_col = (col / BOX_SIZE) * BOX_SIZE;
    for (int i = box_row; i < box_row + BOX_SIZE; i++) {
        for (int j = box_col; j < box_col + BOX_SIZE; j++) {
            if (board[i][j] == num) return false;
        }
    }

    return true;
}

bool solve_sudoku_backtrack(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == '.') {
                for (char num = '1'; num <= '9'; num++) {
                    if (is_valid_sudoku(board, i, j, num)) {
                        board[i][j] = num;

                        if (solve_sudoku_backtrack(board)) {
                            return true;
                        }

                        // 回溯
                        board[i][j] = '.';
                    }
                }
                return false;  // 没有数字可以填
            }
        }
    }
    return true;  // 所有格子都填满了
}

void print_sudoku(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        if (i % 3 == 0 && i > 0) {
            printf("------+-------+------\n");
        }
        for (int j = 0; j < SIZE; j++) {
            if (j % 3 == 0 && j > 0) printf("| ");
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}
```

---

## 5. 剪枝优化技巧

### 5.1 常用剪枝策略

```c
/*
 * 剪枝优化策略
 *
 * 1. 可行性剪枝
 *    如果当前选择导致无法满足约束，停止深入
 *
 * 2. 最优性剪枝
 *    如果当前路径不可能得到比已知解更好的结果，停止
 *
 * 3. 对称性剪枝
 *    避免搜索等价的状态
 *
 * 4. 记忆化
 *    记录已计算的状态，避免重复
 */
```

### 5.2 剪枝示例

```c
/**
 * @brief 0/1背包问题的回溯+剪枝
 */
typedef struct {
    int weight;
    int value;
} Item;

// 按价值密度排序
int compare_item_density(const void *a, const void *b) {
    Item *ia = (Item *)a;
    Item *ib = (Item *)b;
    double da = (double)ia->value / ia->weight;
    double db = (double)ib->value / ib->weight;
    return (da > db) ? -1 : (da < db) ? 1 : 0;
}

void backtrack_knapsack(Item items[], int n, int idx, int current_weight,
                        int current_value, int capacity, int *max_value,
                        double *upper_bounds) {
    // 最优性剪枝：上界估计
    if (idx < n) {
        double upper = current_value + upper_bounds[idx];
        if (upper <= *max_value) return;
    }

    // 更新最优解
    if (current_value > *max_value) {
        *max_value = current_value;
    }

    if (idx >= n) return;

    // 选择当前物品
    if (current_weight + items[idx].weight <= capacity) {
        backtrack_knapsack(items, n, idx + 1,
                           current_weight + items[idx].weight,
                           current_value + items[idx].value,
                           capacity, max_value, upper_bounds);
    }

    // 不选择当前物品
    backtrack_knapsack(items, n, idx + 1,
                       current_weight, current_value,
                       capacity, max_value, upper_bounds);
}

int knapsack_backtracking(Item items[], int n, int capacity) {
    // 排序
    qsort(items, n, sizeof(Item), compare_item_density);

    // 计算上界（剩余物品的最大可能价值）
    double *upper_bounds = malloc((n + 1) * sizeof(double));
    upper_bounds[n] = 0;
    int total_weight = 0;
    double total_value = 0;

    for (int i = n - 1; i >= 0; i--) {
        total_weight += items[i].weight;
        total_value += items[i].value;
        upper_bounds[i] = total_value;
    }

    int max_value = 0;
    backtrack_knapsack(items, n, 0, 0, 0, capacity,
                       &max_value, upper_bounds);

    free(upper_bounds);
    return max_value;
}
```

---

## 6. 贪心vs回溯对比

### 6.1 问题适用性对比表

| 问题 | 贪心 | 回溯 | 推荐算法 |
|------|------|------|---------|
| 活动选择（最大数量） | ✅最优 | ✅最优 | 贪心（更简单） |
| 活动选择（最大价值） | ❌不最优 | ✅最优 | 回溯/DP |
| 0/1背包 | ❌不最优 | ✅最优 | 回溯/DP |
| 分数背包 | ✅最优 | ✅最优 | 贪心（更简单） |
| N皇后 | ❌不适用 | ✅最优 | 回溯 |
| 全排列 | ❌不适用 | ✅最优 | 回溯 |
| 霍夫曼编码 | ✅最优 | ❌不适用 | 贪心 |
| 数独 | ❌不适用 | ✅最优 | 回溯 |

### 6.2 复杂度对比

| 算法 | 时间复杂度 | 空间复杂度 | 适用场景 |
|------|-----------|-----------|---------|
| 贪心 | 通常O(n log n) | O(1)或O(n) | 局部最优即全局最优 |
| 回溯 | 最坏O(2ⁿ)或O(n!) | O(n)递归栈 | 需要穷举或精确解 |
| 回溯+剪枝 | 取决于剪枝效果 | O(n)递归栈 | 大规模搜索问题 |

---

## 7. 实战应用

### 7.1 迷宫求解

```c
/**
 * @brief 迷宫求解 - 回溯算法
 * @param maze 迷宫，0表示通路，1表示墙
 * @param m, n 迷宫大小
 * @param path 输出路径
 * @return 是否找到路径
 */
bool solve_maze(int maze[][100], int m, int n,
                int start_x, int start_y,
                int end_x, int end_y,
                bool visited[][100], int path[][2], int *path_len) {
    // 到达终点
    if (start_x == end_x && start_y == end_y) {
        path[*path_len][0] = start_x;
        path[*path_len][1] = start_y;
        (*path_len)++;
        return true;
    }

    // 检查边界和有效性
    if (start_x < 0 || start_x >= m || start_y < 0 || start_y >= n ||
        maze[start_x][start_y] == 1 || visited[start_x][start_y]) {
        return false;
    }

    // 标记访问
    visited[start_x][start_y] = true;
    path[*path_len][0] = start_x;
    path[*path_len][1] = start_y;
    (*path_len)++;

    // 四个方向搜索
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int new_x = start_x + dx[i];
        int new_y = start_y + dy[i];

        if (solve_maze(maze, m, n, new_x, new_y, end_x, end_y,
                       visited, path, path_len)) {
            return true;
        }
    }

    // 回溯
    (*path_len)--;
    visited[start_x][start_y] = false;
    return false;
}
```

### 7.2 旅行商问题（TSP）- 回溯解法

```c
/**
 * @brief 旅行商问题 - 回溯+分支限界
 * @param graph 邻接矩阵
 * @param n 城市数
 * @return 最短回路长度
 */
int tsp_backtrack(int graph[][20], int n) {
    bool visited[20] = {false};
    int min_cost = INT_MAX;
    int current_path[20];

    void backtrack_tsp(int curr_pos, int count, int cost) {
        if (count == n && graph[curr_pos][0]) {
            // 回到起点
            int total_cost = cost + graph[curr_pos][0];
            if (total_cost < min_cost) {
                min_cost = total_cost;
            }
            return;
        }

        // 剪枝
        if (cost >= min_cost) return;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && graph[curr_pos][i]) {
                visited[i] = true;
                backtrack_tsp(i, count + 1, cost + graph[curr_pos][i]);
                visited[i] = false;
            }
        }
    }

    visited[0] = true;  // 从城市0开始
    backtrack_tsp(0, 1, 0);

    return min_cost;
}
```

### 7.3 任务分配问题（匈牙利算法思想）

```c
/**
 * @brief 任务分配问题 - 回溯（小规模）
 * @param cost cost[i][j] = 人员i完成任务j的成本
 * @param n 任务/人员数量
 * @return 最小总成本
 */
int assignment_problem(int cost[][20], int n) {
    bool assigned[20] = {false};
    int min_cost = INT_MAX;

    void backtrack_assign(int person, int current_cost) {
        if (person == n) {
            if (current_cost < min_cost) {
                min_cost = current_cost;
            }
            return;
        }

        // 剪枝
        if (current_cost >= min_cost) return;

        for (int task = 0; task < n; task++) {
            if (!assigned[task]) {
                assigned[task] = true;
                backtrack_assign(person + 1, current_cost + cost[person][task]);
                assigned[task] = false;
            }
        }
    }

    backtrack_assign(0, 0);
    return min_cost;
}
```

---

## 总结

### 核心要点

| 算法 | 核心思想 | 时间复杂度 | 适用场景 |
|------|---------|-----------|---------|
| 贪心 | 局部最优 | 通常O(n log n) | 活动选择、霍夫曼编码、分数背包 |
| 回溯 | 穷举+剪枝 | 最坏指数级 | N皇后、排列组合、数独、TSP |

### 算法选择指南

```
遇到问题时的选择流程:

                    需要找所有解？
                         │
            ┌───────────┴───────────┐
           是                       否
            │                       │
    需要精确最优解？          只需一个可行解？
         │                       │
    ┌────┴────┐             ┌────┴────┐
   是         否            是         否
    │          │             │          │
  回溯       贪心验证      贪心        回溯
  (+剪枝)    是否最优？
                │
           ┌────┴────┐
          是         否
           │          │
         贪心       回溯/DP
```

### 学习建议

1. **先学贪心**：理解贪心选择性质的证明方法
2. **再学回溯**：掌握DFS框架和剪枝技巧
3. **对比分析**：同一问题用不同算法解决
4. **大量练习**：回溯问题需要大量练习培养直觉
5. **优化意识**：始终思考如何剪枝优化

---

*文档版本: 1.0 | 最后更新: 2026年3月*
