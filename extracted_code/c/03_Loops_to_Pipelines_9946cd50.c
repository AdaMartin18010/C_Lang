/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 113
 * Language: c
 * Block ID: 9946cd50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 嵌套循环
for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
        result[i][j] = process(data[i][j]);
    }
}

/*
┌─────────────────────────────────────────────────────────────────────┐
│                    嵌套循环硬件实现                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  外层循环计数器 (i)                                                  │
│  ┌─────────────────┐                                                 │
│  │ 计数范围: 0~M-1 │                                                 │
│  │ 状态: i_count   │───▶ 触发内层循环开始                           │
│  └─────────────────┘                                                 │
│           │                                                          │
│           ▼                                                          │
│  内层循环计数器 (j)                                                  │
│  ┌─────────────────┐     ┌──────────────────┐                       │
│  │ 计数范围: 0~N-1 │───▶  │ 数据处理单元     │                       │
│  │ 状态: j_count   │     │ (process函数)    │                       │
│  └─────────────────┘     └──────────────────┘                       │
│           │                                                          │
│           └───────────────────────────────▶ 完成时外层计数器+1       │
│                                                                      │
│  地址生成：addr = i * N + j                                          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/
