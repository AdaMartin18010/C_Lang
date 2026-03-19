/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 307
 * Language: c
 * Block ID: bfcca7b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 以下三种声明等价
void f(int a[10]);      // 10被忽略！
void f(int a[]);        // 等价
void f(int *a);         // 实际类型

// ✅ 安全做法：传递大小
void process_array(int *arr, size_t n);
void process_array2(int arr[], size_t n);  // 语义相同

// C99 VLA（可变长度数组）
void process_matrix(int rows, int cols, int mat[rows][cols]);
