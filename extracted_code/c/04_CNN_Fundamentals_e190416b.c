/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 298
 * Language: c
 * Block ID: e190416b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void softmax(const float* input, float* output, int n) {
    // 数值稳定性：减去最大值
    float max_val = input[0];
    for (int i = 1; i < n; i++) {
        if (input[i] > max_val) max_val = input[i];
    }

    float sum = 0;
    for (int i = 0; i < n; i++) {
        output[i] = expf(input[i] - max_val);
        sum += output[i];
    }

    for (int i = 0; i < n; i++) {
        output[i] /= sum;
    }
}
