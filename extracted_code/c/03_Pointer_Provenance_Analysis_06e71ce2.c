/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\03_Pointer_Provenance_Analysis.md
 * Line: 713
 * Language: c
 * Block ID: 06e71ce2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GCC/Clang TBAA 元数据（LLVM IR 级别）
// 实际 C 代码
void tbaa_example(int* ip, float* fp) {
    *ip = 1;
    *fp = 2.0f;
    int x = *ip;  // TBAA 允许编译器假设 x = 1
}

/*
生成的 LLVM IR（简化）：

!0 = !{!"int", !1}
!1 = !{!"omnipotent char", !2}
!2 = !{!"Simple C/C++ TBAA"}
!3 = !{!"float", !1}

define void @tbaa_example(i32* %ip, float* %fp) {
  store i32 1, i32* %ip, !tbaa !0      ; !0 = int type
  store float 2.0, float* %fp, !tbaa !3 ; !3 = float type
  %x = load i32, i32* %ip, !tbaa !0
  ; 由于 !0 和 !3 不同，编译器可以假设 ip != fp
  ret void
}
*/

// 禁用 TBAA 的方法
void no_tbaa(int* ip, float* fp) {
    // 方式 1：使用 -fno-strict-aliasing 编译

    // 方式 2：使用 volatile
    *(volatile int*)ip = 1;
    *(volatile float*)fp = 2.0f;

    // 方式 3：使用 char* 中转
    char* cp = (char*)ip;
    cp[0] = 1;
}

// GCC 属性控制 TBAA
void __attribute__((optimize("no-strict-aliasing")))
no_tbaa_func(int* ip, float* fp) {
    *ip = 1;
    *fp = 2.0f;
}
