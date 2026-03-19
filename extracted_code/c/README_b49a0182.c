/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 88
 * Language: c
 * Block ID: b49a0182
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 1. 单行注释（从 C++ 引入） */
// 这是单行注释

/* 2. 变长数组 (VLA) */
void process_matrix(int rows, int cols, int matrix[rows][cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * j;
        }
    }
}

/* 3. 指定初始化器 */
int arr[10] = { [0] = 1, [5] = 5, [9] = 9 };

struct Point { int x; int y; };
struct Point p = { .y = 20, .x = 10 };  /* 任意顺序 */

/* 4. 复合字面量 */
struct Point get_point(void) {
    return (struct Point){ .x = 1, .y = 2 };
}

void print_array(void) {
    int *p = (int[]){ 1, 2, 3, 4, 5 };  /* 临时数组 */
}

/* 5. for 循环内声明变量 */
for (int i = 0; i < 10; i++) {
    /* i 只在循环内可见 */
}

/* 6. 新的数据类型 */
#include <stdint.h>
uint32_t precise_int;    /* 精确宽度 */
uint_least16_t min_int;  /* 最小宽度 */
uint_fast32_t fast_int;  /* 最快宽度 */

#include <stdbool.h>
bool flag = true;  /* true/false */

#include <complex.h>
double complex z = 1.0 + 2.0*I;

/* 7. 内联函数 */
static inline int max(int a, int b) {
    return a > b ? a : b;
}

/* 8. 匿名结构和联合 */
struct {
    union {
        struct { int x, y, z; };  /* 匿名 */
        int coords[3];
    };
} point;

/* 9. 灵活数组成员 */
struct buffer {
    size_t size;
    char data[];  /* 必须放在最后 */
};

/* 10. _Pragma 操作符 */
_Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
