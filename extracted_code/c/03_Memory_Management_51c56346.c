/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 285
 * Language: c
 * Block ID: 51c56346
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 泄漏示例1: 丢失指针
void leak1() {
    int *p = malloc(100);
    p = malloc(200);  // 第一个100字节丢失！
    free(p);
}

// 泄漏示例2: 异常路径未释放
void leak2() {
    int *p = malloc(100);
    if (some_error()) {
        return;  // 内存泄漏！
    }
    free(p);
}

// 泄漏示例3: 数据结构中的循环引用
// 需要更复杂的检测和打破循环
