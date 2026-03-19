/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 455
 * Language: c
 * Block ID: f9b9e3f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ========== 迁移前 (C17) ========== */
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

/* 旧式: 需要包含头文件 */
typedef enum { FALSE = 0, TRUE = 1 } BOOL;

/* 旧式: 使用 _Bool */
_Bool flag = 1;

/* 旧式: 使用 NULL */
#define MY_NULL ((void*)0)

/* 旧式: 使用 _Static_assert */
_Static_assert(sizeof(int) == 4, "int size check");

/* 旧式: 使用 _Alignas */
_Alignas(64) char buffer[1024];

/* 旧式: 使用 _Thread_local */
static _Thread_local int tls_var = 0;

void example(void) {
    if (flag == TRUE) {
        printf("True\n");
    }
}

/* ========== 迁移后 (C23) ========== */
#include <stdio.h>

/* C23: bool 是原生关键字，无需头文件 */
typedef enum { MY_FALSE = 0, MY_TRUE = 1 } MY_BOOL;

/* C23: bool 为原生类型 */
bool flag = true;

/* C23: nullptr 为原生关键字 */
/* 删除 MY_NULL 宏定义，直接使用 nullptr */

/* C23: static_assert 为原生 */
static_assert(sizeof(int) == 4, "int size check");

/* C23: alignas 为原生关键字 */
alignas(64) char buffer[1024];

/* C23: thread_local 为原生关键字 */
static thread_local int tls_var = 0;

void example(void) {
    if (flag == true) {  /* C23: true/false 为原生常量 */
        printf("True\n");
    }
}
