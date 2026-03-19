/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 829
 * Language: c
 * Block ID: 3279adba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial12_leak.c
#include <stdlib.h>

// 内存泄漏
void memory_leak() {
    int* p = malloc(sizeof(int) * 100);
    *p = 42;
    // 没有free! 内存泄漏!
}

// 正确释放
void no_leak() {
    int* p = malloc(sizeof(int) * 100);
    *p = 42;
    free(p);
}

// 复杂情况
int* maybe_leak(int flag) {
    int* p = malloc(sizeof(int) * 10);
    if (flag) {
        free(p);
        return NULL;
    }
    return p;  // 调用者负责释放
}
