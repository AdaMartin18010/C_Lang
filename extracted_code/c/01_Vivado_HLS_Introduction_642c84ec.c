/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\01_Vivado_HLS_Introduction.md
 * Line: 1067
 * Language: c
 * Block ID: 642c84ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

//===========================================
// HLS调试技巧
//===========================================

// 技巧1：条件编译区分仿真与综合
void debuggable_design(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE

        int temp = compute(in[i]);

        #ifndef __SYNTHESIS__
        // 仅在C仿真时执行
        printf("Debug: i=%d, in=%d, temp=%d\n", i, in[i], temp);
        assert(temp >= 0);  // 运行时检查
        #endif

        out[i] = temp;
    }
}

// 技巧2：使用hls::print（仅仿真）
#include "hls_print.h"

void print_debug(int x) {
    hls::print("Value is %d\n", x);
}

// 技巧3：断点模拟
void breakpoint_simulation(int data[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE

        #ifndef __SYNTHESIS__
        if (i == 50) {
            // 模拟断点
            printf("Breakpoint at i=50, data=%d\n", data[i]);
            getchar();  // 等待输入
        }
        #endif

        data[i] = process(data[i]);
    }
}

// 技巧4：波形文件分析
// 在cosim时生成波形：
// cosim_design -trace_level all
// 使用Vivado查看.wdb文件

// 技巧5：增量调试
void incremental_debug(int in[100], int out[100]) {
    // 阶段1：仅验证输入输出连接
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        out[i] = in[i];  // 直通测试
    }

    // 阶段2：添加简单处理
    // out[i] = in[i] + 1;

    // 阶段3：完整功能
    // out[i] = complex_function(in[i]);
}
