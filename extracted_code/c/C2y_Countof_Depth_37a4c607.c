/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 797
 * Language: c
 * Block ID: 37a4c607
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例6：安全的数组处理模式
 * 展示如何避免常见错误
 */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// 安全的字符串缓冲区
typedef struct {
    char data[256];
} StringBuffer;

void string_buffer_init(StringBuffer *buf) {
    buf->data[0] = '\0';
}

bool string_buffer_append(StringBuffer *buf, const char *str) {
    size_t current_len = strlen(buf->data);
    size_t avail = _Countof(buf->data) - current_len - 1;

    if (strlen(str) > avail) {
        return false;  // 空间不足
    }

    strncat(buf->data, str, avail);
    return true;
}

// 类型安全的数组包装
typedef struct {
    int *data;
    size_t count;
} IntArray;

// 从栈数组创建视图
#define INT_ARRAY_FROM_STACK(arr) \
    ((IntArray){(arr), _Countof(arr)})

// 安全的数组访问
bool int_array_get(IntArray arr, size_t index, int *out) {
    if (index >= arr.count) {
        return false;
    }
    *out = arr.data[index];
    return true;
}

bool int_array_set(IntArray arr, size_t index, int value) {
    if (index >= arr.count) {
        return false;
    }
    arr.data[index] = value;
    return true;
}

void int_array_print(IntArray arr) {
    printf("IntArray[%zu]: {", arr.count);
    for (size_t i = 0; i < arr.count; i++) {
        printf("%d%s", arr.data[i], i < arr.count - 1 ? ", " : "");
    }
    printf("}\n");
}

// 泛型数组排序（使用宏）
#define ARRAY_SORT(arr, compare_func) do { \
    qsort((arr), _Countof(arr), sizeof((arr)[0]), compare_func); \
} while(0)

int compare_int(const void *a, const void *b) {
    return (*(const int*)a - *(const int*)b);
}

int main(void) {
    // 测试StringBuffer
    StringBuffer buf;
    string_buffer_init(&buf);

    string_buffer_append(&buf, "Hello");
    string_buffer_append(&buf, " ");
    string_buffer_append(&buf, "World");

    printf("StringBuffer: %s\n", buf.data);
    printf("Buffer capacity: %zu\n", _Countof(buf.data));

    // 测试IntArray
    int stack_arr[] = {5, 2, 8, 1, 9, 3};
    IntArray arr = INT_ARRAY_FROM_STACK(stack_arr);

    printf("\nOriginal ");
    int_array_print(arr);

    // 安全访问
    int value;
    if (int_array_get(arr, 2, &value)) {
        printf("Element at index 2: %d\n", value);
    }

    if (!int_array_get(arr, 100, &value)) {
        printf("Index 100 out of bounds (correctly detected)\n");
    }

    // 排序
    ARRAY_SORT(stack_arr, compare_int);
    printf("After sort: ");
    int_array_print(arr);

    return 0;
}
