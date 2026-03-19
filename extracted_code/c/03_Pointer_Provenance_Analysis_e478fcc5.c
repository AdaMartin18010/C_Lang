/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 1120
 * Language: c
 * Block ID: e478fcc5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优化示例与来源假设
// 原始代码
void original(int* a, int* b) {
    *a = 1;
    *b = 2;
    int x = *a;
    printf("%d\n", x);
}

// 优化后（假设 a 和 b 不别名）
void optimized(int* a, int* b) {
    *b = 2;     // 重排序（Store 到 a 和 b 无关）
    *a = 1;
    printf("1\n");  // 常量传播：x 必须是 1
}

// 但如果 a == b，原始代码输出 2，优化后输出 1
// 严格别名规则使此优化合法

// 防止不当优化
void prevent_bad_optimization(int* a, int* b) {
    // 方式 1：使用 volatile
    *(volatile int*)a = 1;
    *b = 2;
    int x = *(volatile int*)a;

    // 方式 2：使用内存栅栏
    *a = 1;
    atomic_thread_fence(memory_order_seq_cst);
    *b = 2;
    int y = *a;

    printf("%d %d\n", x, y);
}
