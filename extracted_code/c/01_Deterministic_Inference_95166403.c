/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 187
 * Language: c
 * Block ID: 95166403
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 确定性卷积 (已展开循环以减少分支)
void conv2d_deterministic(const float* input,
                         const float* kernel,
                         float* output,
                         int in_h, int in_w,
                         int k_h, int k_w,
                         int out_h, int out_w) {

    for (int oh = 0; oh < out_h; oh++) {
        for (int ow = 0; ow < out_w; ow++) {
            float sum = 0.0f;

            // 手动展开3x3卷积核 (最常见情况)
            #if KERNEL_SIZE == 3
            const float* inp = &input[oh * in_w + ow];

            // Row 0
            sum += inp[0 * in_w + 0] * kernel[0 * 3 + 0];
            sum += inp[0 * in_w + 1] * kernel[0 * 3 + 1];
            sum += inp[0 * in_w + 2] * kernel[0 * 3 + 2];

            // Row 1
            sum += inp[1 * in_w + 0] * kernel[1 * 3 + 0];
            sum += inp[1 * in_w + 1] * kernel[1 * 3 + 1];
            sum += inp[1 * in_w + 2] * kernel[1 * 3 + 2];

            // Row 2
            sum += inp[2 * in_w + 0] * kernel[2 * 3 + 0];
            sum += inp[2 * in_w + 1] * kernel[2 * 3 + 1];
            sum += inp[2 * in_w + 2] * kernel[2 * 3 + 2];
            #else
            // 通用实现
            for (int kh = 0; kh < k_h; kh++) {
                for (int kw = 0; kw < k_w; kw++) {
                    sum += input[(oh + kh) * in_w + (ow + kw)] *
                           kernel[kh * k_w + kw];
                }
            }
            #endif

            output[oh * out_w + ow] = sum;
        }
    }
}
