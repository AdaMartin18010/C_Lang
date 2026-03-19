/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1292
 * Language: c
 * Block ID: 9f1576b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统sizeof方式 - 代码冗长
void process_old(int arr[]) {
    // 无法在函数内使用sizeof技巧
}

void process_old_fixed(int arr[10]) {
    for (size_t i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        // 实际上这里已经错了！arr是指针
    }
}

// 使用_Countof - 在定义处计算
void process_new(int arr[10]) {
    // 仍然无法在函数内使用_Countof(arr)
    // 但使用处代码更清晰
}

// 调用处对比
int data[100];

// 旧方式：需要定义宏或重复计算
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
process(data, ARRAY_SIZE(data));

// 新方式：直接使用
process(data, _Countof(data));
