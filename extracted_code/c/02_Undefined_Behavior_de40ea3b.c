/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 189
 * Language: c
 * Block ID: de40ea3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用宏包装危险操作
#define CHECK_ADD(a, b, result) \
    do { \
        if (__builtin_add_overflow((a), (b), (result))) { \
            handle_overflow(); \
        } \
    } while(0)

// 断言检查
#include <assert.h>
void process_array(int *arr, size_t n) {
    assert(arr != NULL);
    assert(n > 0 && n <= MAX_SIZE);
    // ...
}

// 使用更安全的数据类型
typedef size_t SafeIndex;  // 无符号，自然检查负值
