/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 446
 * Language: c
 * Block ID: edc8a4aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 合并两个堆
 * 简单方法: 合并数组后重新建堆
 * 时间复杂度: O(n + m)
 */
void heap_merge(Heap* result, const Heap* heap1, const Heap* heap2) {
    if (heap1->type != heap2->type) {
        fprintf(stderr, "Error: Cannot merge heaps of different types\n");
        return;
    }

    size_t total_size = heap1->size + heap2->size;
    int* merged = (int*)malloc(total_size * sizeof(int));

    memcpy(merged, heap1->data, heap1->size * sizeof(int));
    memcpy(merged + heap1->size, heap2->data, heap2->size * sizeof(int));

    heap_build(result, merged, total_size, heap1->type);
    free(merged);
}
