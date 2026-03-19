/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\01_Pointer_Memory_Mapping.md
 * Line: 277
 * Language: c
 * Block ID: 9fbd0600
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指针声明解析（从右向左读）
int *p;              // p 是 指向 int 的指针
int *arr[10];        // arr 是 10个元素的数组，每个元素是 指向 int 的指针
int (*arr)[10];      // arr 是 指向 包含10个int的数组 的指针
int *(*func)(int);   // func 是 指向 接受int返回指向int的指针的函数 的指针

// 可视化
// int *p
// ┌─────┐    ┌─────┐
// │  p  │───→│ int │
// └─────┘    └─────┘
//
// int **pp
// ┌─────┐    ┌─────┐    ┌─────┐
// │ pp  │───→│  *  │───→│ int │
// └─────┘    └─────┘    └─────┘
//
// int (*arr)[10]
// ┌─────┐    ┌──────────────────┐
// │ arr │───→│ [0][1][2]...[9]  │  ← 10个int的数组
// └─────┘    └──────────────────┘
