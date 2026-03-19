/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 497
 * Language: c
 * Block ID: 241e4259
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 函数声明
int add(int a, int b);

// 对应的函数指针类型
int (*fp)(int, int);    // fp是指向函数的指针

// 初始化
fp = &add;              // &可以省略
fp = add;               // 等效

// 调用
int result = (*fp)(2, 3);  // *可以省略
int result = fp(2, 3);     // 等效，推荐写法
