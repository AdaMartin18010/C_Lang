/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 240
 * Language: c
 * Block ID: c3619a56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 内省排序（Introspective Sort）：
 *
 * 结合快速排序、堆排序和插入排序：
 * - 主要使用快速排序
 * - 当递归深度超过阈值时切换到堆排序（保证O(n log n)）
 * - 小数组使用插入排序
 *
 * glibc的qsort实现基于内省排序
 */

#define INTROSORT_DEPTH_LIMIT(n) (2 * (int)log2(n))
#define INSERTION_THRESHOLD 16

// 插入排序（小数组优化）
void insertion_sort(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // 展开比较减少分支
        while (j >= 0) {
            if (arr[j] <= key) break;
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// 堆排序（保证最坏情况）
void sift_down(int *arr, int start, int end) {
    int root = start;

    while (2 * root + 1 <= end) {
        int child = 2 * root + 1;
        int swap = root;

        if (arr[swap] < arr[child]) {
            swap = child;
        }
        if (child + 1 <= end && arr[swap] < arr[child + 1]) {
            swap = child + 1;
        }

        if (swap == root) return;

        SWAP(arr[root], arr[swap]);
        root = swap;
    }
}

void heap_sort(int *arr, int n) {
    // 建堆
    for (int start = (n - 2) / 2; start >= 0; start--) {
        sift_down(arr, start, n - 1);
    }

    // 排序
    for (int end = n - 1; end > 0; end--) {
        SWAP(arr[0], arr[end]);
        sift_down(arr, 0, end - 1);
    }
}

// 内排主函数
void introsort_loop(int *arr, int left, int right, int depth_limit) {
    while (right - left > INSERTION_THRESHOLD) {
        if (depth_limit == 0) {
            // 递归过深，切换堆排序
            heap_sort(arr + left, right - left + 1);
            return;
        }

        depth_limit--;

        // 三路划分
        int pivot = median_of_three(arr, left, right);
        int p = partition_three_way(arr, left, right, pivot);

        // 递归处理较小部分，循环处理较大部分
        if (p.first - left < right - p.second) {
            introsort_loop(arr, left, p.first, depth_limit);
            left = p.second;
        } else {
            introsort_loop(arr, p.second, right, depth_limit);
            right = p.first;
        }
    }

    // 小数组使用插入排序
    insertion_sort(arr + left, right - left + 1);
}

void introsort(int *arr, int n) {
    if (n <= 1) return;

    introsort_loop(arr, 0, n - 1, INTROSORT_DEPTH_LIMIT(n));
}
