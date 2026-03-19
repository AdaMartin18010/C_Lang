/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\03_Storage_Duration_Matrix.md
 * Line: 80
 * Language: c
 * Block ID: bc6c7a26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自动存储期 - 灵活初始化
void func(int n) {
    int arr[n];          // VLA，运行时确定大小
    int x = n * 2;       // 运行时表达式
    for (int i = 0; i < n; i++) arr[i] = i;
}

// 静态存储期 - 常量初始化
static int count = 0;           // ✅ 常量
static int* ptr = NULL;         // ✅ 常量
// static int size = func();    // ❌ C23前错误

// 线程存储期 - 类似静态
_Thread_local int thread_id = 0;  // ✅ 常量初始化

// 分配存储期 - 运行时完全控制
void* p = malloc(size);
memset(p, 0, size);              // 手动初始化
