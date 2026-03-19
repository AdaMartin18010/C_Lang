/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 552
 * Language: c
 * Block ID: 92c994af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移检查清单：
 * 1. 使用 _Generic 替换类型特定宏
 * 2. 考虑使用 _Thread_local 替代 __thread
 * 3. 使用 _Static_assert 替换运行时断言
 * 4. 评估是否需要 threads.h 替代 pthread
 * 5. 使用 _Alignas/_Alignof 替代 __attribute__((aligned))
 */

/* 之前 */
#define max_int(a, b) ((a) > (b) ? (a) : (b))
#define max_double(a, b) ((a) > (b) ? (a) : (b))

/* 之后 */
#define max(a, b) _Generic((a), \
    int: max_int,               \
    double: max_double          \
)(a, b)
