/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 199
 * Language: c
 * Block ID: b0b97221
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int pool_size;
    int stride;
    int* max_indices;  // 用于反向传播
} MaxPool2D;

Tensor* maxpool2d_forward(MaxPool2D* pool, Tensor* input, Tensor* output) {
    int p = pool->pool_size;
    int out_h = output->height;
    int out_w = output->width;

    pool->max_indices = malloc(output->channels * out_h * out_w * sizeof(int));

    for (int c = 0; c < input->channels; c++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float max_val = -INFINITY;
                int max_idx = 0;

                for (int ph = 0; ph < p; ph++) {
                    for (int pw = 0; pw < p; pw++) {
                        int ih = oh * pool->stride + ph;
                        int iw = ow * pool->stride + pw;
                        float val = tensor_get(input, iw, ih, c);

                        if (val > max_val) {
                            max_val = val;
                            max_idx = ph * p + pw;
                        }
                    }
                }
                tensor_set(output, ow, oh, c, max_val);
                pool->max_indices[(c * out_h + oh) * out_w + ow] = max_idx;
            }
        }
    }
    return output;
}
