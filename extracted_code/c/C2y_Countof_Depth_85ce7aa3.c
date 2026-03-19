/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1403
 * Language: c
 * Block ID: 85ce7aa3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型推导示例
typedef int Array10[10];
Array10 arr;

// 编译器内部表示
// TypeOf(arr) = ArrayType(ElementType=int, Size=10)
// _Countof(arr) = ArrayType.Size
