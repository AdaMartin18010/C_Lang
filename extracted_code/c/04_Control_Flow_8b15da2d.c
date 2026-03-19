/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 285
 * Language: c
 * Block ID: 8b15da2d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误期望
void swap_wrong(int a, int b) {
    int temp = a;
    a = b;      // 只修改局部副本
    b = temp;
}

// ✅ 使用指针实现引用语义
void swap_correct(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 使用
int x = 1, y = 2;
swap_correct(&x, &y);
