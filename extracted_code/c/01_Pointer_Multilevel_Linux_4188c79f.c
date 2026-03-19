/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 638
 * Language: c
 * Block ID: 4188c79f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 解引用级数不匹配
void print_value_wrong(int **pp) {
    printf("%d\n", pp);     // ❌ 打印地址，不是值
    printf("%d\n", *pp);    // ❌ 打印指针值（地址）
    // printf("%d\n", **pp); // ✅ 这才是打印int值
}

// ❌ 错误: 过度解引用
void crash_example(int *p) {
    printf("%d\n", **p);   // ❌ 崩溃！p是一级指针，**p非法
}

// ❌ 错误: 解引用不足
void wrong_print(int ***ppp) {
    printf("%d\n", *ppp);   // ❌ 打印地址
    printf("%d\n", **ppp);  // ❌ 还是地址
    // printf("%d\n", ***ppp); // ✅ 需要三级解引用
}

// ✅ 正确: 解引用级数匹配
void print_int(int *p) {
    printf("%d\n", *p);     // ✅ 一级指针，一次解引用
}

void print_int_pp(int **pp) {
    printf("%d\n", **pp);   // ✅ 二级指针，两次解引用
}

void print_int_ppp(int ***ppp) {
    printf("%d\n", ***ppp); // ✅ 三级指针，三次解引用
}
