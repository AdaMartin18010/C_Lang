/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 1268
 * Language: c
 * Block ID: ad3418d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 数组与指针的微妙区别
void array_param(int arr[10]);      // 实际为 int*
void array_param2(int arr[]);       // 实际为 int*
void array_param3(int (*arr)[10]);  // 指向数组的指针
void array_param4(int arr[static 10]); // C99 VLA 提示

int global_array[10];
extern int extern_array[];
