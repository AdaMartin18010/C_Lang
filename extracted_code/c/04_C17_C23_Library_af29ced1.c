/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\04_C17_C23_Library.md
 * Line: 365
 * Language: c
 * Block ID: af29ced1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 返回值不应忽略
[[nodiscard]] int *allocate_buffer(size_t size);
[[nodiscard]] int open_file(const char *path);

// 使用
allocate_buffer(1024);  // 警告：忽略返回值

// 废弃函数
[[deprecated("use new_function() instead")]]
void old_function(void);

void caller(void) {
    old_function();  // 警告：函数已废弃
}

// 可能未使用的参数/变量
void callback(int required, [[maybe_unused]] int optional) {
    // optional可能没有使用，不警告
}

// 纯函数（无副作用，只依赖参数）
[[reproducible]] int square(int x) {
    return x * x;
}

// 更严格的纯函数（不读内存状态）
[[unsequenced]] int add(int a, int b) {
    return a + b;
}
