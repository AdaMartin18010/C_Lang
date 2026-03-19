/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 110
 * Language: c
 * Block ID: e66236f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 静态局部变量：生命周期延长到程序结束
void func() {
    static int count = 0;  // 只初始化一次
    count++;
    printf("%d\n", count);
}

// 2. 静态全局变量/函数：限制作用域在当前文件
static int internal_var;  // 其他文件不可见
static void internal_func() {}  // 其他文件不可调用

// 3. C++中的类静态成员（如果面试C++）
