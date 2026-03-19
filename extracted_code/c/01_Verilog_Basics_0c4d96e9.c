/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\01_Verilog_Basics.md
 * Line: 458
 * Language: c
 * Block ID: 0c4d96e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C算法
int sum_array(int arr[100]) {
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += arr[i];
    }
    return sum;
}

// 对应的Verilog概念
// - 循环展开为并行加法树
// - 数组映射为存储器
// - 函数映射为电路模块
