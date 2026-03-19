/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 217
 * Language: c
 * Block ID: 92f0f9b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 文件作用域（外部链接）
int global_var;

// 内部链接
static int internal_var;

void function(void) {
    // 块作用域
    int local_var;

    {
        // 嵌套作用域，隐藏外部同名变量
        int local_var;  // 不同的变量！
    }
}
