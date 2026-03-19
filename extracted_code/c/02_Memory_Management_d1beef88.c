/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2121
 * Language: c
 * Block ID: d1beef88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 未检查malloc返回值
void unchecked_malloc(void) {
    int *arr = malloc(1000000000000);  // 巨大分配可能失败
    // ❌ 未检查NULL
    for (int i = 0; i < 10; i++) {
        arr[i] = i;  // ❌ 如果arr为NULL，段错误！
    }
}

// ❌ 错误: 未检查calloc返回值
void unchecked_calloc(void) {
    int *arr = calloc(1000, sizeof(int));
    memcpy(arr, source, 1000);  // ❌ 如果失败，NULL解引用
}

// ❌ 错误: 未检查realloc返回值
void unchecked_realloc(void) {
    int *arr = malloc(100);
    arr[0] = 1;

    arr = realloc(arr, 1000000000);  // ❌ 如果失败，原内存丢失！
    // 且arr可能为NULL
}

// ✅ 正确: 总是检查返回值
void checked_allocation(void) {
    int *arr = malloc(100 * sizeof(int));
    if (arr == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // 安全使用
    for (int i = 0; i < 100; i++) {
        arr[i] = i;
    }
    free(arr);
}

// ✅ 正确: realloc的安全模式
void safe_realloc(void) {
    int *arr = malloc(100);
    int *new_arr = realloc(arr, 10000);
    if (new_arr == NULL) {
        // 处理失败，原arr仍然有效
        free(arr);
        return;
    }
    arr = new_arr;
    free(arr);
}
