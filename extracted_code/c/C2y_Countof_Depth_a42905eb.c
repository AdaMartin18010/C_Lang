/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 483
 * Language: c
 * Block ID: a42905eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例1：_Countof基本用法
 * 演示一维数组的元素计数
 */
#include <stdio.h>

int main(void) {
    // 不同大小的数组
    char chars[100];
    short shorts[50];
    int ints[25];
    long longs[10];

    printf("Array element counts:\n");
    printf("  chars:  %zu\n", _Countof(chars));
    printf("  shorts: %zu\n", _Countof(shorts));
    printf("  ints:   %zu\n", _Countof(ints));
    printf("  longs:  %zu\n", _Countof(longs));

    // 类型名用法
    printf("\nFrom type names:\n");
    printf("  int[10]:      %zu\n", _Countof(int[10]));
    printf("  double[100]:  %zu\n", _Countof(double[100]));

    return 0;
}
