/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 242
 * Language: c
 * Block ID: 191b4af5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

Tensor* avgpool2d_forward(Tensor* input, int pool_size, int stride) {
    int out_h = (input->height - pool_size) / stride + 1;
    int out_w = (input->width - pool_size) / stride + 1;
    Tensor* output = tensor_create(out_w, out_h, input->channels);

    for (int c = 0; c < input->channels; c++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float sum = 0;
                for (int ph = 0; ph < pool_size; ph++) {
                    for (int pw = 0; pw < pool_size; pw++) {
                        int ih = oh * stride + ph;
                        int iw = ow * stride + pw;
                        sum += tensor_get(input, iw, ih, c);
                    }
                }
                tensor_set(output, ow, oh, c, sum / (pool_size * pool_size));
            }
        }
    }
    return output;
}
