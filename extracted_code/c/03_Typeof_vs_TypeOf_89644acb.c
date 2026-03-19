/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 773
 * Language: c
 * Block ID: 89644acb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 指针类型推导

void c23_pointer_typeof(void) {
    int x = 42;
    const int cx = 100;

    // 基本指针
    int* p = &x;
    const int* cp = &cx;
    int* const pc = &x;
    const int* const cpc = &cx;

    // 指针推导
    typeof(p) p2;       // int*
    typeof(cp) cp2;     // const int*
    typeof(pc) pc2;     // int* const
    typeof(cpc) cpc2;   // const int* const

    // typeof_unqual 效果
    typeof_unqual(cp) u1;   // const int* (const 不是 cp 的限定符)
    typeof_unqual(pc) u2;   // int* (移除顶层 const)
    typeof_unqual(cpc) u3;  // const int* (移除顶层 const)

    // 多级指针
    int** pp = &p;
    typeof(pp) pp2;     // int**
    typeof(*pp) dp;     // int* (解引用)
    typeof(&p) ap;      // int**

    // 函数指针
    int (*fp)(int, int);
    typeof(fp) fp2;     // int (*)(int, int)

    // void 指针
    void* vp = &x;
    typeof(vp) vp2;     // void*
}
