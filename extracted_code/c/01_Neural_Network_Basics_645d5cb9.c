/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 199
 * Language: c
 * Block ID: 645d5cb9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int n_inputs;           // 输入维度 n^{[l-1]}
    int n_outputs;          // 输出维度 n^{[l]}
    double** weights;       // 权重矩阵 W[n_outputs][n_inputs]
    double* biases;         // 偏置向量 b[n_outputs]
    double** weight_grads;  // 权重梯度 ∇W
    double* bias_grads;     // 偏置梯度 ∇b

    // 前向传播缓存（用于反向传播）
    double* input_cache;    // 缓存输入 a^{[l-1]}
    double* z_cache;        // 缓存线性输出 z^{[l]}

    // 激活函数
    ActivationType act_type;
    double (*activate)(double);
    double (*activate_deriv)(double);
} Layer;

// 层的前向传播
void layer_forward(Layer* layer, const double* input, double* output) {
    // 缓存输入用于反向传播
    memcpy(layer->input_cache, input, layer->n_inputs * sizeof(double));

    for (int i = 0; i < layer->n_outputs; i++) {
        // z_i = sum_j(W_ij * x_j) + b_i
        double z = layer->biases[i];
        for (int j = 0; j < layer->n_inputs; j++) {
            z += layer->weights[i][j] * input[j];
        }
        layer->z_cache[i] = z;
        output[i] = layer->activate(z);
    }
}
