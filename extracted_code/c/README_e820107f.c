/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 472
 * Language: c
 * Block ID: e820107f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// map 函数的实现
typedef int (*map_fn_t)(int);

int* array_map(const int* arr, size_t len, map_fn_t fn) {
    int* result = malloc(len * sizeof(int));
    for (size_t i = 0; i < len; i++) {
        result[i] = fn(arr[i]);
    }
    return result;
}

// 使用示例
int square(int x) { return x * x; }
int double_val(int x) { return x * 2; }

int nums[] = {1, 2, 3, 4, 5};
int* squared = array_map(nums, 5, square);
int* doubled = array_map(nums, 5, double_val);
