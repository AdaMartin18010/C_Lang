/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 305
 * Language: c
 * Block ID: 6eced2ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 限定符处理详解
#include <stdatomic.h>

void qualifier_mechanics(void) {
    // ========== 基本限定符 ==========

    const int ci = 42;
    volatile int vi = 100;
    restrict int* ri = &vi;  // restrict 仅适用于指针
    _Atomic int ai = 0;

    // typeof 保留限定符
    typeof(ci) ci2;          // const int
    typeof(vi) vi2;          // volatile int
    typeof(ai) ai2;          // _Atomic int

    // typeof_unqual 移除限定符
    typeof_unqual(ci) ci3;   // int
    typeof_unqual(vi) vi3;   // int
    typeof_unqual(ai) ai3;   // int

    // ========== 指针限定符 ==========

    int x = 42;
    int* const cp = &x;           // const 指针
    int* volatile vp = &x;        // volatile 指针
    int* restrict rp = &x;        // restrict 指针

    typeof(cp) cp2;               // int* const
    typeof(vp) vp2;               // int* volatile
    typeof(rp) rp2;               // int* restrict

    typeof_unqual(cp) cp3;        // int*
    typeof_unqual(vp) vp3;        // int*
    typeof_unqual(rp) rp3;        // int*

    // ========== 多重限定符 ==========

    const volatile int cvi = 0;
    const int* const volatile* ptr = NULL;

    typeof(cvi) cvi2;             // const volatile int
    typeof(ptr) ptr2;             // const int* const volatile*

    typeof_unqual(cvi) cvi3;      // int
    typeof_unqual(ptr) ptr3;      // const int** (注意：只移除顶层限定符)
}

// 注意：typeof_unqual 只移除最外层的限定符
void qualifier_nesting(void) {
    const int* ptr;               // 指向 const int 的指针
    typeof_unqual(ptr) ptr2;      // int* (const 保留，因为不是 ptr 的限定符)

    int* const cptr = NULL;       // const 指针
    typeof_unqual(cptr) ptr3;     // int* (const 被移除)
}
