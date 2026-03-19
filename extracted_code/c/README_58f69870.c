/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 301
 * Language: c
 * Block ID: 58f69870
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 1. 匿名结构体 */
struct {
    struct {
        int x, y;
    };  /* 匿名 */
} point;

point.x = 10;  /* 直接访问 */

/* 2. static_assert */
_Static_assert(sizeof(int) == 4, "int must be 4 bytes");

/* 3. 对齐支持 */
#include <stdalign.h>

alignas(64) char cache_line[64];  /* 64字节对齐 */

size_t alignment = alignof(max_align_t);  /* 查询对齐要求 */

/* 4. noreturn 函数 */
#include <stdnoreturn.h>

noreturn void fatal_error(const char* msg) {
    fprintf(stderr, "FATAL: %s\n", msg);
    exit(1);
}

/* 5. 字符类型 */
char16_t utf16_char;  /* UTF-16 */
char32_t utf32_char;  /* UTF-32 */
