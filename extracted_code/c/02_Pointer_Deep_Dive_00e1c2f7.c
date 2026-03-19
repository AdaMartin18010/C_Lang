/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 395
 * Language: c
 * Block ID: 00e1c2f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建3行的锯齿数组
int **matrix = malloc(3 * sizeof(int*));
matrix[0] = malloc(3 * sizeof(int));  // 第0行: 3个元素
matrix[1] = malloc(5 * sizeof(int));  // 第1行: 5个元素
matrix[2] = malloc(2 * sizeof(int));  // 第2行: 2个元素

内存布局：
matrix ──► [ptr0] ──► [0][1][2]
          [ptr1] ──► [0][1][2][3][4]
          [ptr2] ──► [0][1]
