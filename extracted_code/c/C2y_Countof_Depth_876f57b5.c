/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 641
 * Language: c
 * Block ID: 876f57b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例4：_Countof与sizeof的详细对比
 */
#include <stdio.h>

#define OLD_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

int main(void) {
    int arr[10];
    int *p = arr;

    printf("=== Array arr[10] ===\n");
    printf("sizeof(arr):          %zu bytes\n", sizeof(arr));
    printf("sizeof(arr[0]):       %zu bytes\n", sizeof(arr[0]));
    printf("OLD_COUNT(arr):       %zu elements\n", OLD_COUNT(arr));
    printf("_Countof(arr):        %zu elements\n", _Countof(arr));

    printf("\n=== Pointer p = arr ===\n");
    printf("sizeof(p):            %zu bytes (pointer size)\n", sizeof(p));
    printf("sizeof(*p):           %zu bytes (int size)\n", sizeof(*p));
    printf("OLD_COUNT(p):         %zu (WRONG! uses pointer size)\n", OLD_COUNT(p));
    // printf("_Countof(p):       // 编译错误 - 正确行为!\n");

    printf("\n=== 2D Array matrix[3][4] ===\n");
    int matrix[3][4];
    printf("sizeof(matrix):       %zu bytes\n", sizeof(matrix));
    printf("sizeof(matrix[0]):    %zu bytes\n", sizeof(matrix[0]));
    printf("_Countof(matrix):     %zu (rows)\n", _Countof(matrix));
    printf("_Countof(matrix[0]):  %zu (cols)\n", _Countof(matrix[0]));

    printf("\n=== Type names ===\n");
    printf("sizeof(int[10]):      %zu bytes\n", sizeof(int[10]));
    printf("_Countof(int[10]):    %zu elements\n", _Countof(int[10]));
    printf("sizeof(double[5]):    %zu bytes\n", sizeof(double[5]));
    printf("_Countof(double[5]):  %zu elements\n", _Countof(double[5]));

    return 0;
}
