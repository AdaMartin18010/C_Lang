/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 932
 * Language: c
 * Block ID: 4d80efe2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// multilevel_pointer_demo.c
// 编译: gcc -std=c11 -Wall -Wextra -o multilevel_pointer_demo multilevel_pointer_demo.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void demo_basic(void) {
    printf("=== 基础多级指针演示 ===\n");

    int x = 42;
    int *p = &x;
    int **pp = &p;
    int ***ppp = &pp;

    printf("x = %d\n", x);
    printf("*p = %d\n", *p);
    printf("**pp = %d\n", **pp);
    printf("***ppp = %d\n", ***ppp);

    // 通过三级指针修改值
    ***ppp = 100;
    printf("After ***ppp = 100: x = %d\n\n", x);
}

void demo_allocation(void) {
    printf("=== 二级指针内存分配演示 ===\n");

    int *arr = NULL;
    int n = 5;

    // 模拟二级指针分配
    int **pp = &arr;
    *pp = malloc(n * sizeof(int));

    if (*pp) {
        for (int i = 0; i < n; i++) {
            (*pp)[i] = i * 10;
        }

        printf("Allocated array: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");

        free(*pp);
        *pp = NULL;
    }
    printf("\n");
}

void demo_pointer_array(void) {
    printf("=== 指针数组与二级指针 ===\n");

    char *strings[] = {"Hello", "World", "C", "Language"};
    int n = sizeof(strings) / sizeof(strings[0]);

    // 使用二级指针遍历
    char **pp = strings;

    printf("Using pointer to pointer traversal:\n");
    for (int i = 0; i < n; i++) {
        printf("  strings[%d] = %s (via **pp = %c)\n",
               i, pp[i], **(pp + i));
    }
    printf("\n");
}

int main(void) {
    demo_basic();
    demo_allocation();
    demo_pointer_array();

    printf("=== 所有演示完成 ===\n");
    return 0;
}
