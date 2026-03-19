/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 391
 * Language: c
 * Block ID: 2bd15339
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 标准类型的综合示例
void standard_types(
    char c_in,      // 映射为8位有符号输入
    short s_in,     // 映射为16位有符号输入
    int i_in,       // 映射为32位有符号输入
    long long ll_in, // 映射为64位有符号输入
    float f_in,     // 映射为32位浮点输入
    double d_in     // 映射为64位浮点输入
) {
    // 运算会实例化相应的硬件单元
    char c_out = c_in + 1;           // 8位加法器
    short s_out = s_in * 2;          // 16位乘法器
    int i_out = i_in / 3;            // 32位除法器（慢！）
    long long ll_out = ll_in << 2;   // 64位移位器
    float f_out = f_in * 1.5f;       // 浮点乘法器
    double d_out = d_in + f_in;      // 类型转换+双精度加法
}
