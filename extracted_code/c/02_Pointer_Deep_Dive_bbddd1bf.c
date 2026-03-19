/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 803
 * Language: c
 * Block ID: bbddd1bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int arr[10];
int *p = arr;

p += 10;      // 合法：指向arr[10]（末尾之后）
// *p = 1;    // 非法：不能解引用

p += 1;       // 未定义行为！超出数组末尾+1

// 指针相减
int diff = &arr[5] - &arr[0];  // 合法：5
// int bad = &arr[5] - (int*)0x1000;  // 未定义！
