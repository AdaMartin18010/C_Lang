/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 164
 * Language: c
 * Block ID: a3aaab88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
