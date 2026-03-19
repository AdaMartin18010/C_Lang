/*
 * Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
 * Line: 223
 * Language: c
 * Block ID: abcd282b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c
#include <stdio.h>

// Zig生成的函数声明
int zig_add(int a, int b);
int zig_sum(const int *arr, size_t len);
double zig_distance(struct { double x; double y; } p1,
                    struct { double x; double y; } p2);
int zig_divide(int a, int b, int *result);

int main() {
    printf("zig_add(3, 5) = %d\n", zig_add(3, 5));

    int arr[] = {1, 2, 3, 4, 5};
    printf("zig_sum = %d\n", zig_sum(arr, 5));

    int result;
    if (zig_divide(10, 0, &result) == 0) {
        printf("10 / 2 = %d\n", result);
    } else {
        printf("Division error\n");
    }

    return 0;
}
