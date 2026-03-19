/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 771
 * Language: c
 * Block ID: 2da582ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// malloc_pitfalls.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

// ========== 常见错误示例 ==========

// 错误1: 内存泄漏
void memory_leak_example() {
    int* data = (int*)malloc(100 * sizeof(int));
    // 使用 data...
    // 忘记 free(data)!
}

// 正确做法
void correct_allocation() {
    int* data = (int*)malloc(100 * sizeof(int));
    if (data) {
        // 使用 data...
        free(data);  // 记得释放！
    }
}

// 错误2: 使用已释放的内存（Use After Free）
void use_after_free() {
    int* ptr = (int*)malloc(sizeof(int));
    *ptr = 42;
    free(ptr);
    // 错误！ptr 已经释放
    // printf("%d\n", *ptr);  // 未定义行为！
}

// 错误3: 重复释放（Double Free）
void double_free() {
    int* ptr = (int*)malloc(sizeof(int));
    free(ptr);
    // free(ptr);  // 错误！重复释放
}

// 错误4: 不匹配 new/delete 和 malloc/free
// C++ 代码中:
// int* p = new int[10];
// free(p);  // 错误！应该用 delete[]

// 错误5: 越界写入
void buffer_overflow() {
    int* arr = (int*)malloc(10 * sizeof(int));
    // arr[10] = 0;  // 错误！越界访问
    free(arr);
}

// 错误6: 未检查 malloc 返回值
void unchecked_malloc() {
    // int* ptr = malloc(1000000000000);  // 大分配可能失败
    // *ptr = 1;  // 如果 ptr 为 NULL，崩溃！
}

// 错误7: 错误的 realloc 使用
void wrong_realloc() {
    int* ptr = (int*)malloc(100);
    // 错误：如果 realloc 失败，原指针丢失
    // ptr = realloc(ptr, 200);  // 不安全！

    // 正确做法
    int* new_ptr = (int*)realloc(ptr, 200);
    if (new_ptr) {
        ptr = new_ptr;
    } else {
        // realloc 失败，ptr 仍然有效
        free(ptr);
    }
}

// 错误8: sizeof 误用
void sizeof_misuse() {
    int* arr = (int*)malloc(10);
    // 错误！应该 malloc(10 * sizeof(int))
    // memset(arr, 0, 10);  // 只清零了 10 字节，不是 10 个 int
    free(arr);
}

// ========== 安全封装 ==========

// 安全的分配宏
#define SAFE_MALLOC(type, count) \
    ((type*)safe_malloc_impl((count) * sizeof(type), __FILE__, __LINE__))

#define SAFE_FREE(ptr) \
    do { safe_free_impl(&(ptr)); } while(0)

static void* safe_malloc_impl(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
    }
    return ptr;
}

static void safe_free_impl(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;  // 避免悬挂指针
    }
}

// 智能指针模拟
typedef struct {
    void* ptr;
    void (*destructor)(void*);
} SmartPtr;

EMSCRIPTEN_KEEPALIVE
SmartPtr* smart_ptr_create(void* ptr, void (*destructor)(void*)) {
    SmartPtr* sp = (SmartPtr*)malloc(sizeof(SmartPtr));
    if (sp) {
        sp->ptr = ptr;
        sp->destructor = destructor;
    }
    return sp;
}

EMSCRIPTEN_KEEPALIVE
void smart_ptr_destroy(SmartPtr* sp) {
    if (sp) {
        if (sp->ptr && sp->destructor) {
            sp->destructor(sp->ptr);
        }
        free(sp);
    }
}
