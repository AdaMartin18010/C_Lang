/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 365
 * Language: c
 * Block ID: 08d964bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 插入元素
 * 时间复杂度: O(log n)
 */
bool heap_push(Heap* heap, int value) {
    // 扩容检查
    if (heap->size >= heap->capacity) {
        size_t new_capacity = heap->capacity * 2;
        int* new_data = (int*)realloc(heap->data, new_capacity * sizeof(int));
        if (new_data == nullptr) {
            fprintf(stderr, "Error: Failed to expand heap\n");
            return false;
        }
        heap->data = new_data;
        heap->capacity = new_capacity;
    }

    // 插入到末尾，然后上浮
    heap->data[heap->size] = value;
    heap_swim(heap, heap->size);
    heap->size++;

    return true;
}

/**
 * 删除堆顶
 * 时间复杂度: O(log n)
 */
bool heap_pop(Heap* heap, int* out_value) {
    if (heap->size == 0) {
        fprintf(stderr, "Error: Heap is empty\n");
        return false;
    }

    // 保存堆顶
    *out_value = heap->data[0];

    // 用最后一个元素替换堆顶，然后下沉
    heap->size--;
    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        heap_sink(heap, 0);
    }

    return true;
}

/**
 * 查看堆顶
 * 时间复杂度: O(1)
 */
bool heap_peek(const Heap* heap, int* out_value) {
    if (heap->size == 0) {
        fprintf(stderr, "Error: Heap is empty\n");
        return false;
    }
    *out_value = heap->data[0];
    return true;
}

/**
 * 替换堆顶
 * 弹出堆顶并插入新元素，比 pop + push 更高效
 * 时间复杂度: O(log n)
 */
bool heap_replace(Heap* heap, int value, int* out_old_value) {
    if (heap->size == 0) {
        return heap_push(heap, value);
    }

    *out_old_value = heap->data[0];
    heap->data[0] = value;
    heap_sink(heap, 0);
    return true;
}
