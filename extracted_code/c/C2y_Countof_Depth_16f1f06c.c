/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 229
 * Language: c
 * Block ID: 16f1f06c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int matrix[3][4][5];

// 计算总元素数
size_t total = _Countof(matrix) * _Countof(matrix[0]) * _Countof(matrix[0][0]);
// total = 3 * 4 * 5 = 60
