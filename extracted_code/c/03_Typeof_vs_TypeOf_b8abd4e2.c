/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 545
 * Language: c
 * Block ID: b8abd4e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 限定符系统详解

void c23_qualifier_system(void) {
    // ========== const 限定符 ==========

    // 顶层 const (Top-level const)
    const int tc = 42;                    // 值不能修改
    typeof(tc) tc2;                       // const int

    // 底层 const (Low-level const)
    const int* lc = &tc;                  // 指向的值不能修改
    typeof(lc) lc2;                       // const int*

    // 双重 const
    const int* const dlc = &tc;           // 指针和值都不能修改
    typeof(dlc) dlc2;                     // const int* const

    // ========== volatile 限定符 ==========

    volatile int vi;                      // 每次访问都从内存读取
    typeof(vi) vi2;                       // volatile int

    volatile int* vip;                    // 指向 volatile int
    typeof(vip) vip2;                     // volatile int*

    int* volatile ivp;                    // volatile 指针
    typeof(ivp) ivp2;                     // int* volatile

    // ========== restrict 限定符 ==========

    // restrict 仅用于指针，表示指针是唯一访问路径
    int arr[100];
    int* restrict rp = arr;               // restrict 指针
    typeof(rp) rp2;                       // int* restrict

    // ========== _Atomic 限定符 ==========

    _Atomic int ai;                       // 原子整数
    typeof(ai) ai2;                       // _Atomic int

    // ========== typeof_unqual 效果 ==========

    const volatile int cvi = 0;
    typeof_unqual(cvi) u1;                // int (移除所有)

    const int* const ptr = NULL;
    typeof_unqual(ptr) u2;                // const int* (只移除顶层 const)
}
