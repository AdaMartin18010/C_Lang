/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\README.md
 * Line: 420
 * Language: c
 * Block ID: 91e80b58
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// example_buggy.c - 故意包含各种内存错误
#include <stdlib.h>
#include <string.h>

void illegal_read() {
    int *p = malloc(10 * sizeof(int));
    int x = p[20];  // 越界读取
    free(p);
}

void illegal_write() {
    int *p = malloc(10 * sizeof(int));
    p[20] = 42;     // 越界写入
    free(p);
}

void use_after_free() {
    int *p = malloc(sizeof(int));
    free(p);
    *p = 42;        // 使用已释放内存
}

void double_free() {
    int *p = malloc(sizeof(int));
    free(p);
    free(p);        // 双重释放
}

void memory_leak() {
    int *p = malloc(sizeof(int));  // 未释放
    *p = 42;
}

void uninitialized() {
    int x;
    if (x > 0) {    // 使用未初始化变量
        printf("x is positive\n");
    }
}

int main() {
    illegal_read();
    illegal_write();
    use_after_free();
    double_free();
    memory_leak();
    uninitialized();
    return 0;
}
