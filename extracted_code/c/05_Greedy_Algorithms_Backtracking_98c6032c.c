/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 975
 * Language: c
 * Block ID: 98c6032c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
