/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\03_Modularization_Linking.md
 * Line: 56
 * Language: c
 * Block ID: 87483a28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 外部链接（可被其他翻译单元引用）
int global_var;           // 定义，外部链接
extern int other_var;     // 声明，外部链接
void global_func(void);   // 函数声明，外部链接

// 内部链接（仅当前翻译单元可见）
static int internal_var;  // 内部链接
static void internal_func(void) { }  // 内部链接

// 无链接（块作用域）
void func(void) {
    int local_var;        // 无链接
}
