/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 901
 * Language: c
 * Block ID: 79dd01d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 完整动态数组实现 - 包含多种扩容策略
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

typedef struct {
    void *data;
    size_t size;      // 当前元素数量
    size_t capacity;  // 当前容量
    size_t elem_size; // 元素大小
} DynArray;

// 初始化动态数组
int dynarray_init(DynArray *arr, size_t elem_size, size_t initial_cap) {
    arr->size = 0;
    arr->elem_size = elem_size;
    arr->capacity = initial_cap > 0 ? initial_cap : 4;
    arr->data = calloc(arr->capacity, elem_size);
    return arr->data ? 0 : -1;
}

void dynarray_destroy(DynArray *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = arr->capacity = 0;
}

/*
 * 扩容策略1: 倍增策略（最常见，均摊O(1)）
 * 优点: 均摊性能优秀，实现简单
 * 缺点: 可能有最多50%的空间浪费
 */
int dynarray_grow_double(DynArray *arr) {
    size_t new_cap = arr->capacity * 2;
    void *new_data = realloc(arr->data, new_cap * arr->elem_size);
    if (new_data == NULL) return -1;

    arr->data = new_data;
    arr->capacity = new_cap;
    return 0;
}

/*
 * 扩容策略2: 1.5倍增长
 * 优点: 更好的内存利用率，缓存友好性更好
 * 缺点: 更多realloc调用，可能更多拷贝
 */
int dynarray_grow_1_5x(DynArray *arr) {
    size_t new_cap = arr->capacity + arr->capacity / 2;
    if (new_cap < 8) new_cap = 8;

    void *new_data = realloc(arr->data, new_cap * arr->elem_size);
    if (new_data == NULL) return -1;

    arr->data = new_data;
    arr->capacity = new_cap;
    return 0;
}

/*
 * 扩容策略3: 斐波那契增长
 * 优点: 渐进式增长，平衡内存和性能
 * 缺点: 计算稍复杂
 */
int dynarray_grow_fibonacci(DynArray *arr) {
    static const size_t fib[] = {8, 13, 21, 34, 55, 89, 144, 233,
                                  377, 610, 987, 1597, 2584, 4181};
    size_t new_cap = arr->capacity;

    for (size_t i = 0; i < sizeof(fib)/sizeof(fib[0]); i++) {
        if (fib[i] > arr->capacity) {
            new_cap = fib[i];
            break;
        }
    }
    if (new_cap == arr->capacity) new_cap = arr->capacity * 2;

    void *new_data = realloc(arr->data, new_cap * arr->elem_size);
    if (new_data == NULL) return -1;

    arr->data = new_data;
    arr->capacity = new_cap;
    return 0;
}

/*
 * 扩容策略4: 按需精确增长（内存敏感场景）
 */
int dynarray_grow_exact(DynArray *arr, size_t min_cap) {
    size_t new_cap = arr->capacity;
    while (new_cap < min_cap) {
        new_cap = new_cap * 2 + 1;
    }

    void *new_data = realloc(arr->data, new_cap * arr->elem_size);
    if (new_data == NULL) return -1;

    arr->data = new_data;
    arr->capacity = new_cap;
    return 0;
}

// 通用push操作
int dynarray_push(DynArray *arr, const void *elem) {
    if (arr->size >= arr->capacity) {
        if (dynarray_grow_double(arr) != 0) return -1;
    }

    void *dest = (char*)arr->data + arr->size * arr->elem_size;
    memcpy(dest, elem, arr->elem_size);
    arr->size++;
    return 0;
}

// 获取元素
void *dynarray_get(DynArray *arr, size_t index) {
    if (index >= arr->size) return NULL;
    return (char*)arr->data + index * arr->elem_size;
}

// 缩容（释放多余内存）
int dynarray_shrink_to_fit(DynArray *arr) {
    if (arr->capacity > arr->size) {
        void *new_data = realloc(arr->data, arr->size * arr->elem_size);
        if (new_data != NULL || arr->size == 0) {
            arr->data = new_data;
            arr->capacity = arr->size;
        }
    }
    return 0;
}
