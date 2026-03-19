/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 36
 * Language: c
 * Block ID: aa59f778
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23之前
int *p1 = NULL;      // 可能定义为 0 或 (void*)0
int *p2 = 0;         // 整数0，可能被警告

// C23
int *p3 = nullptr;   // 明确的空指针常量

// 类型安全
void func(int);
void func(int *);

func(NULL);      // 可能调用 func(int) - 意外！
func(nullptr);   // 明确调用 func(int *) - 正确
