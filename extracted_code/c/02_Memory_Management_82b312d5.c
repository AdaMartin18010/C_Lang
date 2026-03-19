/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2177
 * Language: c
 * Block ID: 82b312d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 整数溢出导致分配不足
void size_overflow_32bit(void) {
    uint32_t n = 0x40000000;  // 约10亿
    // n * sizeof(int) = 0x40000000 * 4 = 0（32位溢出）
    int *arr = malloc(n * sizeof(int));  // ❌ 实际分配0字节！
    arr[0] = 0;  // 立即溢出
}

// ❌ 错误: size_t溢出（64位上较难触发但仍危险）
void size_overflow_large(void) {
    size_t n = SIZE_MAX / 2 + 100;
    int *arr = malloc(n * sizeof(int));  // 乘法溢出
    // 可能分配很小但实际使用巨大索引
}

// ❌ 错误: 多维度数组溢出
void size_overflow_2d(void) {
    int rows = 100000, cols = 100000;
    // rows * cols 溢出
    int *matrix = malloc(rows * cols * sizeof(int));
}

// ❌ 错误: 结构体数组溢出
struct LargeStruct {
    char data[1024];
};

void size_overflow_struct(void) {
    size_t n = SIZE_MAX / 512;
    struct LargeStruct *arr = malloc(n * sizeof(struct LargeStruct));
}

// ✅ 正确: 溢出检查
void size_safe(void) {
    size_t n = get_count();
    size_t elem_size = sizeof(int);

    // 方式1: 手动检查
    if (n > SIZE_MAX / elem_size) {
        // 溢出，处理错误
        return;
    }
    int *arr = malloc(n * elem_size);

    // 方式2: 使用calloc（某些实现有溢出检查）
    int *arr2 = calloc(n, elem_size);

    // 方式3: 使用reallocarray（BSD/GNU）
    #ifdef __GLIBC__
    int *arr3 = reallocarray(NULL, n, elem_size);
    #endif
}
