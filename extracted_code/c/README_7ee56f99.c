/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\README.md
 * Line: 161
 * Language: c
 * Block ID: 7ee56f99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 使用更高精度累加 */
double sum = 0.0;
for (int i = 0; i < n; i++) {
    sum += (double)arr[i];  /* 提升精度 */
}

/* ✅ Kahan求和 */
float kahan_sum(float *arr, int n) {
    float sum = 0.0f, c = 0.0f;
    for (int i = 0; i < n; i++) {
        float y = arr[i] - c;
        float t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}
