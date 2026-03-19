/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 75
 * Language: c
 * Block ID: 2f563beb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不同类型的指针映射为不同的硬件结构

// 1. 标量指针 → 简单寄存器接口
void scalar_pointer(int *input, int *output) {
    #pragma HLS INTERFACE mode=ap_vld port=input
    #pragma HLS INTERFACE mode=ap_vld port=output
    *output = *input * 2;
}
/* 硬件实现：
   input:  [data:32bit] + [vld:1bit]
   output: [data:32bit] + [vld:1bit]
*/

// 2. 数组指针 → 存储器接口
void array_pointer(int arr[100], int idx, int *result) {
    #pragma HLS INTERFACE mode=ram_1p port=arr
    *result = arr[idx];
}
/* 硬件实现：
   arr: [address:7bit] + [data_in:32bit] + [data_out:32bit] +
        [we:1bit] + [en:1bit]
*/

// 3. 多维数组 → 多维存储器或地址计算
void matrix_pointer(int mat[10][10], int row, int col, int *result) {
    *result = mat[row][col];
}
/* 硬件实现：
   方案A: 展平为一维存储器，地址 = row * 10 + col
   方案B: 双端口存储器，row和col分别作为不同维度的索引
*/
