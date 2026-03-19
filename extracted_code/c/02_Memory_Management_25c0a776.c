/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2002
 * Language: c
 * Block ID: 25c0a776
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 数组索引越界
void out_of_bounds_index(void) {
    int arr[10];
    for (int i = 0; i <= 10; i++) {  // ❌ 应该是 i < 10
        arr[i] = i;  // arr[10] 越界
    }
}

// ❌ 错误: 负数索引（无符号数回绕）
void out_of_bounds_negative(void) {
    int arr[10];
    size_t index = -1;  // 回绕为很大的正数
    arr[index] = 0;     // ❌ 巨大越界
}

// ❌ 错误: 指针算术越界
void out_of_bounds_pointer(void) {
    int arr[10];
    int *p = arr;
    p += 10;   // 指向arr[10]，刚好越界
    *p = 0;    // ❌ UB: 即使不访问，也可能无效
    p++;       // 更远越界
    *p = 0;    // ❌ 严重越界
}

// ❌ 错误: 柔性数组越界
struct Buffer {
    size_t len;
    char data[];
};

void out_of_bounds_flexible(void) {
    struct Buffer *buf = malloc(sizeof(struct Buffer) + 10);
    buf->len = 10;
    buf->data[10] = '\0';  // ❌ 越界访问，有效索引是0-9
}

// ✅ 正确: 边界检查
void bounds_safe(void) {
    int arr[10];
    size_t index = get_index();

    // 方式1: 显式检查
    if (index < 10) {
        arr[index] = 0;
    }

    // 方式2: 使用常量
    const size_t N = sizeof(arr) / sizeof(arr[0]);
    if (index < N) {
        arr[index] = 0;
    }
}
