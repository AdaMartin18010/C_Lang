/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 924
 * Language: c
 * Block ID: ebb56300
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 数据流中的中位数
 * 使用两个堆: 最大堆(存较小一半) + 最小堆(存较大一半)
 */

typedef struct {
    Heap max_heap;  // 较小的一半 (最大堆)
    Heap min_heap;  // 较大的一半 (最小堆)
} MedianFinder;

void median_finder_init(MedianFinder* mf) {
    heap_init(&mf->max_heap, MAX_HEAP);
    heap_init(&mf->min_heap, MIN_HEAP);
}

void median_finder_destroy(MedianFinder* mf) {
    heap_destroy(&mf->max_heap);
    heap_destroy(&mf->min_heap);
}

void median_finder_add_num(MedianFinder* mf, int num) {
    // 先放入最大堆
    heap_push(&mf->max_heap, num);

    // 将最大堆顶移到最小堆
    int max_top;
    heap_pop(&mf->max_heap, &max_top);
    heap_push(&mf->min_heap, max_top);

    // 平衡两个堆的大小
    if (mf->min_heap.size > mf->max_heap.size) {
        int min_top;
        heap_pop(&mf->min_heap, &min_top);
        heap_push(&mf->max_heap, min_top);
    }
}

double median_finder_find_median(MedianFinder* mf) {
    if (mf->max_heap.size > mf->min_heap.size) {
        return (double)mf->max_heap.data[0];
    }
    return (mf->max_heap.data[0] + mf->min_heap.data[0]) / 2.0;
}
