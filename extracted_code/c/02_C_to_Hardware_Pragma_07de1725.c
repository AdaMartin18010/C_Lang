/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\02_C_to_Hardware_Pragma.md
 * Line: 51
 * Language: c
 * Block ID: 07de1725
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Pragma是C/C++预处理器指令，用于向HLS工具传递编译指导信息。
 * 这些指令不会影响C代码的功能正确性，但会显著影响生成的硬件。
 */

// 基本语法
#pragma HLS <directive_name> [options]

// 示例
void example(int a[100], int b[100]) {
    #pragma HLS INTERFACE mode=ap_vld port=a
    #pragma HLS INTERFACE mode=ap_vld port=b
    #pragma HLS PIPELINE II=1

    for (int i = 0; i < 100; i++) {
        a[i] = b[i] * 2;
    }
}

/*
 * Pragma的作用域：
 *
 * 1. 文件级：影响整个源文件
 *    #pragma HLS interface mode=ap_ctrl_none port=return
 *
 * 2. 函数级：影响整个函数
 *    void func(...) {
 *        #pragma HLS PIPELINE
 *        ...
 *    }
 *
 * 3. 循环级：影响特定循环
 *    for (...) {
 *        #pragma HLS UNROLL
 *    }
 *
 * 4. 区域级：影响代码区域
 *    {
 *        #pragma HLS PIPELINE
 *        ...
 *    }
 */
