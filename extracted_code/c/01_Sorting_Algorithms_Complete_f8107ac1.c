/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\01_Sorting_Algorithms_Complete.md
 * Line: 2613
 * Language: c
 * Block ID: f8107ac1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
