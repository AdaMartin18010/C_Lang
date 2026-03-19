/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 320
 * Language: c
 * Block ID: f359e934
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 初始化堆
 */
void heap_init(Heap* heap, HeapType type) {
    heap->capacity = HEAP_DEFAULT_CAPACITY;
    heap->data = (int*)malloc(heap->capacity * sizeof(int));
    if (heap->data == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    heap->size = 0;
    heap->type = type;
}

/**
 * 从数组构建堆 (Floyd建堆算法)
 * 时间复杂度: O(n)
 */
void heap_build(Heap* heap, const int* arr, size_t n, HeapType type) {
    heap->capacity = n > HEAP_DEFAULT_CAPACITY ? n : HEAP_DEFAULT_CAPACITY;
    heap->data = (int*)malloc(heap->capacity * sizeof(int));
    memcpy(heap->data, arr, n * sizeof(int));
    heap->size = n;
    heap->type = type;

    // 从最后一个非叶子节点开始下沉
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        heap_sink(heap, (size_t)i);
    }
}

/**
 * 销毁堆
 */
void heap_destroy(Heap* heap) {
    free(heap->data);
    heap->data = nullptr;
    heap->size = 0;
    heap->capacity = 0;
}
