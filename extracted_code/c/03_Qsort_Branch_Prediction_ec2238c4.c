/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 420
 * Language: c
 * Block ID: ec2238c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 缓存行感知（Cache Line Aware）排序：
 *
 * 现代CPU缓存行大小：64字节
 * - int数组：16个int每缓存行
 * - 64位指针：8个每缓存行
 *
 * 优化策略：
 * 1. 分块排序（Tiled sort）
 * 2. 归并排序变体（缓存友好）
 */

#define CACHE_LINE_SIZE 64
#define INTS_PER_CACHE_LINE (CACHE_LINE_SIZE / sizeof(int))

// 分块快速排序
typedef struct {
    int *data;
    int count;
} Block;

// 块内快速排序
void block_quicksort(int *block, int n) {
    // 小块使用插入排序（缓存内）
    if (n <= 64) {
        insertion_sort(block, n);
        return;
    }

    int pivot = median_of_three(block, 0, n - 1);
    PartitionResult p = partition_three_way(block, 0, n - 1, pivot);

    block_quicksort(block, p.lt);
    block_quicksort(block + p.gt + 1, n - p.gt - 1);
}

// 多路归并（k-way merge）
void multiway_merge(int **sources, int *counts, int k, int *dest, int n) {
    // 使用胜者树或败者树
    // 减少比较次数

    int *heap = malloc(k * sizeof(int));  // 索引堆
    int heap_size = 0;

    // 初始化堆
    for (int i = 0; i < k; i++) {
        if (counts[i] > 0) {
            heap[heap_size++] = i;
        }
    }

    // 建堆
    for (int i = heap_size / 2 - 1; i >= 0; i--) {
        sift_down_merge(heap, heap_size, sources, i);
    }

    // 归并
    int *pos = calloc(k, sizeof(int));
    for (int i = 0; i < n; i++) {
        int min_block = heap[0];
        dest[i] = sources[min_block][pos[min_block]++];

        if (pos[min_block] >= counts[min_block]) {
            // 该块耗尽
            heap[0] = heap[--heap_size];
        }

        sift_down_merge(heap, heap_size, sources, 0);
    }

    free(heap);
    free(pos);
}

// 缓存优化的归并排序
void mergesort_cache_optimized(int *arr, int *temp, int n) {
    // 阈值：当子问题适合L1缓存时使用简单归并
    const int CACHE_THRESHOLD = 1024;  // 4KB

    if (n <= 1) return;

    if (n <= CACHE_THRESHOLD) {
        // 标准归并排序
        standard_merge_sort(arr, temp, n);
    } else {
        int mid = n / 2;

        // 递归排序两半
        mergesort_cache_optimized(arr, temp, mid);
        mergesort_cache_optimized(arr + mid, temp + mid, n - mid);

        // 合并（双缓冲，预取）
        merge_with_prefetch(arr, mid, arr + mid, n - mid, temp);
        memcpy(arr, temp, n * sizeof(int));
    }
}

void merge_with_prefetch(int *left, int left_n,
                         int *right, int right_n,
                         int *dest) {
    int i = 0, j = 0, k = 0;

    // 预取
    __builtin_prefetch(left + 16, 0, 3);
    __builtin_prefetch(right + 16, 0, 3);

    while (i < left_n && j < right_n) {
        // 每16个元素预取一次
        if ((i & 15) == 0 && i + 16 < left_n) {
            __builtin_prefetch(left + i + 16, 0, 3);
        }
        if ((j & 15) == 0 && j + 16 < right_n) {
            __builtin_prefetch(right + j + 16, 0, 3);
        }

        // 分支预测友好的比较
        int take_left = (left[i] <= right[j]);
        dest[k++] = take_left ? left[i++] : right[j++];
    }

    // 复制剩余
    while (i < left_n) dest[k++] = left[i++];
    while (j < right_n) dest[k++] = right[j++];
}
