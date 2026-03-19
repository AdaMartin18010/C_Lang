/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
 * Line: 19
 * Language: c
 * Block ID: 505032ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 全局/静态存储区 - 程序生命周期内持续存在
static int global_counter = 0;
const char* message = "Hello";

// 栈区 - 自动分配和释放
void stack_example(void) {
    int local_array[1024];  // 栈分配
    char buffer[256];       // 栈帧内分配
} // 函数返回时自动释放

// 堆区 - 手动管理
void heap_example(void) {
    int* dynamic = malloc(sizeof(int) * 100);
    if (dynamic) {
        // 使用内存...
        free(dynamic);  // 必须手动释放
    }
}
