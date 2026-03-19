/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 498
 * Language: c
 * Block ID: 47b7dae5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
