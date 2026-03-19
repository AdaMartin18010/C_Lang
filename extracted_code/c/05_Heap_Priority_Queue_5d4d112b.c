/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 284
 * Language: c
 * Block ID: 5d4d112b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 下沉操作 (Heapify)
 * 将索引 k 处的元素向下调整，恢复堆性质
 *
 * 时间复杂度: O(log n)
 */
void heap_sink(Heap* heap, size_t k) {
    while (true) {
        size_t left = LEFT(k);
        size_t right = RIGHT(k);
        size_t target = k;

        // 找出父、左子、右子中的"极值"(最大堆找最大，最小堆找最小)
        if (left < heap->size &&
            heap_compare(heap, heap->data[target], heap->data[left])) {
            target = left;
        }

        if (right < heap->size &&
            heap_compare(heap, heap->data[target], heap->data[right])) {
            target = right;
        }

        // 如果 k 已经是极值，停止
        if (target == k) break;

        // 交换
        heap_swap(&heap->data[k], &heap->data[target]);
        k = target;
    }
}
