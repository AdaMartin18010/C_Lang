/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 135
 * Language: c
 * Block ID: 4258abd4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// memory_layout.c
#include <stdio.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>

// 全局变量 - 位于静态数据区
int global_initialized = 42;           // .data 段
int global_uninitialized;              // .bss 段
static int static_var = 100;           // .data 段
const int const_var = 200;             // .rodata 段

// 字符串常量 - 位于只读数据区
const char* hello = "Hello, WASM!";

EMSCRIPTEN_KEEPALIVE
void print_memory_layout() {
    // 局部变量 - 位于栈
    int local_var = 50;
    int local_array[100];

    // 动态分配 - 位于堆
    int* heap_var = (int*)malloc(sizeof(int));
    int* heap_array = (int*)malloc(1000 * sizeof(int));

    printf("=== 内存布局分析 ===\n\n");

    // 静态数据区
    printf("静态数据区:\n");
    printf("  全局初始化变量:     %p (global_initialized)\n", (void*)&global_initialized);
    printf("  全局未初始化变量:   %p (global_uninitialized)\n", (void*)&global_uninitialized);
    printf("  静态变量:           %p (static_var)\n", (void*)&static_var);
    printf("  常量变量:           %p (const_var)\n", (void*)&const_var);
    printf("  字符串常量:         %p (hello)\n", (void*)hello);

    // 栈区
    printf("\n栈区:\n");
    printf("  局部变量:           %p (&local_var)\n", (void*)&local_var);
    printf("  局部数组:           %p (local_array)\n", (void*)local_array);

    // 堆区
    printf("\n堆区:\n");
    printf("  堆变量:             %p (heap_var)\n", (void*)heap_var);
    printf("  堆数组:             %p (heap_array)\n", (void*)heap_array);

    // 内存区间
    printf("\n内存区间分析:\n");
    printf("  静态数据区大小估算: %lu bytes\n",
           (unsigned long)&local_var - (unsigned long)&global_initialized);
    printf("  栈当前位置:         %p\n", (void*)&local_var);
    printf("  堆当前位置:         %p\n", (void*)heap_array);

    // 清理
    free(heap_var);
    free(heap_array);
}

// 栈大小检查
EMSCRIPTEN_KEEPALIVE
void check_stack_usage() {
    char large_array[1024 * 1024];  // 1MB 栈数组
    printf("Large stack array at: %p\n", (void*)large_array);

    // 注意：大数组可能导致栈溢出
    // 默认栈大小通常为 5MB
}
