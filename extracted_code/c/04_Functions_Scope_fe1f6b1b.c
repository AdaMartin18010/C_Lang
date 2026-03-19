/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\04_Functions_Scope.md
 * Line: 25
 * Language: c
 * Block ID: fe1f6b1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整函数定义
return_type function_name(parameter_list) {
    // 函数体
    return value;
}

// 示例
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 函数声明（原型）
int max(int a, int b);  // 参数名可省略: int max(int, int);

// 旧式函数定义（K&R风格，已废弃）
int old_max(a, b)
    int a;
    int b;
{
    return (a > b) ? a : b;
}
