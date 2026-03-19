/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\03_Storage_Duration_Matrix.md
 * Line: 165
 * Language: c
 * Block ID: b36d5ad9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自动存储期 - 每次调用重新开始
void counter_auto(void) {
    int count = 0;  // 每次调用初始化为0
    printf("%d\n", ++count);  // 永远输出1
}

// 静态存储期 - 跨调用保持
void counter_static(void) {
    static int count = 0;  // 只初始化一次
    printf("%d\n", ++count);  // 1, 2, 3, ...
}

// 线程存储期 - 每线程独立
_Thread_local int thread_count = 0;
void counter_thread(void) {
    printf("Thread %d: %d\n", thread_id, ++thread_count);
}

// 分配存储期 - 完全控制
typedef struct { int count; } Counter;
Counter* create_counter(void) {
    Counter* c = malloc(sizeof(Counter));
    c->count = 0;
    return c;
}
