/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 713
 * Language: c
 * Block ID: 12990361
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 不同类型多级指针混用
void type_confusion(void) {
    int x = 42;
    int *p = &x;
    int **pp = &p;

    // 错误: 把int**当int*用
    int *wrong = (int *)pp;  // ❌ 强制转换，类型系统被破坏
    printf("%d\n", *wrong);   // ❌ 解引用地址值，UB！

    // 错误: 把int*当int**用
    int **also_wrong = (int **)&x;  // ❌ &x是一级指针地址
    printf("%d\n", **also_wrong);    // ❌ 试图把int当指针解引用，崩溃！
}

// ✅ 正确: 类型匹配
void type_correct(void) {
    int x = 42;
    int *p = &x;        // ✅ p是int*，&x是int*
    int **pp = &p;      // ✅ pp是int**，&p是int**

    int val = *p;       // ✅ int*解引用得int
    int val2 = **pp;    // ✅ int**解引用两次得int
}
