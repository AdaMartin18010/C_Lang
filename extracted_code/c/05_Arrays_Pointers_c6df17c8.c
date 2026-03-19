/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\05_Arrays_Pointers.md
 * Line: 334
 * Language: c
 * Block ID: c6df17c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void process(int arr[]) {
    // sizeof(arr) 返回指针大小，不是数组大小
    size_t n = sizeof(arr) / sizeof(arr[0]);  // 错误！
}

// 正确做法：显式传递大小
void process_correct(int arr[], size_t n) {
    // 使用n
}
