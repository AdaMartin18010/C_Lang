/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1845
 * Language: c
 * Block ID: 4a9b1776
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 忘记给'\0'预留空间
void heap_overflow_string(void) {
    char *str = malloc(5);  // 分配5字节
    strcpy(str, "Hello");   // ❌ 溢出！写入6字节（包括'\0'）
    free(str);
}

// ❌ 错误: 计算大小时忘记乘以sizeof
void heap_overflow_array(void) {
    int *arr = malloc(100);  // ❌ 只分配了100字节，不是100个int！
    for (int i = 0; i < 100; i++) {
        arr[i] = i;  // 从i=25开始溢出（假设int=4字节）
    }
    free(arr);
}

// ❌ 错误: 整数溢出导致分配不足
void heap_overflow_int_overflow(size_t n) {
    // 如果 n = 0x40000000，n * sizeof(int) 溢出为0
    int *arr = malloc(n * sizeof(int));  // ❌ 溢出！
    for (size_t i = 0; i < n; i++) {
        arr[i] = 0;  // 巨大溢出
    }
}

// ✅ 正确: 安全的数组分配
void heap_safe_allocation(void) {
    // 字符串分配
    const char *source = "Hello";
    char *str = malloc(strlen(source) + 1);  // +1 给 '\0'
    if (str) strcpy(str, source);

    // 数组分配
    int n = 100;
    int *arr = malloc((size_t)n * sizeof(int));  // 显式转换和乘法
    // 或使用calloc
    int *arr2 = calloc(n, sizeof(int));

    free(str);
    free(arr);
    free(arr2);
}
