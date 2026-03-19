/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 237
 * Language: c
 * Block ID: d4b75046
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 比较函数: 根据堆类型决定比较方向
static inline bool heap_compare(const Heap* heap, int parent, int child) {
    if (heap->type == MAX_HEAP) {
        return parent < child;  // 最大堆: 父 < 子 需要交换
    } else {
        return parent > child;  // 最小堆: 父 > 子 需要交换
    }
}

// 交换元素
static inline void heap_swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
