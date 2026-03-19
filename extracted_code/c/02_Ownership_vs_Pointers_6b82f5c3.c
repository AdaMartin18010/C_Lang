/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 606
 * Language: c
 * Block ID: 6b82f5c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C回调：需要手动传递上下文
void process_items(
    int* items,
    size_t count,
    void (*callback)(int, void*),  // 函数指针
    void* user_data                 // 上下文指针
) {
    for (size_t i = 0; i < count; i++) {
        callback(items[i], user_data);
    }
}

// 使用示例
void print_with_prefix(int value, void* user_data) {
    const char* prefix = (const char*)user_data;
    printf("%s: %d\n", prefix, value);
}

// 问题：
// 1. 类型不安全：user_data是void*，需要强制转换
// 2. 生命周期不明确：prefix必须在callback期间有效
// 3. 无法捕获局部变量：需要手动打包到结构体
