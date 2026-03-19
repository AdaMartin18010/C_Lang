/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md
 * Line: 129
 * Language: c
 * Block ID: cfd996f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 顶层函数
void top_function(int in[100], int out[100]) {
    #pragma HLS INTERFACE mode=ap_vld port=in
    #pragma HLS INTERFACE mode=ap_vld port=out

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1

        // 函数调用点1
        int temp1 = stage1_process(in[i]);

        // 函数调用点2
        int temp2 = stage2_process(temp1);

        // 函数调用点3
        out[i] = stage3_process(temp2);
    }
}

// 子函数1
int stage1_process(int x) {
    #pragma HLS INLINE  // 内联展开
    return x * 2 + 1;
}

// 子函数2
int stage2_process(int x) {
    #pragma HLS PIPELINE  // 独立流水线
    return (x > 100) ? x - 100 : x;
}

// 子函数3
int stage3_process(int x) {
    #pragma HLS ALLOCATION instances=mul limit=1 operation  // 限制乘法器
    return x * x;
}

/*
综合结果结构：

┌─────────────────────────────────────────────────────────────────┐
│                      top_function顶层                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐     │
│  │ 输入数据  │──▶│stage1    │──▶│stage2    │──▶│stage3    │──▶ │
│  │          │   │(内联)     │   │(流水线)   │   │(限制资源) │    │
│  └──────────┘   └──────────┘   └──────────┘   └──────────┘     │
│                                                                  │
│  stage1: 直接展开为组合逻辑                                       │
│  stage2: 实例化为独立流水线模块                                   │
│  stage3: 实例化为模块，内部限制1个乘法器                           │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
*/
