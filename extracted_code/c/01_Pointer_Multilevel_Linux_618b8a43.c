/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 550
 * Language: c
 * Block ID: 618b8a43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 混淆一级指针和二级指针
void set_value_wrong(int *p, int value) {
    // 意图: 修改p指向的值
    p = &value;  // ❌ 错误！这只是修改了局部变量p
                 // 外部传入的指针不会被修改
}

// ✅ 正确: 使用二级指针
void set_value_correct(int **pp, int *value_ptr) {
    *pp = value_ptr;  // ✅ 正确！修改外部指针的值
}

// 测试
void test(void) {
    int x = 10, y = 20;
    int *p = &x;

    set_value_wrong(p, y);   // p仍然指向x！
    printf("*p = %d\n", *p);  // 输出: 10

    set_value_correct(&p, &y);  // p现在指向y
    printf("*p = %d\n", *p);     // 输出: 20
}
