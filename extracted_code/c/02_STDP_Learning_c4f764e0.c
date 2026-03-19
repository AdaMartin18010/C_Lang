/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\02_STDP_Learning.md
 * Line: 547
 * Language: c
 * Block ID: c4f764e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 浮点累积误差导致权重异常
syn->weight += dw;  // 多次小更新后精度丢失

// ✅ 解决: 定期归一化或使用定点数
// 或Kahan求和
float kahan_sum(float *values, int n) {
    float sum = 0.0f;
    float c = 0.0f;
    for (int i = 0; i < n; i++) {
        float y = values[i] - c;
        float t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}
