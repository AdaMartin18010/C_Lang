/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 695
 * Language: c
 * Block ID: b7cd6273
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例：处理误报的各种方法

// 方法1: 内联抑制注释
// cppcheck-suppress nullPointer
void* maybe_null(void) {
    return rand() % 2 ? NULL : malloc(10);
}

// 方法2: 使用属性（GCC/Clang）
void __attribute__((analyzer_noreturn)) fatal_error(void);

// 方法3: 断言帮助分析器
#include <assert.h>

void process_buffer(char* buf, size_t len) {
    // 帮助静态分析器理解约束
    assert(buf != NULL);
    assert(len > 0);

    // 现在分析器知道buf不为NULL
    buf[0] = 'A';
}

// 方法4: 使用分析器特定的宏
#ifdef __clang_analyzer__
// 为Clang Static Analyzer提供额外信息
#define ANALYZER_ASSERT(x) __builtin_assume(x)
#else
#define ANALYZER_ASSERT(x) ((void)0)
#endif

void optimized_function(int* p) {
    ANALYZER_ASSERT(p != NULL);
    *p = 42;
}
