/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\README.md
 * Line: 107
 * Language: c
 * Block ID: dde4bba6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题代码示例
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// bugprone: 潜在的空指针解引用
void process_buffer(char* buffer) {
    strcpy(buffer, "data");  // 未检查 buffer 是否为 NULL
}

// cppcoreguidelines: 原始内存管理
int* create_array_wrong(int size) {
    int* arr = (int*)malloc(size * sizeof(int));
    return arr;  // 调用者负责释放，容易出错
}

// performance: 按值传递大对象
struct LargeStruct {
    char data[1024];
};

void process_struct(struct LargeStruct s) {  // 应该使用指针
    printf("%c\n", s.data[0]);
}

// readability: 复杂表达式
int confusing_calc(int a, int b, int c) {
    return a + b * c >> 2 & 0xFF;  // 运算符优先级不明确
}

// modernize: 使用传统 C 风格
void old_style_function() {
    int arr[10];
    memset(arr, 0, sizeof(arr));  // 应该使用初始化
}
