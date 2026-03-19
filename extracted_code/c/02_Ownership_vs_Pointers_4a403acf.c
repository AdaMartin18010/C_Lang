/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 67
 * Language: c
 * Block ID: 4a403acf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C指针的完整能力（同时也是风险来源）
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 示例：C指针的灵活性
void demonstrate_c_pointer_power() {
    // 1. 任意类型转换
    int num = 0x12345678;
    char* byte_view = (char*)&num;  // 将int指针转为char指针

    // 2. 指针算术
    int arr[10] = {0};
    int* p = arr;
    p += 5;  // 跳到第6个元素（越界风险！）

    // 3. 任意内存访问
    int* wild = (int*)0x12345678;  // 可能指向无效地址
    // *wild = 10;  // 可能导致段错误

    // 4. 多个指针指向同一内存
    int* shared1 = malloc(sizeof(int));
    int* shared2 = shared1;  // shared1和shared2拥有同一内存
    *shared1 = 10;
    *shared2 = 20;  // 通过另一个指针修改

    // 问题：谁负责free？双重释放风险
    free(shared1);
    // free(shared2);  // 双重释放！未定义行为
}
