/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 344
 * Language: c
 * Block ID: 55cf3ee8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 三路划分处理重复元素：
 *
 * [ < pivot | == pivot | > pivot ]
 * ^           ^          ^
 * |           |          |
 * left        mid       right
 *
 * 优势：
 * - 大量重复元素时性能优异
 * - 减少递归深度
 */

typedef struct {
    int lt;   // < pivot的右边界
    int gt;   // > pivot的左边界
} PartitionResult;

PartitionResult partition_three_way(int *arr, int left, int right, int pivot) {
    int i = left;
    int lt = left;
    int gt = right;

    while (i <= gt) {
        if (arr[i] < pivot) {
            SWAP(arr[i], arr[lt]);
            i++;
            lt++;
        } else if (arr[i] > pivot) {
            SWAP(arr[i], arr[gt]);
            gt--;
        } else {
            i++;
        }
    }

    return (PartitionResult){lt, gt};
}

// 针对分支预测优化的三路划分
PartitionResult partition_branchless(int *arr, int left, int right, int pivot) {
    int i = left;
    int lt = left;
    int gt = right;

    while (i <= gt) {
        // 使用条件移动减少分支
        int val = arr[i];
        int less = (val < pivot);
        int greater = (val > pivot);
        int equal = !(less | greater);

        // 计算交换位置
        int swap_idx = less ? lt : (greater ? gt : i);

        // 执行交换
        if (swap_idx != i) {
            arr[i] = arr[swap_idx];
            arr[swap_idx] = val;
        }

        // 更新指针
        lt += less;
        gt -= greater;
        i += equal ? 1 : (less ? 1 : 0);
    }

    return (PartitionResult){lt, gt};
}
