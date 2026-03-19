/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 488
 * Language: c
 * Block ID: db2ca451
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires \valid(a) && \valid(b);
    assigns \nothing;
    ensures \result >= *a;
    ensures \result >= *b;
    ensures \result == *a || \result == *b;
*/
int max(int *a, int *b) {
    if (*a > *b) {
        return *a;
    } else {
        return *b;
    }
}

// 运行验证:
// frama-c -wp -rte max.c
//
// -wp: 启用WP插件
// -rte: 生成运行时错误检查
