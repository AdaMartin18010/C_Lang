/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\05_Arrays_Pointers.md
 * Line: 348
 * Language: c
 * Block ID: afecb69e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int matrix[3][4];
int **ptr = matrix;  // 错误！类型不匹配

// matrix的类型是 int (*)[4]
// ptr的类型是 int **
// 它们是不同的！
