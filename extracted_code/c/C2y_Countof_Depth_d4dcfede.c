/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 141
 * Language: c
 * Block ID: d4dcfede
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 形式一：类型名（必须带括号）
size_t n1 = _Countof(int[10]);      // n1 = 10
size_t n2 = _Countof(double[5][3]); // n2 = 5 (第一维大小)

// 形式二：表达式（括号可省略）
int arr[20];
size_t n3 = _Countof arr;           // n3 = 20

// 形式三：表达式（带括号）
size_t n4 = _Countof(arr);          // n4 = 20

// 复杂表达式
int matrix[4][5];
size_t n5 = _Countof(matrix);       // n5 = 4
size_t n6 = _Countof(matrix[0]);    // n6 = 5
