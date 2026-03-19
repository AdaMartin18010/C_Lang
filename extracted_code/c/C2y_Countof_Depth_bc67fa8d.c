/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 567
 * Language: c
 * Block ID: bc67fa8d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例3：数组作为函数参数的处理
 * 演示数组退化问题及解决方案
 */
#include <stdio.h>
#include <string.h>

// 错误示例：数组已退化为指针
void wrong_approach(int arr[]) {
    printf("Inside wrong_approach:\n");
    printf("  sizeof(arr) = %zu (pointer size)\n", sizeof(arr));
    // _Countof(arr);  // 编译错误！
}

// 正确方案一：显式传递大小
void print_ints(int arr[], size_t count) {
    printf("Array elements (%zu): ", count);
    for (size_t i = 0; i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// 正确方案二：使用静态数组大小
void process_fixed_array(int arr[10]) {
    // 虽然arr是指针，但我们知道大小
    printf("Processing fixed array (size 10):\n");
    for (size_t i = 0; i < 10; i++) {
        printf("  %d\n", arr[i]);
    }
}

// 正确方案三：模板化方法（使用指针和大小）
typedef struct {
    int *begin;
    int *end;
} IntRange;

IntRange make_range(int arr[], size_t count) {
    return (IntRange){arr, arr + count};
}

void print_range(IntRange range) {
    printf("Range elements: ");
    for (int *p = range.begin; p < range.end; p++) {
        printf("%d ", *p);
    }
    printf("\n");
}

int main(void) {
    int data[] = {10, 20, 30, 40, 50};
    size_t count = _Countof(data);

    printf("Original array size: %zu elements\n", count);

    wrong_approach(data);
    print_ints(data, count);

    // 使用固定大小函数
    int fixed[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    process_fixed_array(fixed);

    // 使用范围方式
    IntRange range = make_range(data, count);
    print_range(range);

    return 0;
}
