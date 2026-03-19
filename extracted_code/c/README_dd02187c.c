/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 519
 * Language: c
 * Block ID: dd02187c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的写时复制数组
typedef struct {
    int* data;
    size_t len;
    size_t ref_count;  // 引用计数
} immutable_array_t;

immutable_array_t* immutable_array_create(const int* data, size_t len) {
    immutable_array_t* arr = malloc(sizeof(immutable_array_t));
    arr->data = malloc(len * sizeof(int));
    memcpy(arr->data, data, len * sizeof(int));
    arr->len = len;
    arr->ref_count = 1;
    return arr;
}

immutable_array_t* immutable_array_append(immutable_array_t* arr, int value) {
    // 创建新数组而不是修改原数组
    immutable_array_t* new_arr = malloc(sizeof(immutable_array_t));
    new_arr->len = arr->len + 1;
    new_arr->data = malloc(new_arr->len * sizeof(int));
    memcpy(new_arr->data, arr->data, arr->len * sizeof(int));
    new_arr->data[arr->len] = value;
    new_arr->ref_count = 1;
    return new_arr;
}

void immutable_array_release(immutable_array_t* arr) {
    if (--arr->ref_count == 0) {
        free(arr->data);
        free(arr);
    }
}
