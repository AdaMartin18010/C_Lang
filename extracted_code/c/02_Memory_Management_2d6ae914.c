/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1784
 * Language: c
 * Block ID: 2d6ae914
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 明显的双重释放
void double_free_simple(void) {
    int *p = malloc(sizeof(int));
    free(p);
    free(p);  // ❌ 双重释放！未定义行为
}

// ❌ 错误: 条件分支导致的双重释放
void double_free_conditional(int *data, int flag) {
    if (flag) {
        process_data(data);
        free(data);  // 第一个释放
    }

    // 某些清理逻辑...

    if (flag) {  // 条件重复
        free(data);  // ❌ 双重释放！
    }
}

// ❌ 错误: 别名导致的双重释放
void double_free_alias(void) {
    int *p = malloc(sizeof(int));
    int *q = p;  // q是p的别名

    free(p);
    free(q);  // ❌ 同一块内存被释放两次！
}

// ❌ 错误: realloc后错误释放原指针
void double_free_realloc(void) {
    int *p = malloc(100);

    int *new_p = realloc(p, 200);
    if (new_p != NULL) {
        p = new_p;
    }

    free(p);
    free(new_p);  // ❌ 如果realloc成功，p和new_p指向同一块！
}

// ✅ 正确: 防御性释放宏
#define SAFE_FREE(ptr) do { \
    free(ptr); \
    (ptr) = NULL; \
} while(0)

void safe_example(void) {
    int *p = malloc(sizeof(int));
    SAFE_FREE(p);
    SAFE_FREE(p);  // 安全: free(NULL)是无操作的
}
