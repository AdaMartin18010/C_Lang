/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\01_Compcert_Overview.md
 * Line: 334
 * Language: c
 * Block ID: 381108ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CompCert C 不支持或有限支持的特性

// 1. 可变长度数组 (VLA) - 不支持
void vla_not_supported(int n) {
    // int arr[n];  // 错误: VLA 不支持
}

// 2. 非局部跳转 (setjmp/longjmp) - 不支持
#include <setjmp.h>
void nonlocal_jump(void) {
    // jmp_buf env;
    // setjmp(env);  // 不支持
}

// 3. 复合赋值表达式的副作用
void side_effects(void) {
    int a, b;
    // a = (b = 1, b++);  // 序列点行为可能不同
}

// 4. 某些内联汇编扩展
void inline_asm(void) {
    // 支持基本内联汇编，但扩展受限
    // asm volatile("..." : ...);
}

// 5. 某些 GCC 扩展
void gcc_extensions(void) {
    // __attribute__((...)) 部分支持
    // 语句表达式 ({ ... }) 不支持
    // 嵌套函数不支持
}

// 6. 某些标准库函数
void stdlib_limits(void) {
    // 不支持某些复杂数学函数
    // 不支持某些 locale 相关函数
}
