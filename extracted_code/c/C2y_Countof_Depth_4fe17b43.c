/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 195
 * Language: c
 * Block ID: 4fe17b43
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int matrix[3][4];

_Countof(matrix);        // 返回 3（第一维大小）
_Countof(matrix[0]);     // 返回 4（第二维大小）
_Countof(matrix[0][0]);  // 编译错误：int不是数组
