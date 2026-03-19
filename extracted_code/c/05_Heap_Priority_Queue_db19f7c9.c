/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 1561
 * Language: c
 * Block ID: db19f7c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 稳定堆排序
 * 通过存储原始索引来保证相等元素的相对顺序
 */

typedef struct {
    int value;
    int index;  // 原始索引
} IndexedValue;

void stable_heap_sort(int* arr, size_t n) {
    IndexedValue* indexed = (IndexedValue*)malloc(n * sizeof(IndexedValue));
    for (size_t i = 0; i < n; i++) {
        indexed[i].value = arr[i];
        indexed[i].index = (int)i;
    }

    // 比较时，值相等则按原始索引比较
    // 构建最大堆...
    // 堆排序过程中使用自定义比较:
    // indexed[i].value > indexed[j].value ||
    // (indexed[i].value == indexed[j].value && indexed[i].index > indexed[j].index)

    // 将结果复制回原数组
    for (size_t i = 0; i < n; i++) {
        arr[i] = indexed[i].value;
    }

    free(indexed);
}
