/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 76
 * Language: c
 * Block ID: ce9f4e7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int width, height, channels;
    float* data;  // 按CHW排列: data[c*H*W + h*W + w]
} Tensor;

typedef struct {
    int kernel_size;
    int in_channels, out_channels;
    float* weights;  // [out][in][kh][kw]
    float* bias;
    int stride, padding;
} Conv2D_Layer;

// 卷积前向传播
Tensor* conv2d_forward(Conv2D_Layer* layer, Tensor* input) {
    int k = layer->kernel_size;
    int out_h = (input->height + 2*layer->padding - k) / layer->stride + 1;
    int out_w = (input->width + 2*layer->padding - k) / layer->stride + 1;

    Tensor* output = tensor_create(out_w, out_h, layer->out_channels);

    for (int oc = 0; oc < layer->out_channels; oc++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float sum = layer->bias[oc];

                for (int ic = 0; ic < layer->in_channels; ic++) {
                    for (int kh = 0; kh < k; kh++) {
                        for (int kw = 0; kw < k; kw++) {
                            int ih = oh * layer->stride + kh - layer->padding;
                            int iw = ow * layer->stride + kw - layer->padding;

                            if (ih >= 0 && ih < input->height &&
                                iw >= 0 && iw < input->width) {
                                float input_val = tensor_get(input, iw, ih, ic);
                                float weight = layer->weights[((oc * layer->in_channels + ic) * k + kh) * k + kw];
                                sum += input_val * weight;
                            }
                        }
                    }
                }
                tensor_set(output, ow, oh, oc, sum);
            }
        }
    }
    return output;
}
