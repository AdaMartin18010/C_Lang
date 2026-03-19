/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 488
 * Language: c
 * Block ID: 4036fdb8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 必须诊断的问题 (C23)

// 1. 函数声明后没有原型
void func();  // 警告：使用void func(void)

// 2. 隐式int
static x = 0;  // 错误：需要显式类型

// 3. 旧风格函数定义
void old_func(a, b)  // 警告：使用原型
    int a, b;
{
}

// 4. 不兼容指针转换
char *p = malloc(10);  // 警告：需要显式转换
