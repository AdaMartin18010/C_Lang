/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\02_Segfault_Troubleshooting.md
 * Line: 354
 * Language: c
 * Block ID: af2a3965
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// asan_demo.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 检测堆缓冲区溢出
void heap_overflow(void) {
    int *arr = malloc(10 * sizeof(int));
    arr[10] = 42;  // 越界写入
    free(arr);
}

// 检测栈缓冲区溢出
void stack_overflow(void) {
    int arr[10];
    arr[10] = 42;  // 越界写入
}

// 检测使用已释放内存
void use_after_free(void) {
    int *p = malloc(sizeof(int));
    free(p);
    *p = 42;  // 使用已释放内存
}

// 检测双重释放
void double_free(void) {
    int *p = malloc(sizeof(int));
    free(p);
    free(p);  // 双重释放
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    switch (argv[1][0]) {
        case 'h': heap_overflow(); break;
        case 's': stack_overflow(); break;
        case 'u': use_after_free(); break;
        case 'd': double_free(); break;
    }
    return 0;
}
