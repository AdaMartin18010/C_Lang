/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1240
 * Language: c
 * Block ID: 86585e9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C One-Past-End 使用模式
void one_past_end_usage(void) {
    int arr[10];

    // 模式 1：遍历
    for (int* p = arr; p < arr + 10; p++) {
        *p = 0;  // 合法访问
    }

    // 模式 2：使用 one-past-end 作为结束标记
    int* begin = arr;
    int* end = arr + 10;  // one-past-end

    for (int* p = begin; p != end; p++) {
        *p = 42;
    }

    // 模式 3：计算数组长度
    ptrdiff_t len = end - begin;  // 10

    // 危险：解引用 one-past-end
    // *end = 0;  // 未定义行为！

    (void)len;
}

// 动态数组的 one-past-end
void heap_one_past_end(void) {
    size_t n = 10;
    int* arr = malloc(n * sizeof(int));
    if (!arr) return;

    int* begin = arr;
    int* end = arr + n;  // one-past-end

    // 有效操作
    ptrdiff_t size = end - begin;
    _Bool is_empty = (begin == end);

    // 无效操作
    // int x = *end;  // 未定义行为！

    free(arr);
    (void)is_empty;
    (void)size;
}
