/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 155
 * Language: c
 * Block ID: 8ab35f84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 传统快速排序的分支问题：
 *
 * 1. 划分循环中的比较：
 *    while (i <= j) {
 *        while (arr[i] < pivot) i++;  // 预测困难
 *        while (arr[j] > pivot) j--;  // 预测困难
 *        ...
 *    }
 *
 * 2. 随机数据导致50% miss rate
 *
 * 3. 比较函数调用的间接分支
 */

// 传统快速排序（分支预测不友好）
void quicksort_traditional(int *arr, int left, int right) {
    if (left >= right) return;

    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    while (i <= j) {
        // 这两个循环的分支难以预测
        while (arr[i] < pivot) i++;  // 不可预测
        while (arr[j] > pivot) j--;  // 不可预测

        if (i <= j) {
            SWAP(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    quicksort_traditional(arr, left, j);
    quicksort_traditional(arr, i, right);
}

// 分支预测友好的重构
void quicksort_branchless(int *arr, int left, int right) {
    if (left >= right) return;

    int pivot = median_of_three(arr, left, right);
    int i = left, j = right;

    // 使用条件移动替代分支
    while (i <= j) {
        // 预取下一个缓存行
        __builtin_prefetch(&arr[i + 16], 0, 0);

        // 无分支的查找
        i += (arr[i] < pivot);
        j -= (arr[j] > pivot);

        if (i <= j) {
            // 条件交换（无分支）
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    // 尾递归优化
    if (j - left < right - i) {
        quicksort_branchless(arr, left, j);
        left = i;
    } else {
        quicksort_branchless(arr, i, right);
        right = j;
    }
    goto loop_start;  // 避免递归
loop_start:
    if (left < right) {
        // 继续排序
    }
}
