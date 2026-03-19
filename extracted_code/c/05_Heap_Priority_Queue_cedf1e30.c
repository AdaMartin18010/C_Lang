/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 259
 * Language: c
 * Block ID: cedf1e30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 上浮操作
 * 将索引 k 处的元素向上调整，恢复堆性质
 *
 * 时间复杂度: O(log n)
 */
void heap_swim(Heap* heap, size_t k) {
    while (k > 0) {
        size_t parent = PARENT(k);

        // 如果父节点已经满足堆性质，停止
        if (!heap_compare(heap, heap->data[parent], heap->data[k])) {
            break;
        }

        // 交换
        heap_swap(&heap->data[parent], &heap->data[k]);
        k = parent;
    }
}
