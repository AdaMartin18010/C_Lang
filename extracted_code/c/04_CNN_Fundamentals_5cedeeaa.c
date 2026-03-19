/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 324
 * Language: c
 * Block ID: 5cedeeaa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    Tensor* grad_weights;
    Tensor* grad_bias;
    Tensor* grad_input;
} Conv2D_Grads;

void conv2d_backward(Conv2D_Layer* layer, Tensor* input, Tensor* grad_output,
                     Conv2D_Grads* grads) {
    int k = layer->kernel_size;
    int out_h = grad_output->height;
    int out_w = grad_output->width;

    // 初始化梯度
    memset(grads->grad_weights->data, 0, grads->grad_weights->size * sizeof(float));
    memset(grads->grad_bias->data, 0, grads->grad_bias->size * sizeof(float));
    memset(grads->grad_input->data, 0, grads->grad_input->size * sizeof(float));

    for (int oc = 0; oc < layer->out_channels; oc++) {
        // 偏置梯度
        float bias_grad = 0;
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                bias_grad += tensor_get(grad_output, ow, oh, oc);
            }
        }
        grads->grad_bias->data[oc] = bias_grad;

        for (int ic = 0; ic < layer->in_channels; ic++) {
            for (int kh = 0; kh < k; kh++) {
                for (int kw = 0; kw < k; kw++) {
                    float weight_grad = 0;

                    for (int oh = 0; oh < out_h; oh++) {
                        for (int ow = 0; ow < out_w; ow++) {
                            int ih = oh * layer->stride + kh - layer->padding;
                            int iw = ow * layer->stride + kw - layer->padding;

                            if (ih >= 0 && ih < input->height &&
                                iw >= 0 && iw < input->width) {
                                float g_out = tensor_get(grad_output, ow, oh, oc);
                                float inp = tensor_get(input, iw, ih, ic);
                                weight_grad += g_out * inp;

                                // 输入梯度
                                float w = layer->weights[((oc * layer->in_channels + ic) * k + kh) * k + kw];
                                float* g_in = &grads->grad_input->data[(ic * input->height + ih) * input->width + iw];
                                *g_in += g_out * w;
                            }
                        }
                    }
                    grads->grad_weights->data[((oc * layer->in_channels + ic) * k + kh) * k + kw] = weight_grad;
                }
            }
        }
    }
}
