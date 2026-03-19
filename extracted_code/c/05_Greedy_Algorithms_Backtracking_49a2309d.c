/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 333
 * Language: c
 * Block ID: 49a2309d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
