/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 557
 * Language: c
 * Block ID: 2c4c710b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指针映射示例
void pointer_mapping(
    int *scalar_in,      // 映射为单个输入端口
    int *scalar_out,     // 映射为单个输出端口
    int array[100],      // 映射为RAM接口
    int matrix[10][10]   // 映射为2D存储器
) {
    *scalar_out = *scalar_in + array[0];
    matrix[0][0] = array[5];
}
