/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 339
 * Language: c
 * Block ID: e6ee2baf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 多层流水线示例
void deep_pipeline(int in[100], int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1

        // 阶段1：内存读取（2周期延迟）
        int data = in[i];

        // 阶段2：乘法运算（3周期延迟）
        int mult = data * 7;

        // 阶段3：加法运算（1周期延迟）
        int add = mult + 3;

        // 阶段4：比较选择（1周期延迟）
        int result = (add > 100) ? add : 100;

        // 阶段5：内存写入（2周期延迟）
        out[i] = result;
    }
}

/*
流水线结构：

┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
│ 读数据   │───▶│ 乘法器  │───▶│ 加法器  │───▶│ 选择器  │───▶│ 写数据  │
│ (2级)   │    │ (3级)   │    │ (1级)   │    │ (1级)   │    │ (2级)   │
└─────────┘    └─────────┘    └─────────┘    └─────────┘    └─────────┘
    │              │              │              │              │
    └──────────────┴──────────────┴──────────────┴──────────────┘
                           流水线寄存器

总流水线深度 = 2 + 3 + 1 + 1 + 2 = 9级
启动间隔(II) = 1
首次输出延迟 = 9周期
总执行时间 = 100 + 9 - 1 = 108周期
*/
