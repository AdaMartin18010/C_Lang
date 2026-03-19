/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 684
 * Language: c
 * Block ID: 0a1a1e93
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例5：实用的宏封装技巧
 * 展示如何创建可复用的数组工具宏
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

// 基本数组操作宏
#define ARRAY_COUNT(arr)        _Countof(arr)
#define ARRAY_SIZE_BYTES(arr)   sizeof(arr)
#define ARRAY_ELEM_SIZE(arr)    sizeof((arr)[0])

// 数组遍历宏
#define FOR_EACH(elem, arr) \
    for (size_t _i = 0, _n = _Countof(arr); \
         _i < _n && ((elem) = &(arr)[_i], 1); \
         _i++)

#define FOR_INDEX(i, arr) \
    for (size_t i = 0, _n##_i = _Countof(arr); i < _n##_i; i++)

// 数组初始化宏
#define ARRAY_FILL(arr, value) do { \
    for (size_t _i = 0; _i < _Countof(arr); _i++) { \
        (arr)[_i] = (value); \
    } \
} while(0)

#define ARRAY_ZERO(arr) memset((arr), 0, sizeof(arr))

// 数组复制宏（带安全检查）
#define ARRAY_COPY(dst, src) do { \
    _Static_assert(_Generic((dst), typeof(src): 1, default: 0), \
                   "Array types must match"); \
    static_assert(_Countof(dst) >= _Countof(src), \
                  "Destination array too small"); \
    memcpy((dst), (src), sizeof(src)); \
} while(0)

// 打印数组宏
#define PRINT_ARRAY(arr, fmt) do { \
    printf("["); \
    for (size_t _i = 0; _i < _Countof(arr); _i++) { \
        printf(fmt, (arr)[_i]); \
        if (_i < _Countof(arr) - 1) printf(", "); \
    } \
    printf("]\n"); \
} while(0)

// 查找宏
#define ARRAY_FIND(arr, value, index_ptr) do { \
    *(index_ptr) = (size_t)-1; \
    for (size_t _i = 0; _i < _Countof(arr); _i++) { \
        if ((arr)[_i] == (value)) { \
            *(index_ptr) = _i; \
            break; \
        } \
    } \
} while(0)

int main(void) {
    // 测试FOR_EACH
    int nums[] = {10, 20, 30, 40, 50};
    int *p;

    printf("Using FOR_EACH: ");
    FOR_EACH(p, nums) {
        printf("%d ", *p);
    }
    printf("\n");

    // 测试FOR_INDEX
    printf("Using FOR_INDEX: ");
    FOR_INDEX(i, nums) {
        printf("nums[%zu]=%d ", i, nums[i]);
    }
    printf("\n");

    // 测试ARRAY_FILL
    int filled[5];
    ARRAY_FILL(filled, 42);
    printf("After ARRAY_FILL(filled, 42): ");
    PRINT_ARRAY(filled, "%d");

    // 测试ARRAY_ZERO
    int zeros[5] = {1, 2, 3, 4, 5};
    ARRAY_ZERO(zeros);
    printf("After ARRAY_ZERO: ");
    PRINT_ARRAY(zeros, "%d");

    // 测试ARRAY_COPY
    int source[] = {1, 2, 3, 4, 5};
    int dest[10];
    ARRAY_COPY(dest, source);
    printf("After ARRAY_COPY: ");
    PRINT_ARRAY(dest, "%d");

    // 测试ARRAY_FIND
    size_t found_index;
    ARRAY_FIND(nums, 30, &found_index);
    printf("Index of 30 in nums: %zu\n", found_index);

    ARRAY_FIND(nums, 99, &found_index);
    printf("Index of 99 in nums: %zu (not found = -1 cast to size_t)\n", found_index);

    return 0;
}
