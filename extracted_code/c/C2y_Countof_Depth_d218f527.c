/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1460
 * Language: c
 * Block ID: d218f527
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整数组类型
int a[10];
_Countof(a);  // OK

// 不完全数组类型（初始化后完整）
int b[] = {1, 2, 3};
_Countof(b);  // OK，类型为 int[3]

// 多维数组
int c[2][3][4];
_Countof(c);       // OK，返回2
_Countof(c[0]);    // OK，返回3
_Countof(c[0][0]); // OK，返回4

// 类型名
_Countof(int[10]);     // OK
_Countof(double[5]);   // OK
