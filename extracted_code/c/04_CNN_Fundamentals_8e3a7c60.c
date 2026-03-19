/*
 * Auto-generated from: 11_Machine_Learning_C\04_CNN_Fundamentals.md
 * Line: 130
 * Language: c
 * Block ID: 8e3a7c60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// im2col: 将输入图像块展开为列
void im2col(const float* data_im, int channels, int height, int width,
            int kernel_h, int kernel_w, int stride, int pad,
            float* data_col) {
    int height_col = (height + 2*pad - kernel_h) / stride + 1;
    int width_col = (width + 2*pad - kernel_w) / stride + 1;
    int channels_col = channels * kernel_h * kernel_w;

    for (int c = 0; c < channels_col; c++) {
        int w_offset = c % kernel_w;
        int h_offset = (c / kernel_w) % kernel_h;
        int c_im = c / kernel_h / kernel_w;

        for (int h = 0; h < height_col; h++) {
            for (int w = 0; w < width_col; w++) {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;

                if (im_row - pad >= 0 && im_row - pad < height &&
                    im_col - pad >= 0 && im_col - pad < width)
                    data_col[col_index] = data_im[(c_im * height + (im_row - pad)) * width + (im_col - pad)];
                else
                    data_col[col_index] = 0;
            }
        }
    }
}

// 使用BLAS的卷积
void conv2d_blas(Conv2D_Layer* layer, Tensor* input, Tensor* output) {
    int k = layer->kernel_size;
    int out_h = output->height;
    int out_w = output->width;

    // 分配im2col缓冲区
    int col_size = layer->in_channels * k * k * out_h * out_w;
    float* col_buffer = malloc(col_size * sizeof(float));

    // im2col转换
    im2col(input->data, layer->in_channels, input->height, input->width,
           k, k, layer->stride, layer->padding, col_buffer);

    // 矩阵乘法: output = weights * col_buffer
    // 使用OpenBLAS cblas_sgemm
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                layer->out_channels, out_h * out_w, layer->in_channels * k * k,
                1.0f, layer->weights, layer->in_channels * k * k,
                col_buffer, out_h * out_w,
                0.0f, output->data, out_h * out_w);

    // 添加偏置
    for (int oc = 0; oc < layer->out_channels; oc++) {
        for (int i = 0; i < out_h * out_w; i++) {
            output->data[oc * out_h * out_w + i] += layer->bias[oc];
        }
    }

    free(col_buffer);
}
