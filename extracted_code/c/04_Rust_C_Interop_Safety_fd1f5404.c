/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 199
 * Language: c
 * Block ID: fd1f5404
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C语言内存管理示例 - 展示了常见内存安全问题 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 内存泄漏示例 */
void memory_leak_example(void) {
    /* 分配内存但未释放 */
    char *buffer = malloc(1024);
    if (buffer == NULL) return;

    strcpy(buffer, "这段内存永远不会被释放");
    /* 函数返回时buffer丢失，造成内存泄漏 */
}

/* 悬垂指针示例 */
void dangling_pointer_example(void) {
    char *dangling;

    {
        char local[] = "局部变量";
        dangling = local;  /* 指向局部变量 */
    } /* local在这里被销毁 */

    /* 危险！dangling现在是悬垂指针 */
    printf("%s\n", dangling);  /* 未定义行为 */
}

/* 缓冲区溢出示例 */
void buffer_overflow_example(void) {
    char buffer[10];
    /* 危险：字符串长度超过缓冲区 */
    strcpy(buffer, "这是一个非常长的字符串，会溢出缓冲区");
}

/* 双重释放示例 */
void double_free_example(void) {
    char *ptr = malloc(100);
    if (ptr == NULL) return;

    free(ptr);
    /* 危险：重复释放同一内存 */
    free(ptr);  /* 未定义行为 */
}

/* 使用已释放内存示例 */
void use_after_free_example(void) {
    int *data = malloc(sizeof(int) * 10);
    if (data == NULL) return;

    data[0] = 42;
    free(data);

    /* 危险：访问已释放的内存 */
    printf("%d\n", data[0]);  /* 未定义行为 */
}
