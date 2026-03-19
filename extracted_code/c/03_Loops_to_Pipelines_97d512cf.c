/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 308
 * Language: c
 * Block ID: 97d512cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单流水线示例
void simple_pipeline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        int a = in[i];
        int b = a * 2;
        int c = b + 1;
        out[i] = c;
    }
}

/*
流水线执行时序（II=1）：

周期     1      2      3      4      5      6      7      8
        ─────────────────────────────────────────────────────
迭代0   │读a   │乘2   │加1   │写out │      │      │      │
迭代1   │      │读a   │乘2   │加1   │写out │      │      │
迭代2   │      │      │读a   │乘2   │加1   │写out │      │
迭代3   │      │      │      │读a   │乘2   │加1   │写out │
        ─────────────────────────────────────────────────────

每个周期启动一个新迭代，7个周期后开始每周期输出一个结果
总周期数 = 启动间隔(II) * 迭代次数 + 流水线深度 - 1
         = 1 * 100 + 3 = 103周期
*/
